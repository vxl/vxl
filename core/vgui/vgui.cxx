// This is core/vgui/vgui.cxx
//:
//  \file
// \author Philip C. Pritchett, RRG, University of Oxford
// \date   30 Sep 99
// \brief  See vgui.h for a description of this file.

#include <cstring>
#include <cstdlib>
#include <iostream>
#include "vgui.h"
#include <cassert>
#ifdef _MSC_VER
#  include <vcl_msvc_warnings.h>
#endif

#include <vgui/vgui_macro.h>
#include <vgui/vgui_window.h>
#include <vgui/vgui_tableau.h>
#include <vgui/vgui_adaptor.h>
#include <vgui/vgui_tag.h>
#include <vgui/vgui_toolkit.h>
#include <vgui/internals/vgui_accelerate.h>
#include <vgui/internals/vgui_dialog_impl.h>
#include <vgui/internals/vgui_dialog_extensions_impl.h>

// static data
vgui_toolkit *vgui::instance_ = nullptr;
bool vgui::init_called = false;
bool vgui::quit_called = false;


std::ostream vgui::out(std::cout.rdbuf());


bool vgui_emulate_overlays = false;
bool vgui_glerrors_are_bad = false;
bool vgui_mfc_use_bitmap = true;


// make sure that vgui::uninit is called before the application exits.
struct vgui_uninit_caller
{
  ~vgui_uninit_caller() { vgui::uninit(); }
};

vgui_uninit_caller vgui_the_uniniter_caller_;

//----------------------------------------------------------------------------
//: Remove an argument from a command line argument vec*tor :
static void vgui_remove_arg(unsigned index, int &argc, char **argv)
{
  // NB ISO says argv[argc] is required to be 0, so argv[i+1] is right.
  for (int i=index; i<argc; ++i)
    argv[i]=argv[i+1];
  --argc;
}

//----------------------------------------------------------------------------
// [*] Note on vgui_tag_call():
// This may be the first method on vgui to be called so we
// should call the tag functions now. It is not a problem if
// vgui_tag_call() gets invoked multiple times because (a)
// the tag function are supposed to be idempotent and (b) a
// tag function is called at most once per registration.
bool vgui::exists(char const *toolkit)
{
  vgui_tag_call(); // see [*] above.

  std::vector<vgui_toolkit*> *tk = vgui_toolkit::registry();
  for (unsigned int i=0; i<tk->size(); ++i)
    if ( (*tk)[i]->name() == toolkit )
      return true;
  return false;
}

//----------------------------------------------------------------------------
//: Method for selecting a specific toolkit.
//  This will abort() if given a toolkit which is not available.
void vgui::select(char const *toolkit)
{
  vgui_tag_call(); // see [*] above.

  std::vector<vgui_toolkit*> *tk = vgui_toolkit::registry();
  for (unsigned int i=0; i<tk->size(); ++i) {
    if ( (*tk)[i]->name() == toolkit ) {
      instance_ = (*tk)[i];
      return;
    }
  }
  vgui_macro_warning << "no such toolkit \'" << toolkit << "\' -- std::abort()ing\n";
  std::abort();
}

//----------------------------------------------------------------------------
//: Select a toolkit from command line arguments and environment variables.
bool vgui::select(int &argc, char **argv)
{
  vgui_tag_call(); // see [*] above.

  // look for --factory=name
  for (int i=1; i<argc; ) {
    if (std::strncmp(argv[i],"--factory=",10) == 0) {
      instance_ = vgui_toolkit::lookup(argv[i]+10);
      vgui_remove_arg(i, argc, argv);
    }
    else
      ++ i;
  }

  // if there is no instance set (with --factory), check the environment
  // variable 'vgui' :
  if (! instance_) {
    char const *env_name = getenv("vgui");
    if (env_name)
      instance_ = vgui_toolkit::lookup(env_name);
  }

  // return false if instance_ is still not set:
  return instance_ ? true : false;
}


//----------------------------------------------------------------------------
//: Initialise the selected toolkit passing it the given command line.
void vgui::init(int &argc, char **argv)
{
  vgui_tag_call(); // see [*] above.

  // avoid double init.
  assert(! init_called);
  init_called = true;

  // list the toolkits registered :
  std::cerr << "vgui : registered ";
  for (unsigned i=0; i<vgui_toolkit::registry()->size(); ++i)
    std::cerr << '\'' << (*vgui_toolkit::registry())[i]->name() << "\' ";
  std::cerr << std::endl;

  // if no toolkit was selected, try using the command line arguments.
  if (! instance_)
    select(argc, argv);

  // if there is still no instance, take the first one registered :
  if (! instance_) {
    if (! vgui_toolkit::registry()->empty())
      instance_ = vgui_toolkit::registry()->front();
  }

  // abort if no toolkit has been selected.
  if (! instance_) {
    vgui_macro_warning << "failed to find a toolkit implementation - std::abort()ing.\n";
    std::abort();
  }
  assert(instance_); // need an instance.

  // Look for command line options.
  for (int i=1; i<argc; )
  {
    if (std::strncmp(argv[i],"--factory=",10) == 0)
    {
      // --factory=<name>
      vgui_macro_warning << "superfluous command line argument \'"
                         << argv[i] << "\' ignored\n";
      vgui_remove_arg(i, argc, argv);
    }
    else if (std::strncmp(argv[i],"--no-accel",10) == 0)
    {
      // matches --no-accel*
      vgui_accelerate::vgui_no_acceleration = true;
      vgui_remove_arg(i, argc, argv);
    }
    else if (std::strcmp(argv[i],"--mfc-use-bitmap") == 0)
    {
      vgui_mfc_use_bitmap = true;
      vgui_remove_arg(i, argc, argv);
    }
    else if (std::strcmp(argv[i],"--mfc-use-gl") == 0)
    {
      vgui_mfc_use_bitmap = false;
      vgui_remove_arg(i, argc, argv);
    }
    else if (std::strcmp(argv[i],"--emulate-overlays") == 0) {
      vgui_emulate_overlays = true;
      vgui_remove_arg(i, argc, argv);
    }
    else if (std::strcmp(argv[i],"--glerrors-are-bad") == 0) {
      vgui_glerrors_are_bad = true;
      vgui_remove_arg(i, argc, argv);
    }
    else
      ++i;
  }

  // print a message prior to initializing the toolkit.
  std::cerr << "vgui : initialize \'" << instance_->name() << "\'\n";
  instance_->init(argc, argv);
}


void vgui::uninit()
{
  std::cout << "vgui::uninit called" << std::endl;
  // make sure uninit does something only once
  static bool uninit_called = false;
  if ( !uninit_called && init_called && instance_ )
    instance_->uninit();
  uninit_called = true;
}

//----------------------------------------------------------------------------
//: Produce window with menubar.
vgui_window *vgui::produce_window(int width,
                                  int height,
                                  vgui_menu const &menubar,
                                  std::string const &title)
{
  if (instance_)
    return instance_->produce_window(width, height, menubar, title.c_str());
  else {
    vgui_macro_warning << "no toolkit selected\n";
    return nullptr;
  }
}

//----------------------------------------------------------------------------
//: Produce window without menubar.
vgui_window *vgui::produce_window(int width,
                                  int height,
                                  std::string const &title)
{
  if (instance_)
    return instance_->produce_window(width, height, title.c_str());
  else {
    vgui_macro_warning << "no toolkit selected\n";
    return nullptr;
  }
}

//----------------------------------------------------------------------------
//: Produce dialog box.
vgui_dialog_impl *vgui::produce_dialog(std::string const &name)
{
  if (instance_)
    return instance_->produce_dialog(name.c_str());
  else {
    vgui_macro_warning << "no toolkit selected\n";
    return nullptr;
  }
}

//----------------------------------------------------------------------------
//: Produce dialog box.
vgui_dialog_extensions_impl *vgui::produce_extension_dialog(std::string const &name)
{
  if (instance_)
    return instance_->produce_dialog_extension(name.c_str());
  else {
    vgui_macro_warning << "no toolkit selected\n";
    return nullptr;
  }
}
//----------------------------------------------------------------------------
//: Quit application.
void vgui::quit()
{
  quit_called = true;
  if (instance_)
    instance_->quit();
  else {
    vgui_macro_warning << "no instance_ to call quit() on\n";
    //exit(1);
  }
}


bool vgui::quit_was_called()
{
  return quit_called;
}


//----------------------------------------------------------------------------
//: Run until quit is called.
int vgui::run()
{
  if (instance_) {
    instance_->run();
    return 0;
  }
  else {
    vgui_macro_warning << "no toolkit selected\n";
    return 1;
  }
}

//----------------------------------------------------------------------------
//: Run the next event in the event queue.
void vgui::run_one_event()
{
  if (instance_)
    instance_->run_one_event();
  else
    vgui_macro_warning << "no toolkit selected\n";
}

//----------------------------------------------------------------------------
//: Run all events in the event queue.
void vgui::run_till_idle()
{
  if (instance_)
    instance_->run_till_idle();
  else
    vgui_macro_warning << "no toolkit selected\n";
}

//----------------------------------------------------------------------------
//: Remove all events from the event queue.
void vgui::flush()
{
  if (instance_)
    instance_->flush();
  else
    vgui_macro_warning << "no toolkit selected\n";
}

//-----------------------------------------------------------------------------
//: Add event to the event queue.
void vgui::add_event(vgui_event const& e)
{
  if (instance_)
    instance_->add_event(e);
  else
    vgui_macro_warning << "no toolkit selected\n";
}

//-----------------------------------------------------------------------------
//: Display this tableau and run till dead (no menubar).
int vgui::run(vgui_tableau_sptr const& tableau, int width, int height,
              std::string const &title)
{
  adapt(tableau, width, height, title);
  return vgui::run();
}

//-----------------------------------------------------------------------------
//: Display this tableau and run till dead (with menubar).
int vgui::run(vgui_tableau_sptr const& tableau, int width, int height,
              vgui_menu const &menubar, std::string const &title)
{
  adapt(tableau, width, height, menubar, title);
  return vgui::run();
}

//-----------------------------------------------------------------------------
//: Create the vgui_window but don't run it (no menubar).
vgui_window *vgui::adapt(vgui_tableau_sptr const& tableau, int width,
                         int height, std::string const &title)
{
  vgui_window *win = vgui::produce_window(width, height, title);
  win->get_adaptor()->set_tableau(tableau);
  win->show();
  return win;
}

//-----------------------------------------------------------------------------
//: Create the vgui_window but don't run it (with menubar).
vgui_window *vgui::adapt(vgui_tableau_sptr const& tableau, int width,
                         int height, vgui_menu const &mb, std::string const &title)
{
  vgui_window *win = vgui::produce_window(width, height, mb, title);
  win->get_adaptor()->set_tableau(tableau);
  win->show();
  return win;
}
