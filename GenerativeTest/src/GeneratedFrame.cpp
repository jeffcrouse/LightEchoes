//
//  GeneratedFrame.cpp
//  GenerativeTest
//
//  Created by Jeffrey Crouse on 6/21/15.
//  Copyright (c) 2015 __MyCompanyName__. All rights reserved.
//

#include "GeneratedFrame.h"


// ---------------------------------------------
TextLine::TextLine(GeneratedFrame* parent, int _size, int _margin, int _offset, string _textPath, bool _flipped) {
    
    font.loadFont("fonts/Neue Helvetica/HelveticaNeueLTCom-BdCn.ttf", _size);

    bounds.set(0, 0, parent->getWidth(), parent->getHeight());
    flipped = _flipped;
    margin = _margin;
    speed = font.getSize() * -0.05;
    spaceWidth = font.getSize() * 0.4;
    offset = _offset;
    string text = ofToUpper(ofBufferFromFile(_textPath));
    ofStringReplace(text, "\n", " ");
    ofStringReplace(text, ",", " ");
    words = ofSplitString(text, " ");
}

// ---------------------------------------------
bool TextLine::isRed(string word) {
    return word.at(0)=='P';
}

// ---------------------------------------------
void TextLine::draw(int index, int yPos) {
    ofSetColor(ofColor::white);
    
    ofPushMatrix();
    if(flipped) {
        ofRotateZ(180);
        float y = (yPos * -2) + font.getSize(); // fuck
        ofTranslate(-bounds.getWidth(), y);
    }
    
    float xPos = (index * speed) + offset;
    int i=0;
    ofRectangle bb;
    while( xPos < bounds.width ) {
        string& word = words[i];
        
        bb = font.getStringBoundingBox(word, xPos, yPos);
        if(bb.intersects(bounds)) {
            ofSetColor(isRed(word) ? ofColor::red: ofColor::white);
            font.drawString(word, xPos, yPos);
        }
        
        xPos += bb.width + spaceWidth;
        ++i %= words.size();
    }
    
    ofPopMatrix();
}

// ---------------------------------------------
int TextLine::getHeight() {
    return font.getSize()+margin; //font.getLineHeight() * 0.7;
}






// ---------------------------------------------
void GeneratedFrame::setup() {
    allocate(5000, 1600);
    
    theTemplate.loadImage("Poetry_layout_01.jpg");
    
    lines.push_back(new TextLine(this, 621, 10, -200, "text/line01.txt"));
    lines.push_back(new TextLine(this, 260, 12, -100, "text/line01.txt"));
    lines.push_back(new TextLine(this, 150, 11, -500, "text/line01.txt"));
    lines.push_back(new TextLine(this, 80, 12, -1400, "text/line01.txt", true));
    lines.push_back(new TextLine(this, 40, 10, -100, "text/line01.txt", true));
    lines.push_back(new TextLine(this, 130, 10, -8000, "text/line01.txt", true));
    lines.push_back(new TextLine(this, 60, 10, -6000, "text/line01.txt", true));
    lines.push_back(new TextLine(this, 190, 10, -6000, "text/line01.txt", true));
    lines.push_back(new TextLine(this, 90, 10, -1800, "text/line01.txt", true));
    lines.push_back(new TextLine(this, 150, 10, -1500, "text/line01.txt", true));
    lines.push_back(new TextLine(this, 255, 10, 0, "text/line01.txt", true));
    lines.push_back(new TextLine(this, 600, 10, 0, "text/line01.txt", true));
}

// ---------------------------------------------
ofFloatColor GeneratedFrame::getColor(int x, int y) {
    return pixels.getColor(x, y);
}

// ---------------------------------------------
void GeneratedFrame::generate(int index) {
    
    int y = -590;
    
    begin();
    ofClear(ofColor::black);
    ofSetColor(ofColor::white);
    //theTemplate.draw(0, 0);
    
    for(int i=0; i<lines.size(); i++) {
        y += lines[i]->getHeight();
        lines[i]->draw(index, y);
    }
    end();
    
    readToPixels(pixels);
}