//
//  SourceMaterial.cpp
//  Laser02
//
//  Created by Jeffrey Crouse on 6/17/15.
//  Copyright (c) 2015 __MyCompanyName__. All rights reserved.
//

#include "SourceMaterial.h"

// -------------------------------------------------
void SourceMaterial::setup() {
    allocate(800, 1600);
    
    begin();
    ofClear(ofColor::black);
    ofDrawBitmapStringHighlight("NOT LOADED", 10, 20, ofColor::red, ofColor::white);
    end();
    
    state.open(SOURCE_MATERIAL_STATE_JSON);
    
    if (!state.isMember("index")) state["index"] = 0;
    index = state["index"].asInt();
    
    load();
}

// -------------------------------------------------
ofFloatColor SourceMaterial::getColor(int x, int y) {
    return (ofFloatColor)pixels.getColor(x, y);
}

// -------------------------------------------------
bool SourceMaterial::increment() {
    index++;
    state["index"] = index;
    state.save(SOURCE_MATERIAL_STATE_JSON, true);
    return load();
}

// -------------------------------------------------
void SourceMaterial::reset() {
    index = 0;
    state["index"] = 0;
    state.save(SOURCE_MATERIAL_STATE_JSON, true);
    load();
}

// -------------------------------------------------
int SourceMaterial::getIndex() {
    return index;
}

// -------------------------------------------------
void SourceMaterial::updatePixels() {
    readToPixels(pixels);
}

// -------------------------------------------------
bool SourceMaterial::load() {
    ofDirectory dir;
    dir.listDir("Source");
    
    if(dir.size()==0 || index > dir.size()-1)
        return false;
    
    ofLogNotice("SourceMaterial") << "Loading " << index;
    name = dir.getName(index);
    image.clear();
    image.loadImage(dir.getPath(index));
    image.mirror(true, true);
    
    begin();
    ofClear(ofColor::black);
    ofSetColor(ofColor::white);
    image.draw(0, 0, getWidth(), getHeight());
    end();
    
    updatePixels();
    return true;
}