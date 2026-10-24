#pragma once
#include "PluginProcessor.h"

// Das ganze Plugin-Fenster ist EIN Button: dein Kopf.
//   Linksklick           -> naechstes Preset (A -> B -> ... -> F -> A)
//   Shift + Linksklick   -> vorheriges Preset
//   Rechtsklick          -> Plugin ein/aus
class SoufgoodizerEditor : public juce::AudioProcessorEditor, private juce::Timer
{
public:
    explicit SoufgoodizerEditor (SoufgoodizerProcessor&);
    ~SoufgoodizerEditor() override = default;

    void paint (juce::Graphics&) override;
    void mouseDown (const juce::MouseEvent&) override;
    void mouseEnter (const juce::MouseEvent&) override { hover = true; }
    void mouseExit  (const juce::MouseEvent&) override { hover = false; }

private:
    void timerCallback() override;
    void setPreset (int index);

    SoufgoodizerProcessor& proc;
    juce::Image head;
    float level = 0.f, hoverAmount = 0.f, labelAlpha = 0.f;
    bool hover = false;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (SoufgoodizerEditor)
};
