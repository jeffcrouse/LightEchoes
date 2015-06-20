//
//  SoundEngine.h
//  Laser02
//
//  Created by Jeffrey Crouse on 6/20/15.
//  Copyright (c) 2015 __MyCompanyName__. All rights reserved.
//

#pragma once
#include "ofMain.h"
#include "ofxSoundPlayerMultiOutput.h"
#include "ofxUI.h"

#define NUM_FXS 7
#define NUM_HARPS 5
#define NUM_PADS 4

class SoundEngine {
public:
    

    void setup();
    void onBeat();
    void update();
    void randomHarp();
    
    float nextBeat;
    float nextFX;
    float harpCooldown;
    int padIndex;
    int beat;
    
    ofxUISlider* tempo;
    ofxUIRangeSlider* harpVolume;
    ofxUISlider* padVolume;
    ofxUISlider* fxVolume;
    ofxUISlider* drumVolume;
    
    ofxSoundPlayerMultiOutput endClap;
    ofxSoundPlayerMultiOutput startClap;
    ofxSoundPlayerMultiOutput kick;
    ofxSoundPlayerMultiOutput snare;
    ofxSoundPlayerMultiOutput hihat;
    ofxSoundPlayerMultiOutput pad[NUM_PADS];
    ofxSoundPlayerMultiOutput harp[NUM_HARPS];
    ofxSoundPlayerMultiOutput fx[NUM_FXS];
    
};