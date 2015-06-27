//
//  SourceMaterial.cpp
//  Laser02
//
//  Created by Jeffrey Crouse on 6/17/15.
//  Copyright (c) 2015 __MyCompanyName__. All rights reserved.
//

#include "SourceMaterial.h"
#include "ofxModifierKeys.h"

#define SOURCE_DIR "Frames02"
#define SOURCE_MATERIAL_STATE_JSON "source-state.json"
#define SOURCE_MATERIAL_WARP_JSON "warper.json"
#define NUM_ROWS 40
#define NUM_COLS 4


// -------------------------------------------------
void SourceMaterial::setup() {
    allocate(800, 5000);
    
    fonts["small"].loadFont("fonts/HelveticaNeueLTCom-Th.ttf", 100);
    fonts["med"].loadFont("fonts/HelveticaNeueLTCom-Th.ttf", 200);
    fonts["large"].loadFont("fonts/HelveticaNeueLTCom-Th.ttf", 300);
    
    
    begin();
    ofClear(ofColor::black);
    ofDrawBitmapStringHighlight("NOT LOADED", 10, 20, ofColor::red, ofColor::white);
    end();
    
    v = 0;
    bWarpMode=false;
    
    state.open(SOURCE_MATERIAL_STATE_JSON);
    
    if (!state.isMember("index")) state["index"] = 0;
    index = state["index"].asInt();
 
    mesh.setMode(OF_PRIMITIVE_TRIANGLES);

    float col_pos[NUM_COLS] = {0.0, 0.25, 0.75, 1.0};
    
    ofVec3f vert;
    ofVec3f normal(0, 0, 1); // always facing forward //
    ofVec2f texcoord;
    int rows = NUM_ROWS;
    int columns = NUM_COLS;
    for(int iy = 0; iy < rows; iy++) {
        for(int ix = 0; ix < columns; ix++) {
            
            // normalized tex coords //
            //texcoord.x = ((float)ix/((float)columns-1.f));
            texcoord.x = col_pos[ix];
            texcoord.y = ((float)iy/((float)rows-1.f));
            
            vert.x = texcoord.x * getWidth();
            vert.y = texcoord.y * getHeight();
            
            mesh.addVertex(vert);
            mesh.addTexCoord(texcoord);
            mesh.addNormal(normal);
        }
    }
    
    // Triangles //
    for(int y = 0; y < rows-1; y++) {
        for(int x = 0; x < columns-1; x++) {
            // first triangle //
            mesh.addIndex((y)*columns + x);
            mesh.addIndex((y)*columns + x+1);
            mesh.addIndex((y+1)*columns + x);
            
            // second triangle //
            mesh.addIndex((y)*columns + x+1);
            mesh.addIndex((y+1)*columns + x+1);
            mesh.addIndex((y+1)*columns + x);
        }
    }
    
    Poco::Path path = Poco::Path::home();
    path.pushDirectory("Dropbox");
    path.pushDirectory("LE Shared");
    path.pushDirectory(SOURCE_DIR);
    ofDirectory::createDirectory(path.toString(), false, true);
    
    dir.listDir(path.toString());
    loadWarp();
    loadImage();
}

// -------------------------------------------------
void SourceMaterial::onKeyReleased(int key) {
    
    bool dirty = false;
    
    if(key=='O') {
        drawIntoFBO();
        updatePixels();
        ofSaveImage(pixels, "WarpedImage.png");
    }
    
    if(key=='w') {
        bWarpMode = !bWarpMode;
        dirty = true;
    }
    
    if(bWarpMode) {
        vector<ofPoint>& verts = mesh.getVertices();
        if(key=='v') {
            ++v %= verts.size(); // oooh, tricky
            dirty = true;
        }
        if(key=='V') {
            --v %= verts.size(); // oooh, tricky
            dirty = true;
        }
        if(key==OF_KEY_LEFT) {
            verts[v].x -= ofGetModifierPressed(OF_KEY_SHIFT) ? 4 : 0.5;
            dirty = true;
        }
        if(key==OF_KEY_RIGHT) {
            verts[v].x += ofGetModifierPressed(OF_KEY_SHIFT) ? 4 : 0.5;
            dirty = true;
        }
        if(key==OF_KEY_UP) {
            verts[v].y -= ofGetModifierPressed(OF_KEY_SHIFT) ? 4 : 0.5;
            dirty = true;
        }
        if(key==OF_KEY_DOWN) {
            verts[v].y += ofGetModifierPressed(OF_KEY_SHIFT) ? 4 : 0.5;
            dirty = true;
        }
    }
    
    if(dirty) {
        drawIntoFBO();
        updatePixels();
        saveWarp();
    }
}


// -------------------------------------------------
void SourceMaterial::saveWarp() {
    vector<ofVec3f>& verts = mesh.getVertices();
    for(int i=0; i<verts.size(); i++) {
        warper["verts"][i]["x"] = verts[i].x;
        warper["verts"][i]["y"] = verts[i].y;
    }
    warper["v"] = v;
    warper["show"] = bWarpMode;
    warper.save(SOURCE_MATERIAL_WARP_JSON, true);
    
}
// -------------------------------------------------
void SourceMaterial::loadWarp() {
    if(!ofFile::doesFileExist(SOURCE_MATERIAL_WARP_JSON)) return;
    
    warper.open(SOURCE_MATERIAL_WARP_JSON);
    vector<ofVec3f>& verts = mesh.getVertices();
    for(int i=0; i<verts.size(); i++) {
        verts[i].x = warper["verts"][i]["x"].asFloat();
        verts[i].y = warper["verts"][i]["y"].asFloat();
    }
    v = warper["v"].asInt();
    bWarpMode = warper["show"].asBool();
}

// -------------------------------------------------
ofFloatColor SourceMaterial::getColor(int x, int y) {
    return (ofFloatColor)pixels.getColor(x, y);
}

// -------------------------------------------------
void SourceMaterial::increment(bool forward) {
    index+= forward ? 1 : -1;
    index %= dir.size();
    state["index"] = index;
    state.save(SOURCE_MATERIAL_STATE_JSON, true);
    loadImage();
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
void SourceMaterial::loadImage() {
    
    if(dir.size()==0 || index > dir.size()-1 || index < 0)
        return false;
    
    ofLogNotice("SourceMaterial") << "Loading " << index;
    
    name = dir.getName(index);
    image.clear();
    image.loadImage(dir.getPath(index));
    //image.mirror(true, false);
    
    drawIntoFBO();
    updatePixels();
}

// -------------------------------------------------
void SourceMaterial::drawIntoFBO() {
    begin();
    ofPushStyle();
    ofSetRectMode(OF_RECTMODE_CENTER);
    
    ofClear(ofColor::black);
    ofSetColor(ofColor::white);
    
    ofEnableNormalizedTexCoords();
    image.bind();
    mesh.draw();
    image.unbind();
    ofDisableNormalizedTexCoords();
    
    if(bWarpMode) {
        ofNoFill();
        vector<ofPoint>& verts = mesh.getVertices();
        for(int i=0; i<verts.size(); i++) {
            if(i==v) {
                ofSetLineWidth(10);
                ofSetColor(ofColor::green);
                ofRect(verts[i], 100, 100);
            } else {

                ofSetLineWidth(1);
                ofSetColor(ofColor::gray);
                ofRect(verts[i], 20, 20);
            }
            
        }
    }
    
    ofPopStyle();
    end();
}
