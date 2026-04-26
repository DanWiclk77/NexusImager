#include "PluginProcessor.h"
#include "PluginEditor.h"

NexusImagerAudioProcessorEditor::NexusImagerAudioProcessorEditor (NexusImagerAudioProcessor& p)
    : AudioProcessorEditor (&p), audioProcessor (p)
{
    setSize (800, 600);

    juce::StringArray bandNames = { "LOW", "LOW-MID", "HIGH-MID", "HIGH" };

    for (int i = 0; i < 4; ++i)
    {
        auto prefix = juce::String(i);

        addAndMakeVisible(bands[i].title);
        bands[i].title.setText(bandNames[i], juce::dontSendNotification);
        bands[i].title.setJustificationType(juce::Justification::centred);
        bands[i].title.setFont(juce::Font(14.0f, juce::Font::bold));
        bands[i].title.setColour(juce::Label::textColourId, juce::Colours::cyan);

        // Combo
        addAndMakeVisible(bands[i].modeSelector);
        bands[i].modeSelector.addItemList(juce::StringArray{"Stereo", "Mid", "Side"}, 1);
        bands[i].modeAttachment = std::make_unique<juce::AudioProcessorValueTreeState::ComboBoxAttachment>(audioProcessor.apvts, "mod" + prefix, bands[i].modeSelector);

        // Stereo Enhancer (Width)
        addAndMakeVisible(bands[i].labelWidth);
        bands[i].labelWidth.setText("Enhancer", juce::dontSendNotification);
        bands[i].labelWidth.setFont(juce::Font(10.0f));
        addAndMakeVisible(bands[i].widthSlider);
        bands[i].widthSlider.setSliderStyle(juce::Slider::RotaryHorizontalVerticalDrag);
        bands[i].widthSlider.setTextBoxStyle(juce::Slider::TextBoxBelow, false, 50, 15);
        bands[i].widthSlider.setTextValueSuffix("%");
        bands[i].widthAttachment = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(audioProcessor.apvts, "wid" + prefix, bands[i].widthSlider);

        // Widener
        addAndMakeVisible(bands[i].labelWiden);
        bands[i].labelWiden.setText("Widener", juce::dontSendNotification);
        bands[i].labelWiden.setFont(juce::Font(10.0f));
        addAndMakeVisible(bands[i].widenSlider);
        bands[i].widenSlider.setSliderStyle(juce::Slider::LinearBarVertical);
        bands[i].widenSlider.setTextBoxStyle(juce::Slider::TextBoxBelow, false, 50, 15);
        bands[i].widenSlider.setTextValueSuffix("%");
        bands[i].widenAttachment = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(audioProcessor.apvts, "wdr" + prefix, bands[i].widenSlider);

        // Buttons
        addAndMakeVisible(bands[i].soloButton);
        bands[i].soloButton.setButtonText("S");
        bands[i].soloAttachment = std::make_unique<juce::AudioProcessorValueTreeState::ButtonAttachment>(audioProcessor.apvts, "sol" + prefix, bands[i].soloButton);

        addAndMakeVisible(bands[i].muteButton);
        bands[i].muteButton.setButtonText("M");
        bands[i].muteAttachment = std::make_unique<juce::AudioProcessorValueTreeState::ButtonAttachment>(audioProcessor.apvts, "mut" + prefix, bands[i].muteButton);
    }

    // Freq Sliders
    for (int i = 0; i < 3; ++i)
    {
        addAndMakeVisible(freqSliders[i]);
        freqSliders[i].setSliderStyle(juce::Slider::LinearHorizontal);
        freqSliders[i].setTextBoxStyle(juce::Slider::TextBoxBelow, false, 60, 15);
        freqSliders[i].setTextValueSuffix(" Hz");
        freqAttachments[i] = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(audioProcessor.apvts, "f" + juce::String(i+1), freqSliders[i]);
    }

    startTimerHz(30); // 30 FPS para el scope
}

NexusImagerAudioProcessorEditor::~NexusImagerAudioProcessorEditor() 
{
    stopTimer();
}

void NexusImagerAudioProcessorEditor::timerCallback()
{
    audioProcessor.getVectorscopePoints(scopePoints);
    repaint();
}

void NexusImagerAudioProcessorEditor::paint (juce::Graphics& g)
{
    g.fillAll (juce::Colour(0xFF070707));

    // Logo
    g.setColour (juce::Colours::cyan);
    g.setFont (juce::Font("Orbitron", 24.0f, juce::Font::bold));
    g.drawText ("NEXUS IMAGER", 0, 10, getWidth(), 40, juce::Justification::centred);

    auto area = getLocalBounds().reduced(20);
    auto scopeArea = area.removeFromTop(200);
    
    // Background Scope
    g.setColour(juce::Colours::black);
    g.fillRect(scopeArea);
    g.setColour(juce::Colours::cyan.withAlpha(0.1f));
    g.drawRect(scopeArea, 1);

    // Crosshair
    g.setColour(juce::Colours::white.withAlpha(0.1f));
    g.drawVerticalLine(scopeArea.getCentreX(), (float)scopeArea.getY(), (float)scopeArea.getBottom());
    g.drawHorizontalLine(scopeArea.getCentreY(), (float)scopeArea.getX(), (float)scopeArea.getRight());

    drawVectorscope(g, scopeArea);

    // Footer info
    g.setColour(juce::Colours::grey);
    g.setFont(10.0f);
    g.drawText("NEXUSAUDIO RESEARCH LABS", 0, getHeight() - 20, getWidth(), 20, juce::Justification::centred);
}

void NexusImagerAudioProcessorEditor::drawVectorscope(juce::Graphics& g, juce::Rectangle<int> area)
{
    if (scopePoints.empty()) return;

    juce::Path p;
    float centerX = (float)area.getCentreX();
    float centerY = (float)area.getCentreY();
    float scale = (float)area.getWidth() * 0.4f;

    bool first = true;
    for (const auto& pt : scopePoints)
    {
        // Goniometer rotation: M = (L+R)/2, S = (L-R)/2
        float m = (pt.x + pt.y) * 0.5f;
        float s = (pt.x - pt.y) * 0.5f;

        float x = centerX + s * scale;
        float y = centerY - m * scale;

        if (first) { p.startNewSubPath(x, y); first = false; }
        else p.lineTo(x, y);
    }

    g.setColour(juce::Colours::cyan.withAlpha(0.7f));
    g.strokePath(p, juce::PathStrokeType(1.0f));
    
    // Glow effect
    g.setColour(juce::Colours::cyan.withAlpha(0.2f));
    g.strokePath(p, juce::PathStrokeType(3.0f));
}

void NexusImagerAudioProcessorEditor::resized()
{
    auto area = getLocalBounds().reduced(20);
    area.removeFromTop(220); // Scope space

    // Freq sliders area
    auto fArea = area.removeFromTop(60);
    int fWidth = fArea.getWidth() / 3;
    for (int i = 0; i < 3; ++i)
        freqSliders[i].setBounds(fArea.removeFromLeft(fWidth).reduced(10, 5));

    area.removeFromTop(10); // padding

    // Bands area
    auto bandWidth = area.getWidth() / 4;
    for (int i = 0; i < 4; ++i)
    {
        auto bArea = area.removeFromLeft(bandWidth).reduced(5);
        
        bands[i].title.setBounds(bArea.removeFromTop(20));
        bands[i].modeSelector.setBounds(bArea.removeFromTop(25).reduced(10, 2));
        
        auto controls = bArea.removeFromTop(160);
        auto leftPart = controls.removeFromLeft(controls.getWidth() * 0.6);
        
        bands[i].labelWidth.setBounds(leftPart.removeFromTop(15));
        bands[i].widthSlider.setBounds(leftPart.removeFromTop(80));
        
        auto buttons = leftPart.removeFromTop(30).reduced(5, 2);
        bands[i].soloButton.setBounds(buttons.removeFromLeft(buttons.getWidth() * 0.5));
        bands[i].muteButton.setBounds(buttons);

        bands[i].labelWiden.setBounds(controls.removeFromTop(15));
        bands[i].widenSlider.setBounds(controls.removeFromTop(100));
    }
}
