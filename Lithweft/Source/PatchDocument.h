#pragma once
#include "PatchController.h"

class PatchDocument : public juce::FileBasedDocument {
public:
    explicit PatchDocument(PatchController& controller)
        : juce::FileBasedDocument(".lithweft", "*.lithweft;*.maxpatch", "Open Lithweft Patch", "Save Lithweft Patch"), patch(controller) {
        rememberSnapshot();
    }
    void checkForChanges() {
        setChangedFlag(snapshot() != savedSnapshot);
    }
    void reset() {
        patch.clearPatch();
        setFile({});
        rememberSnapshot();
        setChangedFlag(false);
    }
    juce::String title() const {
        return (getFile() == juce::File() ? juce::String("Untitled") : getFile().getFileNameWithoutExtension())
            + (hasChangedSinceSaved() ? " *" : "") + juce::String::fromUTF8(" — Lithweft");
    }
private:
    PatchController& patch;
    juce::String savedSnapshot;
    juce::File lastFile = juce::File::getSpecialLocation(juce::File::userDocumentsDirectory);
    juce::String snapshot() { return juce::JSON::toString(patch.getPatchState(), true); }
    void rememberSnapshot() { savedSnapshot = snapshot(); }
    juce::String getDocumentTitle() override { return getFile() == juce::File() ? "Untitled" : getFile().getFileNameWithoutExtension(); }
    juce::Result loadDocument(const juce::File& file) override {
        auto result = patch.loadPatch(file);
        if (result.wasOk()) rememberSnapshot();
        return result;
    }
    juce::Result saveDocument(const juce::File& file) override {
        if (patch.isShowing()) patch.grabKeyboardFocus(); // Commit an active node edit before saving.
        auto result = patch.savePatch(file);
        if (result.wasOk()) rememberSnapshot();
        return result;
    }
    juce::File getLastDocumentOpened() override { return lastFile; }
    void setLastDocumentOpened(const juce::File& file) override { lastFile = file; }
};
