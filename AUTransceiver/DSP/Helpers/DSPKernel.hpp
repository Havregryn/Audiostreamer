//
//  DSPKernel.hpp
//  AUTransceiver
//
//  Created by Hallgrim Bratberg on 08/08/2021.
//

#ifndef DSPKernel_h
#define DSPKernel_h

#import <AudioToolbox/AudioToolbox.h>
#import <algorithm>

// Put your DSP code into a subclass of DSPKernel.
class DSPKernel {
public:
    virtual void process(AUAudioFrameCount frameCount, AUAudioFrameCount bufferOffset) = 0;

    // Override to handle MIDI events.
    virtual void handleMIDIEvent(AUMIDIEvent const& midiEvent) {}
    virtual void handleParameterEvent(AUParameterEvent const& parameterEvent) {}

    void processWithEvents(AudioTimeStamp const* timestamp, AUAudioFrameCount frameCount, AURenderEvent const* events, AUMIDIOutputEventBlock midiOut);

    AUAudioFrameCount maximumFramesToRender() const {
        return maxFramesToRender;
    }

    void setMaximumFramesToRender(const AUAudioFrameCount &maxFrames) {
        maxFramesToRender = maxFrames;
    }

private:
    void handleOneEvent(AURenderEvent const* event);
    void performAllSimultaneousEvents(AUEventSampleTime now, AURenderEvent const*& event, AUMIDIOutputEventBlock midiOut);

    AUAudioFrameCount maxFramesToRender = 4096;
};

#endif /* DSPKernel_h */
