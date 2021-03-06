/*
 * Copyright (C) 2010 Robotics at Maryland
 * Copyright (C) 2010 Jonathan Sternberg
 * All rights reserved.
 *
 * Author: Jonathan Sternberg <jsternbe@umd.edu>
 * File:  packages/vision/include/WindowDetector.h
 */

#ifndef RAM_WINDOW_DETECTOR_H_06_01_2010
#define RAM_WINDOW_DETECTOR_H_06_01_2010

// STD Includes
#include <map>
#include <vector>

// Library Includes
#include "cv.h"

// Project Includes
#include "vision/include/Common.h"
#include "vision/include/Color.h"
#include "vision/include/Detector.h"
#include "vision/include/BlobDetector.h"

#include "core/include/ConfigNode.h"

// Must be incldued last
#include "vision/include/Export.h"

namespace ram {
namespace vision {

class RAM_EXPORT WindowDetector : public Detector
{
  public:
    WindowDetector(core::ConfigNode config,
                   core::EventHubPtr eventHub = core::EventHubPtr());
    WindowDetector(Camera* camera);
    ~WindowDetector();

    void update();
    void processImage(Image* input, Image* output = 0);

    void show(char* window);
    IplImage* getAnalyzedImage();
    
  private:
    void init(core::ConfigNode config);

    /* Normal processing to find one blob/color */
    bool processColor(Image* input, Image* output,
                      ColorFilter& filter,
                      BlobDetector::Blob& outerBlob,
                      BlobDetector::Blob& innerBlob);

    bool processBackground(Image *input, ColorFilter& filter,
                           BlobDetector::Blob& outerBlob,
                           BlobDetector::Blob& innerBlob);
    
    /* Process current state, and publishes TARGET_FOUND event */
    void publishFoundEvent(const BlobDetector::Blob& blob,
                           Color::ColorType color);

    void publishLostEvent(Color::ColorType color);

    Camera *cam;
    
    /** Stores the various color filters */
    ColorFilter *m_redFilter;
    ColorFilter *m_greenFilter;
    ColorFilter *m_yellowFilter;
    ColorFilter *m_blueFilter;

    ColorFilter *m_antiRedFilter;
    ColorFilter *m_antiGreenFilter;
    ColorFilter *m_antiYellowFilter;
    ColorFilter *m_antiBlueFilter;

    /** Blob detector */
    BlobDetector m_blobDetector;

    Image *frame;
    Image *tempFrame;
    Image *redFrame;
    Image *greenFrame;
    Image *yellowFrame;
    Image *blueFrame;

    bool m_redFound;
    bool m_greenFound;
    bool m_yellowFound;
    bool m_blueFound;

    /* Configuration variables */
    double m_maxAspectRatio;
    double m_minAspectRatio;

    int m_minWidth;
    int m_minHeight;

    double m_minRelInnerWidth;
    double m_minRelInnerHeight;

    int m_centerXDisagreement;
    int m_centerYDisagreement;

    double m_minPixelPercentage;
    double m_maxPixelPercentage;

    double m_innerMinPixelPercentage;

    int m_erodeIterations;
    int m_dilateIterations;

    int m_debug;
};
	
} // namespace vision
} // namespace ram

#endif // RAM_WINDOW_DETECTOR_H_06_01_2010
