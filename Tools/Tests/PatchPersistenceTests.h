#pragma once
#include "../../Lithweft/Source/PatchDocument.h"
#include <stdexcept>

namespace PatchPersistenceTests {
inline int run(const juce::File& suppliedPatch = {}) {
    int checks = 0;
    const auto check = [&](bool condition, const juce::String& message) {
        ++checks;
        if (!condition) throw std::runtime_error(message.toStdString());
    };
    const auto directory = juce::File::getSpecialLocation(juce::File::tempDirectory)
        .getChildFile("Lithweft-patch-tests-" + juce::Uuid().toString());
    directory.createDirectory();
    struct Cleanup { juce::File directory; ~Cleanup() { directory.deleteRecursively(); } } cleanup { directory };
    try {
        auto fixture = juce::JSON::parse(juce::String::fromUTF8(R"JSON({
            "format": "Lithweft Patch", "version": 1, "mode": "editing",
            "nodes": [
                {"id":1,"expression":"number","resolved":true,"bounds":{"x":45,"y":50,"width":60,"height":30},"state":{"value":37.25}},
                {"id":2,"expression":"+ 2","resolved":true,"bounds":{"x":95,"y":160,"width":86,"height":28},"state":{}},
                {"id":3,"expression":"slider","resolved":true,"bounds":{"x":180,"y":40,"width":40,"height":120},"state":{"value":72.4}},
                {"id":4,"expression":"box","resolved":true,"bounds":{"x":290,"y":50,"width":230,"height":48},"state":{"text":"read \"/tmp/a b.wav\", 中文\nsecond line"}},
                {"id":5,"expression":"trigger","resolved":true,"bounds":{"x":300,"y":170,"width":30,"height":30},"state":{}},
                {"id":6,"expression":"unfinished node","resolved":false,"bounds":{"x":450,"y":200,"width":120,"height":28},"state":{}},
                {"id":7,"expression":"scope~","resolved":true,"bounds":{"x":90,"y":360,"width":120,"height":120},"state":{}},
                {"id":8,"expression":"print","resolved":true,"bounds":{"x":95,"y":250,"width":80,"height":28},"state":{}},
                {"id":9,"expression":"osc~ 440","resolved":true,"bounds":{"x":65,"y":300,"width":95,"height":28},"state":{}}
            ],
            "connections": [
                {"source":1,"outlet":0,"destination":2,"inlet":0},
                {"source":3,"outlet":0,"destination":2,"inlet":1},
                {"source":2,"outlet":0,"destination":8,"inlet":0},
                {"source":9,"outlet":0,"destination":7,"inlet":0}
            ]
        })JSON"));
        PatchController patch;
        check(patch.restorePatch(fixture).wasOk(), "restore mixed UI and dynamic nodes");
        auto original = juce::JSON::toString(patch.getPatchState(), true);
        auto state = patch.getPatchState();
        check(state["nodes"].size() == 9 && state["connections"].size() == 4, "all nodes and connections restored");
        check(state["nodes"][1]["expression"].toString() == "+ 2", "alias and creation argument preserved");
        check(static_cast<double>(state["nodes"][0]["state"]["value"]) == 37.25, "number value preserved");
        check(std::abs(static_cast<double>(state["nodes"][2]["state"]["value"]) - 72.4) < 0.001, "slider value preserved");
        check(state["nodes"][3]["state"]["text"].toString().contains(juce::String::fromUTF8("中文\nsecond line")), "quoted multiline message preserved");
        check(static_cast<int>(state["nodes"][6]["bounds"]["x"]) == 90, "scope position restored without centering drift");
        check(!static_cast<bool>(state["nodes"][5]["resolved"]), "unfinished node remains editable");
        const auto file = directory.getChildFile("roundtrip.lithweft");
        check(patch.savePatch(file).wasOk(), "save to disk");
        patch.clearPatch();
        check(patch.getPatchState()["nodes"].size() == 0, "new patch clears graph");
        check(patch.loadPatch(file).wasOk(), "open saved file");
        check(juce::JSON::toString(patch.getPatchState(), true) == original, "complete save/open roundtrip");

        auto legacy = juce::JSON::parse(original);
        legacy.getDynamicObject()->setProperty("format", "Max-liked Patch");
        const auto legacyFile = directory.getChildFile("legacy.maxpatch");
        const auto legacyText = juce::JSON::toString(legacy, true);
        check(legacyFile.replaceWithText(legacyText), "create historical patch fixture");
        PatchController legacyPatch;
        check(legacyPatch.loadPatch(legacyFile).wasOk(), "historical patch opens");
        check(juce::JSON::toString(legacyPatch.getPatchState(), true) == original, "historical patch retains graph under new format");
        const auto migratedFile = directory.getChildFile("migrated.lithweft");
        check(legacyPatch.savePatch(migratedFile).wasOk(), "historical patch saves under new name");
        check(juce::JSON::parse(migratedFile)["format"].toString() == "Lithweft Patch", "saved file uses Lithweft identifier");
        check(legacyFile.loadFileAsString() == legacyText, "migration leaves historical file unchanged");

        for (const auto& alias : juce::StringArray { "+", "-", "*", "/", "*~" }) {
            PatchController aliasPatch;
            auto aliasState = juce::JSON::parse(original);
            const auto expression = alias + " 100";
            aliasState["nodes"][1].getDynamicObject()->setProperty("expression", expression);
            check(aliasPatch.restorePatch(aliasState).wasOk(), "operator alias restores: " + alias);
            const auto aliasFile = directory.getChildFile("alias.lithweft");
            check(aliasPatch.savePatch(aliasFile).wasOk() && aliasPatch.loadPatch(aliasFile).wasOk(), "operator alias save/open: " + alias);
            check(aliasPatch.getPatchState()["nodes"][1]["expression"].toString() == expression, "operator expression preserved: " + alias);
        }

        const auto reject = [&](juce::var malformed, const juce::String& reason) {
            check(patch.restorePatch(malformed).failed(), reason);
            check(juce::JSON::toString(patch.getPatchState(), true) == original, "failed load preserves current patch: " + reason);
        };
        auto modified = juce::JSON::parse(original);
        modified.getDynamicObject()->setProperty("version", 99);
        reject(modified, "future version rejected");
        for (const auto& path : juce::StringArray { "../divide", "/tmp/divide", "folder/divide", "folder\\divide" }) {
            modified = juce::JSON::parse(original);
            modified["nodes"][1].getDynamicObject()->setProperty("expression", path);
            reject(modified, "module paths remain rejected: " + path);
        }
        modified = juce::JSON::parse(original);
        modified["nodes"][1].getDynamicObject()->setProperty("id", 1);
        reject(modified, "duplicate node ID rejected");
        modified = juce::JSON::parse(original);
        modified["connections"][0].getDynamicObject()->setProperty("inlet", 999);
        reject(modified, "invalid port rejected");
        modified = juce::JSON::parse(original);
        modified["connections"][0].getDynamicObject()->setProperty("destination", 999);
        reject(modified, "missing node rejected");
        modified = juce::JSON::parse(original);
        modified["nodes"][1].getDynamicObject()->setProperty("expression", "missing_module_for_test");
        reject(modified, "missing dynamic module rejected");
        modified = juce::JSON::parse(original);
        modified["nodes"][0]["state"].getDynamicObject()->setProperty("value", "bad");
        reject(modified, "invalid number state rejected");
        const auto broken = directory.getChildFile("broken.lithweft");
        broken.replaceWithText("{broken json");
        check(patch.loadPatch(broken).failed(), "invalid JSON rejected");
        check(juce::JSON::toString(patch.getPatchState(), true) == original, "malformed JSON preserves current patch");
        check(patch.savePatch(directory.getChildFile("missing/sub/test.lithweft")).failed(), "failed write reported");
        check(juce::JSON::toString(juce::JSON::parse(file), true) == original, "failed save preserves prior disk file");

        PatchDocument document(patch);
        check(!document.hasChangedSinceSaved(), "initial document clean");
        check(document.title().endsWith("Lithweft"), "window title uses product name");
        auto* node = patch.getAllBlockComponents()[0];
        node->setTopLeftPosition(120, 180);
        document.checkForChanges();
        check(document.hasChangedSinceSaved(), "movement marks document dirty");
        bool saved = false;
        document.saveAsAsync(file, false, false, false, [&](juce::FileBasedDocument::SaveResult result) { saved = result == juce::FileBasedDocument::savedOk; });
        check(saved, "document save completed");
        document.checkForChanges();
        check(!document.hasChangedSinceSaved(), "successful save clears dirty flag");
        document.reset();
        check(!document.hasChangedSinceSaved() && patch.getPatchState()["nodes"].size() == 0, "new document empty and clean");
        bool loaded = false;
        document.loadFromAsync(file, false, [&](juce::Result result) { loaded = result.wasOk(); });
        check(loaded && !document.hasChangedSinceSaved(), "document opens saved patch cleanly");
        // Exercise ownership cleanup over repeated loads and UI node deletion.
        for (int i = 0; i < 5; ++i) check(patch.loadPatch(file).wasOk(), "repeat load and cleanup");
        patch.requestDeleteBlock(patch.getAllBlockComponents()[0]);
        check(patch.getPatchState()["nodes"].size() == 8 && patch.getPatchState()["connections"].size() == 3, "UI node deletion removes attached connection");
        if (suppliedPatch != juce::File()) {
            const auto before = suppliedPatch.loadFileAsString();
            PatchController supplied;
            const auto result = supplied.loadPatch(suppliedPatch);
            check(result.wasOk(), "supplied patch opens: " + result.getErrorMessage());
            auto expected = juce::JSON::parse(before);
            expected.getDynamicObject()->setProperty("format", "Lithweft Patch");
            check(juce::JSON::toString(supplied.getPatchState(), true) == juce::JSON::toString(expected, true), "supplied patch restores every saved property");
            check(suppliedPatch.loadFileAsString() == before, "supplied patch file unchanged");
            std::cout << "Supplied patch verified: " << supplied.getPatchState()["nodes"].size()
                      << " nodes / " << supplied.getPatchState()["connections"].size() << " connections" << std::endl;
        }
        std::cout << "PASS: " << checks << " patch persistence checks" << std::endl;
        return 0;
    } catch (const std::exception& error) {
        std::cerr << "FAIL: " << error.what() << std::endl;
        return 1;
    }
}
}
