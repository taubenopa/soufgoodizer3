#pragma once
#include <juce_audio_basics/juce_audio_basics.h>
#include <cmath>

namespace souf
{
// Einfacher Kompressor mit Soft-Knee. Die Erkennung (Detector) bekommt
// den Peak aller Kanaele, damit das Stereobild stabil bleibt (Stereo-Link).
class BandCompressor
{
public:
    void prepare (double sr) { sampleRate = sr; reset(); setParameters (-18.f, 2.f, 10.f, 100.f); }

    void reset() { grEnvDb = 0.f; }

    void setParameters (float thresholdDbIn, float ratioIn, float attackMs, float releaseMs)
    {
        thresholdDb = thresholdDbIn;
        ratio       = ratioIn;
        attackCoef  = std::exp (-1.f / (float) (0.001 * attackMs  * sampleRate));
        releaseCoef = std::exp (-1.f / (float) (0.001 * releaseMs * sampleRate));
    }

    // peak: groesster Betrag ueber alle Kanaele; Rueckgabe: linearer Gain
    float getGain (float peak) noexcept
    {
        const float levelDb = 20.f * std::log10 (peak + 1.0e-9f);
        const float over    = levelDb - thresholdDb;

        float grDb = 0.f;
        if (2.f * over > -kneeDb)
        {
            if (2.f * std::abs (over) <= kneeDb)
            {
                const float x = over + kneeDb * 0.5f;
                grDb = (1.f - 1.f / ratio) * x * x / (2.f * kneeDb);
            }
            else
                grDb = (1.f - 1.f / ratio) * over;
        }

        const float coef = grDb > grEnvDb ? attackCoef : releaseCoef;
        grEnvDb = coef * grEnvDb + (1.f - coef) * grDb;

        return juce::Decibels::decibelsToGain (-grEnvDb);
    }

private:
    static constexpr float kneeDb = 6.f;
    double sampleRate = 44100.0;
    float thresholdDb = -18.f, ratio = 2.f;
    float attackCoef = 0.f, releaseCoef = 0.f;
    float grEnvDb = 0.f;
};
} // namespace souf
