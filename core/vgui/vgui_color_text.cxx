// This is core/vgui/vgui_color_text.cxx
#include "vgui_color_text.h"
//:
// \file
// \brief  See vgui_color_text.h for a description of this file.
// \author K.Y.McGaul
// \date   25-FEB-2000
//
// \verbatim
//  Modifications
//   25-FEB-2000 K.Y.McGaul - Initial version.
// \endverbatim

#include <vcl_cstdio.h>
#include <vcl_iostream.h>
#include <vgui/vgui_macro.h>

static bool debug = false;

static float red;
static float green;
static float blue;

vcl_string colors[][2] = {
  { "white",          "1.000 1.000 1.000" },
  { "black",          "0.000 0.000 0.000" },
  { "blue",           "0.000 0.000 1.000" },
  { "red",            "1.000 0.000 0.000" },
  { "green",          "0.000 1.000 0.000" },
  { "yellow",         "1.000 1.000 0.000" },
  { "grey",           "0.745 0.745 0.745" },
  { "orange",         "1.000 0.647 0.000" },
  { "pink",           "1.000 0.752 0.796" },
  { "purple",         "0.627 0.125 0.941" },
  { "cyan",           "0.000 1.000 1.000" },
  { "brown",          "0.647 0.165 0.165" },
  { "light blue",     "0.678 0.847 0.902" },
  { "sky blue",       "0.529 0.808 0.922" },
  { "dark blue",      "0.000 0.000 0.545" },
  { "light pink",     "1.000 0.714 0.757" },
  { "deep pink",      "1.000 0.078 0.576" },
  { "orange red",     "1.000 0.270 0.000" },
  { "light green",    "0.565 0.933 0.565" },
  { "dark green",     "0.000 0.392 0.000" },
  { "gold",           "1.000 0.843 0.000" },
  { "tan",            "0.824 0.706 0.549" },
  { "dim grey",       "0.412 0.412 0.412" },
  { "dark slate grey","0.184 0.310 0.310" }
};
#define NB_COLORS (sizeof(colors)/sizeof(colors[0]))

//:
vcl_string text_to_color(vcl_string txt)
{
  vcl_string color = "";
  if (txt[0] == '0' || txt[0] == '1')
    color = txt;

  for (unsigned int i = 0; i < NB_COLORS; i++)
  {
    if (txt == colors[i][0])
      color =  colors[i][1];
  }

  if (color == "")
    vgui_macro_warning << "Unknown color string: " << txt << vcl_endl;
  else
    vcl_sscanf(color.c_str(), "%f %f %f", &red, &green, &blue);

  if (debug)
    vcl_cerr << "vgui_color_text:: color string= " << color << ", red="
             << red << ", green=" << green << ", blue=" << blue << vcl_endl;
  return color;
}

float red_value(vcl_string txt)
{
  vcl_string nb_txt = text_to_color(txt);
  return red;
}

float green_value(vcl_string txt)
{
  vcl_string nb_txt = text_to_color(txt);
  return green;
}

float blue_value(vcl_string txt)
{
  vcl_string nb_txt = text_to_color(txt);
  return blue;
}

