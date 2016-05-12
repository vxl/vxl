// This is core/vgui/vgui_font_textured.h
#ifndef vgui_font_textured_h_
#define vgui_font_textured_h_
//=========================================================================
//:
// \file
// \brief  vgui_font derived class that uses textured fonts.
// \author Miguel A. Figueroa-Villanueva (miguelf at ieee dot org)
//
// \verbatim
//  Modifications
//   05/27/2006 - File created. (miguelfv)
// \endverbatim
//=========================================================================

#include <vgui/vgui_font.h>
#include <vgui/vgui_gl.h>
template <typename T> class vil_image_view;

//-------------------------------------------------------------------------
//: vgui_font derived class that uses textured fonts.
//-------------------------------------------------------------------------
class vgui_font_textured : public vgui_font
{
 public:
#if 0
  // Default constructor
  vgui_font_textured();
#endif

  //: Constructor - from a font file (BMF font file only, for now).
  vgui_font_textured(const std::string& font_file);

  //: Draw a string of font symbols.
  virtual void draw(const std::string& str) const;

  //: Draw a string of font symbols.
  virtual void draw(const std::string& str, unsigned int size) const;

#if 0
 protected:
#endif

  //: Destructor.
  virtual ~vgui_font_textured();

 private:
  //: Type to store symbol location in texture.
  struct texture_coord
  {
    float x, y, width, height;
  };

  //: Load font from BMF file.
  bool load_bmf_font(const std::string &font_file);

  //: Load OpenGL texture for all symbols.
  void load_texture(const vil_image_view<GLubyte>& image);

  //: Create OpenGL display list for each symbol.
  void create_display_lists();

  //: Draw symbol using OpenGL texture map primitives.
  void gl_draw(unsigned int i) const;

  //: 256 symbol data
  texture_coord symbol_coords_[256];

  //: Base display list identifier (i.e., for the first symbol).
  GLuint display_list_base_id_;

  //: OpenGL texture identifier.
  GLuint texture_id_;
};

#endif // vgui_font_textured_h_
