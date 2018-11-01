// This is core/vgui/impl/mfc/vgui_mfc_window.h
#ifndef vgui_mfc_window_h_
#define vgui_mfc_window_h_
//:
// \file
// \brief   The mfc implementation of vgui_window
// \author  Marko Bacic, Oxford RRG
// \date    24-JUL-2000
//
//  Contains class vgui_mfc_window
//
// \verbatim
//  Modifications
//   24-JUL-2000  Marko Bacic, Oxford RRG - Initial version.
//   17-AUG-2000  Marko Bacic, Oxford RRG - Now windows can have specified size
//   23-AUG-2000  Marko Bacic, Oxford RRG - Added support for scrollbars
//   20-JUL-2001  K.Y.McGaul  Added init_window function carry out common
//                            functionality from all constructors.
//   23-DEC-2002  J.L. Mundy added scrollbar positioning methods
// \endverbatim

class vgui_mfc_adaptor;
class vgui_mfc_statusbar;

#include <vgui/vgui_menu.h>
#include <vgui/vgui_window.h>
#include <afxwin.h>

//: The MFC implementation of vgui_window.
//  Based on vgui_glut_win.
class vgui_mfc_window : public vgui_window
{
 public:
  //: Initialise window - common functionality called by all constructors.
  void init_window(char const *title, vgui_menu const &menubar,
                   bool has_menu, unsigned width, unsigned height, int posx, int posy);

  //: Constructor for window without menubar.
  vgui_mfc_window(char const *title, unsigned w, unsigned h,
                  int posx =-1, int posy=-1);

  //: Constructor for window with menubar.
  vgui_mfc_window(unsigned w,unsigned h,vgui_menu const &menubar,
                  char const *title);

  //: Destructor - delete this vgui_mfc_window.
  ~vgui_mfc_window();

  void set_menubar(vgui_menu const &m);

  //: Return the vgui_mfc_adaptor associated with this window.
  vgui_adaptor *get_adaptor();

  //: Return the vgui_mfc_adaptor associated with this window.
  vgui_statusbar *get_statusbar();

  //
  void show();
  void hide();
  void iconify();
  //: Enable horizontal scrollbar on window.
  void enable_hscrollbar(bool);
  //: Enable vertical scrollbar on window.
  void enable_vscrollbar(bool);
  void reshape(unsigned, unsigned);
  void reposition(int,int);
  void set_title(std::string const &);

  //: Set the position of the horizontal scrollbar, returns old position
  virtual int set_hscrollbar(int pos);

  //: Set the position of the vertical scrollbar, returns old position
  virtual int set_vscrollbar(int pos);

  POSITION pos1, pos2, pos3;

 private:
  vgui_menu          menubar;

  vgui_mfc_adaptor   *mfcwin;
  vgui_mfc_statusbar *statusbar;
};

#endif // vgui_mfc_window_h_
