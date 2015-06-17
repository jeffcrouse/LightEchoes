//
//  WarpableImage.cpp
//  ImageWarper
//
//  Created by Jeffrey Crouse on 6/17/15.
//  Copyright (c) 2015 __MyCompanyName__. All rights reserved.
//

#include "WarpableImage.h"

// ------------------------------------
WarpableImage::WarpableImage()
: bWarpMode(false), v(0){ }

// ------------------------------------
WarpableImage::~WarpableImage() {
    save();
}

// ------------------------------------
void WarpableImage::setup(string path){
    jsonFilename = path+".warp.json";
    
    image.loadImage(path);
    
    mesh.setMode(OF_PRIMITIVE_TRIANGLES);
    
    vector<ofPoint> verts;
    for(int slice=0; slice<30; slice++) {
        int y = ofMap(slice, 0, 30, 0, image.getHeight());
        verts.push_back(ofPoint(0, y));
        verts.push_back(ofPoint(image.getWidth(), y));
    }
    
    // Add the verts to the mesh
    for(int i=0; i<verts.size(); i++) {
        mesh.addVertex( verts[i] );
        mesh.addTexCoord( verts[i] );
    }
    
    // Make the triangles with the indices
    for(int i=0; i<verts.size()-2; i+=2) {
        // Triangle 1
        mesh.addIndex(i+0);
        mesh.addIndex(i+1);
        mesh.addIndex(i+2);
        
        // Triangle 2
        mesh.addIndex(i+1);
        mesh.addIndex(i+2);
        mesh.addIndex(i+3);
    }
    
    load();
}

// ------------------------------------
void WarpableImage::draw() {
    ofSetColor(ofColor::white);
    image.bind();
    mesh.draw();
    image.unbind();
    
    if(bWarpMode) {
        vector<ofVec3f>& verts = mesh.getVertices();
        for(int i=0; i<verts.size(); i++) {
            if(i==v) {
                ofSetColor(ofColor::green);
                ofFill();
            }
            else {
                ofSetColor(255, 255, 255, 200);
                ofNoFill();
            }
            
            ofCircle(verts[i].x, verts[i].y, 4);
        }
    }
}
// ------------------------------------
void WarpableImage::onKeyReleased(int key) {
    vector<ofVec3f>& verts = mesh.getVertices();
    
    if(key=='w') {
        bWarpMode = !bWarpMode;
    }
    
    if(bWarpMode) {
        if(key=='v') {
            ++v %= verts.size(); // oooh, tricky
        }
        if(key=='V') {
            v--;
            v = v % verts.size();
        }
        if(key==OF_KEY_LEFT) {
            verts[v].x --;
        }
        if(key==OF_KEY_RIGHT) {
            verts[v].x ++;
        }
        if(key==OF_KEY_UP) {
            verts[v].y --;
        }
        if(key==OF_KEY_DOWN) {
            verts[v].y ++;
        }
        save();
    }
}

// ------------------------------------
void WarpableImage::save() {
    vector<ofVec3f>& verts = mesh.getVertices();

    for(int i=0; i<verts.size(); i++) {
        json["verts"][i]["x"] = verts[i].x;
        json["verts"][i]["y"] = verts[i].y;
    }
    json.save(jsonFilename, true);
}

// ------------------------------------
void WarpableImage::load() {
    if(!ofFile::doesFileExist(jsonFilename)) return;
    
    json.open(jsonFilename);
    
    vector<ofVec3f>& verts = mesh.getVertices();
    for(int i=0; i<verts.size(); i++) {
        verts[i].x = json["verts"][i]["x"].asFloat();
        verts[i].y = json["verts"][i]["y"].asFloat();
    }
}

