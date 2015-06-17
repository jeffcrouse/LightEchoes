//
//  WarpableImage.h
//  ImageWarper
//
//  Created by Jeffrey Crouse on 6/17/15.
//  Copyright (c) 2015 __MyCompanyName__. All rights reserved.
//

#pragma once
#include "ofMain.h"
#include "ofxJson.h"

class WarpableImage {
public:
    WarpableImage();
    ~WarpableImage();
    void setup(string path);
    void draw();
    void onKeyReleased(int key);
    void save();
    void load();
    
protected:
    int v;
    bool bWarpMode;
    string jsonFilename;
    ofxJSONElement json;
    ofMesh mesh;
    ofImage image;
};
