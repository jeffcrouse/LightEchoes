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


class SourceMaterial : public ofFbo {
public:
    
    void setup();
    ofFloatColor getColor(int x, int y);
    bool increment();
    int getIndex();
    string getName() {
        return dir.getName(index);
    }
    void onKeyReleased(int key);
    
protected:
    
    void updatePixels();
    bool loadImage();
    void drawIntoFBO();
    ofPixels pixels;
    string name;
    ofImage image;
    string dirPath;
    int index;
    ofxJSONElement state;
    ofxJSONElement warper;
    ofDirectory dir;
    
    void saveWarp();
    void loadWarp();
    
    int v;
    bool bWarpMode;
    ofMesh mesh;
    
    ofPlanePrimitive plane;
};
