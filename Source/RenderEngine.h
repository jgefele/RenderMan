/*
  ==============================================================================

    RenderEngine.h
    Created: 19 Feb 2017 9:47:15pm
    Author:  tollie

  ==============================================================================
*/

#ifndef RENDERENGINE_H_INCLUDED
#define RENDERENGINE_H_INCLUDED

#include <array>
#include <cstdint>
#include <memory>
#include <string>
#include <utility>
#include <vector>


namespace juce
{
    class AudioPluginInstance;
    template <typename T>
    class AudioBuffer;
    class MidiBuffer;
    typedef AudioBuffer<float> AudioSampleBuffer;
}
using namespace juce;

class maxiFFT;
template <typename T>
class maxiMFCCAnalyser;
typedef maxiMFCCAnalyser<double> maxiMFCC;




typedef std::vector<std::pair<int, float>>  PluginPatch;
typedef std::vector<std::array<double, 13>> MFCCFeatures;


class RenderEngine
{
public:
    RenderEngine (int sr, int bs, int ffts);

    virtual ~RenderEngine();

    bool loadPlugin (const std::string& path);

    void setPatch (const PluginPatch patch);

    const PluginPatch getPatch();

    void renderPatch (const uint8_t  midiNote,
                      const uint8_t  midiVelocity,
                      const double noteLength,
                      const double renderLength);

    const MFCCFeatures getMFCCFrames();

    const MFCCFeatures getNormalisedMFCCFrames (const std::array<double, 13>& mean,
                                                const std::array<double, 13>& variance);

    const std::vector<double> getRMSFrames();

    const size_t getPluginParameterSize();

    const std::string getPluginParametersDescription();

    bool overridePluginParameter (const int   index,
                                  const float value);

    bool removeOverridenParameter (const int index);

    const std::vector<double> getAudioFrames();

    bool writeToWav(const std::string& path);

private:
    void fillAudioFeatures (const AudioSampleBuffer& data,
                            maxiFFT&                 fft);

    void ifTimeSetNoteOff (const double&  noteLength,
                           const double&  sampleRate,
                           const int&     bufferSize,
                           const uint8_t& midiChannel,
                           const uint8_t& midiPitch,
                           const uint8_t& midiVelocity,
                           const int&     currentBufferIndex,
                           MidiBuffer&    bufferToNoteOff);

    void fillAvailablePluginParameters (PluginPatch& params);

    double                               sampleRate;
    int                                  bufferSize;
    int                                  fftSize;
    std::unique_ptr<maxiMFCC>            mfcc;
    std::unique_ptr<AudioPluginInstance> plugin;
    PluginPatch                          pluginParameters;
    PluginPatch                          overridenParameters;
    MFCCFeatures                         mfccFeatures;
    std::vector<double>                  processedMonoAudioPreview;
    std::vector<double>                  rmsFrames;
    double                               currentRmsFrame;
};


#endif  // RENDERENGINE_H_INCLUDED
