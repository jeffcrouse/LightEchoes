//
//  ContourLine.cpp
//  Laser02
//
//  Created by Jeffrey Crouse on 6/19/15.
//  Copyright (c) 2015 __MyCompanyName__. All rights reserved.
//

#include "ContourLine.h"


void ContourLine::draw() {
    ofPolyline::draw();
    
    vector<ofPoint>& pts = getVertices();
    for(int i=0; i<pts.size(); i++) {
        if(i==selected) ofFill();
        else ofNoFill();
        ofCircle(pts[i], 5);
    }
}

void ContourLine::onMouseMoved(int x, int y) {
    selected = -1;
    vector<ofPoint>& pts = getVertices();
    for(int i=0; i<pts.size(); i++) {
        if(ofDist(x, y, pts[i].x, pts[i].y) < 5) {
            selected = i;
        }
    }
}

void ContourLine::onMouseDragged(int x, int y) {
    if(selected != -1) {
        vector<ofPoint>& pts = getVertices();
        pts[selected].set(x, y);
    }
}


