#include "PluginProcessor.h"
#include "PluginEditor.h"

NexusImagerAudioProcessorEditor::NexusImagerAudioProcessorEditor (NexusImagerAudioProcessor& p)
    : AudioProcessorEditor (&p), audioProcessor (p)
{
    setSize (800, 500);

    for (int i = 0; i < 4; ++i)
    {
        auto prefix = juce::String(i);

        // Configuración de Sliders
        addAndMakeVisible(bands[i].widthSlider);
        bands[i].widthSlider.setSliderStyle(juce::Slider::RotaryHorizontalVerticalDrag);
        bands[i].widthSlider.setTextBoxStyle(juce::Slider::NoTextBox, false, 0, 0);
        bands[i].widthAttachment = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(audioProcessor.apvts, "width" + prefix, bands[i].widthSlider);

        addAndMakeVisible(bands[i].widenSlider);
        bands[i].widenSlider.setSliderStyle(juce::Slider::LinearBarVertical);
        bands[i].widenAttachment = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(audioProcessor.apvts, "widener" + prefix, bands[i].widenSlider);

        // Configuración de Botones
        addAndMakeVisible(bands[i].soloButton);
        bands[i].soloButton.setButtonText("S");
        bands[i].soloAttachment = std::make_unique<juce::AudioProcessorValueTreeState::ButtonAttachment>(audioProcessor.apvts, "solo" + prefix, bands[i].soloButton);

        addAndMakeVisible(bands[i].muteButton);
        bands[i].muteButton.setButtonText("M");
        bands[i].muteAttachment = std::make_unique<juce::AudioProcessorValueTreeState::ButtonAttachment>(audioProcessor.apvts, "mute" + prefix, bands[i].muteButton);

        // Selector de Modo
        addAndMakeVisible(bands[i].modeSelector);
        bands[i].modeSelector.addItemList(juce::StringArray{"Stereo", "Mid", "Side"}, 1);
        bands[i].modeAttachment = std::make_unique<juce::AudioProcessorValueTreeState::ComboBoxAttachment>(audioProcessor.apvts, "mode" + prefix, bands[i].modeSelector);
    }
}

NexusImagerAudioProcessorEditor::~NexusImagerAudioProcessorEditor() {}

void NexusImagerAudioProcessorEditor::paint (juce::Graphics& g)
{
    // Dark Carbon Theme
    g.fillAll (juce::Colour(0xFF0D0D0D));

    // Logo / Header
    g.setColour (juce::Colours::cyan);
    g.setFont (juce::Font("Orbitron", 22.0f, juce::Font::bold));
    g.drawText ("NEXUS IMAGER", getLocalBounds().removeFromTop(40), juce::Justification::centred, true);

    // Reference Grid for Vectorscope
    auto area = getLocalBounds().reduced(20);
    auto scopeArea = area.removeFromTop(200);
    
    g.setColour(juce::Colours::grey.withAlpha(0.1f));
    for (int i = 0; i < 10; ++i)
    {
        float x = (float)scopeArea.getX() + (float)scopeArea.getWidth() * (float)i / 10.0f;
        g.drawVerticalLine((int)x, (float)scopeArea.getY(), (float)scopeArea.getBottom());
    }

    drawVectorscope(g, scopeArea);

    g.setColour(juce::Colours::cyan.withAlpha(0.3f));
    g.drawRect(scopeArea, 1);
}

void NexusImagerAudioProcessorEditor::drawVectorscope(juce::Graphics& g, juce::Rectangle<int> area)
{
    g.setColour(juce::Colours::cyan.withAlpha(0.6f));
    // Simulación de puntos Glow
    // p = pivot + Point<float> (radius * cosf(angle), radius * sinf(angle))
}

void NexusImagerAudioProcessorEditor::resized()
{
    auto area = getLocalBounds().reduced(20);
    area.removeFromTop(220); // Espacio para el Vectorscope

    auto bandWidth = area.getWidth() / 4;
    for (int i = 0; i < 4; ++i)
    {
        auto bArea = area.removeFromLeft(bandWidth).reduced(10);
        
        auto modeArea = bArea.removeFromTop(20);
        bands[i].modeSelector.setBounds(modeArea);

        auto sliderArea = bArea.removeFromTop(bArea.getHeight() * 0.7);
        bands[i].widthSlider.setBounds(sliderArea.removeFromTop(sliderArea.getHeight() * 0.6));
        bands[i].widenSlider.setBounds(sliderArea);
        
        auto buttonArea = bArea;
        bands[i].soloButton.setBounds(buttonArea.removeFromLeft(buttonArea.getWidth() * 0.5));
        bands[i].muteButton.setBounds(buttonArea);
    }
}
