#pragma once
#include "ofMain.h"
#include "PluginComponent.h"
#include "ofxGui.h"
#include "ofxSyphon.h"
#include <vector>
#include <memory>

class ofApp : public ofBaseApp {
public:
    void setup();
    void update();
    void draw();
    void keyPressed(int key);

    void addPlugin(std::unique_ptr<PluginComponent> plugin);

private:
    std::vector<std::unique_ptr<PluginComponent>> plugins;
    cv::Mat sharedFrame;
    ofTexture previewTex;
    ofRectangle previewRect;

    // Video Source handling
    ofVideoGrabber vidGrabber;
    ofxSyphonClient syphonClient;
    ofFbo syphonFbo;
    ofPixels syphonPixels;

    ofxPanel gui;
    ofParameterGroup sourceGroup;
    ofParameter<bool> useWebcam;
    ofParameter<bool> useSyphon;
    ofParameter<int> camWidth, camHeight;

    ofParameterGroup pluginGroup;
    std::vector<std::shared_ptr<ofParameter<bool>>> pluginToggles;

    void updateSharedFrame();
};
