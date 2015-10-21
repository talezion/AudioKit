//
//  AKFlatFrequencyResponseReverbDSPKernel.hpp
//  AudioKit
//
//  Autogenerated by scripts by Aurelius Prochazka. Do not edit directly.
//  Copyright (c) 2015 Aurelius Prochazka. All rights reserved.
//

#ifndef AKFlatFrequencyResponseReverbDSPKernel_hpp
#define AKFlatFrequencyResponseReverbDSPKernel_hpp

#import "AKDSPKernel.hpp"
#import "AKParameterRamper.hpp"

extern "C" {
#include "base.h"
#include "allpass.h"
}

enum {
    reverbDurationAddress = 0
};

class AKFlatFrequencyResponseReverbDSPKernel : public AKDSPKernel {
public:
    // MARK: Member Functions

    AKFlatFrequencyResponseReverbDSPKernel() {}

    void init(int channelCount, double inSampleRate) {
        channels = channelCount;

        sampleRate = float(inSampleRate);

        sp_create(&sp);
        sp_allpass_create(&allpass);
        sp_allpass_init(sp, allpass, 0.1); // Temporary Modification
        allpass->revtime = 0.5;
    }

    void reset() {
    }

    void setParameter(AUParameterAddress address, AUValue value) {
        switch (address) {
            case reverbDurationAddress:
                reverbDurationRamper.set(clamp(value, (float)0.0, (float)10.0));
                break;

        }
    }

    AUValue getParameter(AUParameterAddress address) {
        switch (address) {
            case reverbDurationAddress:
                return reverbDurationRamper.goal();

            default: return 0.0f;
        }
    }

    void startRamp(AUParameterAddress address, AUValue value, AUAudioFrameCount duration) override {
        switch (address) {
            case reverbDurationAddress:
                reverbDurationRamper.startRamp(clamp(value, (float)0.0, (float)10.0), duration);
                break;

        }
    }

    void setBuffers(AudioBufferList* inBufferList, AudioBufferList* outBufferList) {
        inBufferListPtr = inBufferList;
        outBufferListPtr = outBufferList;
    }

    void process(AUAudioFrameCount frameCount, AUAudioFrameCount bufferOffset) override {
        // For each sample.
        for (int frameIndex = 0; frameIndex < frameCount; ++frameIndex) {
            double reverbDuration = double(reverbDurationRamper.getStep());

            int frameOffset = int(frameIndex + bufferOffset);

            allpass->revtime = (float)reverbDuration;

            for (int channel = 0; channel < channels; ++channel) {
                float* in  = (float*)inBufferListPtr->mBuffers[channel].mData  + frameOffset;
                float* out = (float*)outBufferListPtr->mBuffers[channel].mData + frameOffset;

                sp_allpass_compute(sp, allpass, in, out);
            }
        }
    }

    // MARK: Member Variables

private:

    int channels = 2;
    float sampleRate = 44100.0;

    AudioBufferList* inBufferListPtr = nullptr;
    AudioBufferList* outBufferListPtr = nullptr;

    sp_data *sp;
    sp_allpass *allpass;

public:
    AKParameterRamper reverbDurationRamper = 0.5;
};

#endif /* AKFlatFrequencyResponseReverbDSPKernel_hpp */
