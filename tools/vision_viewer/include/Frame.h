/*
 * Copyright (C) 2008 Robotics at Maryland
 * Copyright (C) 2008 Joseph Lisee <jlisee@umd.edu>
 * All rights reserved.
 *
 * Author: Joseph Lisee <jlisee@umd.edu>
 * File:  tools/vision_viewer/include/Frame.h
 */

#ifndef RAM_TOOLS_VISIONVWR_FRAME_H_01_20_2008
#define RAM_TOOLS_VISIONVWR_FRAME_H_01_20_2008

// Library Includes
#include <wx/frame.h>

namespace ram {
namespace tools {
namespace visionvwr {

class CameraView;

enum
{
    ID_Quit = 1,
    ID_About,
    ID_OpenFile,
    ID_OpenCamera
};

class Frame : public wxFrame
{
public:

    Frame(const wxString& title, const wxPoint& pos, const wxSize& size);
    
private:
    void onQuit(wxCommandEvent& event);
    void onAbout(wxCommandEvent& event);
    void onOpenFile(wxCommandEvent& event);
    void onOpenCamera(wxCommandEvent& event);
    
    CameraView* m_view;
    
    DECLARE_EVENT_TABLE()
};

} // namespace visionvwr
} // namespace tools
} // namespace ram
    
#endif // RAM_TOOLS_VISIONVWR_FRAME_H_01_20_2008
