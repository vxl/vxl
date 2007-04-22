// This is core/vgui/vgui_font_textured.cxx
//=========================================================================
//:
// \file
// \brief  vgui_font derived class that uses hardcoded fonts.
//
// See vgui_font_textured.h for details.
//=========================================================================

#include <vgui/vgui_font_textured.h>
#include <vgui/vgui_macro.h>

#include <vil/vil_load.h>
#include <vil/vil_copy.h>
#include <vil/vil_plane.h>
#include <vil/vil_stream_fstream.h>
#include <vil/vil_stream_section.h>

#include <vcl_cassert.h>

//-------------------------------------------------------------------------
// Helper functions.
//-------------------------------------------------------------------------
namespace
{
  void swap32(char *a, unsigned n)
  {
    char c;
    for (unsigned int i = 0; i < n * 4; i += 4)
    {
      c = a[i];
      a[i] = a[i+3];
      a[i+3] = c;
      c = a[i+1];
      a[i+1] = a[i+2];
      a[i+2] = c;
    }
  }
}

//-------------------------------------------------------------------------
// vgui_font_textured implementation.
//-------------------------------------------------------------------------
//: Constructor - from a font file (BMF font file only, for now).
vgui_font_textured::vgui_font_textured(const vcl_string& font_file)
  : display_list_base_id_(0)
  , texture_id_(0)
{
  vgui_macro_report_errors;

  if (load_bmf_font(font_file)) { create_display_lists(); }

  vgui_macro_report_errors;
}

vgui_font_textured::~vgui_font_textured(void)
{
  if (display_list_base_id_) { glDeleteLists(display_list_base_id_, 256); }
  if (texture_id_) { glDeleteTextures(1, &texture_id_); }
}

//: Draw font symbol.
void vgui_font_textured::gl_draw(unsigned int i) const
{
  // The texture coordinates have a positive y-axis pointing up
  // (cartesian coordinate system), while the OpenGL context in vgui
  // has a positive y-axis pointing down (image coordinate system).
  glBegin(GL_QUADS);

    glTexCoord2f(symbol_coords_[i].x,
                 1.f - symbol_coords_[i].y - symbol_coords_[i].height);
    glVertex2f(0.f, 0.f);

    glTexCoord2f(symbol_coords_[i].x + symbol_coords_[i].width,
                 1.f - symbol_coords_[i].y - symbol_coords_[i].height);
    glVertex2f(symbol_coords_[i].width, 0.f);

    glTexCoord2f(symbol_coords_[i].x + symbol_coords_[i].width,
                 1.f - symbol_coords_[i].y);
    glVertex2f(symbol_coords_[i].width, symbol_coords_[i].height);

    glTexCoord2f(symbol_coords_[i].x,
                 1.f - symbol_coords_[i].y);
    glVertex2f(0.f, symbol_coords_[i].height);

  glEnd();

  const float spacing_ = 0.001f;
  glTranslatef(symbol_coords_[i].width + spacing_, 0.f, 0.f);
}

//: Draw a string of font symbols.
void vgui_font_textured::draw(const vcl_string& str) const
{
  draw(str, 24);
}

//: Draw a string of font symbols.
void vgui_font_textured::draw(const vcl_string& str,
                              unsigned int size) const
{
  if (!display_list_base_id_) { return; }

  GLboolean  prev_texture_enabled;
  glGetBooleanv(GL_TEXTURE_2D, &prev_texture_enabled);

  glEnable(GL_TEXTURE_2D);
  glBindTexture(GL_TEXTURE_2D, texture_id_);

  glPushMatrix();

  // scale to size
  const float scale = size / symbol_coords_[0].height;
  glScalef(scale, scale, 1.f);

  // print the string
  glPushAttrib(GL_LIST_BIT);
  glListBase(display_list_base_id_);
  glCallLists(str.size(), GL_UNSIGNED_BYTE, str.c_str());
  glPopAttrib();

  glPopMatrix();

  if (!prev_texture_enabled) { glDisable(GL_TEXTURE_2D); }
}

//: Load font from file.
bool vgui_font_textured::load_bmf_font(const vcl_string &font_file)
{
  // create and open the file
  vil_smart_ptr<vil_stream> stream
    = new vil_stream_fstream(font_file.c_str(), "r");
  if (!stream->ok())
  {
    vcl_cerr << __FILE__ ":couldn't open font file:"
             << font_file << vcl_endl;
    return false;
  }

  // read format magic number
  char format[3];
  if (stream->read(format, 3) != 3)
  {
    vcl_cerr << __FILE__ ":couldn't read magic number!\n";
    return false;
  }
  if (vcl_string(format, 3) != "BMF")
  {
    vcl_cerr << __FILE__ ":not a BMF file!\n";
    return false;
  }

  // read font name
  char font_name[96];
  if (stream->read(font_name, 96) != 96)
  {
    vcl_cerr << __FILE__ ":couldn't read font name!\n";
    return false;
  }

  // *****
  // read 256 x 4 floats (assuming float is of size 4 bytes)
  assert(sizeof(float) == 4);
  float sum = 0.f;
  for (unsigned int i = 0; i < 256; i++)
  {
    sum += stream->read(&symbol_coords_[i].x,      4);
    sum += stream->read(&symbol_coords_[i].y,      4);
    sum += stream->read(&symbol_coords_[i].width,  4);
    sum += stream->read(&symbol_coords_[i].height, 4);

#if VXL_BIG_ENDIAN
    swap32(&symbol_coords_[i], 4);
#endif
  }
  if (sum != 256*4*4) // 256 symbols * 4 floats * 4 bytes
  {
    vcl_cerr << __FILE__ ":couldn't read symbol info!\n";
    return false;
  }

  // read sgi (iris) embedded image
  vil_smart_ptr<vil_stream> sgi_section
    = new vil_stream_section(stream.ptr(), 3 + 96 + 256*4*4);
  vil_image_view<GLubyte> texture_image
    = vil_load_image_resource_raw(sgi_section.ptr())->get_view();

  load_texture(texture_image);

  return true;
}

//: Load OpenGL texture for all symbols.
void vgui_font_textured::load_texture(const vil_image_view<GLubyte>& image)
{
  vgui_macro_report_errors;

  // set row alignment to byte aligned
  glPixelStorei(GL_UNPACK_ALIGNMENT, 1);

  // generate a texture id
  glGenTextures(1, &texture_id_);

  // bind this texture to a target object
  glBindTexture(GL_TEXTURE_2D, texture_id_);

  // sets some properties on the target object
  //glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
  //glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
  glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
  glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

  vil_image_view<GLubyte> texture(image.ni(), image.nj());
  vil_copy_reformat(vil_plane(image, 1), texture);

  // specify the 2D texture image
  glTexImage2D(GL_TEXTURE_2D,          // texture object
               0,                      // resolution level
               GL_RGBA,                // OpenGL internalformat
               image.ni(),             // width
               image.nj(),             // height
               0,                      // add border
               GL_LUMINANCE,           // pixel format
               GL_UNSIGNED_BYTE,       // pixel data type
               texture.memory_chunk()->data());  // pointer to image

  vgui_macro_report_errors;
}

//: Create OpenGL display list for each symbol.
void vgui_font_textured::create_display_lists(void)
{
  vgui_macro_report_errors;

  // build the display list for each symbol
  display_list_base_id_ = glGenLists(256);

  for (unsigned int i = 0; i < 256; i++)
  {
    glNewList(display_list_base_id_ + i, GL_COMPILE);
    gl_draw(i);
    glEndList();
  }

  vgui_macro_report_errors;
}
