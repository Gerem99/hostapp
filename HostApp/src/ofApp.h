#pragma once

#include "ofMain.h"
#include "ofxGui.h"
#include "ofxSyphon.h"
#include "ofxPostGlitch.h"
#include "ofxOsc.h"
#include <vector>
#include <memory>


struct ContainerApp {
    ofxSyphonClient client;
    ofFbo fbo;
    ofxPanel gui;
    ofParameter<bool> active;
    ofParameter<float> opacity;
    
    std::string folderName;
    std::string displayName;
    std::string serverName;
    bool isRunning;
    int framesToSync;
    
    ContainerApp() {
        isRunning = false;
        framesToSync = -1;
    }
    
    void setup(const std::string& folder, const std::string& display, const std::string& server, float x, float y) {
        folderName = folder;
        displayName = display;
        serverName = server;
        
        fbo.allocate(320, 180, GL_RGBA);
        fbo.begin(); ofClear(0, 0, 0, 0); fbo.end();
        
        client.setup();
        
        gui.setup(displayName, folderName + "_launcher.xml", x, y);
        gui.setHeaderBackgroundColor(ofColor(30, 30, 30));
        gui.setBackgroundColor(ofColor(15, 15, 15, 200));
        gui.setTextColor(ofColor(220, 220, 220));
        gui.setFillColor(ofColor(255, 128, 0)); // Neon orange highlight
        
        gui.add(active.set("Run App", false));
        gui.add(opacity.set("Opacity", 1.0f, 0.0f, 1.0f));
    }
};

class ofApp : public ofBaseApp {
public:
    void setup();
    void update();
    void draw();
    void keyPressed(int key);
    void mousePressed(int x, int y, int button);
    
private:

    // Syphon Discovery
    ofxSyphonServerDirectory syphonDir;
    
    // Master Composition FBO
    ofFbo masterFbo;
    
    // Master Syphon Publisher
    ofxSyphonServer masterServer;
    

    // Container App Launcher Toggles & Previews
    std::vector<std::shared_ptr<ContainerApp>> containerApps;
    int selectedAppIndex; // Currently highlighted dashboard app card (-1 for none)
    
    // OSC Senders for sub-app parameter sync (dedicated to avoid socket recreation)
    ofxOscSender oscSenderMosh;
    ofxOscSender oscSenderText;
    ofxOscSender oscSenderGlitch;
    ofxOscSender oscSenderAlchemy;
    ofxOscSender oscSenderRutt;
    
    // Native sub-app GUI panels
    ofxPanel moshPanel;
    ofParameterGroup moshParams;
    ofParameter<bool> moshActive;
    ofParameter<float> moshStrength;
    ofParameter<float> moshFeedback;
    ofParameter<float> moshMixAB;
    ofParameter<bool> moshDoPixelSort;
    ofParameter<float> moshSortThreshold;
    ofParameter<bool> moshDoPixelate;
    ofParameter<int> moshPixelateSize;
    ofParameter<bool> moshDoThreshold;
    ofParameter<float> moshEffectParam;
    ofParameter<int> moshCamIndexA;
    ofParameter<int> moshCamIndexB;
    
    ofxPanel textPanel;
    ofParameterGroup textParams;
    ofParameter<std::string> textInput;
    ofParameter<ofColor> textColor;
    ofParameter<int> textFontSize;
    ofParameter<float> textScale;
    ofParameter<float> textScrollSpeed;
    ofParameter<int> textPosX;
    ofParameter<int> textPosY;
    ofParameter<bool> textTrail;
    ofParameter<int> textTrailAmount;
    ofParameter<bool> textAscii;
    ofParameter<int> textAsciiRes;
    ofParameter<bool> textAutoScramble;
    ofParameter<float> textScrambleSpeed;
    ofParameter<int> textFontIndex;
    ofParameter<int> textAlignment;
    ofParameter<float> textLetterSpacing;
    ofParameter<int> textOutlineThickness;
    ofParameter<bool> textWave;
    ofParameter<float> textWaveFreq;
    ofParameter<float> textWaveAmp;
    ofParameter<bool> textGlow;
    ofParameter<int> textGlowDistance;
    
    ofxPanel glitchPanel;
    ofParameterGroup glitchParams;
    ofParameterGroup glitchMixerGroup;
    ofParameterGroup glitchEnginesGroup;
    ofParameterGroup glitchColorsGroup;
    ofParameter<int> glitchCamIndex;
    ofParameter<float> glitchAmountSlider;
    ofParameter<float> glitchChaosSlider;
    ofParameter<bool> glitchSlitscanToggle;
    ofParameter<int> glitchSlitscanDelay;
    ofParameter<int> glitchSliceHeight;
    ofParameter<float> glitchSlideSpeed;
    ofParameter<bool> glitchV002Toggle;
    ofParameter<float> glitchV002Bars;
    ofParameter<float> glitchV002Distortion;
    ofParameter<float> glitchV002Res;
    ofParameter<bool> glitchFboToggle;
    ofParameter<float> glitchJpegQuality;
    ofParameter<float> glitchJpegCorruption;
    ofParameter<bool> glitchMeshToggle;
    ofParameter<float> glitchMeshDisplace;
    ofParameter<float> glitchMeshNoiseSpeed;
    ofParameter<float> glitchMeshNoiseScale;
    ofParameter<float> glitchLowRed;
    ofParameter<float> glitchLowGreen;
    ofParameter<float> glitchLowBlue;
    ofParameter<float> glitchMedRedInv;
    ofParameter<float> glitchMedGreenInv;
    ofParameter<float> glitchMedBlueInv;
    
    ofxPanel alchemyPanel;
    ofParameterGroup alchemyParams;
    ofParameterGroup alchemyBrushGroup;
    ofParameterGroup alchemyStyleGroup;
    ofParameter<bool> alchemyMirrorH;
    ofParameter<bool> alchemyMirrorV;
    ofParameter<ofColor> alchemyBrushColor;
    ofParameter<bool> alchemyCycleColor;
    ofParameter<float> alchemyCycleSpeed;
    ofParameter<int> alchemyBrushType;
    ofParameter<float> alchemyBrushSens;
    ofParameter<float> alchemySpatterSize;
    ofParameter<int> alchemyAlpha;
    
    ofxPanel ruttEtraPanel;
    ofParameterGroup ruttEtraParams;
    ofParameter<int> ruttZDepth;
    ofParameter<int> ruttMeshDensity;
    ofParameter<ofVec3f> ruttRotation;
    ofParameter<int> ruttCamIndex;
    
    // OSC Sender helper
    void sendParam(const std::string& appFolder, const std::string& name, const std::string& valueStr);
    void syncAllParamsForApp(int index);
    
    // Parameter changed listener callbacks
    void onMoshParamChanged(ofAbstractParameter& p);
    void onTextParamChanged(ofAbstractParameter& p);
    void onGlitchParamChanged(ofAbstractParameter& p);
    void onAlchemyParamChanged(ofAbstractParameter& p);
    void onRuttEtraParamChanged(ofAbstractParameter& p);
    
    void launchApp(const std::string& folderName);
    void killApp(const std::string& folderName);
    void killAllApps();
    void globalStop(); // Reset software and stop engine
    void exit();

    ofxPanel globalControlsPanel;
    ofxButton globalStopBtn;
};
