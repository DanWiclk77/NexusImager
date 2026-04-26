#include "PluginProcessor.h"
#include "PluginEditor.h"

NexusImagerAudioProcessorEditor::NexusImagerAudioProcessorEditor (NexusImagerAudioProcessor& p)
    : AudioProcessorEditor (&p), audioProcessor (p)
{
    setSize (800, 500);

    for (int i = 0; i < 4; ++i)
    {
        addAndMakeVisible(bands[i].widthSlider);
        bands[i].widthSlider.setSliderStyle(juce::Slider::RotaryHorizontalVerticalDrag);
        bands[i].widthSlider.setTextBoxStyle(juce::Slider::NoTextBox, false, 0, 0);

        addAndMakeVisible(bands[i].widenSlider);
        bands[i].widenSlider.setSliderStyle(juce::Slider::LinearBarVertical);
    }
}

NexusImagerAudioProcessorEditor::~NexusImagerAudioProcessorEditor() {}

void NexusImagerAudioProcessorEditor::paint (juce::Graphics& g)
{
    // Fondo oscuro profesional estilo Slate Digital / iZotope
    g.fillAll (juce::Colour(0xFF1A1A1A));

    g.setColour (juce::Colours::white);
    g.setFont (24.0f);
    g.drawText ("NEXUS MULTIBAND IMAGER", getLocalBounds().removeFromTop(50), juce::Justification::centred, true);

    auto mainArea = getLocalBounds().reduced(20);
    auto scopeArea = mainArea.removeFromTop(200);
    
    drawVectorscope(g, scopeArea);

    g.setColour(juce::Colours::grey.withAlpha(0.3f));
    g.drawRect(scopeArea, 2);
}

void NexusImagerAudioProcessorEditor::drawVectorscope(juce::Graphics& g, juce::Rectangle<int> area)
{
    g.setColour(juce::Colours::cyan.withAlpha(0.5f));
    // Aquí iría la lógica de dibujo de puntos basada en el AudioBuffer real
    // Pro-Tip: El "Glow" se logra pintando líneas semi-transparentes acumulativas.
}

void NexusImagerAudioProcessorEditor::resised()
{
    auto area = getLocalBounds().reduced(20);
    area.removeFromTop(250); // Espacio para el Vectorscope

    auto bandWidth = area.getWidth() / 4;
    for (int i = 0; i < 4; ++i)
    {
        auto bArea = area.removeFromLeft(bandWidth).reduced(10);
        bands[i].widthSlider.setBounds(bArea.removeFromTop(bArea.getHeight() * 0.6));
        bands[i].widenSlider.setBounds(bArea);
    }
}
