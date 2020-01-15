// This is core/vgui/impl/wx/vgui_wx.cxx
//=========================================================================
//:
// \file
// \brief  wxWidgets implementation of vgui_toolkit.
//
// See vgui_wx.h for details.
//=========================================================================

#include <cstdlib>
#include <cstddef>
#include <iostream>
#include "vgui_wx.h"
#include "vgui_wx_app.h"
#include "vgui_wx_window.h"
#include "vgui_wx_dialog_impl.h"
#include "vgui/vgui_gl.h"
#include "vgui_wx_statusbar.h"
#include "vgui_wx_adaptor.h"

#ifdef _MSC_VER
#  include "vcl_msvc_warnings.h"
#endif
#include <cassert>

#include <wx/app.h>
#include <wx/log.h>
#include <wx/wxchar.h>
#include <wx/strconv.h>
#ifdef __WXMSW__
#  include <wx/msw/private.h>
#include <wx/msw/msvcrt.h> 
#endif


vgui_wx* vgui_wx::instance_ = nullptr;

vgui_wx* vgui_wx::instance()
{
  if (!instance_) {
    instance_ = new vgui_wx();
  }
  return vgui_wx::instance_;
}
void vgui_wx::delete_instance(){
  delete instance_;
  instance_ = nullptr;
}
//-------------------------------------------------------------------------
// Private helpers - declarations.
//-------------------------------------------------------------------------
namespace
{
  wxAppConsole* vgui_wx_create_app(void);//needed to intialize the wxApp
  wxChar** g_wxCharArgv = NULL;
  int g_Argc = 0;
}

//-------------------------------------------------------------------------
// vgui_wx implementation - construction & destruction.
//-------------------------------------------------------------------------

//: Returns the name of the GUI toolkit ("wx").
std::string
vgui_wx::name(void) const
{
  return "wx";
}

//: Constructor - default.
vgui_wx::vgui_wx(void)
  : adaptor_embedded_(true), top_level_window_(nullptr)
{
#ifdef DEBUG
  std::cout << "vgui_wx::vgui_wx() - Constructor" << std::endl;
#endif
}

//: Destructor.
vgui_wx::~vgui_wx(void)
{
#ifdef DEBUG
  std::cout << "vgui_wx::~vgui_wx() - Destructor" << std::endl;
#endif
}

//: Initialize the wxWidgets GUI framework.
void
vgui_wx::init(int & argc, char ** argv)
{
  // ***** ensure this is only entered once...

#ifdef DEBUG
  std::cout << "vgui_wx::init()" << std::endl;
#endif

  if (wxTheApp)
  {
    // if we are here, then the wxWidgets main already exists.
    // wxWidgets typically creates its own main but the vgui app 
    // requires a main program vgui_wx implements the functionality of 
    //  wxIMPLEMENT_APP_NO_MAIN
    std::cerr << "vgui_wx::init(): wxApp object already exists!\n";
    // unrecoverable error
    std::exit(-1);
  }

  // Set the app initializer funcion to create the vgui_wx_app.
  wxAppInitializer vgui_wx_app_initializer(static_cast<wxAppInitializerFunction>(vgui_wx_create_app));

#ifdef __WXMSW__
  //wxSetInstance() should be called with the correct HINSTANCE if it differs from
  //the main program executable, which is returned by GetModuleHandle(nullptr)
  wxSetInstance(GetModuleHandle(nullptr));
  wxApp::m_nCmdShow = 0;
#endif

  // If necessary, convert the char** argv to the Unicode wxChar**
  // version.
  wxChar ** wxArgv;
#if wxUSE_UNICODE
  g_Argc = argc;
  g_wxCharArgv = new wxChar *[argc + 1];
  for (int cnt = 0; cnt < argc; ++cnt)
  {
    std::size_t len = wxConvLocal.MB2WC(NULL, argv[cnt], 0);
    g_wxCharArgv[cnt] = new wxChar[len + 1];
    wxConvLocal.MB2WC(g_wxCharArgv[cnt], argv[cnt], len + 1);
  }
  g_wxCharArgv[argc] = NULL;
  wxArgv = g_wxCharArgv;
#else
  wxArgv = argv;
#endif

  // wxWidgets initialization
  if (!wxInitialize(argc, wxArgv))
  {
    std::cerr << "vgui_wx::init(): wxInitialize failed!\n";
    // ***** exit here... or can we recover from this?
    std::exit(-1);
  }

  // wxApp initialization
  if (!wxTheApp->CallOnInit())
  {
    std::cerr << "vgui_wx::init(): wxTheApp->OnInit failed!\n";
    // ***** exit here... or can we recover from this?
    // ***** do we need to call wxUninitialize (or uninit) before exit?
    std::exit(-1);
  }

  // adaptor is being extended (i.e., vgui is controlling the event loop)
  adaptor_embedded_ = false;

  // ***** Conditionally set these logging levels?
  // wxLog* logger = new wxLogStderr;
  // wxLog::SetActiveTarget(logger);
  wxLog::AddTraceMask(wxTRACE_RefCount);
}
// called when application exits
void
vgui_wx::uninit(void)
{
#ifdef DEBUG
  std::cout << "vgui_wx::uninit()" << std::endl;
#endif

  // not controlling the main loop from vgui_wx
  if (adaptor_embedded_)
  {
    std::cerr << __FILE__ ":embedding adaptor; don't call uninit!\n";
    return;
  }

  // ***** This should only be called if OnInit was called.
  wxTheApp->OnExit();

  
  // If we convert the char** argv to a wxChar** version, free our
  // conversion now.
#if wxUSE_UNICODE
  for (int cnt = 0; cnt < g_Argc; ++cnt)
  {
    delete[] g_wxCharArgv[cnt];
  }
  delete g_wxCharArgv;
  g_wxCharArgv = NULL;
  g_Argc = 0;
#endif
  //here is where items created during initialization are deleted
  vgui_wx::delete_instance();
  vgui_wx_app::delete_instance();
}

//-------------------------------------------------------------------------
// vgui_wx implementation - event handling.
// - These should only be called if creating the application in vgui_wx
//   (i.e., we are extending the adaptor instead of embedding it).
//
// - ***** ensure that these are only called if not embedding...
//-------------------------------------------------------------------------
//: Run the event loop.
void
vgui_wx::run(void)
{
#ifdef DEBUG
  std::cout << "vgui_wx::run()" << std::endl;
#endif

  // not controlling the main loop from vgui_wx
  if (adaptor_embedded_)
  {
    std::cerr << __FILE__ ":embedding adaptor; don't call run!\n";
    return;
  }
  //Tell wxWidgets to start the event loop
  wxTheApp->OnRun();
}

//: Run the next event.
void
vgui_wx::run_one_event(void)
{
  wxTheApp->Dispatch();
}

//: Run until event queue is empty.
void
vgui_wx::run_till_idle(void)
{
  while (wxTheApp->Pending())
  {
    wxTheApp->Dispatch();
    glFlush();
  }
  wxTheApp->ProcessIdle();
}

//: Clear all events from the queue.
void
vgui_wx::flush(void)
{
  glFlush();
  run_till_idle();
}

//: Add an event to the queue.
void
vgui_wx::add_event(const vgui_event &)
{
  // ***** not implemented, yet
  assert(false);
}

//: Quit the application.
void
vgui_wx::quit(void)
{
#ifdef DEBUG
  std::cout << "vgui_wx::quit()" << std::endl;
#endif
  if (top_level_window_)
      top_level_window_->add_close_event();
  
  // not controlling the main loop from vgui_wx
  if (adaptor_embedded_)
  {
    return;
  }

  if (wxTheApp->IsMainLoopRunning())
  {
    wxTheApp->ExitMainLoop();
  }
}

//-------------------------------------------------------------------------
// vgui_wx implementation - window creation.
// vgui_wx_window is a subclass of wxFrame so 
//  wxWidgets should handle cleanup on exit
//-------------------------------------------------------------------------
//: Create a new window with a menubar.
vgui_window *
vgui_wx::produce_window(int width, int height, const vgui_menu & menubar, const char * title)
{
  top_level_window_ = new vgui_wx_window(width, height, menubar, title);
  vgui_window* win_tmp = dynamic_cast<vgui_window*>(top_level_window_);
  return win_tmp;
}

//: Create a new window.
vgui_window *
vgui_wx::produce_window(int width, int height, const char * title)
{
  vgui_wx_window* wx_win = new vgui_wx_window(width, height, title);
  vgui_window* win_tmp = dynamic_cast<vgui_window*>(wx_win);
  return win_tmp;
}

//: Create a new dialog window.
vgui_dialog_impl *
vgui_wx::produce_dialog(const char * name)
{
  return new vgui_wx_dialog_impl(name);
}

//-------------------------------------------------------------------------
// Private helpers - definitions.
//-------------------------------------------------------------------------

namespace
{
   // this function is required to setup the wxApp when 
   // wxWidgets doesn't produce its own main
   //(see the implementation of the macro below)
   // IMPLEMENT_APP_NO_MAIN(vgui_wx_app)
wxAppConsole *
vgui_wx_create_app(void)
{
  wxAppConsole::CheckBuildOptions(WX_BUILD_OPTIONS_SIGNATURE, "your program");
    
    return vgui_wx_app::instance();//new vgui_wx_app (singleton)
  }

} // unnamed namespace
