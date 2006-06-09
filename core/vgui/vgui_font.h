// This is core/vgui/vgui_font.h
#ifndef vgui_font_h_
#define vgui_font_h_
//=========================================================================
//:
// \file
// \brief  Font class for rendering text in OpenGL.
// \author Miguel A. Figueroa-Villanueva (miguelf at ieee dot org)
//
// \verbatim
//  Modifications
//   05/19/2006 - File created. (miguelfv)
// \endverbatim
//=========================================================================

#include <vcl_string.h>
#include <vbl/vbl_ref_count.h>

//-------------------------------------------------------------------------
//: Abstract font class.
//-------------------------------------------------------------------------
class vgui_font : public vbl_ref_count
{
public:
  //: Constructor - default.
  vgui_font() { }

  //: Draw a string of font symbols.
  virtual void draw(const vcl_string& str) const = 0;

  //: Draw a string of font symbols.
  virtual void draw(const vcl_string& str, unsigned int size) const;

protected:
  //: Destructor.
  virtual ~vgui_font() { }

private:
  // Disable copy constructor and assignment.
  vgui_font(const vgui_font&);
  vgui_font& operator=(const vgui_font&);
};

#endif // vgui_font_h_
