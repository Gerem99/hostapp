#include "ofApp.h"

void ofApp::setup() {
    ofSetWindowTitle("ANTIGRAVITY // VDMX-style Syphon Mixer");
    ofSetWindowShape(1600, 900);
    ofSetFrameRate(60);
    ofSetVerticalSync(true);

    // Set compact GUI style defaults
    ofxBaseGui::setDefaultWidth(200);
    ofxBaseGui::setDefaultHeight(14);
    ofxBaseGui::setDefaultTextPadding(4);

    // 1. Initialize Syphon Discovery Directory
    syphonDir.setup();

    // 2. Setup Global Controls Panel
    globalControlsPanel.setup("SYSTEM", "system_settings.xml", 1080, 80);
    globalControlsPanel.setHeaderBackgroundColor(ofColor(30, 30, 30));
    globalControlsPanel.setBackgroundColor(ofColor(15, 15, 15, 200));
    globalControlsPanel.setTextColor(ofColor(255, 100, 100));
    globalControlsPanel.setFillColor(ofColor(255, 0, 0));
    globalControlsPanel.add(globalStopBtn.setup("GLOBAL STOP (Space)"));

    // 4b. Setup 5 Container Apps (with launch checkboxes and internal previews)
    std::vector<std::pair<std::string, std::pair<std::string, std::string>>> appInfo = {
        {"LiveDatamosh", {"LiveDatamosh", "LiveDatamosh Output"}},
        {"TextGenerator", {"TextGenerator", "TextGenerator Output"}},
        {"GlitchLibrary", {"Glitch Library", "Glitch Library Output"}},
        {"AlchemyReimagined", {"Alchemy Reimagined", "AlchemyReimagined"}},
        {"RuttEtraSynthesizer", {"RuttEtra Synth", "RuttEtraSynthesizer Output"}}
    };
    
    for (int i = 0; i < appInfo.size(); i++) {
        auto app = std::make_shared<ContainerApp>();
        float x = 20.0f + 15.0f + i * 200.0f;
        float y = 640.0f;
        app->setup(appInfo[i].first, appInfo[i].second.first, appInfo[i].second.second, x, y);
        containerApps.push_back(app);
    }

    // Initialize OSC parameters and UI panels
    selectedAppIndex = -1;
    oscSenderMosh.setup("127.0.0.1", 9001);
    oscSenderText.setup("127.0.0.1", 9002);
    oscSenderGlitch.setup("127.0.0.1", 9003);
    oscSenderAlchemy.setup("127.0.0.1", 9004);
    oscSenderRutt.setup("127.0.0.1", 9005);

    // --- A. LiveDatamosh GUI Setup ---
    moshParams.setName("LiveDatamosh Settings");
    moshParams.add(moshActive.set("DATAMOSH (g)", false));
    moshParams.add(moshStrength.set("Mosh Strength", 1.0f, 0.0f, 10.0f));
    moshParams.add(moshFeedback.set("Feedback", 0.9f, 0.0f, 1.0f));
    moshParams.add(moshMixAB.set("Mix A/B", 0.5f, 0.0f, 1.0f));
    moshParams.add(moshDoThreshold.set("Threshold", false));
    moshParams.add(moshEffectParam.set("Param (Thresh/Edge)", 0.5f, 0.0f, 1.0f));
    moshParams.add(moshDoPixelate.set("Pixelate", false));
    moshParams.add(moshPixelateSize.set("Pixelate Size", 8, 1, 64));
    moshParams.add(moshDoPixelSort.set("Pixel Sort", false));
    moshParams.add(moshSortThreshold.set("Sort Threshold", 0.5f, 0.0f, 1.0f));
    moshParams.add(moshCamIndexA.set("Cam A Index", 0, 0, 4));
    moshParams.add(moshCamIndexB.set("Cam B Index", 0, 0, 4));
    
    moshPanel.setup(moshParams, "mosh_settings.xml", 20.0f, 80.0f);
    moshPanel.setHeaderBackgroundColor(ofColor(30, 30, 30));
    moshPanel.setBackgroundColor(ofColor(15, 15, 15, 200));
    moshPanel.setTextColor(ofColor(220, 220, 220));
    moshPanel.setFillColor(ofColor(255, 128, 0));
    ofAddListener(moshParams.parameterChangedE(), this, &ofApp::onMoshParamChanged);

    // --- B. TextGenerator GUI Setup ---
    textParams.setName("Text Generator Settings");
    textParams.add(textInput.set("Input", "Type here..."));
    textParams.add(textColor.set("Text Color", ofColor(255), ofColor(0), ofColor(255)));
    textParams.add(textFontSize.set("Font Size", 48, 10, 300));
    textParams.add(textScale.set("Scale", 1.0f, 0.1f, 10.0f));
    textParams.add(textScrollSpeed.set("Scroll Speed", 0.0f, 0.0f, 20.0f));
    textParams.add(textPosX.set("Pos X", 960, 0, 1920));
    textParams.add(textPosY.set("Pos Y", 540, 0, 1080));
    textParams.add(textTrail.set("Trail Effect", true));
    textParams.add(textTrailAmount.set("Trail Amount", 230, 0, 255));
    textParams.add(textAscii.set("ASCII Mode", false));
    textParams.add(textAsciiRes.set("ASCII Resolution", 10, 4, 30));
    textParams.add(textAutoScramble.set("Auto Scramble", false));
    textParams.add(textScrambleSpeed.set("Scramble Speed", 1.0f, 0.1f, 10.0f));
    textParams.add(textFontIndex.set("Font Index", 0, 0, 1000));
    textParams.add(textAlignment.set("Alignment", 1, 0, 2));
    textParams.add(textLetterSpacing.set("Letter Spacing", 1.0f, 0.5f, 3.0f));
    textParams.add(textOutlineThickness.set("Outline Thickness", 0, 0, 3));
    textParams.add(textWave.set("Wave Motion", false));
    textParams.add(textWaveFreq.set("Wave Freq", 2.0f, 0.1f, 10.0f));
    textParams.add(textWaveAmp.set("Wave Amp", 10.0f, 0.0f, 100.0f));
    textParams.add(textGlow.set("Glow Effect", false));
    textParams.add(textGlowDistance.set("Glow Distance", 5, 1, 20));

    textPanel.setup(textParams, "text_settings.xml", 20.0f, 80.0f);
    textPanel.setHeaderBackgroundColor(ofColor(30, 30, 30));
    textPanel.setBackgroundColor(ofColor(15, 15, 15, 200));
    textPanel.setTextColor(ofColor(220, 220, 220));
    textPanel.setFillColor(ofColor(255, 128, 0));
    ofAddListener(textParams.parameterChangedE(), this, &ofApp::onTextParamChanged);

    // --- C. GlitchLibrary GUI Setup ---
    glitchMixerGroup.setName("System Mixer");
    glitchMixerGroup.add(glitchCamIndex.set("Cam Index", 0, 0, 4));
    glitchMixerGroup.add(glitchAmountSlider.set("Glitch Amount", 0.0f, 0.0f, 1.0f));
    glitchMixerGroup.add(glitchChaosSlider.set("Chaos Master", 0.0f, 0.0f, 1.0f));
    glitchMixerGroup.add(glitchSlitscanToggle.set("Enable Slitscan", false));
    glitchMixerGroup.add(glitchSlitscanDelay.set("Delay (Frames)", 1, 1, 5));
    glitchMixerGroup.add(glitchSliceHeight.set("Slice Height (px)", 4, 1, 30));
    glitchMixerGroup.add(glitchSlideSpeed.set("Slicer Speed", 1.0f, 0.0f, 10.0f));

    glitchEnginesGroup.setName("Glitch Engines");
    glitchEnginesGroup.add(glitchV002Toggle.set("V002 Engine", false));
    glitchEnginesGroup.add(glitchV002Bars.set("V002 Bars", 1.0f, 0.0f, 10.0f));
    glitchEnginesGroup.add(glitchV002Distortion.set("V002 Distortion", 1.0f, 0.0f, 10.0f));
    glitchEnginesGroup.add(glitchV002Res.set("V002 Res", 1.0f, 0.0f, 10.0f));
    glitchEnginesGroup.add(glitchFboToggle.set("Fbo Glitch", false));
    glitchEnginesGroup.add(glitchJpegQuality.set("Jpeg Quality", 1.0f, 0.0f, 1.0f));
    glitchEnginesGroup.add(glitchJpegCorruption.set("Jpeg Corruption", 0.0f, 0.0f, 1.0f));

    glitchColorsGroup.setName("Glitch Colors/Mesh");
    glitchColorsGroup.add(glitchMeshToggle.set("Mesh Glitch", false));
    glitchColorsGroup.add(glitchMeshDisplace.set("Displace Strength", 20.0f, 0.0f, 150.0f));
    glitchColorsGroup.add(glitchMeshNoiseSpeed.set("Noise Speed", 1.0f, 0.0f, 10.0f));
    glitchColorsGroup.add(glitchMeshNoiseScale.set("Noise Scale", 1.0f, 0.0f, 10.0f));
    glitchColorsGroup.add(glitchLowRed.set("lowRedRaise", 0.0f, 0.0f, 1.0f));
    glitchColorsGroup.add(glitchLowGreen.set("lowGreenRaise", 0.0f, 0.0f, 1.0f));
    glitchColorsGroup.add(glitchLowBlue.set("lowBlueRaise", 0.0f, 0.0f, 1.0f));
    glitchColorsGroup.add(glitchMedRedInv.set("medRedInvert", false));
    glitchColorsGroup.add(glitchMedGreenInv.set("medGreenInvert", false));
    glitchColorsGroup.add(glitchMedBlueInv.set("medBlueInvert", false));

    glitchParams.setName("Glitch Library Settings");
    glitchParams.add(glitchMixerGroup);
    glitchParams.add(glitchEnginesGroup);
    glitchParams.add(glitchColorsGroup);

    glitchPanel.setup(glitchParams, "glitch_settings.xml", 20.0f, 80.0f);
    glitchPanel.setHeaderBackgroundColor(ofColor(30, 30, 30));
    glitchPanel.setBackgroundColor(ofColor(15, 15, 15, 200));
    glitchPanel.setTextColor(ofColor(220, 220, 220));
    glitchPanel.setFillColor(ofColor(255, 128, 0));

    ofAddListener(glitchParams.parameterChangedE(), this, &ofApp::onGlitchParamChanged);

    // --- D. AlchemyReimagined GUI Setup ---
    alchemyBrushGroup.setName("Brush Details");
    alchemyBrushGroup.add(alchemyBrushType.set("Brush Type", 0, 0, 7));
    alchemyBrushGroup.add(alchemyBrushSens.set("Brush Sens", 1.0f, 0.1f, 20.0f));
    alchemyBrushGroup.add(alchemySpatterSize.set("Spatter Size", 10.0f, 0.0f, 50.0f));
    alchemyBrushGroup.add(alchemyAlpha.set("Alpha", 255, 0, 255));
    alchemyBrushGroup.add(alchemyBrushColor.set("Brush Color", ofColor(255), ofColor(0), ofColor(255)));

    alchemyStyleGroup.setName("Style/Mirror");
    alchemyStyleGroup.add(alchemyMirrorH.set("Mirror H (h)", false));
    alchemyStyleGroup.add(alchemyMirrorV.set("Mirror V (v)", false));
    alchemyStyleGroup.add(alchemyCycleColor.set("Cycle Color", false));
    alchemyStyleGroup.add(alchemyCycleSpeed.set("Cycle Speed", 0.5f, 0.0f, 5.0f));

    alchemyParams.setName("Alchemy Controls");
    alchemyParams.add(alchemyBrushGroup);
    alchemyParams.add(alchemyStyleGroup);

    alchemyPanel.setup(alchemyParams, "alchemy_settings.xml", 20.0f, 80.0f);
    alchemyPanel.setHeaderBackgroundColor(ofColor(30, 30, 30));
    alchemyPanel.setBackgroundColor(ofColor(15, 15, 15, 200));
    alchemyPanel.setTextColor(ofColor(220, 220, 220));
    alchemyPanel.setFillColor(ofColor(255, 128, 0));

    ofAddListener(alchemyParams.parameterChangedE(), this, &ofApp::onAlchemyParamChanged);

    // --- E. RuttEtraSynthesizer GUI Setup ---
    ruttEtraParams.setName("Rutt-Etra Synth");
    ruttEtraParams.add(ruttZDepth.set("Z-Depth", 50, 0, 200));
    ruttEtraParams.add(ruttMeshDensity.set("Density", 100, 10, 300));
    ruttEtraParams.add(ruttRotation.set("Rotation", ofVec3f(0,0,0), ofVec3f(-180), ofVec3f(180)));
    ruttEtraParams.add(ruttCamIndex.set("Cam Index", 0, 0, 4));

    ruttEtraPanel.setup(ruttEtraParams, "rutt_etra_settings.xml", 20.0f, 80.0f);
    ruttEtraPanel.setHeaderBackgroundColor(ofColor(30, 30, 30));
    ruttEtraPanel.setBackgroundColor(ofColor(15, 15, 15, 200));
    ruttEtraPanel.setTextColor(ofColor(220, 220, 220));
    ruttEtraPanel.setFillColor(ofColor(255, 128, 0));
    ofAddListener(ruttEtraParams.parameterChangedE(), this, &ofApp::onRuttEtraParamChanged);

    // 5. Allocate Master composition buffer
    masterFbo.allocate(1920, 1080, GL_RGBA);
    masterFbo.begin(); ofClear(0, 0, 0, 0); masterFbo.end();

    // 6. Setup Master Syphon Publisher
    masterServer.setName("Antigravity Master Output");
}
void ofApp::update() {
    int numServers = syphonDir.size();
    // --- Update App Launcher Toggles & Syphon Clients ---
    for (int i = 0; i < containerApps.size(); i++) {
        auto& app = containerApps[i];
        if (app->active.get() && !app->isRunning) {
            app->isRunning = true;
            app->framesToSync = 90; // Sync after 1.5 seconds when process starts
            launchApp(app->folderName);
        } else if (!app->active.get() && app->isRunning) {
            app->isRunning = false;
            app->framesToSync = -1;
            killApp(app->folderName);
        }
        
        // Count down to OSC sync
        if (app->framesToSync > 0) {
            app->framesToSync--;
            if (app->framesToSync == 0) {
                syncAllParamsForApp(i);
                app->framesToSync = -1;
            }
        }
        
        // Update Syphon Client connection for this app
        if (app->active.get() && app->isRunning) {
            bool foundServer = false;
            for (int s = 0; s < numServers; s++) {
                const auto& desc = syphonDir.getDescription(s);
                if (desc.serverName == app->serverName) {
                    app->client.set(desc);
                    foundServer = true;
                    break;
                }
            }
            
            app->fbo.begin();
            ofClear(0, 0, 0, 0);
            if (foundServer && app->client.isSetup()) {
                ofSetColor(255);
                app->client.draw(0, 0, 320, 180);
            }
            app->fbo.end();
        } else {
            app->fbo.begin();
            ofClear(0, 0, 0, 0);
            app->fbo.end();
        }
    }

    float dt = ofGetLastFrameTime();
    
    // --- Composite Container Apps into Master FBO ---
    masterFbo.begin();
    ofClear(0, 0, 0, 0);
    for (auto& app : containerApps) {
        if (app->active.get() && app->isRunning && app->client.isSetup()) {
            ofEnableBlendMode(OF_BLENDMODE_ALPHA);
            ofSetColor(255, 255, 255, app->opacity.get() * 255.0f);
            app->client.draw(0, 0, 1920, 1080);
            ofDisableBlendMode();
        }
    }
    masterFbo.end();

    if (globalStopBtn) {
        globalStop();
    }

    // --- Publish Final Frame to Syphon Server ---
    masterServer.publishTexture(&masterFbo.getTexture());
}

void ofApp::draw() {
    // Elegant sleek space-age dashboard background
    ofBackground(12, 12, 16);

    // Subtle technical grid
    ofPushStyle();
    ofSetColor(20, 20, 28, 120);
    ofSetLineWidth(1);
    for (int x = 0; x < ofGetWidth(); x += 40) {
        ofDrawLine(x, 0, x, ofGetHeight());
    }
    for (int y = 0; y < ofGetHeight(); y += 40) {
        ofDrawLine(0, y, ofGetWidth(), y);
    }
    ofPopStyle();

    // --- 1. Draw Top Header ---
    ofPushStyle();
    ofFill();
    ofSetColor(18, 18, 24);
    ofDrawRectangle(0, 0, ofGetWidth(), 55);
    // Neon accent bar below header
    ofSetColor(255, 0, 128);
    ofDrawRectangle(0, 53, ofGetWidth(), 2);
    
    // Header texts
    ofSetColor(255, 255, 255);
    ofDrawBitmapString("ANTIGRAVITY // VDMIX-STYLE SYPHON MIXER", 20, 32);
    
    ofSetColor(0, 168, 204);
    ofDrawBitmapString("FPS: " + ofToString(ofGetFrameRate(), 1) + " | MASTER OUT: 1920x1080 | SYPHON SERVER: ACTIVE", 400, 32);
    
    ofSetColor(120, 120, 130);
    ofDrawBitmapString("Press ESC to Exit | Press F to Toggle Fullscreen", ofGetWidth() - 380, 32);
    ofPopStyle();

    // --- 2. Draw 4 Mixer Channels (GUI, Preview) ---
    // (Channel panels have been removed from GUI)

    // --- 3. Draw Selected Sub-App Control Panel (starts at y = 80) ---
    if (selectedAppIndex >= 0 && selectedAppIndex < containerApps.size()) {
        std::string folderName = containerApps[selectedAppIndex]->folderName;
        if (folderName == "LiveDatamosh") {
            moshPanel.draw();
        } else if (folderName == "TextGenerator") {
            textPanel.draw();
        } else if (folderName == "GlitchLibrary") {
            glitchPanel.draw();
        } else if (folderName == "AlchemyReimagined") {
            alchemyPanel.draw();
        } else if (folderName == "RuttEtraSynthesizer") {
            ruttEtraPanel.draw();
        }
    }

    // --- 4. Draw Active Syphon Sources list (Bottom Panel) ---
    float listX = 20.0f;
    float listY = 625.0f;
    float listW = 1020.0f;
    float listH = 250.0f;
    
    ofPushStyle();
    ofFill();
    ofSetColor(15, 15, 22, 220);
    ofDrawRectRounded(listX, listY, listW, listH, 6);
    
    ofNoFill();
    ofSetLineWidth(2);
    ofSetColor(0, 168, 204, 120);
    ofDrawRectRounded(listX, listY, listW, listH, 6);
    
    // Draw 5 app cards side-by-side inside the bottom panel
    for (int i = 0; i < containerApps.size(); i++) {
        auto& app = containerApps[i];
        float px = listX + 15.0f + i * 200.0f;
        float py = listY + 45.0f;
        float pw = 190.0f;
        float ph = 107.0f;
        
        // Draw toggle panel
        app->gui.draw();
        
        // Draw preview FBO slot
        ofFill();
        ofSetColor(5, 5, 8);
        ofDrawRectangle(px, py, pw, ph);
        
        if (app->active.get() && app->isRunning) {
            ofSetColor(255);
            app->fbo.draw(px, py, pw, ph);
        } else {
            ofSetColor(80, 80, 90);
            ofDrawBitmapString("OFFLINE", px + 65, py + 58);
        }
        
        // Draw frame border
        ofNoFill();
        ofSetLineWidth(1);
        if (i == selectedAppIndex) {
            ofSetColor(0, 255, 255, 255); // Cyan outline for selected card
            ofSetLineWidth(3);
        } else if (app->active.get()) {
            ofSetColor(255, 128, 0, 180); // orange for running
        } else {
            ofSetColor(30, 30, 40);
        }
        ofDrawRectangle(px, py, pw, ph);
        ofSetLineWidth(1);
        
        // Draw status label below the preview
        float tx = px;
        float ty = py + ph + 15.0f;
        if (app->active.get() && app->isRunning) {
            // Check if Syphon server is announced in directory
            bool online = false;
            for (int s = 0; s < syphonDir.size(); s++) {
                if (syphonDir.getDescription(s).serverName == app->serverName) {
                    online = true;
                    break;
                }
            }
            if (online) {
                ofSetColor(0, 255, 128);
                ofDrawBitmapString("Status: LIVE", tx, ty);
            } else {
                ofSetColor(255, 165, 0);
                ofDrawBitmapString("Status: STARTING...", tx, ty);
            }
        } else {
            ofSetColor(120, 120, 130);
            ofDrawBitmapString("Status: OFFLINE", tx, ty);
        }
    }
    ofPopStyle();

    // --- 5. Draw Global Controls ---
    globalControlsPanel.draw();

    // --- 6. Draw Master Preview (Bottom Right) ---
    float previewX = 1080.0f;
    float previewY = 590.0f;
    float previewW = 500.0f;
    float previewH = 281.0f; // 16:9 aspect ratio

    ofPushStyle();
    ofFill();
    ofSetColor(0, 0, 0);
    ofDrawRectangle(previewX, previewY, previewW, previewH);
    
    ofSetColor(255);
    masterFbo.draw(previewX, previewY, previewW, previewH);
    
    // Draw neon pink frame around master preview
    ofNoFill();
    ofSetLineWidth(2);
    ofSetColor(255, 0, 128, 200);
    ofDrawRectangle(previewX, previewY, previewW, previewH);
    
    // Label for master output
    ofSetColor(255, 255, 255);
    ofDrawBitmapStringHighlight("MASTER OUTPUT PREVIEW", previewX + 15, previewY + 25, ofColor(30, 30, 35), ofColor(255, 0, 128));
    ofPopStyle();
}

void ofApp::keyPressed(int key) {
    if (key == 'f' || key == 'F') {
        ofToggleFullscreen();
    }
    if (key == ' ') {
        globalStop();
    }
}

void ofApp::globalStop() {
    for (auto& app : containerApps) {
        app->active.set(false);
    }
    killAllApps();
}

void ofApp::launchApp(const std::string& folderName) {
    std::string myAppsPath = "";
    std::string path1 = ofFilePath::getAbsolutePath(ofFilePath::join(ofToDataPath(""), "../../../"));
    std::string path2 = ofFilePath::getAbsolutePath(ofFilePath::join(ofToDataPath(""), "../../../../../"));
    
    if (ofDirectory(ofFilePath::join(path1, folderName)).exists()) {
        myAppsPath = path1;
    } else if (ofDirectory(ofFilePath::join(path2, folderName)).exists()) {
        myAppsPath = path2;
    } else {
        myAppsPath = "/Users/kareemosman/Downloads/of_v0.12.1_osx_release/apps/myApps";
    }
    
    std::string appPath = ofFilePath::join(myAppsPath, folderName) + "/bin/" + folderName + ".app";
    std::string cmd;
    if (folderName == "AlchemyReimagined") {
        cmd = "\"" + appPath + "/Contents/MacOS/" + folderName + "\" >/dev/null 2>&1 &";
    } else {
        cmd = "\"" + appPath + "/Contents/MacOS/" + folderName + "\" -hosted >/dev/null 2>&1 &";
    }
    std::system(cmd.c_str());
    ofLogNotice() << "Launching container app: " << folderName << " from " << myAppsPath;
}

void ofApp::killApp(const std::string& folderName) {
    std::string cmd = "killall " + folderName + "Debug " + folderName + " 2>/dev/null";
    std::system(cmd.c_str());
    ofLogNotice() << "Terminated container app: " << folderName;
}
void ofApp::killAllApps() {
    std::system("killall LiveDatamoshDebug LiveDatamosh TextGeneratorDebug TextGenerator GlitchLibraryDebug GlitchLibrary AlchemyReimaginedDebug AlchemyReimagined RuttEtraSynthesizerDebug RuttEtraSynthesizer JpegMeshGlitchDebug JpegMeshGlitch 2>/dev/null");
    ofLogNotice() << "Terminated all container apps";
}

void ofApp::exit() {
    killAllApps();
}

void ofApp::mousePressed(int x, int y, int button) {
    // Check if clicked on sub-app cards
    float listX = 20.0f;
    float listY = 625.0f;
    float listW = 1020.0f;
    float listH = 250.0f;
    
    if (x >= listX && x <= listX + listW && y >= listY && y <= listY + listH) {
        for (int i = 0; i < containerApps.size(); i++) {
            float px = listX + 15.0f + i * 200.0f;
            float py = listY + 45.0f;
            float pw = 190.0f;
            float ph = 107.0f;
            
            // Allow selection by clicking anywhere in the card's column width
            if (x >= px && x <= px + pw) {
                selectedAppIndex = i;
                ofLogNotice() << "Selected sub-app card: " << containerApps[i]->displayName;
                break;
            }
        }
    }
}

void ofApp::sendParam(const std::string& appFolder, const std::string& name, const std::string& valueStr) {
    ofxOscMessage m;
    m.setAddress("/param");
    m.addStringArg(name);
    m.addStringArg(valueStr);
    
    if (appFolder == "LiveDatamosh") oscSenderMosh.sendMessage(m);
    else if (appFolder == "TextGenerator") oscSenderText.sendMessage(m);
    else if (appFolder == "GlitchLibrary") oscSenderGlitch.sendMessage(m);
    else if (appFolder == "AlchemyReimagined") oscSenderAlchemy.sendMessage(m);
    else if (appFolder == "RuttEtraSynthesizer") oscSenderRutt.sendMessage(m);
}

void ofApp::syncAllParamsForApp(int index) {
    if (index < 0 || index >= containerApps.size()) return;
    std::string appFolder = containerApps[index]->folderName;
    ofLogNotice() << "Syncing all parameters to sub-app: " << appFolder;
    
    ofParameterGroup* group = nullptr;
    if (appFolder == "LiveDatamosh") group = &moshParams;
    else if (appFolder == "TextGenerator") group = &textParams;
    else if (appFolder == "GlitchLibrary") group = &glitchParams;
    else if (appFolder == "AlchemyReimagined") group = &alchemyParams;
    else if (appFolder == "RuttEtraSynthesizer") group = &ruttEtraParams;
    
    if (group == nullptr) return;
    
    // Recursively send all parameter values inside the group
    std::function<void(ofParameterGroup&)> sendGroup = [&](ofParameterGroup& g) {
        for (std::size_t i = 0; i < g.size(); i++) {
            if (g.getType(i) == "group") {
                sendGroup(g.getGroup(i));
            } else {
                auto& p = g.get(i);
                sendParam(appFolder, p.getName(), p.toString());
            }
        }
    };
    sendGroup(*group);
}

void ofApp::onMoshParamChanged(ofAbstractParameter& p) {
    sendParam("LiveDatamosh", p.getName(), p.toString());
}

void ofApp::onTextParamChanged(ofAbstractParameter& p) {
    sendParam("TextGenerator", p.getName(), p.toString());
}

void ofApp::onGlitchParamChanged(ofAbstractParameter& p) {
    sendParam("GlitchLibrary", p.getName(), p.toString());
}

void ofApp::onAlchemyParamChanged(ofAbstractParameter& p) {
    sendParam("AlchemyReimagined", p.getName(), p.toString());
}

void ofApp::onRuttEtraParamChanged(ofAbstractParameter& p) {
    sendParam("RuttEtraSynthesizer", p.getName(), p.toString());
}
