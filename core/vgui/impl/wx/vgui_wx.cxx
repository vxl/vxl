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
#include "vgui_wx_window.h"
#include "vgui_wx_dialog_impl.h"
#include <vgui/vgui_gl.h>

#ifdef _MSC_VER
#  include <vcl_msvc_warnings.h>
#endif
#include <cassert>

#include <wx/app.h>
#include <wx/log.h>
#include <wx/wxchar.h>
#include <wx/strconv.h>
#ifdef __WXMSW__
#include <wx/msw/private.h>
#endif

//-------------------------------------------------------------------------
// Private helpers - declarations.
//-------------------------------------------------------------------------
namespace
{
  class vgui_wx_app;
  wxAppConsole* vgui_wx_create_app(void);
  wxChar** g_wxCharArgv = NULL;
  int g_Argc = 0;
}

//-------------------------------------------------------------------------
// vgui_wx implementation - construction & destruction.
//-------------------------------------------------------------------------
//: Singleton method instance.
vgui_wx* vgui_wx::instance()
{
  static vgui_wx* instance_ = new vgui_wx;
  return instance_;
}

//: Returns the name of the GUI toolkit ("wx").
std::string vgui_wx::name(void) const { return "wx"; }

//: Constructor - default.
vgui_wx::vgui_wx(void)
  : adaptor_embedded_(true)
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
void vgui_wx::init(int& argc, char** argv)
{
  // ***** ensure this is only entered once...

#ifdef DEBUG
  std::cout << "vgui_wx::init()" << std::endl;
#endif

  if (wxTheApp)
  {
    // if we are here, then we aren't trying to use vgui in a wxWidgets App
    std::cerr << "vgui_wx::init(): wxApp object already exists!\n";
    // ***** exit here... or can we recover from this?
    std::exit(-1);
  }

  // Set the app initializer so that we can create the vgui_wx_app.
  wxAppInitializer vgui_wx_app_initializer(
    static_cast<wxAppInitializerFunction>(vgui_wx_create_app));

#ifdef __WXMSW__
  wxSetInstance(GetModuleHandle(0));
  wxApp::m_nCmdShow = 0;
#endif

  // If necessary, convert the char** argv to the Unicode wxChar**
  // version.
  wxChar** wxArgv;
#if wxUSE_UNICODE
  g_Argc = argc;
  g_wxCharArgv = new wxChar*[argc+1];
  for ( int cnt = 0; cnt < argc; ++cnt ) {
    std::size_t len = wxConvLocal.MB2WC( NULL, argv[cnt], 0 );
    g_wxCharArgv[cnt] = new wxChar[len+1];
    wxConvLocal.MB2WC( g_wxCharArgv[cnt], argv[cnt], len+1 );
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
  //wxLog* logger = new wxLogStderr;
  //wxLog::SetActiveTarget(logger);
  wxLog::AddTraceMask(wxTRACE_RefCount);
}

void vgui_wx::uninit(void)
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

  // ***** This call is causing system crashes, in WinXP?!?
  //wxUninitialize();

  // ***** Memory should be managed elsewhere... smart_ptr's???
  //for (unsigned int i = 0; i < windows_to_delete_.size(); i++)
  //{
  //  delete windows_to_delete_[i]; // ***** what if user deleted it???
  //  windows_to_delete_.clear();
  //}

  // If we convert the char** argv to a wxChar** version, free our
  // conversion now.
#if wxUSE_UNICODE
  for ( int cnt = 0; cnt < g_Argc; ++cnt ) {
    delete[] g_wxCharArgv[cnt];
  }
  delete g_wxCharArgv;
  g_wxCharArgv = NULL;
  g_Argc = 0;
#endif
}

//-------------------------------------------------------------------------
// vgui_wx implementation - event handling.
// - These should only be called if creating the application in vgui_wx
//   (i.e., we are extending the adaptor instead of embedding it).
//
// - ***** ensure that these are only called if not embedding...
//-------------------------------------------------------------------------
//: Run the event loop.
void vgui_wx::run(void)
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

  wxTheApp->OnRun();
}

//: Run the next event.
void vgui_wx::run_one_event(void)
{
  wxTheApp->Dispatch();
}

//: Run until event queue is empty.
void vgui_wx::run_till_idle(void)
{
  while (wxTheApp->Pending())
  {
    wxTheApp->Dispatch();
    glFlush();
  }
  wxTheApp->ProcessIdle();
}

//: Clear all events from the queue.
void vgui_wx::flush(void)
{
  glFlush();
  run_till_idle();
}

//: Add an event to the queue.
void vgui_wx::add_event(const vgui_event&)
{
  // ***** not implemented, yet
  assert(false);
}

//: Quit the application.
void vgui_wx::quit(void)
{
#ifdef DEBUG
  std::cout << "vgui_wx::quit()" << std::endl;
#endif

  // not controlling the main loop from vgui_wx
  if (adaptor_embedded_) { return; }

  if (wxTheApp->IsMainLoopRunning()) { wxTheApp->ExitMainLoop(); }
}

//-------------------------------------------------------------------------
// vgui_wx implementation - window creation.
// ***** all of these should return smart pointers???
//-------------------------------------------------------------------------
//: Create a new window with a menubar.
vgui_window* vgui_wx::produce_window(int width, int height,
                                     const vgui_menu& menubar,
                                     const char* title)
{
  vgui_window* win_tmp = new vgui_wx_window(width, height, menubar, title);
  windows_to_delete_.push_back(win_tmp);
  return win_tmp;
}

//: Create a new window.
vgui_window* vgui_wx::produce_window(int width, int height,
                                     const char* title)
{
  vgui_window* win_tmp = new vgui_wx_window(width, height, title);
  windows_to_delete_.push_back(win_tmp);
  return win_tmp;
}

//: Create a new dialog window.
vgui_dialog_impl* vgui_wx::produce_dialog(const char* name)
{
  return new vgui_wx_dialog_impl(name);
}

//-------------------------------------------------------------------------
// Private helpers - definitions.
//-------------------------------------------------------------------------
namespace
{
  class vgui_wx_app : public wxApp
  {
   public:
    //: Constructor - default.
    vgui_wx_app()
    {
#ifdef DEBUG
      std::cout << "vgui_wx_app: Constructor" << std::endl;
#endif
    }

    //: Destructor.
    virtual ~vgui_wx_app()
    {
#ifdef DEBUG
      std::cout << "vgui_wx_app: Destructor" << std::endl;
#endif
    }

    //: Called on app initialization.
    bool OnInit()
    {
#ifdef DEBUG
      std::cout << "vgui_wx_app: OnInit()" << std::endl;
#endif

      // ***** wxApp's OnInit command parser usually gets in the way...
      return true; //wxApp::OnInit();
    }

    //: Called on app exit.
    virtual int OnExit()
    {
#ifdef DEBUG
      std::cout << "vgui_wx_app: OnExit()" << std::endl;
#endif

      return wxApp::OnExit();
    }

    //: Called if unhandled exception occurs inside main event loop.
    // Return true to ignore the exception and false to exit the loop.
    virtual bool OnExceptionInMainLoop()
    {
#ifdef DEBUG
      std::cout << "vgui_wx_app: OnExceptionInMainLoop()" << std::endl;
#endif

      return false;
    }
  };

  //IMPLEMENT_APP_NO_MAIN(vgui_wx_app)
  wxAppConsole* vgui_wx_create_app(void)
  {
    wxAppConsole::CheckBuildOptions(WX_BUILD_OPTIONS_SIGNATURE,
                                    "your program");
    return new vgui_wx_app;
  }

  //static vgui_wx_app& wxGetApp() { return *(vgui_wx_app *)wxTheApp; }

} // unnamed namespace
