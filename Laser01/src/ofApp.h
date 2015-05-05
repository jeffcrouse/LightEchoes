#pragma once

#include "ofMain.h"
#include "ofxEtherdream.h"
#include "ofxUI.h"
#include "ofxEdsdk.h"


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
	
    
        ofTrueTypeFont font;
    
        void incrementSourceImage() {
            currentImage++;
            if(currentImage>sourceImageDir.size()-1) currentImage=0;
            
            sourceImageName = sourceImageDir.getName(currentImage);
            sourceImage.loadImage(sourceImageDir.getPath(currentImage));
            sourceImagePreview.allocate(sourceImage.getWidth(), sourceImage.getHeight());
        }
        void toggleDirection() {
            bForward = !bForward;
            forwardToggle->setValue(bForward);
        }
        void startCapture() {
            samplePos = 0;
            startTime = ofGetElapsedTimef();
            camera.pressShutterButton();
            bIsRunning=true;
        }
        void endCapture() {
            camera.releaseShutterButton();
            bIsRunning=false;
        }
    
        void updateSourceImagePreview() {
            sourceImagePreview.begin();
            ofClear(0);
            sourceImage.draw(0, 0);
            float y = samplePos * sourceImage.getHeight();
            ofPushStyle();
            ofNoFill();
            ofSetLineWidth(3);
            ofSetColor(255, 0, 0);
            ofRect(0, y-1, sourceImage.getWidth(), 3);
            
            ofPopStyle();
            sourceImagePreview.end();
        }
    
        ofFloatColor map(ofFloatColor c);
        bool bIsRecordingMovie;
        bool bIsRunning;
        bool bForward;
    
        // PREVIEW MODE
        bool bPreviewMode;
        float previewAnimSpeed;
        float previewNextFrame;
    
        vector<ofImage> previewFrames;
        vector<ofImage>::iterator previewIterator;
    
    
        // CAPTURE MODE STUFF
        float startTime;
        ofDirectory sourceImageDir;
        string sourceImageName;
        string outputDir;
        ofxUITextInput* outputDirInput;
    
    
        // SOURCE IMAGE STUFF
        ofFbo sourceImagePreview;
        ofImage sourceImage;
        ofShader adjust;
    
    
        ofxEtherdream etherdream;
        int currentImage;

    
        vector<ofxIlda::Point> points;
        ofxIlda::Frame calibration;
        ofxUISuperCanvas *gui0;
        //ofxUISlider *samplePosSlider;
    
        ofxUISlider* scanSpeedSlider;
        ofxUIToggle* forwardToggle;
    
        bool bDrawCalibration;
        void guiEvent(ofxUIEventArgs &e);
        float drawY;            // Y Position for the laser to draw at
        float samplePos;
        float saturation;
        float brightness;
        ofVec2f noise;
    float overscanSpeed;
    
        float scanSpeed;
        float redMin, redMax;
        float greenMin, greenMax;
        float blueMin, blueMax;
        ofxEdsdk::Camera camera;
};
