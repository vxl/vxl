#include "bgui_style.h"
#include <vcl_iostream.h>
//--------------------------------------------------------------------------
//  bgui style 
//--------------------------------------------------------------------------

bgui_style::bgui_style(const float r, const float g, const float b, 
                       const float s, const float w)
{
  change_color(r, g, b);
  change_point_size(s);
  change_line_width(w);
}

void bgui_style::change_color(const float r,
                              const float g,
                              const float b)
{
  rgba[0]=r;  rgba[1]=g;
  rgba[2]=b;
}

void bgui_style::change_point_size(const float s)
{
  point_size=s;
}

void bgui_style::change_line_width(const float w)
{
  line_width=w;
}

void bgui_style::clone_style(vgui_style* sty)
{
  if (!sty)
    {
      vcl_cout << "In bgui_style::clone_style(.) - null input style\n";
      return;
    }
  for (int i = 0; i<3; i++)
    sty->rgba[i]=rgba[i];
  sty->point_size = point_size;
  sty->line_width = line_width;
}

vgui_style* bgui_style::style_new()
{
  vgui_style* s = new vgui_style();
  this->clone_style(s);
  return s;
}
