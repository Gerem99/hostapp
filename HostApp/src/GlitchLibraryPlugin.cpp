#include "GlitchLibraryPlugin.h"

void GlitchLibraryPlugin::setup() {
    gui.setup("GlitchLibrary Settings");
    gui.setPosition(440, 10);
    gui.add(doConvergence.setup("Convergence", false));
    gui.add(doShaker.setup("Shaker", false));
    gui.add(doCutslider.setup("Cut Slider", false));
    gui.add(doNoise.setup("Noise", false));
    
    fbo.allocate(1280, 720);
    glitch.setup(&fbo);
}

void GlitchLibraryPlugin::update(cv::Mat& frame) {
    if (!active || frame.empty()) return;
    
    // Set effects based on GUI
    glitch.setFx(OFXPOSTGLITCH_CONVERGENCE, doConvergence);
    glitch.setFx(OFXPOSTGLITCH_SHAKER, doShaker);
    glitch.setFx(OFXPOSTGLITCH_CUTSLIDER, doCutslider);
    glitch.setFx(OFXPOSTGLITCH_NOISE, doNoise);

    // Convert frame to texture and render to FBO
    if (!img.isAllocated() || img.getWidth() != frame.cols || img.getHeight() != frame.rows) {
        img.allocate(frame.cols, frame.rows, OF_IMAGE_COLOR);
    }
    ofxCv::toOf(frame, img);
    img.update();
    
    fbo.begin();
    ofClear(0, 255);
    img.draw(0, 0, fbo.getWidth(), fbo.getHeight());
    fbo.end();
    
    glitch.generateFx();
}

void GlitchLibraryPlugin::draw(int x, int y, int w, int h) {
    if (!active) return;
    
    // Draw the FBO after glitching
    fbo.draw(x, y, w, h);
    gui.draw();
}

void GlitchLibraryPlugin::handleInput(int key) {
    // Handle input
}
