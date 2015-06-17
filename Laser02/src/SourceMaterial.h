//
//  SourceMaterial.h
//  Laser02
//
//  Created by Jeffrey Crouse on 6/17/15.
//  Copyright (c) 2015 __MyCompanyName__. All rights reserved.
//

#pragma once
#include "ofMain.h"
#include "ofxJSON.h"

#define SOURCE_MATERIAL_STATE_JSON "source-state.json"

class SourceMaterial : public ofFbo {
public:
    
    void setup();
    ofFloatColor getColor(int x, int y);
    bool increment();
    void reset();
    int getIndex();
    
protected:
    
    void updatePixels();
    bool load();
    
    ofPixels pixels;
    string name;
    ofImage image;
    string dirPath;
    int index;
    ofxJSONElement state;
    
};
