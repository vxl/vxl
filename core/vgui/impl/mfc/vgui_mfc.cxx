// This is core/vgui/impl/mfc/vgui_mfc.cxx
#ifdef VCL_NEEDS_PRAGMA_INTERFACE
#pragma implementation
#endif
//:
// \file
// \brief See vgui_mfc.h for a description of this file.
// \author awf@robots.ox.ac.uk
// \date   July 2000
//
// \verbatim
//  Modifications
//   K.Y.McGaul  29-AUG-2001  Add destructor to remove Purify mem leak.
//                            Tidy up indentation and documentation.
// \endverbatim
//-----------------------------------------------------------------------------
#include "vgui_mfc.h"

#include <vgui/vgui_adaptor.h>
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

#include "vgui_mfc_app.h"

#include <vcl_cassert.h>
#include <vcl_iostream.h>
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
  : theApp_( 0 )
{
  utils = vgui_mfc_utils::instance();
}

//: Destructor
vgui_mfc::~vgui_mfc()
{
  for (unsigned i=0; i<windows_to_delete.size(); i++)
    delete windows_to_delete[i];
}

//--------------------------------------------------------------------------------
//: Pure virtual function from vgui - returns the name of the GUI toolkit.
vcl_string vgui_mfc::name() const { return "mfc"; }


//--------------------------------------------------------------------------------
//: Virtual function from vgui - initialise the implementation of vgui.
void vgui_mfc::init(int &argc, char **argv) {
  if (debug) vcl_cerr << "vgui_mfc::init()\n";

  // If we are here, then we aren't trying to use vgui in an MFC
  // framework. That is, there shouldn't be another CWinApp
  // somewhere. So, create the vgui one.
  if( AfxGetApp() ) {
    vcl_cerr << "vgui_mfc::init(): another CWinApp object exists!\n";
  } else {
    theApp_ = new vgui_mfc_app;
  }

  //: Initialise MFC foundation classes
  if (!AfxWinInit(::GetModuleHandle(NULL), NULL, ::GetCommandLine(), SW_SHOW))
  {
    // TODO: change error code to suit your needs
    vcl_cerr << _T("Fatal Error: MFC initialization failed") << vcl_endl;
    assert(0);
  }
  AfxGetApp()->InitInstance();
}

void vgui_mfc::uninit() {
  delete theApp_;
  theApp_ = 0;
}


//--------------------------------------------------------------------------------
//: Virtual function from vgui - runs the event loop.
void vgui_mfc::run() {
  if (debug) vcl_cerr << "vgui_mfc::run()\n";

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
//: Virtual function from vgui - creates a new window with a menubar.
vgui_window* vgui_mfc::produce_window(int width, int height, const vgui_menu& menubar,
                                      const char* title)
{
  vgui_window* a_window = new vgui_mfc_window(width, height, menubar, title);
  windows_to_delete.push_back(a_window);
  return a_window;
  //return new vgui_mfc_window(width, height, menubar, title);
}


//--------------------------------------------------------------------------------
//: Virtual function from vgui - creates a new window.
vgui_window* vgui_mfc::produce_window(int width, int height,
                                      const char* title)
{
  vgui_window* a_window = new vgui_mfc_window(title, width, height);
  windows_to_delete.push_back(a_window);
  return a_window;
  //return new vgui_mfc_window(title,width, height);
}


//--------------------------------------------------------------------------------
//: Virtual function from vgui - creates a new dialog box.
vgui_dialog_impl* vgui_mfc::produce_dialog(const char* name)
{
  return new vgui_mfc_dialog_impl(name);
  return 0;
}
