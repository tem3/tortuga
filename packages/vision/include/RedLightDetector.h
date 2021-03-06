/*
 * Copyright (C) 2007 Robotics at Maryland
 * Copyright (C) 2007 Daniel Hakim
 * All rights reserved.
 *
 * Author: Daniel Hakim <dhakim@umd.edu>
 * File:  packages/vision/include/RedLightDetector.h
 */

#ifndef RAM_RED_LIGHT_DETECTOR_H_06_23_2007
#define RAM_RED_LIGHT_DETECTOR_H_06_23_2007

// Library Includes
#include "cv.h"

// Project Includes
#include "vision/include/Common.h"
#include "vision/include/Detector.h"
#include "vision/include/BlobDetector.h"

#include "core/include/ConfigNode.h"

// Must be incldued last
#include "vision/include/Export.h"

namespace ram {
namespace vision {


/** Detects the Red "Buoy" in the given image
 *
 *  This first runs a color filter to produce an image where only the red parts
 *  are white, and everything is black.  Following that it runs a configurable
 *  number of erosion, then dilation passes.  After that it runs a blob
 *  detector, and filters for the biggest blog that is square enough to be
 *  considered "round", and reports that as the buoy with an event.
 */  
class RAM_EXPORT RedLightDetector : public Detector
{
  public:
    RedLightDetector(core::ConfigNode config,
                     core::EventHubPtr eventHub = core::EventHubPtr());
    RedLightDetector(Camera* camera);
    ~RedLightDetector();

    void update();
    void processImage(Image* input, Image* output= 0);

    bool found;

    /** Center of the red light in the local horizontal, -4/3 to 4/3 */
    double getX();
    
    /** Center of the red light in the local vertical, -1 to 1 */
    double getY();

    /** The percent of the top of the image blacked out */
    void setTopRemovePercentage(double percent);
    /** The percent of the bottom of the image blacked out */
    void setBottomRemovePercentage(double percent);

    /** Set whether or not to use the LUV filter */
    void setUseLUVFilter(bool value);
    
    void show(char* window);
    IplImage* getAnalyzedImage();
    
  private:
    void init(core::ConfigNode config);

    /** Use Dan's custom redorange function */
    void filterForRedOld(IplImage* image, IplImage* flashFrame);

    /** Use LUV color mask function  */
    void filterForRedNew(IplImage* image);
    
    // Process current state, and publishes LIGHT_FOUND event
    void publishFoundEvent(double lightPixelRadius);

    /** Processes the list of all found blobs and finds the larget valid one */
    bool processBlobs(const BlobDetector::BlobList& blobs,
                      BlobDetector::Blob& outBlob);
    
    int lightFramesOff;
    int lightFramesOn;
    int blinks;
    int spooky;
    bool startCounting;
    double m_redLightCenterX;
    double m_redLightCenterY;
    int minRedPixels;
    IplImage* image;
    IplImage* raw;
    IplImage* flashFrame;
    IplImage* saveFrame;
    CvPoint lightCenter;
    
    Image* frame;
    Camera* cam;

    /** Finds the red light */
    BlobDetector m_blobDetector;

    /** Initial level of minimum red pixels (and lower bound) */
    int m_initialMinRedPixels;

    /** Factor to change minRedPixels by for each found frame */
    double m_foundMinPixelScale;

    /** Factor to change minRedPixels by for each found frame */
    double m_lostMinPixelScale;

    /** Threshold for almost hitting the red light */
    double m_almostHitPercentage;

    /** How un square are blob can be and still be considered a red light */
    double m_maxAspectRatio;
    
    /** Percentage of the image to remove from the top */
    double m_topRemovePercentage;

    /** Percentage of the image to remove from the bottom */
    double m_bottomRemovePercentage;

    /** Percentage of the image to remove from the left */
    double m_leftRemovePercentage;

    /** Percentage of the image to remove from the right */
    double m_rightRemovePercentage;
    
    /** The threshold of the percentage of red in the image */
    double m_redPercentage;

    /** The threshold of the intensity of red in the image */
    int m_redIntensity;

    /** Filters for orange */
    ColorFilter* m_filter;

    /** Whether or not to use the newer LUV color filter */
    bool m_useLUVFilter;
    
    /** The ammout the aspect ratio of the bounding box can be non-square */
    //double m_aspectRatioDeviation;
};
	
} // namespace vision
} // namespace ram

#endif // RAM_RED_LIGHT_DETECTOR_H_06_23_2007
