// This is core/vgui/impl/wx/vgui_wx.cxx
//=========================================================================
//:
// \file
// \brief  wxWidgets implementation of vgui_toolkit.
//
// See vgui_wx.h for details.
//=========================================================================

#include "vgui_wx.h"
#include "vgui_wx_window.h"
#include "vgui_wx_dialog_impl.h"

#include <vcl_cstdlib.h> // for vcl_exit()
#include <vcl_cassert.h>
#include <vcl_iostream.h>

#include <wx/app.h>

//-------------------------------------------------------------------------
// Private helpers - declarations.
//-------------------------------------------------------------------------
namespace
{
  class vgui_wx_app;
  wxAppConsole* vgui_wx_create_app(void);
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
vcl_string vgui_wx::name(void) const { return "wx"; }

//: Constructor - default.
vgui_wx::vgui_wx(void)
  : adaptor_embedded_(true)
{
#ifdef DEBUG
  vcl_cout << "vgui_wx::vgui_wx() - Constructor" << vcl_endl;
#endif
}

//: Destructor.
vgui_wx::~vgui_wx(void)
{
#ifdef DEBUG
  vcl_cout << "vgui_wx::~vgui_wx() - Destructor" << vcl_endl;
#endif
}

//: Initialize the wxWidgets GUI framework.
void vgui_wx::init(int& argc, char** argv)
{
  // ***** ensure this is only entered once...

#ifdef DEBUG
  vcl_cout << "vgui_wx::init()" << vcl_endl;
#endif

  if (wxTheApp)
  {
    // if we are here, then we aren't trying to use vgui in a wxWidgets App
    vcl_cerr << "vgui_wx::init(): wxApp object already exists!" << vcl_endl;
    // ***** exit here... or can we recover from this?
    vcl_exit(-1);
  }

  // Set the app initializer so that we can create the vgui_wx_app.
  wxAppInitializer vgui_wx_app_initializer(
    static_cast<wxAppInitializerFunction>(vgui_wx_create_app));

#ifdef __WXMSW__
  wxSetInstance(GetModuleHandle(0));
  wxApp::m_nCmdShow = 0;
#endif

  // wxWidgets initialization
  if (!wxInitialize(argc, argv))
  {
    vcl_cerr << "vgui_wx::init(): wxInitialize failed!" << vcl_endl;
    // ***** exit here... or can we recover from this?
    vcl_exit(-1);
  }

  // wxApp initialization
  if (!wxTheApp->CallOnInit())
  {
    vcl_cerr << "vgui_wx::init(): wxTheApp->OnInit failed!" << vcl_endl;
    // ***** exit here... or can we recover from this?
    // ***** do we need to call wxUninitialize (or uninit) before exit?
    vcl_exit(-1);
  }

  // adaptor is being extended (i.e., vgui is controlling the event loop)
  adaptor_embedded_ = false;
}

void vgui_wx::uninit(void)
{
#ifdef DEBUG
  vcl_cout << "vgui_wx::uninit()" << vcl_endl;
#endif

  // not controlling the main loop from vgui_wx
  if (adaptor_embedded_)
  {
    vcl_cerr << __FILE__ ":embedding adaptor; don't call uninit!" << vcl_endl;
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
  vcl_cout << "vgui_wx::run()" << vcl_endl;
#endif

  // not controlling the main loop from vgui_wx
  if (adaptor_embedded_)
  {
    vcl_cerr << __FILE__ ":embedding adaptor; don't call run!" << vcl_endl;
    return;
  }

  wxTheApp->OnRun();
}

//: Run the next event.
void vgui_wx::run_one_event(void)
{
  // ***** not implemented, yet
  assert(false);
}

//: Run until event queue is empty.
void vgui_wx::run_till_idle(void)
{
  // ***** not implemented, yet
  assert(false);

  while (wxTheApp->Pending())
  {
    wxTheApp->Dispatch();
  }
}

//: Clear all events from the queue.
void vgui_wx::flush(void)
{
  // ***** not implemented, yet
  assert(false);
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
  vcl_cout << "vgui_wx::quit()" << vcl_endl;
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
      vcl_cout << "vgui_wx_app: Constructor" << vcl_endl;
#endif
    }

    //: Destructor.
    virtual ~vgui_wx_app()
    {
#ifdef DEBUG
      vcl_cout << "vgui_wx_app: Destructor" << vcl_endl;
#endif
    }

    //: Called on app initialization.
    bool OnInit()
    {
#ifdef DEBUG
      vcl_cout << "vgui_wx_app: OnInit()" << vcl_endl;
#endif

      // ***** wxApp's OnInit command parser usually gets in the way...
      return true; //wxApp::OnInit();
    }

    //: Called on app exit.
    virtual int OnExit()
    {
#ifdef DEBUG
      vcl_cout << "vgui_wx_app: OnExit()" << vcl_endl;
#endif

      return wxApp::OnExit();
    }

    //: Called if unhandled exception occurs inside main event loop.
    // Return true to ignore the exception and false to exit the loop.
    virtual bool OnExceptionInMainLoop()
    {
#ifdef DEBUG
      vcl_cout << "vgui_wx_app: OnExceptionInMainLoop()" << vcl_endl;
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
