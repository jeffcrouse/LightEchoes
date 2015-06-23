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

/*
class CurveSound  {
public:
    CurveSound() {
        volume = 1.0;
    }
    
    void setup(string path) {
        front.loadSound(path);
        middle.loadSound(path);
        back.loadSound(path);
    }
    
    void play() {
        front.playTo(0, 1);
        middle.playTo(2, 3);
        back.playTo(4, 5);
    }
    
    void setLevels(float f, float m, float b) {
        front.setVolume(f*volume);
        middle.setVolume(m*volume);
        back.setVolume(b*volume);
    }
    
    float volume;
    ofxSoundPlayerMultiOutput front;
    ofxSoundPlayerMultiOutput middle;
    ofxSoundPlayerMultiOutput back;
};
*/

class SoundEngine {
public:
    ~SoundEngine();
    void setup();
    void onBeat();
    void update(float trackPos);
    void playHarp();
    void newMelody() {
        melody.clear();
        int len = ofRandom(5, 10);
        for(int i=0; i<len; i++) {
            melody.push_back( ofRandom(0, NUM_HARPS) );
        }
        melodyIt = melody.begin();
    }
    
    float nextBeat;
    //float nextFX;
    float harpCooldown;
    int padIndex;
    //float front, middle, back;
    int beat;
    
    vector<int> melody;
    vector<int>::iterator melodyIt;
    
    ofxUISlider* masterVolume;
    ofxUISlider* tempo;
    ofxUIRangeSlider* harpVolume;
    ofxUISlider* padVolume;
    ofxUISlider* fxVolume;
    //ofxUISlider* arpVolume;
    ofxUISlider* clapVolume;
    ofxUISlider* drumVolume;
    ofxUISlider* lightATheEndVolume;
    
    ofxSoundPlayerMultiOutput pads[NUM_PADS];
    ofxSoundPlayerMultiOutput fx[NUM_FXS];
    ofxSoundPlayerMultiOutput harp[NUM_HARPS];
    
    //ofxSoundPlayerMultiOutput arp;
    ofxSoundPlayerMultiOutput endClap;
    ofxSoundPlayerMultiOutput startClap;
    ofxSoundPlayerMultiOutput kick;
    ofxSoundPlayerMultiOutput snare;
    ofxSoundPlayerMultiOutput hihat;
    

    
    ofxSoundPlayerMultiOutput lightATheEnd;
};