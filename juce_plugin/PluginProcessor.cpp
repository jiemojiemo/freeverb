#include "PluginProcessor.h"
#include <algorithm>
//==============================================================================
AudioPluginAudioProcessor::AudioPluginAudioProcessor()
    : AudioProcessor(
          BusesProperties()
#if !JucePlugin_IsMidiEffect
#if !JucePlugin_IsSynth
              .withInput("Input", juce::AudioChannelSet::stereo(), true)
#endif
              .withOutput("Output", juce::AudioChannelSet::stereo(), true)
#endif
      ) {
  juce::StringArray choices = {"Normal", "Freeze"};
  addParameter(mode =
                   new juce::AudioParameterChoice("mode", "mode", choices, 0));

  addParameter(roomsize = new juce::AudioParameterFloat("roomsize", "roomsize",
                                                        0, 1, 0.5));
  addParameter(damp = new juce::AudioParameterFloat("damp", "damp", 0, 1, 0.5));
  addParameter(wet = new juce::AudioParameterFloat("wet", "wet", 0, 1, 0.5));
  addParameter(dry = new juce::AudioParameterFloat("dry", "dry", 0, 1, 0.5));
  addParameter(width =
                   new juce::AudioParameterFloat("width", "width", 0, 1, 0.5));
}

AudioPluginAudioProcessor::~AudioPluginAudioProcessor() {}

//==============================================================================
const juce::String AudioPluginAudioProcessor::getName() const {
  return JucePlugin_Name;
}

bool AudioPluginAudioProcessor::acceptsMidi() const {
#if JucePlugin_WantsMidiInput
  return true;
#else
  return false;
#endif
}

bool AudioPluginAudioProcessor::producesMidi() const {
#if JucePlugin_ProducesMidiOutput
  return true;
#else
  return false;
#endif
}

bool AudioPluginAudioProcessor::isMidiEffect() const {
#if JucePlugin_IsMidiEffect
  return true;
#else
  return false;
#endif
}

double AudioPluginAudioProcessor::getTailLengthSeconds() const { return 0.0; }

int AudioPluginAudioProcessor::getNumPrograms() {
  return 1; // NB: some hosts don't cope very well if you tell them there are 0
            // programs, so this should be at least 1, even if you're not really
            // implementing programs.
}

int AudioPluginAudioProcessor::getCurrentProgram() { return 0; }

void AudioPluginAudioProcessor::setCurrentProgram(int index) {
  juce::ignoreUnused(index);
}

const juce::String AudioPluginAudioProcessor::getProgramName(int index) {
  juce::ignoreUnused(index);
  return {};
}

void AudioPluginAudioProcessor::changeProgramName(int index,
                                                  const juce::String &newName) {
  juce::ignoreUnused(index, newName);
}

//==============================================================================
void AudioPluginAudioProcessor::prepareToPlay(double sampleRate,
                                              int samplesPerBlock) {}

void AudioPluginAudioProcessor::releaseResources() {
  // When playback stops, you can use this as an opportunity to free up any
  // spare memory, etc.
}

bool AudioPluginAudioProcessor::isBusesLayoutSupported(
    const BusesLayout &layouts) const {
#if JucePlugin_IsMidiEffect
  juce::ignoreUnused(layouts);
  return true;
#else
  // This is the place where you check if the layout is supported.
  // In this template code we only support mono or stereo.
  // Some plugin hosts, such as certain GarageBand versions, will only
  // load plugins that support stereo bus layouts.
  if (layouts.getMainOutputChannelSet() != juce::AudioChannelSet::mono() &&
      layouts.getMainOutputChannelSet() != juce::AudioChannelSet::stereo())
    return false;

    // This checks if the input layout matches the output layout
#if !JucePlugin_IsSynth
  if (layouts.getMainOutputChannelSet() != layouts.getMainInputChannelSet())
    return false;
#endif

  return true;
#endif
}

void AudioPluginAudioProcessor::processBlock(juce::AudioBuffer<float> &buffer,
                                             juce::MidiBuffer &midiMessages) {
  juce::ignoreUnused(midiMessages);

  juce::ScopedNoDenormals noDenormals;
  auto totalNumInputChannels = getTotalNumInputChannels();
  auto totalNumOutputChannels = getTotalNumOutputChannels();

  float mode_val = float(mode->getIndex());
  model.setmode(mode_val);
  model.setdamp(*damp);
  model.setdry(*dry);
  model.setroomsize(*roomsize);
  model.setwet(*wet);
  model.setwidth(*width);

  // In case we have more outputs than inputs, this code clears any output
  // channels that didn't contain input data, (because these aren't
  // guaranteed to be empty - they may contain garbage).
  // This is here to avoid people getting screaming feedback
  // when they first compile a plugin, but obviously you don't need to keep
  // this code if your algorithm always overwrites all the output channels.
  for (auto i = totalNumInputChannels; i < totalNumOutputChannels; ++i)
    buffer.clear(i, 0, buffer.getNumSamples());

  assert(totalNumInputChannels == 2);
  // This is the place where you'd normally do the guts of your plugin's
  // audio processing...
  // Make sure to reset the state if your inner loop is processing
  // the samples and the outer loop is handling the channels.
  // Alternatively, you can process the samples with the channels
  // interleaved by keeping the same state.
  float* input_output_L = buffer.getWritePointer(0);
  float* input_output_R = buffer.getWritePointer(1);
  model.processreplace(input_output_L, input_output_R, input_output_L, input_output_R, buffer.getNumSamples(), 1);

  //  for (int channel = 0; channel < totalNumInputChannels; ++channel) {
  //    float *channelData = buffer.getWritePointer(channel);
  //    float *dest = block.buffer.getWriterPointer(channel);
  //    std::copy_n(channelData, buffer.getNumSamples(), dest);
  //  }
  //
  //  proc.processBlock(&block, &block);
  //
  //  for (int channel = 0; channel < totalNumInputChannels; ++channel) {
  //    float *channelData = buffer.getWritePointer(channel);
  //    float *src = block.buffer.getWriterPointer(channel);
  //    std::copy_n(src, buffer.getNumSamples(), channelData);
  //  }
}

//==============================================================================
bool AudioPluginAudioProcessor::hasEditor() const {
  return true; // (change this to false if you choose to not supply an editor)
}

juce::AudioProcessorEditor *AudioPluginAudioProcessor::createEditor() {
  return new juce::GenericAudioProcessorEditor(*this);
}

//==============================================================================
void AudioPluginAudioProcessor::getStateInformation(
    juce::MemoryBlock &destData) {
  // You should use this method to store your parameters in the memory block.
  // You could do that either as raw data, or use the XML or ValueTree classes
  // as intermediaries to make it easy to save and load complex data.
  juce::ignoreUnused(destData);
}

void AudioPluginAudioProcessor::setStateInformation(const void *data,
                                                    int sizeInBytes) {
  // You should use this method to restore your parameters from this memory
  // block, whose contents will have been created by the getStateInformation()
  // call.
  juce::ignoreUnused(data, sizeInBytes);
}
//==============================================================================
// This creates new instances of the plugin..
juce::AudioProcessor *JUCE_CALLTYPE createPluginFilter() {
  return new AudioPluginAudioProcessor();
}
