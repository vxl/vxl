// This is core/vgui/vgui_text_put.cxx
//:
// \file
// \author Philip C. Pritchett, RRG, University of Oxford
// \date   19 Oct 99
// \brief  See vgui_text_put.h for a description of this file.


#include "vgui_text_put.h"
#include <vgui/vgui_gl.h>
#include <vgui/vgui_config.h>

#if defined(HAS_GLUT) && defined(VGUI_USE_GLUT)
#include <vgui/vgui_glut.h>

// This ugliness is used to make sure we don't try to initialize GLUT
// multiple times if glut is used as the toolkit too.
bool glut_was_initialized = false;

void vgui_text_put(const char* str, unsigned size)
{
  if( ! glut_was_initialized ) {
    glut_was_initialized = true;
    int argc = 0;
    char* argv[1] = { nullptr };
    glutInit( &argc, argv );
  }
  if (size > 20) {
    for (unsigned k=0; str[k]; ++k){
      int ic = static_cast<int>(str[k]);
      glutBitmapCharacter(GLUT_BITMAP_TIMES_ROMAN_24, ic);
    }
  }
  else if(size > 17){
    for (unsigned k=0; str[k]; ++k){
      int ic = static_cast<int>(str[k]);
      glutBitmapCharacter(GLUT_BITMAP_HELVETICA_18, ic);
    }
  } else if (size > 10){
    for (unsigned k=0; str[k]; ++k){
      int ic = static_cast<int>(str[k]);
      glutBitmapCharacter(GLUT_BITMAP_HELVETICA_12, ic);
    }
  }else{
    for (unsigned k=0; str[k]; ++k){
      int ic = static_cast<int>(str[k]);
      glutBitmapCharacter(GLUT_BITMAP_TIMES_ROMAN_10, ic);
    }
  }
}
int vgui_text_length(const char* str, unsigned size){
  if( ! glut_was_initialized ) {
    glut_was_initialized = true;
    int argc = 0;
    char* argv[1] = { nullptr };
    glutInit( &argc, argv );
  }
  int w = 0;
  if (size > 20) {
    for (unsigned k=0; str[k]; ++k){
      int ic = static_cast<int>(str[k]);
      w+= glutBitmapWidth(GLUT_BITMAP_TIMES_ROMAN_24, ic);
    }
  } else if(size > 17){
    for (unsigned k=0; str[k]; ++k){
      int ic = static_cast<int>(str[k]);
      w+= glutBitmapWidth(GLUT_BITMAP_HELVETICA_18, ic);
    }
  } else if (size > 10){
    for (unsigned k=0; str[k]; ++k){
      int ic = static_cast<int>(str[k]);
      w+=  glutBitmapWidth(GLUT_BITMAP_HELVETICA_12, ic);
    }
  }else{
    for (unsigned k=0; str[k]; ++k){
      int ic = static_cast<int>(str[k]);
      w+=  glutBitmapWidth(GLUT_BITMAP_TIMES_ROMAN_10, ic);
    }
  }
  return w;
}
int vgui_char_width(unsigned char c, unsigned size){
  if( ! glut_was_initialized ) {
    glut_was_initialized = true;
    int argc = 0;
    char* argv[1] = { nullptr };
    glutInit( &argc, argv );
  }
  int ic = static_cast<int>(c);
  if (size > 20) {
    return glutBitmapWidth(GLUT_BITMAP_TIMES_ROMAN_24, ic);
  } else if(size > 17){
    return glutBitmapWidth(GLUT_BITMAP_HELVETICA_18, ic);
  } else if (size > 10){
    return  glutBitmapWidth(GLUT_BITMAP_HELVETICA_12, ic);
  }else{
    return  glutBitmapWidth(GLUT_BITMAP_TIMES_ROMAN_10, ic);
  }
  return 0;
}
#else
#include <vgui/vgui_font_bitmap.h>
void vgui_text_put(char const *str, unsigned int size)
{
  vgui_font_bitmap font(vgui_font_bitmap::BITMAP_FONT_8_13);
  font.draw(str);
}
int vgui_text_length(const unsigned char *s, unsigned size){
  int w = 0;
  int i = 0;
  while(s[i]!=0){
    i++; w+=8;
  }
  return w;
}
int vgui_char_width(unsigned char c, unsigned size){
  return 8;
}
#endif
