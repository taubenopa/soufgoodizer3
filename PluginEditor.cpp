#include "PluginEditor.h"
#include "BinaryData.h"

SoufgoodizerEditor::SoufgoodizerEditor (SoufgoodizerProcessor& p)
    : AudioProcessorEditor (&p), proc (p)
{
    head = juce::ImageCache::getFromMemory (BinaryData::head_png, BinaryData::head_pngSize);
    setSize (280, 280);
    setResizable (false, false);
    setMouseCursor (juce::MouseCursor::PointingHandCursor);
    startTimerHz (30);
}

void SoufgoodizerEditor::setPreset (int index)
{
    index = (index % souf::numPresets + souf::numPresets) % souf::numPresets;
    auto* prm = proc.presetParam;
    prm->beginChangeGesture();
    prm->setValueNotifyingHost (prm->convertTo0to1 ((float) index));
    prm->endChangeGesture();
    labelAlpha = 1.f;
}

void SoufgoodizerEditor::mouseDown (const juce::MouseEvent& e)
{
    if (e.mods.isPopupMenu())
    {
        auto* prm = proc.activeParam;
        prm->beginChangeGesture();
        prm->setValueNotifyingHost (prm->get() ? 0.f : 1.f);
        prm->endChangeGesture();
        labelAlpha = 1.f;
        return;
    }

    const int cur = proc.presetParam->getIndex();
    setPreset (e.mods.isShiftDown() ? cur - 1 : cur + 1);
}

void SoufgoodizerEditor::timerCallback()
{
    const float target = proc.outputLevel.exchange (0.f);
    level += (juce::jmin (target, 1.f) - level) * 0.35f;
    hoverAmount += ((hover ? 1.f : 0.f) - hoverAmount) * 0.25f;
    labelAlpha = juce::jmax (0.f, labelAlpha - 0.03f);
    repaint();
}

void SoufgoodizerEditor::paint (juce::Graphics& g)
{
    const bool active = proc.activeParam->get();
    const auto& preset = souf::presets[(size_t) proc.presetParam->getIndex()];
    const auto glow = juce::Colour ((juce::uint32) preset.glowColour);

    auto area = getLocalBounds().toFloat();
    const auto centre = area.getCentre();

    g.setGradientFill (juce::ColourGradient (juce::Colour (0xff23232b), centre.x, 0.f,
                                             juce::Colour (0xff111116), centre.x, area.getHeight(), false));
    g.fillAll();

    const float base  = juce::jmin (area.getWidth(), area.getHeight()) * 0.66f;
    const float scale = 1.f + 0.07f * level + 0.04f * hoverAmount;
    const float size  = base * scale;

    // Glow in der Farbe des aktuellen Presets
    if (active)
    {
        const float r = size * 0.95f;
        g.setGradientFill (juce::ColourGradient (glow.withAlpha (0.45f + 0.3f * level), centre.x, centre.y,
                                                 glow.withAlpha (0.f), centre.x + r, centre.y, true));
        g.fillEllipse (centre.x - r, centre.y - r, r * 2.f, r * 2.f);
    }

    g.setImageResamplingQuality (juce::Graphics::highResamplingQuality);
    g.setOpacity (active ? 1.f : 0.35f);
    g.drawImage (head, juce::Rectangle<float> (size, size).withCentre (centre), juce::RectanglePlacement::centred);

    // Preset-Name blendet kurz nach einem Klick ein und wieder aus
    if (labelAlpha > 0.f)
    {
        g.setColour (juce::Colours::white.withAlpha (labelAlpha));
        g.setFont (juce::FontOptions (16.f, juce::Font::bold));
        g.drawText (active ? juce::String (preset.name) : juce::String ("OFF"),
                    getLocalBounds().removeFromBottom (36), juce::Justification::centred);
    }
}
