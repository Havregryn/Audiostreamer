//
//  AudioUnitViewController.swift
//  AUTransceiver
//
//  Created by Hallgrim Bratberg on 08/08/2021.
//

import CoreAudioKit

public class AudioUnitViewController: AUViewController, AUAudioUnitFactory {
    var audioUnit: AUAudioUnit?
    
    public override func viewDidLoad() {
        super.viewDidLoad()
        
        if audioUnit == nil {
            return
        }
        
        // Get the parameter tree and add observers for any parameters that the UI needs to keep in sync with the AudioUnit
    }
    
    public func createAudioUnit(with componentDescription: AudioComponentDescription) throws -> AUAudioUnit {
        audioUnit = try AUTransceiverAudioUnit(componentDescription: componentDescription, options: [])
        
        return audioUnit!
    }
    @IBAction func onOffSwitchChanged(_ sender: NSSwitch) {
        guard let auUnit = audioUnit as? AUTransceiverAudioUnit else{ return ;}
        
    }
    
    @IBAction func SendRcvSwitchChanged(_ sender: NSSwitch) {
        
    }
    
}
