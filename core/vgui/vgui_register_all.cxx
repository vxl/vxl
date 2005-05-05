// This is core/vgui/vgui_register_all.cxx

//:
// \file
// \author fsm
//
// Toolkits are registered by registering a "tag function" at
// library initialization time. That usually works fine with
// shared builds on sane architectures, but it doesn't work
// for static builds and for shared builds on certain architectures.
//
// Thus, explicitly calling toolkit tag functions is currently
// unavoidable. The purpose of this file is to keep all that
// hackery in one place. The mere act of including this file
// from the source file containing main() will result in tag
// functions being registered before main() is entered.
//
// vgui is supposed to be cross-platform and xcv was supposed
// to work with all toolkits, not just mfc and gtk, even if they
// are the most important ones. So try not to remove stuff unless
// there is a good reason which you are willing to document next
// to the change.
//
// The vgui::select() mechanism should make it unnecessary to
// comment out toolkits merely to stop them being initialized.
// If that mechanism doesn't work, the problem should be fixed
// in vgui.cxx, not here.
//
// Note that vgui::select() and vgui::init() will automatically
// invoke vgui_tag_call() so it is unnecessary to do so here.

// This used to be vgui_linker_hack.h. Since it is impossible portably
// to do the link-time checking of toolkit implementations, I've
// changed things to a compile time check. Thus, this function is
// called on all platforms to initialise whatever toolkits where
// compiled when the base vgui was compiled. As things stand, you
// could still link in an unknown implementation at link time and, if
// you have shared libraries, the unknown toolkit will probably be
// linked in and initialised. However, you're on your own if you
// attempt this. -- Amitha Perera.

#include <vgui/vgui_config.h>

extern int vgui_gtk_tag_function();
extern int vgui_gtk2_tag_function();
extern int vgui_glut_tag_function();
extern int vgui_mfc_tag_function();
extern int vgui_fltk_tag_function();
extern int vgui_glX_tag_function();
extern int vgui_Xm_tag_function();
extern int vgui_qt_tag_function();
extern int vgui_SDL_tag_function();
extern int vgui_accelerate_x11_tag_function();
//extern int vgui_accelerate_mfc_tag_function();

//: Registers all the available toolkit implementations.
// This function is called from vgui.cxx.
int vgui_register_all_implementations()
{
#ifdef VGUI_USE_GTK
  vgui_gtk_tag_function();
#endif
#ifdef VGUI_USE_GTK2
  vgui_gtk2_tag_function();
#endif
#if VGUI_MESA
  vgui_accelerate_x11_tag_function();
#endif
#ifdef VGUI_USE_MFC
  vgui_mfc_tag_function();
  //vgui_accelerate_mfc_tag_function();
#endif
#ifdef VGUI_USE_FLTK
  vgui_fltk_tag_function();
#endif
#ifdef VGUI_USE_GLX
  vgui_glX_tag_function();
#endif
#ifdef VGUI_USE_MOTIFGL
  vgui_Xm_tag_function();
#endif
#ifdef VGUI_USE_QT
  vgui_qt_tag_function();
#endif
#ifdef VGUI_USE_SDL
  vgui_SDL_tag_function();
#endif
#ifdef VGUI_USE_GLUT
  vgui_glut_tag_function();
#endif

  return 0;
}
