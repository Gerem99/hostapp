#pragma once
#include "ofMain.h"
#include "PluginComponent.h"
#include "ofxGui.h"
#include "ofxCv.h"

class DatamoshPlugin : public PluginComponent {
public:
    void setup() override;
    void update(cv::Mat& frame) override;
    void draw(int x, int y, int w, int h) override;
    void handleInput(int key) override;

    string getName() override { return "DatamoshPlugin"; }
    bool isActive() override { return active; }
    void setActive(bool active) override { this->active = active; }

private:
    bool active = true;
    
    // Datamosh specific members
    ofxCv::FlowFarneback flow;
    cv::Mat prevFrame;
    cv::Mat accumulatedMat;
    ofxFloatSlider moshStrength;
    ofxFloatSlider feedback;
    ofxToggle doMosh, doPixelSort;
    ofxPanel gui;
};
