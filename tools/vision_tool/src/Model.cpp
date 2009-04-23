/*
 * Copyright (C) 2009 Robotics at Maryland
 * Copyright (C) 2009 Joseph Lisee <jlisee@umd.edu>
 * All rights reserved.
 *
 * Author: Joseph Lisee <jlisee@umd.edu>
 * File:  tools/vision_viewer/include/Model.h
 */

// STD Includes
#include <utility>

// Library Includes
#include <wx/timer.h>

// Core Includes
#include "Model.h"

#include "core/include/EventPublisher.h"

#include "vision/include/Camera.h"
#include "vision/include/OpenCVImage.h"
#include "vision/include/OpenCVCamera.h"
#include "vision/include/FFMPEGCamera.h"
#include "vision/include/Detector.h"
#include "vision/include/DetectorMaker.h"
#include "vision/include/Events.h"


RAM_CORE_EVENT_TYPE(ram::tools::visionvwr::Model, IMAGE_SOURCE_CHANGED);
RAM_CORE_EVENT_TYPE(ram::tools::visionvwr::Model, NEW_IMAGE);

namespace ram {
namespace tools {
namespace visionvwr {

BEGIN_EVENT_TABLE(Model, wxEvtHandler)
END_EVENT_TABLE()
    
Model::Model() :
    m_camera(0),
    m_timer(new wxTimer(this)),
    m_latestImage(new vision::OpenCVImage(640, 480)),
    m_detectorInput(new vision::OpenCVImage(640, 480)),
    m_detectorOutput(new vision::OpenCVImage(640, 480)),
    m_imageToSend(m_latestImage),
    m_detector(vision::DetectorPtr())
{
    Connect(m_timer->GetId(), wxEVT_TIMER,
            wxTimerEventHandler(Model::onTimer));
}

Model::~Model()
{
    m_timer->Stop();
    delete m_camera;
    delete m_timer;
    delete m_latestImage;
    delete m_detectorInput;
    delete m_detectorOutput;
}
    
void Model::openFile(std::string filename)
{
    if (m_camera)
        delete m_camera;
        
    m_camera = new vision::FFMPEGCamera(filename);
    
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
        m_camera = new vision::OpenCVCamera(num, true);
    
    sendImageSourceChanged();
    sendNewImage();
}


void Model::stop()
{
    m_timer->Stop();
}

void Model::start()
{
    double fpsNum = fps();
    if (fpsNum == 0.0)
        fpsNum = 30;
    m_timer->Start((int)(1000 / fpsNum));
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
    // Make sure we are displaying the results of the detector
    m_imageToSend = m_detectorOutput;

    // Make sure we have a valid detector
    if (!vision::DetectorMaker::isKeyRegistered(detectorType))
    {
        std::cerr << "Detector '" << detectorType
                  << "' is not a valid detector" << std::endl;
	assert(false && "Not a valid detector");
    }
    
    // Setup the configuration file
    core::ConfigNode config(core::ConfigNode::fromString("{}"));
    config.set("type", detectorType);

    // Make the detector
    m_detector = vision::DetectorMaker::newObject(
        std::make_pair(config, core::EventHubPtr()));

    // Update display without getting a new image from the source
    sendNewImage(false);
}


void Model::disableDetector()
{
    // Drop reference to the detector, which delete it
    m_detector = vision::DetectorPtr();
    // Change the image we are sending to the latest image form the image source
    m_imageToSend = m_latestImage;

    // Update display without getting a new image from the source
    sendNewImage(false);
}

void Model::detectorSettingsChanged()
{
    // Send a new image, but just reprocess the currently captured one
    sendNewImage(false);
}

void Model::onTimer(wxTimerEvent &event)
{
    if (m_camera)
        sendNewImage();
}
    
void Model::sendNewImage(bool grabFromSource)
{
    // Grab the latest image
    if (grabFromSource)
    {
        m_camera->update(0);
	m_camera->getImage(m_latestImage);
    }
    // If we havea  detector process the image so we can show the debug result
    if (m_detector)
    {
        m_detectorInput->copyFrom(m_latestImage);
	m_detector->processImage(m_detectorInput, m_detectorOutput);
    }

    // Send the event
    vision::ImageEventPtr event(new vision::ImageEvent(m_imageToSend));
    publish(NEW_IMAGE, event);
}
    
void Model::sendImageSourceChanged()
{
    publish(IMAGE_SOURCE_CHANGED, core::EventPtr(new core::Event));
}


} // namespace visionvwr
} // namespace tools
} // namespace ram