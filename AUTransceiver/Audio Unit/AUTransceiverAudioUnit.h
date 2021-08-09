//
//  AUTransceiverAudioUnit.h
//  AUTransceiver
//
//  Created by Hallgrim Bratberg on 08/08/2021.
//

#import <AudioToolbox/AudioToolbox.h>
#import "AUTransceiverDSPKernelAdapter.h"

// Define parameter addresses.
extern const AudioUnitParameterID myParam1;

@interface AUTransceiverAudioUnit : AUAudioUnit

@property (nonatomic, readonly) AUTransceiverDSPKernelAdapter *kernelAdapter;
- (void)setupAudioBuses;
- (void)setupParameterTree;
- (void)setupParameterCallbacks;
@end
