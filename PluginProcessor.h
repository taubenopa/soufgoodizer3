#pragma once
#include <juce_audio_processors/juce_audio_processors.h>
#include <juce_dsp/juce_dsp.h>
#include <atomic>
#include "BandCompressor.h"
#include "Presets.h"

class SoufgoodizerProcessor : public juce::AudioProcessor
{
public:
    SoufgoodizerProcessor();
    ~SoufgoodizerProcessor() override = default;

    void prepareToPlay (double sampleRate, int samplesPerBlock) override;
    void releaseResources() override {}
    bool isBusesLayoutSupported (const BusesLayout& layouts) const override;
    void processBlock (juce::AudioBuffer<float>&, juce::MidiBuffer&) override;

    juce::AudioProcessorEditor* createEditor() override;
    bool hasEditor() const override { return true; }

    const juce::String getName() const override { return "Soufgoodizer"; }
    bool acceptsMidi() const override { return false; }
    bool producesMidi() const override { return false; }
    double getTailLengthSeconds() const override { return 0.0; }

    int getNumPrograms() override { return 1; }
    int getCurrentProgram() override { return 0; }
    void setCurrentProgram (int) override {}
    const juce::String getProgramName (int) override { return {}; }
    void changeProgramName (int, const juce::String&) override {}

    void getStateInformation (juce::MemoryBlock& destData) override;
    void setStateInformation (const void* data, int sizeInBytes) override;

    // Fuer den Editor
    juce::AudioParameterChoice* presetParam = nullptr;
    juce::AudioParameterBool*   activeParam = nullptr;
    std::atomic<float> outputLevel { 0.f };

private:
    void applyPreset (int index);

    static constexpr int numBands = 3;
    static constexpr int maxChannels = 2;

    juce::dsp::LinkwitzRileyFilter<float> split1, split2, align2;
    std::array<souf::BandCompressor, numBands> comps;
    std::array<juce::SmoothedValue<float>, numBands> makeup;
    juce::SmoothedValue<float> outGain, mix;
    juce::dsp::Limiter<float> limiter;
    juce::AudioBuffer<float> dry;

    int currentPreset = -1;
    double sr = 44100.0;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (SoufgoodizerProcessor)
};
