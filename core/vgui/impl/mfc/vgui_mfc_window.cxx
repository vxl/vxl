// This is core/vgui/impl/mfc/vgui_mfc_window.cxx
#ifdef VCL_NEEDS_PRAGMA_INTERFACE
#pragma implementation
#endif
//:
// \file
// \brief   See vgui_mfc_window.h for a description of this file.
// \author  Marko Bacic, Oxford RRG
// \date    24 July 2000
//
// \verbatim
//  Modifications
//   20-JUL-2001  K.Y.McGaul  Added init_window function carry out common
//                            functionality from all constructors.
// \endverbatim

#include "vgui_mfc_window.h"
#include "vgui_mfc_adaptor.h"
#include "vgui_mfc_mainfrm.h"
#include "vgui_mfc.h"
#include "vgui_mfc_utils.h"
#include "vgui_mfc_statusbar.h"
#include <vgui/vgui.h>
#include <vgui/vgui_gl.h>

static bool first_window = true;

void vgui_mfc_window::init_window(char const *title,
                                  vgui_menu const &menubar,
                                  bool has_menu,
                                  unsigned width, unsigned height,
                                  int posx,
                                  int posy)
{
  //if (first_window) // First time use the main window
  {
    // Get a pointer to the single CWinApp object for the application:
    CWinApp *app = AfxGetApp();
    app->GetMainWnd()->SetWindowText(title);
    pos1 = app->GetFirstDocTemplatePosition();
    CDocTemplate *tmpl = app->GetNextDocTemplate(pos1);
    pos2 = tmpl->GetFirstDocPosition();
    CDocument *pdoc = tmpl->GetNextDoc(pos2);
    pos3 = pdoc->GetFirstViewPosition();
    mfcwin = (vgui_mfc_adaptor *)pdoc->GetNextView(pos3);
    mfcwin->set_window( this );
    if (has_menu)
      vgui_mfc::instance()->utils->set_menu(menubar);
    statusbar = new vgui_mfc_statusbar();
    CWnd *main_wnd = app->GetMainWnd();
    ((vgui_mfc_mainfrm *)main_wnd)->SetStatusBar(statusbar);
    vgui::out.rdbuf(statusbar->statusbuf);
    // This part of code changes the window size to the one requested
    WINDOWPLACEMENT w_placement;
    // Obtain window geometry information
    main_wnd->GetWindowPlacement(&w_placement);
    // Modify bottom right corner coordinates to account for the width
    w_placement.rcNormalPosition.right = w_placement.rcNormalPosition.left+width;
    w_placement.rcNormalPosition.bottom = w_placement.rcNormalPosition.top+height;
    // Store the geometry information back into window
    main_wnd->SetWindowPlacement(&w_placement);
    first_window = false;
  }
}

//: Constructor for window without menubar.
vgui_mfc_window::vgui_mfc_window(char const *title,
                                 unsigned width, unsigned height,
                                 int posx,
                                 int posy)
  : vgui_window()
  , mfcwin(0)
{
  init_window(title, vgui_menu(), false, width, height, posx, posy);
}

//: Constructor for window with menubar.
vgui_mfc_window::vgui_mfc_window(unsigned width, unsigned height,
                                 vgui_menu const &menubar,
                                 char const *title)
  : vgui_window()
  , mfcwin(0)
{
  init_window(title, menubar, true, width, height, 0, 0);
}

//: Destructor
vgui_mfc_window::~vgui_mfc_window()
{
  glFlush();
  delete mfcwin;
  mfcwin = 0;
}

//: Return the vgui_mfc_adaptor associated with this window.
vgui_adaptor* vgui_mfc_window::get_adaptor()
{
  return mfcwin;
}

void vgui_mfc_window::show()
{
  CWnd *wnd = AfxGetApp()->GetMainWnd();
  wnd->ShowWindow(SW_SHOWNORMAL);
}

void vgui_mfc_window::hide()
{
  CWnd *wnd = AfxGetApp()->GetMainWnd();
  wnd->ShowWindow(SW_HIDE);
}

void vgui_mfc_window::enable_hscrollbar(bool show)
{
  CWnd *wnd = AfxGetApp()->GetMainWnd();
  wnd->ShowScrollBar(SB_HORZ,show);
}

void vgui_mfc_window::enable_vscrollbar(bool show)
{
  CWnd *wnd = AfxGetApp()->GetMainWnd();
  wnd->ShowScrollBar(SB_VERT,show);
}

void vgui_mfc_window::iconify()
{
  CWnd *wnd = AfxGetApp()->GetMainWnd();
  wnd->ShowWindow(SW_MINIMIZE);
}

void vgui_mfc_window::reshape(unsigned w, unsigned h)
{
  CWnd *main_wnd = AfxGetApp()->GetMainWnd();
  WINDOWPLACEMENT w_placement;
  // Obtain window geometry information
  main_wnd->GetWindowPlacement(&w_placement);
  // Quick hack, needs more research into MFC land
  // Modify bottom right position to adapt to new shape
  // But, there are margins associated with the scroll bars and
  // menu bar, which need to be determined.
  //Maybe there is a way to resize the active window and that will take
  //care of the other stuff.  JLM 10/3/2002
  int kludge_width_margin = 35, kludge_height_margin=100;

  w_placement.rcNormalPosition.right = w_placement.rcNormalPosition.left+w+kludge_width_margin;
  w_placement.rcNormalPosition.bottom = w_placement.rcNormalPosition.top+h+kludge_height_margin;
  // Store the geometry information back into window
  main_wnd->SetWindowPlacement(&w_placement);
}

void vgui_mfc_window::reposition(int x,int y)
{
  CWnd *main_wnd = AfxGetApp()->GetMainWnd();
  WINDOWPLACEMENT w_placement;

  // Obtain window geometry information
  main_wnd->GetWindowPlacement(&w_placement);
  int width = w_placement.rcNormalPosition.right -
               w_placement.rcNormalPosition.left;
  int height = w_placement.rcNormalPosition.bottom -
               w_placement.rcNormalPosition.top;

  // Modify upper left hand corner
  w_placement.rcNormalPosition.left=x;
  w_placement.rcNormalPosition.top=y;

  // Adjust the lower right accordingly
  w_placement.rcNormalPosition.right=x+width;
  w_placement.rcNormalPosition.bottom=y+height;

  // Store the geometry information back into window
  main_wnd->SetWindowPlacement(&w_placement);
}

void vgui_mfc_window::set_title(const vcl_string &s)
{
  CWinApp *app = AfxGetApp();
  app->GetMainWnd()->SetWindowText(s.c_str());
}

//Set the position of the horizontal scrollbar
int vgui_mfc_window::set_hscrollbar(int pos)
{
  CWnd *wnd = AfxGetApp()->GetMainWnd();
  return wnd->SetScrollPos(SB_HORZ, pos);
}

//Set the position of the vertical scrollbar
int vgui_mfc_window::set_vscrollbar(int pos)
{
  CWnd *wnd = AfxGetApp()->GetMainWnd();
  return wnd->SetScrollPos(SB_VERT, pos);
}
