#pragma once
#include "ofMain.h"
#include "PluginComponent.h"
#include "ofxGui.h"
#include "ofxPostGlitch.h"

class GlitchLibraryPlugin : public PluginComponent {
public:
    void setup() override;
    void update(cv::Mat& frame) override;
    void draw(int x, int y, int w, int h) override;
    void handleInput(int key) override;

    string getName() override { return "GlitchLibraryPlugin"; }
    bool isActive() override { return active; }
    void setActive(bool active) override { this->active = active; }

private:
    bool active = true;
    ofxPanel gui;
    
    // Glitch members
    ofFbo fbo;
    ofxPostGlitch glitch;
    ofImage img;

    ofxToggle doConvergence, doShaker, doCutslider, doNoise;
};
