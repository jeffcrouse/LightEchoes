//
//  GeneratedFrame.h
//  GenerativeTest
//
//  Created by Jeffrey Crouse on 6/21/15.
//  Copyright (c) 2015 __MyCompanyName__. All rights reserved.
//

#pragma once
#include "ofMain.h"

class TextLine {
public:
    TextLine(int size, int height, string textPath, bool flipped=false);
    void draw(int index, int yPos);
    int getHeight();
    
    int height;
    bool flipped;
    string line;
    vector<string> words;
    ofTrueTypeFont font;
    ofRectangle bounds;
};


class GeneratedFrame : public ofFbo {
public:
    void setup();
    void generate(int index);
    
    vector<TextLine*> lines;
};
