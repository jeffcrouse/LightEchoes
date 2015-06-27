//
//  SoundEngine.cpp
//  Laser02
//
//  Created by Jeffrey Crouse on 6/20/15.
//  Copyright (c) 2015 __MyCompanyName__. All rights reserved.
//

#include "SoundEngine.h"
#include "ofApp.h"

#define NEAR 0,1
#define MID 2,3
#define FAR 4,5
#define TRACK 6

// ------------------------------------------------
SoundEngine::~SoundEngine() {
    lightATheEnd.stop();
    
}

// ------------------------------------------------
void SoundEngine::setup() {
    
    ofFmodSelectDriver(0);
    ofFmodSetNumOutputs(8);
    
    newMelody();
    
    // LOAD SOUNDS
    hihat.loadSound("sounds/LE.Elements_0617.hihat.aif");
    kick.loadSound("sounds/LE.Elements_0617.kick.aif");
    snare.loadSound("sounds/LE.Elements_0619.snare_verb.aif");
    
    startClap.loadSound("sounds/LE.Elements_0617.start_clap.aif");
    endClap.loadSound("sounds/LE.Elements_0617.end_clap.aif");
    
    lightATheEnd.loadSound("sounds/LE.Elements_0617.light_at_the_end.aif");
    
    
//    front = 1.0;
//    middle = 1.0;
//    back = 1.0;
    
    pads[0].loadSound("sounds/pad/LE.Elements_0619.pad_A.aif");
    pads[1].loadSound("sounds/pad/LE.Elements_0619.pad_C.aif");
    pads[2].loadSound("sounds/pad/LE.Elements_0619.pad_e.aif");
    pads[3].loadSound("sounds/pad/LE.Elements_0619.pad_G.aif");
    padIndex = 0;
    
    
    harp[0].loadSound("sounds/harp2/Harp_A3.aif");
    harp[1].loadSound("sounds/harp2/Harp_B3.aif");
    harp[2].loadSound("sounds/harp2/Harp_D3.aif");
    harp[3].loadSound("sounds/harp2/Harp_E3.aif");
    harp[4].loadSound("sounds/harp2/Harp_G3.aif");
    
    
    harpReverb[0].loadSound("sounds/harp2/Harp_A_rev.aif");
    harpReverb[1].loadSound("sounds/harp2/Harp_B_rev.aif");
    harpReverb[2].loadSound("sounds/harp2/Harp_D_rev.aif");
    harpReverb[3].loadSound("sounds/harp2/Harp_E_rev.aif");
    harpReverb[4].loadSound("sounds/harp2/Harp_G_rev.aif");
    
    fx[0].loadSound("sounds/LE.Elements_0617.railroad.aif");
    fx[1].loadSound("sounds/LE.Elements_0617.machine_down.aif");
    fx[2].loadSound("sounds/LE.Elements_0617.machine_up.aif");
    fx[3].loadSound("sounds/LE.Elements_0617.rolled_synth_2.aif");
    fx[4].loadSound("sounds/LE.Elements_0617.rolled_synth_downbeat.aif");
    fx[5].loadSound("sounds/LE.Elements_0617.pitched_synth.aif");
    fx[6].loadSound("sounds/LE.Elements_0617.warped_synth_downbeat.aif");
    
    
    //nextFX = 8;
    app = (ofApp*)ofGetAppPtr();
    nextBeat = app->elapsedTime+(60.0/tempo->getValue());
    beat = 0;
    padIndex=0;
}



// ------------------------------------------------
void SoundEngine::onBeat() {
    if(bMuted) return;
    
    int speakers[6] = {0,1,2,3,4,5};
    
    hihat.setVolume(drumVolume->getValue()*masterVolume->getValue());
    kick.setVolume(drumVolume->getValue()*masterVolume->getValue());
    snare.setVolume(drumVolume->getValue()*masterVolume->getValue());
    
    if(app->bRunning) {
        
        hihat.playTo(FAR);
        
        if(beat % 4 == 0) {
            kick.playTo(FAR);
        } else if(beat % 2 == 0) {
            snare.playTo(FAR);
        }
    }
    
    if(beat % 8 == 0) {
        pads[padIndex].volume = padVolume->getValue();
        pads[padIndex].setVolume(padVolume->getValue()*masterVolume->getValue());
     
        
        pads[padIndex].playTo(speakers, 6);
        ++padIndex %= NUM_PADS;
        
        int nfx = ofRandom(2, 4);
        vector<int> ids;
        while(ids.size() < nfx) {
            int n = ofRandom(NUM_FXS);
            if(find(ids.begin(), ids.end(), n) == ids.end()) {
                ids.push_back(n);
                fx[n].setVolume(fxVolume->getValue()*masterVolume->getValue());
                fx[n].playTo(speakers, 6);
            }
        }
    }
    

    beat++;
}

// ------------------------------------------------
void SoundEngine::mute() {
    bMuted=true;
}


// ------------------------------------------------
void SoundEngine::unmute() {
    bMuted=false;
}

// ------------------------------------------------
void SoundEngine::update() { // float trackPos) {
    
    ofFmodSoundUpdate();
    //ofFmodSoundSetVolume(masterVolume->getValue());
    
    startClap.setVolume(clapVolume->getValue()*masterVolume->getValue());
    endClap.setVolume(clapVolume->getValue()*masterVolume->getValue());
    lightATheEnd.setVolume(lightATheEndVolume->getValue()*masterVolume->getValue());
//    front = ofMap(trackPos, 0, 0.4, 1, 0, true);
//    middle = ofMap(cos(trackPos * TWO_PI), -1, 1, 1, 0);
//    back = ofMap(trackPos, 0.6, 1, 0, 1, true);


    if(app->elapsedTime > nextBeat) {
        onBeat();
        nextBeat = app->elapsedTime + (60.0/tempo->getValue());
    }
    
    harpCooldown -= ofGetLastFrameTime();
}

// ------------------------------------------------
void SoundEngine::playHarp() {
    if(harpCooldown>0) return;
    
    int n = *melodyIt;
    float volume =ofRandom(harpVolume->getValueLow(), harpVolume->getValueHigh());
    
    harp[n].setVolume(volume);
    harp[n].playTo(TRACK);
    if(ofRandomf()<0.2) {
        harpReverb[n].setVolume(volume);
        harpReverb[n].playTo(TRACK);
    }
    harpCooldown = (15.0/tempo->getValue());
    
    melodyIt++;
    if(melodyIt == melody.end())
        melodyIt = melody.begin();
}
