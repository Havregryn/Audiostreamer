//
//  AUTransceiverDSPKernel.hpp
//  AUTransceiver
//
//  Created by Hallgrim Bratberg on 08/08/2021.
//

#ifndef AUTransceiverDSPKernel_hpp
#define AUTransceiverDSPKernel_hpp

#import "DSPKernel.hpp"
#import "GrimsUdpLib.hpp"

enum {
    paramOne = 0,
};

/*
 AUTransceiverDSPKernel
 Performs simple copying of the input signal to the output.
 As a non-ObjC class, this is safe to use from render thread.
 */
class AUTransceiverDSPKernel : public DSPKernel {
public:
    
    // MARK: Member Functions

    AUTransceiverDSPKernel() {}

    void init(int channelCount, double inSampleRate) {
        chanCount = channelCount;
        sampleRate = float(inSampleRate);
    }

    void reset() {
    }
    
    void deallocate() {
        udp_au.deallocate();
    }

    bool isBypassed() {
        return bypassed;
    }

    void setBypass(bool shouldBypass) {
        bypassed = shouldBypass;
    }

    void setParameter(AUParameterAddress address, AUValue value) {
        switch (address) {
            case paramOne:

                break;
        }
    }

    AUValue getParameter(AUParameterAddress address) {
        switch (address) {
            case paramOne:
                // Return the goal. It is not thread safe to return the ramping value.
                return 0.f;

            default: return 0.f;
        }
    }

    void setBuffers(AudioBufferList* inBufferList, AudioBufferList* outBufferList) {
        inBufferListPtr = inBufferList;
        outBufferListPtr = outBufferList;
    }

    void process(AUAudioFrameCount frameCount, AUAudioFrameCount bufferOffset) override {
        if (bypassed) {
            // Pass the samples through
            for (int channel = 0; channel < chanCount; ++channel) {
                if (inBufferListPtr->mBuffers[channel].mData ==  outBufferListPtr->mBuffers[channel].mData) {
                    continue;
                }
                
                for (int frameIndex = 0; frameIndex < frameCount; ++frameIndex) {
                    const int frameOffset = int(frameIndex + bufferOffset);
                    const float* in  = (float*)inBufferListPtr->mBuffers[channel].mData  + frameOffset;
                    float* out = (float*)outBufferListPtr->mBuffers[channel].mData + frameOffset;
                    *out = *in;
                }
            }
            return;
        }
        
        // Perform per sample dsp on the incoming float *in before assigning it to *out
        for (int channel = 0; channel < chanCount; ++channel) {
            
            // Get pointer to immutable input buffer and mutable output buffer
            const float* in = (float*)inBufferListPtr->mBuffers[channel].mData;
            float* out = (float*)outBufferListPtr->mBuffers[channel].mData;
            
            // Preparing a new message:
            udp_au.initNewAudioMsg(frameCount, channel);
            
            for (int frameIndex = 0; frameIndex < frameCount; ++frameIndex) {
                const int frameOffset = int(frameIndex + bufferOffset);
                
                // Do your sample by sample dsp here...
                out[frameOffset] = in[frameOffset];
                
                //Copying current sample into the message:
                if(sampleRate < 96000 && frameCount <= 512){
                    udp_au.copySampleToMsg(in, frameOffset);
                }
                
            }
            // Transmitting the frames of current channel:
            if(sampleRate < 96000 && frameCount <= 512){
                udp_au.transmit();
            }
        }
    }

    // MARK: Member Variables

private:
    int chanCount = 0;
    float sampleRate = 44100.0;
    bool bypassed = false;
    AudioBufferList* inBufferListPtr = nullptr;
    AudioBufferList* outBufferListPtr = nullptr;
    
    // UDP network audio variables:
    UdpAudioTransceiver udp_au{};
};

#endif /* AUTransceiverDSPKernel_hpp */
