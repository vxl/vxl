//this-sets-emacs-to-*-c++-*-mode
#ifndef bgui_style_h_
#define bgui_style_h_
//--------------------------------------------------------------------------------
//:
// \file
// \brief A local version of vgui_style. So far identical to vgui_style but 
//        with a smart pointer and set methods.
// \author
//   J.L. Mundy
//
// \verbatim
//  Modifications:
//   J.L. Mundy December 22, 2002
// \endverbatim
//--------------------------------------------------------------------------------
#include <vbl/vbl_ref_count.h>
#include <vgui/vgui_style.h>


class bgui_style : public vgui_style, public vbl_ref_count
{
 public:
  //: Constructor
  bgui_style(const float r, const float g, const float b, 
             const float s, const float w);
  void change_color(const float r, const float g, const float b);

  void change_point_size(const float s);

  void change_line_width(const float w);

  //: change the style attributes of sty to those of this bgui_style
  void clone_style(vgui_style* sty);

  //: get a new vgui_style that is a copy of this bgui_style
  vgui_style* style_new();

  //: Destructor
  ~bgui_style() {;}

};


#endif // bgui_style_h_
