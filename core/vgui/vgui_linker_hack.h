#ifndef vgui_linker_hack_h_
#define vgui_linker_hack_h_
/*
  fsm@robots.ox.ac.uk
*/

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

#include <vcl_compiler.h>
#include <vgui/vgui_gl.h>

extern int vgui_gtk_tag_function();
extern int vgui_glut_tag_function();
extern int vgui_mfc_tag_function();
extern int vgui_fltk_tag_function();
extern int vgui_glX_tag_function();
extern int vgui_Xm_tag_function();
extern int vgui_qt_tag_function();
extern int vgui_SDL_tag_function();
extern int vgui_accelerate_x11_tag_function();
extern int vgui_accelerate_mfc_tag_function();

inline int vgui_linker_hack()
{
#ifdef HAS_GTK
  vgui_gtk_tag_function();
#endif
#if VGUI_MESA
  vgui_accelerate_x11_tag_function();
#endif
#ifdef HAS_MFC
  vgui_mfc_tag_function();
  vgui_accelerate_mfc_tag_function();
#endif
#ifdef HAS_FLTK
  vgui_fltk_tag_function();
#endif
#ifdef HAS_GLX
  vgui_glX_tag_function();
#endif
#ifdef HAS_MOTIFGL
  vgui_Xm_tag_function();
#endif
#ifdef HAS_QGL
  vgui_qt_tag_function();
#endif
#ifdef HAS_SDL
  vgui_SDL_tag_function();
#endif
#ifdef HAS_GLUT
  vgui_glut_tag_function();
#endif

  return 0;
}

static int vgui_linker_hack_init = vgui_linker_hack();

#ifdef HAS_MFC
# include <vgui/impl/mfc/vgui_mfc_app_init.h>
vgui_mfc_app_init theAppinit;
#endif

#endif
