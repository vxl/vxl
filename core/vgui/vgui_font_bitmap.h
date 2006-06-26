// This is core/vgui/vgui_font_bitmap.h
#ifndef vgui_font_bitmap_h_
#define vgui_font_bitmap_h_
//=========================================================================
//:
// \file
// \brief  vgui_font derived class that uses hardcoded fonts.
// \author Miguel A. Figueroa-Villanueva (miguelf at ieee dot org)
//
// \verbatim
//  Modifications
//   05/27/2006 - File created. (miguelfv)
// \endverbatim
//=========================================================================

#include <vgui/vgui_font.h>
#include <vgui/vgui_gl.h>

//-------------------------------------------------------------------------
//: vgui_font derived class that uses hardcoded fonts.
// Currently, it only supports a 8x13 size font type.
//-------------------------------------------------------------------------
class vgui_font_bitmap : public vgui_font
{
public:
  enum bitmap_font_type
  {
    BITMAP_FONT_8_13
  };

  //: Constructor - default.
  vgui_font_bitmap(bitmap_font_type t = BITMAP_FONT_8_13);

  //: Draw a string of font symbols.
  virtual void draw(const vcl_string& str) const;

//protected:
  //: Destructor.
  virtual ~vgui_font_bitmap();

private:
  //: Base display list identifier (i.e., for the first symbol).
  GLuint display_list_base_id_;
};

#endif // vgui_font_bitmap_h_
