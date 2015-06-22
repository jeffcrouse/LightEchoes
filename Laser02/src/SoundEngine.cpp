//
//  SoundEngine.cpp
//  Laser02
//
//  Created by Jeffrey Crouse on 6/20/15.
//  Copyright (c) 2015 __MyCompanyName__. All rights reserved.
//

#include "SoundEngine.h"

#define NEAR 0,1
#define MID 2,3
#define FAR 4,5

// ------------------------------------------------
void SoundEngine::setup() {
    
    ofFmodSelectDriver(0);
    ofFmodSetNumOutputs(6);
    
    // LOAD SOUNDS
    hihat.loadSound("sounds/LE.Elements_0617.hihat.aif");
    kick.loadSound("sounds/LE.Elements_0617.kick.aif");
    snare.loadSound("sounds/LE.Elements_0619.snare_verb.aif");
    
    startClap.loadSound("sounds/LE.Elements_0617.start_clap.aif");
    endClap.loadSound("sounds/LE.Elements_0617.end_clap.aif");
    
    lightATheEnd.loadSound("sounds/LE.Elements_0617.light_at_the_end.aif");
    lightATheEnd.setLoop(true);
    lightATheEnd.playTo(2, 3);
    
    arp.loadSound("sounds/LE.Elements_0617.arp_pad.aif");
    arp.setLoop(true);
    //arp.play();

    front = 1.0;
    middle = 1.0;
    back = 1.0;
    
    pads[0].setup("sounds/pad/LE.Elements_0619.pad_A.aif");
    pads[1].setup("sounds/pad/LE.Elements_0619.pad_C.aif");
    pads[2].setup("sounds/pad/LE.Elements_0619.pad_e.aif");
    pads[3].setup("sounds/pad/LE.Elements_0619.pad_G.aif");
    padIndex = 0;
    
    harp[0].setup("sounds/harp/LE.Elements_0619.harp_A.aif");
    harp[1].setup("sounds/harp/LE.Elements_0619.harp_B.aif");
    harp[2].setup("sounds/harp/LE.Elements_0619.harp_D.aif");
    harp[3].setup("sounds/harp/LE.Elements_0619.harp_E.aif");
    harp[4].setup("sounds/harp/LE.Elements_0619.harp_G.aif");
    
    fx[0].loadSound("sounds/LE.Elements_0617.railroad.aif");
    fx[1].loadSound("sounds/LE.Elements_0617.machine_down.aif");
    fx[2].loadSound("sounds/LE.Elements_0617.machine_up.aif");
    fx[3].loadSound("sounds/LE.Elements_0617.rolled_synth_2.aif");
    fx[4].loadSound("sounds/LE.Elements_0617.rolled_synth_downbeat.aif");
    fx[5].loadSound("sounds/LE.Elements_0617.pitched_synth.aif");
    fx[6].loadSound("sounds/LE.Elements_0617.warped_synth_downbeat.aif");
    
    
    //nextFX = 8;
    nextBeat = ofGetElapsedTimef()+(60.0/tempo->getValue());
    beat = 0;
    padIndex=0;
}



// ------------------------------------------------
void SoundEngine::onBeat() {
    
    lightATheEnd.setVolume(lightATheEndVolume->getValue());
    arp.setVolume(arpVolume->getValue());
    hihat.setVolume(drumVolume->getValue());
    kick.setVolume(drumVolume->getValue());
    snare.setVolume(drumVolume->getValue());
    
    hihat.playTo(FAR);
    
    if(beat % 4 == 0) {
        kick.playTo(FAR);
    } else if(beat % 2 == 0) {
        snare.playTo(FAR);
    }
    
    
    if(beat % 8 == 0) {
        pads[padIndex].volume = padVolume->getValue();
        pads[padIndex].setLevels(front, middle, back);
        pads[padIndex].play();
        ++padIndex %= NUM_PADS;
        
        int nfx = ofRandom(2, 4);
        vector<int> ids;
        while(ids.size() < nfx) {
            int n = ofRandom(NUM_FXS);
            if(find(ids.begin(), ids.end(), n) == ids.end()) {
                ids.push_back(n);
                fx[n].setVolume(fxVolume->getValue());
                fx[n].playTo(FAR);
            }
        }
    }
    
    /*
    if(beat > nextFX) {
        int n = ofRandom(NUM_FXS);
        //fx[n].volume = fxVolume->getValue();
        //fx[n].setLevels(front, middle, back);
        //fx[n].play();
        nextFX = beat + ofRandom(4, 12) * 2;
    }
     */
    beat++;
}

// ------------------------------------------------
void SoundEngine::update(float trackPos) {
    ofFmodSoundUpdate();
    
    front = ofMap(trackPos, 0, 0.4, 1, 0, true);
    middle = ofMap(cos(trackPos * TWO_PI), -1, 1, 1, 0);
    back = ofMap(trackPos, 0.6, 1, 0, 1, true);

    float now = ofGetElapsedTimef();
    if(now > nextBeat) {
        onBeat();
        nextBeat = now + (60.0/tempo->getValue());
    }
    
    harpCooldown -= ofGetLastFrameTime();
}

// ------------------------------------------------
void SoundEngine::playHarp() {
    if(harpCooldown>0) return;
    
    int n = *melodyIt;
    
    harp[n].volume = ofRandom(harpVolume->getValueLow(), harpVolume->getValueHigh());
    harp[n].setLevels(front, middle, back);
    harp[n].play();
    harpCooldown = (30.0/tempo->getValue());
    
    melodyIt++;
    if(melodyIt == melody.end())
        melodyIt = melody.begin();
}
