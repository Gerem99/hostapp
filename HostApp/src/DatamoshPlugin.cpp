#include "DatamoshPlugin.h"

void DatamoshPlugin::setup() {
    gui.setup("Datamosh Settings");
    gui.setPosition(220, 10);
    gui.add(moshStrength.setup("Mosh Strength", 1.0, 0, 10));
    gui.add(feedback.setup("Feedback", 0.9, 0, 1.0));
    gui.add(doMosh.setup("DATAMOSH", false));
    gui.add(doPixelSort.setup("Pixel Sort", false));
}

struct PixelBR {
    uint8_t r, g, b;
    float getBrightness() const { return 0.299f * r + 0.587f * g + 0.114f * b; }
};

void DatamoshPlugin::update(cv::Mat& frame) {
    if (!active || frame.empty()) return;

    if (prevFrame.empty() || prevFrame.size() != frame.size()) {
        frame.copyTo(prevFrame);
        accumulatedMat = frame.clone();
        return;
    }

    if (doMosh) {
        cv::Mat greyPrev, greyCurr;
        cv::cvtColor(prevFrame, greyPrev, cv::COLOR_RGB2GRAY);
        cv::cvtColor(frame, greyCurr, cv::COLOR_RGB2GRAY);
        
        flow.calcOpticalFlow(greyPrev, greyCurr);
        cv::Mat flowMat = flow.getFlow();
        
        cv::Mat mapX(frame.size(), CV_32F), mapY(frame.size(), CV_32F);
        float force = moshStrength;
        
        for (int y = 0; y < mapX.rows; y++) {
            for (int x = 0; x < mapX.cols; x++) {
                cv::Vec2f f = flowMat.at<cv::Vec2f>(y, x);
                mapX.at<float>(y, x) = (float)x - f[0] * force;
                mapY.at<float>(y, x) = (float)y - f[1] * force;
            }
        }
        
        cv::Mat warped;
        cv::remap(accumulatedMat, warped, mapX, mapY, cv::INTER_LINEAR);
        
        // feedback loop: blend warped accumulated with current frame
        cv::addWeighted(warped, feedback, frame, 1.0 - feedback, 0, accumulatedMat);
        
        // Copy back to frame to show output
        accumulatedMat.copyTo(frame);
    } else {
        frame.copyTo(accumulatedMat);
    }

    if (doPixelSort) {
        float threshold = 127.0f; // Simple fixed threshold for now
        for (int y = 0; y < frame.rows; y++) {
            PixelBR* row = frame.ptr<PixelBR>(y);
            int x = 0;
            while (x < frame.cols) {
                while (x < frame.cols && row[x].getBrightness() < threshold) x++;
                int start = x;
                while (x < frame.cols && row[x].getBrightness() >= threshold) x++;
                int end = x;
                if (start < end) std::sort(row + start, row + end, [](const PixelBR& a, const PixelBR& b) { return a.getBrightness() < b.getBrightness(); });
            }
        }
    }
    
    frame.copyTo(prevFrame);
}

void DatamoshPlugin::draw(int x, int y, int w, int h) {
    if (!active) return;
    gui.draw();
}

void DatamoshPlugin::handleInput(int key) {
    // Handle datamosh-specific keys
}
