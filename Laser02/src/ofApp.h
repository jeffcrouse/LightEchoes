#pragma once

#include "ofMain.h"
#include "ofMain.h"
#include "ofxEtherdream.h"
#include "ofxUI.h"
#include "ofxEdsdk.h"
#include "ofxJSON.h"
#include "ofxDmx.h"

class SamplerLine: public vector<ofxIlda::Point> {
public:
    SamplerLine() : sampleWidth(800) {
        drawPos.y = 0.5;
    }
    void update(float yPos, ofImage& img) {
        clear();
        
        ofColor color;
        
        samplePos.y = yPos * img.getHeight();
        for (int i=0; i<sampleWidth; i++) {
            samplePos.x = ofMap(i, 0, sampleWidth, 0, img.getWidth(), true);
            color = (i%10<9)
                ? (ofFloatColor)img.getColor(samplePos.x, samplePos.y)
                : ofFloatColor::black;
            
            drawPos.x = samplePos.x / (float)img.getWidth();
            drawPos.x = ofClamp(drawPos.x, 0, 1);
            
            push_back(ofxIlda::Point(drawPos, color));
        }
    }
    
    void addSafetyPoints() {
        // Add safety points to the line
        vector<ofxIlda::Point> finalPoints;
        ofxIlda::Point startPoint = points[0];
        ofxIlda::Point endPoint = points[1];
        
        // repeat at start
        for(int n=0; n<lineBlankCount; n++) {
            finalPoints.push_back( ofxIlda::Point(startPoint.getPosition(), ofFloatColor(0, 0, 0, 0)) );
        }
        for(int n=0; n<lineEndCount; n++) {
            finalPoints.push_back( startPoint );
        }
        for(int j=0; j<points.size(); j++) {
            finalPoints.push_back( points[j] );
        }
        for(int n=0; n<lineEndCount; n++) {
            finalPoints.push_back( endPoint );
        }
        for(int n=0; n<lineBlankCount; n++) {
            finalPoints.push_back( ofxIlda::Point(endPoint.getPosition(), ofFloatColor(0, 0, 0, 0) ));
        }
        etherdream.addPoints(finalPoints);
    }
    
    ofPoint samplePos;
    ofPoint drawPos;
    int sampleWidth;    // How many samples should we take from the image?
};

class Pendulum : public ofxIlda::Frame {
public:
    void update(float yPos) {
        ofxIlda::Frame::clear();

        vector<ofxIlda::Point> points;
        
        
        ofxIlda::Frame::update();
    }
    
    
    ofFloatColor color;
    float height;
    float speed;
    float offset;
};

class ofApp : public ofBaseApp{

	public:
		void setup();
		void update();
		void draw();
        void exit();
    
		void keyPressed(int key);
		void keyReleased(int key);
		void mouseMoved(int x, int y );
		void mouseDragged(int x, int y, int button);
		void mousePressed(int x, int y, int button);
		void mouseReleased(int x, int y, int button);
		void windowResized(int w, int h);
		void dragEvent(ofDragInfo dragInfo);
		void gotMessage(ofMessage msg);
		void guiEvent(ofxUIEventArgs &e);
    
        void loadSourceImage(bool increment=true);
        void makeNewSavePath();
        void processFrames();
        void startRun();
        void endRun();
        void toggleDirection();

    
    
        ofxEtherdream etherdream;
        ofxEdsdk::Camera camera;
        ofxUISuperCanvas *gui;
        ofxDmx dmx;
    
        float yPos;
        float startTime;
        bool bForward;
        bool bRunning;
        ofxUISlider* trackTimeSlider;
        ofxUISlider* autoRunDelaySlider;
        ofxUIToggle* directionToggle;
        ofxUILabelToggle* drawCalibPatternToggle;
        ofxUILabelToggle* autoRunToggle;
    
    
        ofTrueTypeFont font;
        ofxIlda::Frame calibPattern;
        Pendulum pendulum;
        SamplerLine drawLine;
        string savePath;
    
        ofxJSONElement persist;
    
        ofDirectory sourceImageDir;
        string sourceDirPath;
        int sourceImageIndex;
        string sourceImageName;
        ofImage sourceImage;
        ofFbo laserPreview;
};
