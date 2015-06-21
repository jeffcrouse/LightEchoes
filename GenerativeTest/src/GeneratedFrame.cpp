//
//  GeneratedFrame.cpp
//  GenerativeTest
//
//  Created by Jeffrey Crouse on 6/21/15.
//  Copyright (c) 2015 __MyCompanyName__. All rights reserved.
//

#include "GeneratedFrame.h"


// ---------------------------------------------
TextLine::TextLine(int _size, int _height, string _textPath, bool _flipped) {
    font.loadFont("fonts/Neue Helvetica/HelveticaNeueLTCom-BdCn.ttf", _size);

    bounds.set(0, 0, 1500, 800);
    flipped = _flipped;
    height = _height;
    
    string text = ofToUpper(ofBufferFromFile(_textPath));
    ofStringReplace(text, "\n", " ");
    ofStringReplace(text, ",", " ");
    
    
    words = ofSplitString(text, " ");
}

// ---------------------------------------------
void TextLine::draw(int index, int yPos) {
    ofSetColor(ofColor::white);
    
    float xPos = index * (font.getSize() * -0.1);
    
    int i=0;
    while( xPos < bounds.width ) {
        
        ofRectangle bb = font.getStringBoundingBox(words[i], xPos, yPos);
        
        if(bb.intersects(bounds)) {
            ofSetColor(ofColor::white);
            if(words[i].at(0)=='P') {
                ofSetColor(ofColor::red);
            }
            font.drawString(words[i], xPos, yPos);
        }
        
        xPos += bb.width + (font.getSize() * 0.4);
        
        ++i %= words.size();
    }
}

// ---------------------------------------------
int TextLine::getHeight() {
    return height; //font.getLineHeight() * 0.7;
}



// ---------------------------------------------
void GeneratedFrame::setup() {
    allocate(1500, 800);

    lines.push_back(new TextLine(300, 310, "text/line01.txt"));
    lines.push_back(new TextLine(100, 110, "text/line02.txt"));
    lines.push_back(new TextLine(50, 60, "text/line03.txt", true));
    lines.push_back(new TextLine(100, 110, "text/line04.txt", true));
    lines.push_back(new TextLine(200, 210, "text/line05.txt", true));
    lines.push_back(new TextLine(50, 70, "text/line06.txt", true));
    lines.push_back(new TextLine(30, 35, "text/line07.txt", true));
    lines.push_back(new TextLine(200, 200, "text/line08.txt", true));
}


// ---------------------------------------------
void GeneratedFrame::generate(int index) {
    
    int y = -200;
    
    begin();
    ofClear(ofColor::black);
    for(int i=0; i<lines.size(); i++) {
        y += lines[i]->getHeight();
        lines[i]->draw(index, y);
        
    }
    end();
}