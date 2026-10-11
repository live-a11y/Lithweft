#include "PatchController.h"
#include <set>
#include <tuple>
#include <dlfcn.h>

namespace {
juce::var object(std::initializer_list<std::pair<juce::Identifier, juce::var>> values) {
    auto* result = new juce::DynamicObject();
    for (const auto& value : values) result->setProperty(value.first, value.second);
    return juce::var(result);
}
bool integer(const juce::var& value, int minimum, int maximum) {
    return (value.isInt() || value.isInt64()) && static_cast<juce::int64>(value) >= minimum
        && static_cast<juce::int64>(value) <= maximum;
}
}

juce::var PatchController::getPatchState() {
    const juce::ScopedLock lock(graphLock);
    juce::Array<juce::var> nodes, connections;
    std::map<BlockInterface*, int> ids;
    int id = 1;
    for (auto* component : getAllBlockComponents()) {
        auto* instance = getBlockInterfaceForComponent(component);
        if (instance != nullptr) ids[instance] = id;
        const auto bounds = component->getBounds();
        nodes.add(object({{"id", id++}, {"expression", component->getCreationText()},
                          {"resolved", instance != nullptr},
                          {"bounds", object({{"x", bounds.getX()}, {"y", bounds.getY()},
                                             {"width", bounds.getWidth()}, {"height", bounds.getHeight()}})},
                          {"state", component->getPatchState()}}));
    }
    std::vector<std::tuple<int, int, int, int>> edges;
    for (const auto& edge : blockChain->getConnections()) {
        const auto source = ids.find(edge.sourceBlock), destination = ids.find(edge.destinationBlock);
        if (source != ids.end() && destination != ids.end())
            edges.emplace_back(source->second, static_cast<int>(edge.outletIndex), destination->second, static_cast<int>(edge.inletIndex));
    }
    std::sort(edges.begin(), edges.end());
    for (const auto& edge : edges)
        connections.add(object({{"source", std::get<0>(edge)}, {"outlet", std::get<1>(edge)},
                                {"destination", std::get<2>(edge)}, {"inlet", std::get<3>(edge)}}));
    return object({{"format", "Lithweft Patch"}, {"version", 1},
                   {"mode", currentMode == ApplicationMode::Editing ? "editing" : "performance"},
                   {"nodes", nodes}, {"connections", connections}});
}

void PatchController::clearPatch() {
    const juce::ScopedLock lock(graphLock);
    for (auto* line : lineComponents) delete line;
    lineComponents.clear();
    blockMapping.clear();
    // The chain destroys UI blocks. Delete the remaining display/empty blocks
    // afterwards, so neither kind of component can be deleted twice.
    blockChain.reset();
    for (auto* component : getAllBlockComponents()) delete component;
    for (auto* handle : libraryHandles) dlclose(handle);
    libraryHandles.clear();
    blockChain = std::make_unique<BlockChain>();
    selectedBlockComponent = lastCreatedComponent = nullptr;
    startPort = nullptr;
    initialPositions.clear();
    isDragging = isSelecting = isDraggingSelectedComponents = false;
    selectionStartPoint = selectionEndPoint = {};
    currentMode = ApplicationMode::Editing;
    repaint();
}

juce::Result PatchController::restorePatch(const juce::var& state) {
    const auto format = state["format"].toString();
    // Only the historical file identifier is retained for reading existing patches.
    const bool legacyFormat = format == "Max-liked Patch";
    if (state.getDynamicObject() == nullptr || (format != "Lithweft Patch" && !legacyFormat))
        return juce::Result::fail("This is not a Lithweft patch file.");
    if (!integer(state["version"], 1, 1)) return juce::Result::fail("This patch file version is not supported.");
    const auto* nodes = state["nodes"].getArray();
    const auto* connections = state["connections"].getArray();
    if (nodes == nullptr || connections == nullptr || nodes->size() > 10000 || connections->size() > 50000)
        return juce::Result::fail("Invalid node/connection lists, or size limits exceeded.");
    const auto mode = state["mode"].toString();
    if (mode != "editing" && mode != "performance") return juce::Result::fail("Invalid patch mode.");

    // Build and validate a separate graph. No part of the current patch is
    // removed until all nodes, UI state and port references have succeeded.
    PatchController staged;
    {
        const juce::ScopedLock lock(graphLock);
        staged.numSamples = numSamples;
        staged.sampleRate = sampleRate;
    }
    std::map<int, BlockComponent*> components;
    for (const auto& node : *nodes) {
        const auto bounds = node["bounds"];
        if (node.getDynamicObject() == nullptr || !integer(node["id"], 1, 1000000000)
            || !node["expression"].isString() || !node["resolved"].isBool()
            || node["state"].getDynamicObject() == nullptr
            || !integer(bounds["x"], -1000000, 1000000) || !integer(bounds["y"], -1000000, 1000000)
            || !integer(bounds["width"], 1, 10000) || !integer(bounds["height"], 1, 10000))
            return juce::Result::fail("Invalid node data or bounds.");
        const int id = static_cast<int>(node["id"]);
        if (components.count(id) != 0) return juce::Result::fail("Duplicate node ID.");
        const auto expression = node["expression"].toString();
        if (expression.length() > 65536) return juce::Result::fail("Node expression is too long.");
        auto* component = new BlockComponent();
        component->setListener(&staged);
        component->setCreationText(expression);
        component->setBounds(static_cast<int>(bounds["x"]), static_cast<int>(bounds["y"]),
                             static_cast<int>(bounds["width"]), static_cast<int>(bounds["height"]));
        staged.addAndMakeVisible(component);
        if (static_cast<bool>(node["resolved"])) {
            const auto text = expression.trim();
            const auto separator = text.indexOfAnyOf(" \t\r\n");
            const auto name = separator < 0 ? text : text.substring(0, separator);
            const auto args = separator < 0 ? juce::String() : text.substring(separator + 1).trimStart();
            // Validate the module name after resolving operator aliases. In
            // particular, "/" is the division node, rather than a file path.
            const auto moduleName = juce::String(staged.resolveBlockName(name.toStdString()));
            if (moduleName.isEmpty() || moduleName.containsAnyOf("/\\") || moduleName.contains(".."))
                return juce::Result::fail("Invalid node name: " + name);
            staged.requestCreateBlock(component, name.toStdString(), args.toStdString(), false);
            component = staged.lastCreatedComponent;
            if (component == nullptr) return juce::Result::fail("Unable to load node: " + name + ". Check that the matching block library is available.");
            // Preserve the user's alias and exact expression, rather than its resolved module name.
            component->setCreationText(expression);
        }
        if (auto result = component->restorePatchState(node["state"]); result.failed()) return result;
        component->setBounds(static_cast<int>(bounds["x"]), static_cast<int>(bounds["y"]),
                             static_cast<int>(bounds["width"]), static_cast<int>(bounds["height"]));
        component->layoutPorts();
        components[id] = component;
    }
    std::set<std::tuple<int, int, int, int>> seen;
    for (const auto& connection : *connections) {
        if (connection.getDynamicObject() == nullptr || !integer(connection["source"], 1, 1000000000)
            || !integer(connection["destination"], 1, 1000000000)
            || !integer(connection["outlet"], 0, 100000) || !integer(connection["inlet"], 0, 100000))
            return juce::Result::fail("Invalid connection data.");
        const int source = static_cast<int>(connection["source"]), destination = static_cast<int>(connection["destination"]);
        const int outlet = static_cast<int>(connection["outlet"]), inlet = static_cast<int>(connection["inlet"]);
        if (components.count(source) == 0 || components.count(destination) == 0)
            return juce::Result::fail("A connection references a missing node.");
        const auto edge = std::make_tuple(source, outlet, destination, inlet);
        if (!seen.insert(edge).second) return juce::Result::fail("The file contains duplicate connections.");
        auto* sourcePort = components[source]->getPortComponent(outlet, PortComponent::Type::Outlet);
        auto* destinationPort = components[destination]->getPortComponent(inlet, PortComponent::Type::Inlet);
        if (sourcePort == nullptr || destinationPort == nullptr)
            return juce::Result::fail("A connection references a missing inlet or outlet.");
        staged.createConnection(sourcePort, destinationPort);
    }
    staged.currentMode = mode == "performance" ? ApplicationMode::Performance : ApplicationMode::Editing;
    for (auto* component : staged.getAllBlockComponents()) component->switchMode(staged.currentMode);
    {
        const juce::ScopedLock lock(graphLock);
        // Retain the most recent audio configuration, in case it changed while loading.
        staged.blockChain->prepareToProcess(numSamples, sampleRate);
        clearPatch();
        blockChain.swap(staged.blockChain);
        libraryHandles.swap(staged.libraryHandles);
        blockMapping.swap(staged.blockMapping);
        currentMode = staged.currentMode;
        for (auto* component : staged.getAllBlockComponents()) {
            staged.removeChildComponent(component);
            component->setListener(this);
            addAndMakeVisible(component);
        }
        updateConnectionLines();
    }
    repaint();
    return juce::Result::ok();
}

juce::Result PatchController::savePatch(const juce::File& file) {
    if (!file.getParentDirectory().isDirectory())
        return juce::Result::fail("The save directory does not exist.");
    const auto text = juce::JSON::toString(getPatchState(), false);
    juce::TemporaryFile temporary(file);
    if (!temporary.getFile().replaceWithText(text, false, false, "\n"))
        return juce::Result::fail("Unable to write the patch. Check the output location and permissions.");
    if (!temporary.overwriteTargetFileWithTemporary())
        return juce::Result::fail("Unable to replace the destination. The original patch file has been preserved.");
    return juce::Result::ok();
}

juce::Result PatchController::loadPatch(const juce::File& file) {
    if (!file.existsAsFile()) return juce::Result::fail("The patch file does not exist.");
    if (file.getSize() > 32 * 1024 * 1024) return juce::Result::fail("The patch file exceeds 32 MB.");
    juce::var parsed;
    if (auto result = juce::JSON::parse(file.loadFileAsString(), parsed); result.failed())
        return juce::Result::fail("Invalid patch JSON: " + result.getErrorMessage());
    return restorePatch(parsed);
}
