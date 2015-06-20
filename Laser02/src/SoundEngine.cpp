//
//  SoundEngine.cpp
//  Laser02
//
//  Created by Jeffrey Crouse on 6/20/15.
//  Copyright (c) 2015 __MyCompanyName__. All rights reserved.
//

#include "SoundEngine.h"


// ------------------------------------------------
void SoundEngine::setup() {
    // LOAD SOUNDS
    hihat.loadSound("sounds/LE.Elements_0617.hihat.aif");
    kick.loadSound("sounds/LE.Elements_0617.kick.aif");
    snare.loadSound("sounds/LE.Elements_0619.snare_verb.aif");
    
    startClap.loadSound("sounds/LE.Elements_0617.start_clap.aif");
    endClap.loadSound("sounds/LE.Elements_0617.end_clap.aif");
    
    
    pad[0].loadSound("sounds/pad/LE.Elements_0619.pad_A.aif");
    pad[1].loadSound("sounds/pad/LE.Elements_0619.pad_C.aif");
    pad[2].loadSound("sounds/pad/LE.Elements_0619.pad_e.aif");
    pad[3].loadSound("sounds/pad/LE.Elements_0619.pad_G.aif");
    
    harp[0].loadSound("sounds/harp/LE.Elements_0619.harp_A.aif");
    harp[1].loadSound("sounds/harp/LE.Elements_0619.harp_B.aif");
    harp[2].loadSound("sounds/harp/LE.Elements_0619.harp_D.aif");
    harp[3].loadSound("sounds/harp/LE.Elements_0619.harp_E.aif");
    harp[4].loadSound("sounds/harp/LE.Elements_0619.harp_G.aif");
    
    fx[0].loadSound("sounds/LE.Elements_0617.railroad.aif");
    fx[1].loadSound("sounds/LE.Elements_0617.machine_down.aif");
    fx[2].loadSound("sounds/LE.Elements_0617.machine_up.aif");
    fx[3].loadSound("sounds/LE.Elements_0617.rolled_synth_2.aif");
    fx[4].loadSound("sounds/LE.Elements_0617.rolled_synth_downbeat.aif");
    fx[5].loadSound("sounds/LE.Elements_0617.pitched_synth.aif");
    fx[6].loadSound("sounds/LE.Elements_0617.warped_synth_downbeat.aif");
    
    nextFX = 8;
    nextBeat = ofGetElapsedTimef()+(60.0/tempo->getValue());
    beat = 0;
    padIndex=0;
}

// ------------------------------------------------
void SoundEngine::onBeat() {
    
    hihat.setVolume(drumVolume->getValue());
    kick.setVolume(drumVolume->getValue());
    snare.setVolume(drumVolume->getValue());
    
    hihat.play();

    if(beat % 4 == 0) {
        kick.play();
    }
    
    if(beat % 2 == 0) {
        if(beat%4 != 0) snare.play();
    }
    
    if(beat % 8 == 0) {
        pad[padIndex].setVolume( padVolume->getValue() );
        pad[padIndex].play();
        ++padIndex %= NUM_PADS;
    }
    
    if(beat > nextFX) {
        int n = ofRandom(NUM_FXS);
        fx[n].setVolume( fxVolume->getValue() );
        fx[n].play();
        nextFX = beat + ofRandom(4, 12) * 2;
    }

    beat++;
}

// ------------------------------------------------
void SoundEngine::update() {
    ofFmodSoundUpdate();
    
    float now = ofGetElapsedTimef();
    if(now > nextBeat) {
        onBeat();
        nextBeat = now + (60.0/tempo->getValue());
    }
    
    harpCooldown -= ofGetLastFrameTime();
}

// ------------------------------------------------
void SoundEngine::randomHarp() {
    if(harpCooldown>0) return;
    
    int n = ofRandom(NUM_HARPS);
    float vol = ofRandom(harpVolume->getValueLow(), harpVolume->getValueHigh());
    harp[n].setVolume(vol);
    harp[n].play();
    harpCooldown = (30.0/tempo->getValue());
}
