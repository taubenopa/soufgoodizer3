#include "PluginProcessor.h"
#include "PluginEditor.h"

SoufgoodizerProcessor::SoufgoodizerProcessor()
    : AudioProcessor (BusesProperties()
                          .withInput  ("Input",  juce::AudioChannelSet::stereo(), true)
                          .withOutput ("Output", juce::AudioChannelSet::stereo(), true))
{
    juce::StringArray names;
    for (auto& p : souf::presets)
        names.add (p.name);

    addParameter (presetParam = new juce::AudioParameterChoice (juce::ParameterID { "preset", 1 }, "Preset", names, 0));
    addParameter (activeParam = new juce::AudioParameterBool   (juce::ParameterID { "active", 1 }, "Active", true));
}

bool SoufgoodizerProcessor::isBusesLayoutSupported (const BusesLayout& l) const
{
    const auto out = l.getMainOutputChannelSet();
    return (out == juce::AudioChannelSet::mono() || out == juce::AudioChannelSet::stereo())
           && out == l.getMainInputChannelSet();
}

void SoufgoodizerProcessor::prepareToPlay (double sampleRate, int samplesPerBlock)
{
    sr = sampleRate;

    juce::dsp::ProcessSpec spec { sampleRate, (juce::uint32) samplesPerBlock, (juce::uint32) maxChannels };
    split1.prepare (spec);
    split2.prepare (spec);
    align2.prepare (spec);
    limiter.prepare (spec);
    limiter.setThreshold (-0.3f);
    limiter.setRelease (60.f);

    for (auto& c : comps) c.prepare (sampleRate);

    dry.setSize (maxChannels, juce::jmax (samplesPerBlock, 512));

    currentPreset = -1;
    applyPreset (presetParam->getIndex());

    for (auto& m : makeup) m.setCurrentAndTargetValue (m.getTargetValue());
    outGain.setCurrentAndTargetValue (outGain.getTargetValue());
    mix.reset (sampleRate, 0.02);
    mix.setCurrentAndTargetValue (activeParam->get() ? 1.f : 0.f);

    split1.reset(); split2.reset(); align2.reset(); limiter.reset();
}

void SoufgoodizerProcessor::applyPreset (int index)
{
    index = juce::jlimit (0, souf::numPresets - 1, index);
    const auto& p = souf::presets[(size_t) index];

    split1.setCutoffFrequency (p.crossover1Hz);
    split2.setCutoffFrequency (p.crossover2Hz);
    align2.setCutoffFrequency (p.crossover2Hz);

    for (int b = 0; b < numBands; ++b)
    {
        const auto& s = p.bands[(size_t) b];
        comps[(size_t) b].setParameters (s.thresholdDb, s.ratio, s.attackMs, s.releaseMs);

        auto& m = makeup[(size_t) b];
        m.reset (sr, 0.05);
        m.setTargetValue (juce::Decibels::decibelsToGain (s.makeupDb));
        if (currentPreset < 0) m.setCurrentAndTargetValue (m.getTargetValue());
    }

    outGain.reset (sr, 0.05);
    outGain.setTargetValue (juce::Decibels::decibelsToGain (p.outputDb));
    if (currentPreset < 0) outGain.setCurrentAndTargetValue (outGain.getTargetValue());

    currentPreset = index;
}

void SoufgoodizerProcessor::processBlock (juce::AudioBuffer<float>& buffer, juce::MidiBuffer&)
{
    juce::ScopedNoDenormals noDenormals;

    const int numCh = juce::jmin (buffer.getNumChannels(), maxChannels);
    const int numSamples = buffer.getNumSamples();

    for (int ch = numCh; ch < buffer.getNumChannels(); ++ch)
        buffer.clear (ch, 0, numSamples);

    if (presetParam->getIndex() != currentPreset)
        applyPreset (presetParam->getIndex());

    mix.setTargetValue (activeParam->get() ? 1.f : 0.f);

    if (dry.getNumSamples() < numSamples)
        dry.setSize (maxChannels, numSamples, false, false, true);
    for (int ch = 0; ch < numCh; ++ch)
        dry.copyFrom (ch, 0, buffer, ch, 0, numSamples);

    // ---- 3-Band-Kompression (Linkwitz-Riley 4. Ordnung) ----
    for (int i = 0; i < numSamples; ++i)
    {
        float low[maxChannels] {}, mid[maxChannels] {}, high[maxChannels] {};

        for (int ch = 0; ch < numCh; ++ch)
        {
            const float in = buffer.getSample (ch, i);
            float lo, rest, m, h, apLo, apHi;

            split1.processSample (ch, in, lo, rest);
            split2.processSample (ch, rest, m, h);
            align2.processSample (ch, lo, apLo, apHi); // Phasen-Angleichung des Low-Bands
            low[ch]  = apLo + apHi;
            mid[ch]  = m;
            high[ch] = h;
        }

        float* bands[numBands] = { low, mid, high };
        float gains[numBands];

        for (int b = 0; b < numBands; ++b)
        {
            float peak = 0.f;
            for (int ch = 0; ch < numCh; ++ch)
                peak = juce::jmax (peak, std::abs (bands[b][ch]));
            gains[b] = comps[(size_t) b].getGain (peak) * makeup[(size_t) b].getNextValue();
        }

        const float og = outGain.getNextValue();

        for (int ch = 0; ch < numCh; ++ch)
        {
            const float sum = low[ch] * gains[0] + mid[ch] * gains[1] + high[ch] * gains[2];
            buffer.setSample (ch, i, sum * og);
        }
    }

    // ---- Sicherheits-Limiter ----
    {
        juce::dsp::AudioBlock<float> block (buffer.getArrayOfWritePointers(), (size_t) numCh, (size_t) numSamples);
        juce::dsp::ProcessContextReplacing<float> ctx (block);
        limiter.process (ctx);
    }

    // ---- Ein/Aus mit kurzem Crossfade (kein Knacken) ----
    for (int i = 0; i < numSamples; ++i)
    {
        const float m = mix.getNextValue();
        for (int ch = 0; ch < numCh; ++ch)
        {
            const float d = dry.getSample (ch, i);
            const float w = buffer.getSample (ch, i);
            buffer.setSample (ch, i, d + m * (w - d));
        }
    }

    const float mag = buffer.getMagnitude (0, numSamples);
    outputLevel.store (juce::jmax (mag, outputLevel.load() * 0.9f));
}

juce::AudioProcessorEditor* SoufgoodizerProcessor::createEditor()
{
    return new SoufgoodizerEditor (*this);
}

void SoufgoodizerProcessor::getStateInformation (juce::MemoryBlock& destData)
{
    juce::MemoryOutputStream s (destData, false);
    s.writeInt (presetParam->getIndex());
    s.writeBool (activeParam->get());
}

void SoufgoodizerProcessor::setStateInformation (const void* data, int size)
{
    juce::MemoryInputStream s (data, (size_t) size, false);
    if (size >= 5)
    {
        const int idx = juce::jlimit (0, souf::numPresets - 1, s.readInt());
        const bool on = s.readBool();
        presetParam->setValueNotifyingHost (presetParam->convertTo0to1 ((float) idx));
        activeParam->setValueNotifyingHost (on ? 1.f : 0.f);
    }
}

juce::AudioProcessor* JUCE_CALLTYPE createPluginFilter()
{
    return new SoufgoodizerProcessor();
}
