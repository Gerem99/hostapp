#pragma once
#include "ofMain.h"
#include "ofxCv.h"

// Abstract base class for all plugins
class PluginComponent {
public:
    virtual ~PluginComponent() {}

    // Lifecycle methods
    virtual void setup() = 0;
    virtual void update(cv::Mat& frame) = 0; // Shared frame input
    virtual void draw(int x, int y, int w, int h) = 0;
    virtual void handleInput(int key) = 0;

    // Resource management
    virtual string getName() = 0;
    virtual bool isActive() = 0;
    virtual void setActive(bool active) = 0;
};
