//-*- c++ -*-------------------------------------------------------------------
#ifdef __GNUC__
#pragma implementation
#endif
//
// .NAME vgui_mfc
// Author: awf@robots.ox.ac.uk
// Created: July 2000
//
//
//-----------------------------------------------------------------------------

#include "vgui_mfc.h"
#include "vgui_mfc_window.h"
#include "vgui_mfc_dialog_impl.h"
#include "vgui_mfc_utils.h"

#define VC_EXTRALEAN  // Exclude rarely-used stuff from Windows headers

#include <afxwin.h>         // MFC core and standard components
#include <afxext.h>         // MFC extensions
#include <afxdisp.h>        // MFC Automation classes
#include <afxdtctl.h>       // MFC support for Internet Explorer 4 Common Controls
#ifndef _AFX_NO_AFXCMN_SUPPORT
#include <afxcmn.h>         // MFC support for Windows Common Controls
#endif // _AFX_NO_AFXCMN_SUPPORT

#include <vcl_cassert.h>
#include <vgui/vgui_gl.h>

static bool debug = false;

vgui_mfc* vgui_mfc::instance_ = 0;

vgui_mfc* vgui_mfc::instance() {
  if (instance_ == 0)
    instance_ = new vgui_mfc;

  return instance_;
}

//---------------------------------------------------------------------------------
//: Default constructor
vgui_mfc::vgui_mfc()
{
        utils = vgui_mfc_utils::instance();
}

//--------------------------------------------------------------------------------
//: Pure virtual function from vgui (this must be implemented).  Returns the
// name of the GUI toolkit.
vcl_string vgui_mfc::name() const { return "mfc"; }


//--------------------------------------------------------------------------------
//: Virtual function from vgui.  Initialise the implementation of vgui.
void vgui_mfc::init(int &argc, char **argv) {
  if (debug) vcl_cerr << "vgui_mfc::init()" << vcl_endl;

        //: Initialise MFC foundation classes
        if (!AfxWinInit(::GetModuleHandle(NULL), NULL, ::GetCommandLine(), SW_SHOW))
        {
                // TODO: change error code to suit your needs
                vcl_cerr << _T("Fatal Error: MFC initialization failed") << vcl_endl;
                assert(0);
        }
        AfxGetApp()->InitInstance();
}


//--------------------------------------------------------------------------------
//: Virtual function from vgui.   Runs the event loop.
void vgui_mfc::run() {
  if (debug) vcl_cerr << "vgui_mfc::run()" << vcl_endl;

  //: Start the main thread
  AfxGetThread()->Run();
  // Once the window has been closed Run() will terminate
  // and we clean-up the mess
  AfxWinTerm();
}

void vgui_mfc::run_one_event() {
  run_till_idle();
  MSG m_msg;
  AfxGetThread()->PumpMessage();
  ::PeekMessage(&m_msg, NULL, NULL, NULL, PM_NOREMOVE);
}

void vgui_mfc::run_till_idle() {

    MSG msg;
    while ( ::PeekMessage( &msg, NULL, 0, 0, PM_NOREMOVE ) )
    {
        if ( !AfxGetApp()->PumpMessage( ) )
        {
            ::PostQuitMessage(0);
            break;
        }
    }
    // let MFC do its idle processing
    //LONG lIdle = 0;
    //while ( AfxGetApp()->OnIdle(lIdle++ ) )
    //    ;
}

void vgui_mfc::flush() {
  glFlush();
  run_till_idle();
}

void vgui_mfc::quit()
{
  // From MFC FAQ:
    // Same as double-clicking on main window close box.
    ASSERT(AfxGetMainWnd() != NULL);
    AfxGetMainWnd()->SendMessage(WM_CLOSE);
}

void vgui_mfc::add_event(const vgui_event& event) {
}


//--------------------------------------------------------------------------------
//: Virtual function from vgui.  Creates a new window with a menubar.
vgui_window* vgui_mfc::produce_window(int width, int height, const vgui_menu& menubar,
                                           const char* title) {
  return new vgui_mfc_window(width, height, menubar, title);
  return 0;
}


//--------------------------------------------------------------------------------
//: Virtual function from vgui.  Creates a new window.
vgui_window* vgui_mfc::produce_window(int width, int height,
                                           const char* title) {

  return new vgui_mfc_window(title,width, height);
}


//--------------------------------------------------------------------------------
//: Virtual function from vgui.  Creates a new dialog box.
vgui_dialog_impl* vgui_mfc::produce_dialog(const char* name) {
  return new vgui_mfc_dialog_impl(name);
  return 0;
}
