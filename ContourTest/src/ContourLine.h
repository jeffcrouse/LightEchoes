//
//  ContourLine.h
//  ContourTest
//
//  Created by Jeffrey Crouse on 6/19/15.
//  Copyright (c) 2015 __MyCompanyName__. All rights reserved.
//

#pragma once
#include "ofMain.h"

class ContourLine : public ofPolyline {
public:
    
    void draw();
    void onMouseMoved(int x, int y);
    void onMouseDragged(int x, int y);
    int selected;
};
