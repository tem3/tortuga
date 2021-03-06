/*pp
 * Copyright (C) 2009 Robotics at Maryland
 * Copyright (C) 2009 Joseph Lisee <jlisee@umd.edu>
 * All rights reserved.
 *
 * Author: Joseph Lisee <jlisee@umd.edu>
 * File:  tools/vision_viewer/include/Model.h
 */

// STD Includes
#include <utility>
#include <iostream>

// Library Includes
#include <wx/timer.h>
#include <cv.h>
#include <highgui.h>
#include <boost/filesystem.hpp>
#include <boost/foreach.hpp>

// Core Includes
#include "Model.h"

#include "core/include/EventPublisher.h"
#include "core/include/PropertySet.h"

#include "vision/include/VisionSystem.h"
#include "vision/include/Camera.h"
#include "vision/include/Image.h"
#include "vision/include/OpenCVImage.h"
#include "vision/include/OpenCVCamera.h"
#include "vision/include/RawFileCamera.h"
#include "vision/include/NetworkCamera.h"
#include "vision/include/Detector.h"
#include "vision/include/DetectorMaker.h"
#include "vision/include/Events.h"

RAM_CORE_EVENT_TYPE(ram::tools::visiontool::Model, IMAGE_SOURCE_CHANGED);
RAM_CORE_EVENT_TYPE(ram::tools::visiontool::Model, NEW_RAW_IMAGE);
RAM_CORE_EVENT_TYPE(ram::tools::visiontool::Model, NEW_PROCESSED_IMAGE);
RAM_CORE_EVENT_TYPE(ram::tools::visiontool::Model, NEW_CH1_IMAGE);
RAM_CORE_EVENT_TYPE(ram::tools::visiontool::Model, NEW_CH2_IMAGE);
RAM_CORE_EVENT_TYPE(ram::tools::visiontool::Model, NEW_CH3_IMAGE);
RAM_CORE_EVENT_TYPE(ram::tools::visiontool::Model, NEW_CH1_HIST_IMAGE);
RAM_CORE_EVENT_TYPE(ram::tools::visiontool::Model, NEW_CH2_HIST_IMAGE);
RAM_CORE_EVENT_TYPE(ram::tools::visiontool::Model, NEW_CH3_HIST_IMAGE);
RAM_CORE_EVENT_TYPE(ram::tools::visiontool::Model, NEW_CH12_HIST_IMAGE);
RAM_CORE_EVENT_TYPE(ram::tools::visiontool::Model, NEW_CH23_HIST_IMAGE);
RAM_CORE_EVENT_TYPE(ram::tools::visiontool::Model, NEW_CH13_HIST_IMAGE);
RAM_CORE_EVENT_TYPE(ram::tools::visiontool::Model, DETECTOR_CHANGED);
RAM_CORE_EVENT_TYPE(ram::tools::visiontool::Model, DETECTOR_PROPERTIES_CHANGED);

namespace bfs = boost::filesystem;

namespace ram {
namespace tools {
namespace visiontool {

BEGIN_EVENT_TABLE(Model, wxEvtHandler)
END_EVENT_TABLE()
    
Model::Model() :
m_camera(0),
    m_timer(new wxTimer(this)),
    m_blankImage(new vision::OpenCVImage(
                     640, 480, vision::Image::PF_BGR_8)),
    m_latestImage(new vision::OpenCVImage(
                      640, 480, vision::Image::PF_BGR_8)),
    m_detectorInput(new vision::OpenCVImage(
                        640, 480, vision::Image::PF_BGR_8)),
    m_detectorOutput(new vision::OpenCVImage(
                         640, 480, vision::Image::PF_BGR_8)),
    m_processingImage(new vision::OpenCVImage(640, 480, vision::Image::PF_BGR_8)),
    m_ch1Image(new vision::OpenCVImage(640, 480, vision::Image::PF_BGR_8)),
    m_ch2Image(new vision::OpenCVImage(640, 480, vision::Image::PF_BGR_8)),
    m_ch3Image(new vision::OpenCVImage(640, 480, vision::Image::PF_BGR_8)),
    m_ch1HistImage(new vision::OpenCVImage(640, 480, vision::Image::PF_BGR_8)),
    m_ch2HistImage(new vision::OpenCVImage(640, 480, vision::Image::PF_BGR_8)),
    m_ch3HistImage(new vision::OpenCVImage(640, 480, vision::Image::PF_BGR_8)),
    m_ch12HistImage(new vision::OpenCVImage(640, 480, vision::Image::PF_BGR_8)),
    m_ch23HistImage(new vision::OpenCVImage(640, 480, vision::Image::PF_BGR_8)),
    m_ch13HistImage(new vision::OpenCVImage(640, 480, vision::Image::PF_BGR_8)),
    m_detector(vision::DetectorPtr()),
    m_detectorType(""),
    m_configPath("")
{
    Connect(m_timer->GetId(), wxEVT_TIMER,
            wxTimerEventHandler(Model::onTimer));
}

Model::~Model()
{
    m_timer->Stop();
    delete m_camera;
    delete m_timer;
    delete m_blankImage;
    delete m_latestImage;
    delete m_detectorInput;
    delete m_detectorOutput;
}

void Model::fillHistogram(vision::Image *input, vision::Image *histogram)
{
    // only for a 1 channel image
    cv::Mat matImage(input->asIplImage());
    
    // histogram properties
    const int bins = 255;
    float range[] = {0, 255};
    const float *ranges[] = {range};
    cv::MatND hist;
    const int channels = 0;

    // turn the input image into a histogram
    cv::calcHist(&matImage, 1, &channels, cv::Mat(),
                 hist, 1, &bins, ranges, true, false);

    cv::Mat histImage(histogram->asIplImage());
    histImage.setTo(0);
    int width = histImage.cols;
    int height = histImage.rows;
    
    double minValue = 0;
    double maxValue = 0;
    cv::minMaxLoc(hist, &minValue, &maxValue);

    int binWidth = width / bins;
    for(int b = 0; b < bins; b++)
    {
        // get the histogram value for the current bin
        double binVal = log(hist.at<float>(b) + 1);

        // figure out how high the bar should be
        int intensity = binVal / log(maxValue + 1) * height;

        // bar bounds
        int x = b * binWidth;
        int y = height - intensity;
        cv::Point upperLeft(x, y);
        cv::Point lowerRight(x + binWidth - 1, height);

        // draw the rectangle
        cv::rectangle(histImage, upperLeft, lowerRight,
                      cv::Scalar::all(255), -10);
    }
}

void Model::fill2DHistogram(vision::Image* input, int channel1,
                            int channel2, vision::Image* histogram)
{
    // only for a 1 channel image
    cv::Mat matImage(input->asIplImage());
    
    // histogram properties
    const int bins1 = 64;
    const int bins2 = 64;
    int bins[] = {bins1, bins2};

    float range1[] = {0, 256};
    float range2[] = {0, 256};
    const float *ranges[] = {range1, range2};
    cv::MatND hist;
    const int channels[] = {channel1, channel2};

    // turn the input image into a histogram
    cv::calcHist(&matImage, 1, channels, cv::Mat(),
                 hist, 2, bins, ranges, true, false);

    cv::Mat histImage(histogram->asIplImage());
    histImage.setTo(0);

    double minValue = 0;
    double maxValue = 0;
    cv::minMaxLoc(hist, &minValue, &maxValue);
    
    double scale1 = static_cast<double>(histogram->getWidth()) / bins1;
    double scale2 = static_cast<double>(histogram->getHeight()) / bins2;

    for(int b1 = 0; b1 < bins[0]; b1++)
    {
        for(int b2 = 0; b2 < bins[1]; b2++)
        {
            // get the histogram value for the current bin
            double binVal = log(hist.at<float>(b1, b2) + 1);
        
            // figure out how high the bar should be
            int intensity = binVal / log(maxValue + 1) * 255;

            // bar bounds
            int xul = b1 * scale1;
            int yul = b2 * scale2;
            int xlr = (b1 + 1) * scale1 - 1;
            int ylr = (b2 + 1) * scale2 - 1;

            // std::cout << "ul (" << xul << "," << yul << ") "
            //           << "lr (" << xlr << "," << ylr << ") " 
            //           << "int: " << intensity << std::endl;

            cv::Point upperLeft(xul, yul);
            cv::Point lowerRight(xlr, ylr);

            // draw the rectangle
            cv::rectangle(histImage, upperLeft, lowerRight,
                          cv::Scalar::all(intensity), -10);
        }
    }
}

    
void Model::setConfigPath(std::string configPath)
{
    m_configPath = configPath;
    if (m_detectorType.size() > 0)
        changeToDetector(m_detectorType);
}

void Model::openFile(std::string filename)
{
    if (m_camera)
        delete m_camera;

    std::string extension = bfs::path(filename).extension();

    if (".rmv" == extension)
        m_camera = new vision::RawFileCamera(filename);
    else
        m_camera = new vision::OpenCVCamera(filename);
    
    sendImageSourceChanged();
    sendNewImage();
}
    
void Model::openCamera(int num)
{
    if (m_camera)
        delete m_camera;

    if (-1 == num)
        m_camera = new vision::OpenCVCamera();
    else
        m_camera = new vision::OpenCVCamera(num);
    
    sendImageSourceChanged();
    sendNewImage();
}

void Model::openNetworkCamera(std::string hostname, unsigned int port)
{
    if(m_camera)
        delete m_camera;

    m_camera = new vision::NetworkCamera(hostname, port);
    sendImageSourceChanged();
    sendNewImage();
}

void Model::stop()
{
    m_timer->Stop();
}

void Model::start()
{
    if (running())
        stop();
    
    double fpsNum = fps();
    if (fpsNum < 0.1)
        fpsNum = 30;
    m_updateInterval = 1.0 / fpsNum;

    m_nextUpdate = core::TimeVal::timeOfDay() + core::TimeVal(m_updateInterval);
    
    m_timer->Start(static_cast<int>(1000 / fpsNum), true);
}

bool Model::running()
{
    return m_timer->IsRunning();
}

double Model::fps()
{
    if (m_camera)
        return m_camera->fps();
    else
        return 0;
}
    
double Model::duration()
{
    if (m_camera)
        return m_camera->duration();
    else
        return 0;
}
    
void Model::seekToTime(double seconds)
{
    if (m_camera)
    {
        m_camera->seekToTime(seconds);
        m_nextUpdate = core::TimeVal::timeOfDay() +
            core::TimeVal(m_updateInterval * 2);
        sendNewImage();
    }
}
    

double Model::currentTime()
{
    if (m_camera)
        return m_camera->currentTime();
    else
        return 0;
}

std::vector<std::string> Model::getDetectorNames()
{
    return vision::DetectorMaker::getRegisteredKeys();
}

void Model::changeToDetector(std::string detectorType)
{
    // Drop our detector
    m_detector = vision::DetectorPtr();

    // Make sure we have a valid detector
    if (!vision::DetectorMaker::isKeyRegistered(detectorType))
    {
        std::cerr << "Detector '" << detectorType
                  << "' is not a valid detector" << std::endl;
        assert(false && "Not a valid detector");
    }

    if (m_configPath.size() > 0)
    {
        std::string nodeUsed;
        core::ConfigNode cfg(core::ConfigNode::fromFile(m_configPath));

        // Attempt to find at the base level
        m_detector = createDetectorFromConfig(detectorType, cfg, nodeUsed);

        if (!m_detector)
        {
            cfg = vision::VisionSystem::findVisionSystemConfig(cfg, nodeUsed);
            if (nodeUsed.size() > 0)
            {
                m_detector = createDetectorFromConfig(detectorType, cfg, 
                                                      nodeUsed);
            }
        }
    }

    if (!m_detector)
    {
        // Setup the configuration file
        m_detectorType = detectorType;
        core::ConfigNode config(core::ConfigNode::fromString("{}"));
        config.set("type", m_detectorType);

        // Make the detector
        m_detector = vision::DetectorMaker::newObject(
            std::make_pair(config, core::EventHubPtr()));
    }

    // Update display without getting a new image from the source
    sendNewImage(false);
    sendDetectorChanged();
}


void Model::disableDetector()
{
    // Drop reference to the detector, which delete it
    m_detector = vision::DetectorPtr();
    m_detectorType = "";
    m_detectorOutput = m_blankImage;

    // Update display without getting a new image from the source
    sendNewImage(false);
    sendDetectorChanged();
}

void Model::detectorPropertiesChanged()
{
    // Send a new image, but just reprocess the currently captured one
    sendNewImage(false);

    // Tell everyone our properties have been updated
    publish(DETECTOR_PROPERTIES_CHANGED, core::EventPtr(new core::Event));
}

core::PropertySetPtr Model::getDetectorPropertySet()
{
    core::PropertySetPtr propSet;
    if (m_detector)
        propSet = m_detector->getPropertySet();
    return propSet;
}

vision::Image* Model::getLatestImage()
{
    if (m_timer->IsRunning())
        // Not thread safe if this is running
        return NULL;
    else
        return m_latestImage;
}

void Model::onTimer(wxTimerEvent &event)
{
    if (m_camera)
    {
        // Send an image
        sendNewImage();

        // Advance the update the standard ammount
        m_nextUpdate += core::TimeVal(m_updateInterval);

        // Advance out next update until its in the future
        while ((m_nextUpdate - core::TimeVal::timeOfDay()).get_double() < 0)
        {
            m_nextUpdate += core::TimeVal(m_updateInterval);
            m_camera->seekToTime(currentTime() + m_updateInterval);
        }

        // Sleep until that time
        core::TimeVal sleepTime(m_nextUpdate - core::TimeVal::timeOfDay());
        m_timer->Start((int)(sleepTime.get_double() * 1000), true);
    }
}
    
void Model::sendNewImage(bool grabFromSource)
{
    // Grab the latest image
    if (grabFromSource)
    {
        m_camera->update(0);
        m_camera->getImage(m_latestImage);
        m_latestImage->setPixelFormat(vision::Image::PF_RGB_8);
    }

    // If we have detector process the image so we can show the debug result
    if (m_detector)
    {
        m_detectorInput->copyFrom(m_latestImage);
        m_detector->processImage(m_detectorInput, m_detectorOutput);
    }

    m_processingImage->copyFrom(m_latestImage);
    m_processingImage->setPixelFormat(vision::Image::PF_HSV_8);

    cv::vector<cv::Mat> subImages(3);
    cv::split(cv::Mat(m_processingImage->asIplImage()), subImages);

    cv::vector<cv::Mat> ch1x3(3);
    ch1x3[0] = subImages[0];
    ch1x3[1] = subImages[0];
    ch1x3[2] = subImages[0];

    cv::vector<cv::Mat> ch2x3(3);
    ch2x3[0] = subImages[1];
    ch2x3[1] = subImages[1];
    ch2x3[2] = subImages[1];

    cv::vector<cv::Mat> ch3x3(3);
    ch3x3[0] = subImages[2];
    ch3x3[1] = subImages[2];
    ch3x3[2] = subImages[2];

    cv::Mat mCh1(m_ch1Image->asIplImage());
    cv::Mat mCh2(m_ch2Image->asIplImage());
    cv::Mat mCh3(m_ch3Image->asIplImage());

    cv::merge(ch1x3, mCh1);
    cv::merge(ch2x3, mCh2);
    cv::merge(ch3x3, mCh3);

    fillHistogram(m_ch1Image, m_ch1HistImage);
    fillHistogram(m_ch2Image, m_ch2HistImage);
    fillHistogram(m_ch3Image, m_ch3HistImage);
    fill2DHistogram(m_processingImage, 0, 1, m_ch12HistImage);
    fill2DHistogram(m_processingImage, 1, 2, m_ch23HistImage);
    fill2DHistogram(m_processingImage, 0, 2, m_ch13HistImage);

    // Send the event
    vision::ImageEventPtr rawEvent(new vision::ImageEvent(m_latestImage));
    publish(NEW_RAW_IMAGE, rawEvent);

    vision::ImageEventPtr processedEvent(new vision::ImageEvent(m_detectorOutput));
    publish(NEW_PROCESSED_IMAGE, processedEvent);

    vision::ImageEventPtr ch1Event(new vision::ImageEvent(m_ch1Image));
    publish(NEW_CH1_IMAGE, ch1Event);

    vision::ImageEventPtr ch2Event(new vision::ImageEvent(m_ch2Image));
    publish(NEW_CH2_IMAGE, ch2Event);

    vision::ImageEventPtr ch3Event(new vision::ImageEvent(m_ch3Image));    
    publish(NEW_CH3_IMAGE, ch3Event);

    vision::ImageEventPtr ch1HistEvent(new vision::ImageEvent(m_ch1HistImage));    
    publish(NEW_CH1_HIST_IMAGE, ch1HistEvent);

    vision::ImageEventPtr ch2HistEvent(new vision::ImageEvent(m_ch2HistImage));    
    publish(NEW_CH2_HIST_IMAGE, ch2HistEvent);

    vision::ImageEventPtr ch3HistEvent(new vision::ImageEvent(m_ch3HistImage));    
    publish(NEW_CH3_HIST_IMAGE, ch3HistEvent);

    vision::ImageEventPtr ch12HistEvent(new vision::ImageEvent(m_ch12HistImage));
    publish(NEW_CH12_HIST_IMAGE, ch12HistEvent);

    vision::ImageEventPtr ch23HistEvent(new vision::ImageEvent(m_ch23HistImage));
    publish(NEW_CH23_HIST_IMAGE, ch23HistEvent);

    vision::ImageEventPtr ch13HistEvent(new vision::ImageEvent(m_ch13HistImage));
    publish(NEW_CH13_HIST_IMAGE, ch13HistEvent);

}
    
void Model::sendImageSourceChanged()
{
    publish(IMAGE_SOURCE_CHANGED, core::EventPtr(new core::Event));
}

void Model::sendDetectorChanged()
{
    publish(DETECTOR_CHANGED, core::EventPtr(new core::Event));
}

vision::DetectorPtr Model::createDetectorFromConfig(std::string detectorType,
                                                    core::ConfigNode cfg,
                                                    std::string& nodeUsed)
{
    core::NodeNameList nodeNames(cfg.subNodes());
    // Go through each section and check its type
    BOOST_FOREACH(std::string nodeName, nodeNames)
    {
        core::ConfigNode cfgSection(cfg[nodeName]);
        if ((detectorType == cfgSection["type"].asString("NONE"))
            || (nodeName == detectorType))
        {
            nodeUsed = nodeName;
            cfgSection.set("type", detectorType);
            return vision::DetectorMaker::newObject(
                vision::DetectorMakerParamType(cfgSection,
                                               core::EventHubPtr()));
        }
    }
    
    return vision::DetectorPtr();
}


} // namespace visiontool
} // namespace tools
} // namespace ram
