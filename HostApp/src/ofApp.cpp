#include "ofApp.h"
#include "DatamoshPlugin.h"
#include "GlitchLibraryPlugin.h"

void ofApp::setup() {
    ofLogNotice() << "HostApp setup started";

    // Setup Video Source
    camWidth = 1280;
    camHeight = 720;
    vidGrabber.setup(camWidth, camHeight);
    
    syphonClient.setup();
    syphonFbo.allocate(camWidth, camHeight, GL_RGB);

    // Setup GUI
    gui.setup("HostApp Controls");
    sourceGroup.setName("Source Settings");
    sourceGroup.add(useWebcam.set("Use Webcam", true));
    sourceGroup.add(useSyphon.set("Use Syphon", false));
    gui.add(sourceGroup);

    // Add plugins
    addPlugin(std::make_unique<DatamoshPlugin>());
    addPlugin(std::make_unique<GlitchLibraryPlugin>());

    pluginGroup.setName("Plugins");
    int guiX = 220;
    for (auto& plugin : plugins) {
        plugin->setup();
        // Position plugin GUIs
        // Note: plugin->draw() usually calls gui.draw()
        // We can't easily position them here if they are internal to the plugin,
        // but let's assume they might handle it or we can modify them.
        
        auto toggle = std::make_shared<ofParameter<bool>>();
        toggle->set(plugin->getName(), plugin->isActive());
        pluginToggles.push_back(toggle);
        pluginGroup.add(*toggle);
    }
    gui.add(pluginGroup);
    
    previewRect.set(20, 20, 320, 180);
}

void ofApp::update() {
    // Update plugin active state from GUI
    for (int i = 0; i < plugins.size(); i++) {
        plugins[i]->setActive(*pluginToggles[i]);
    }

    if (useWebcam) {
        vidGrabber.update();
        if (vidGrabber.isFrameNew()) {
            sharedFrame = ofxCv::toCv(vidGrabber.getPixels());
        }
    } else if (useSyphon) {
        if (syphonClient.isSetup()) {
            syphonFbo.begin();
            ofClear(0, 0, 0, 255);
            syphonClient.draw(0, 0, camWidth, camHeight);
            syphonFbo.end();
            syphonFbo.readToPixels(syphonPixels);
            sharedFrame = ofxCv::toCv(syphonPixels);
        }
    }

    for (auto& plugin : plugins) {
        if (plugin->isActive()) {
            plugin->update(sharedFrame);
        }
    }
    
    // Update texture if frame exists
    if (!sharedFrame.empty()) {
        ofImage temp;
        ofxCv::toOf(sharedFrame, temp);
        previewTex = temp.getTexture();
    }
}

void ofApp::draw() {
    ofPushStyle();
    ofBackground(0);
    ofSetColor(255);
    
    // Draw base layer (might be processed by some plugins that only modify CV Mat)
    if (previewTex.isAllocated()) {
        previewTex.draw(0, 0, ofGetWidth(), ofGetHeight());
    }

    // Draw plugins that draw to screen (like GlitchLibrary FBO)
    for (auto& plugin : plugins) {
        if (plugin->isActive()) {
            plugin->draw(0, 0, ofGetWidth(), ofGetHeight());
        }
    }

    // Draw input preview (smaller, floating)
    if (previewTex.isAllocated()) {
        ofSetColor(255);
        previewTex.draw(previewRect);
        
        // Use separate color for outline to avoid it bleeding into other elements
        ofNoFill();
        ofSetLineWidth(1);
        ofSetColor(255, 255, 255, 150); // White outline with transparency
        ofDrawRectangle(previewRect);
        ofDrawBitmapStringHighlight("Preview", previewRect.x, previewRect.y - 5);
    }
    ofPopStyle();

    gui.draw();
}

void ofApp::keyPressed(int key) {
    for (auto& plugin : plugins) {
        plugin->handleInput(key);
    }
}

void ofApp::addPlugin(std::unique_ptr<PluginComponent> plugin) {
    plugins.push_back(std::move(plugin));
}
