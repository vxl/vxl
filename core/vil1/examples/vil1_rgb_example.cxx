// Example use of the vil1_rgb and vil1_rgb_cell classes.
//
// Author: Peter Vanroose, KULeuven/ESAT, December 1999

#include <iostream>
#ifdef _MSC_VER
#  include <vcl_msvc_warnings.h>
#endif
#include <vil1/vil1_rgb.h>
typedef vil1_rgb<unsigned char> vil1_rgb_cell;

static char* as_hex(vil1_rgb_cell const&);

int main(int /*argc*/, char** /*argv*/)
{
  // The "normal" vil1_rgb class, with ubyte cells, is called vil1_rgb_cell;
  // its constructor takes the R, G and B values to be set:
  vil1_rgb_cell orange(255,140,0);
  // A ubyte (= grey pixel) value is automatically converted to a vil1_rgb_cell:
  vil1_rgb_cell black = (unsigned char)0;
  vil1_rgb_cell white = (unsigned char)255;
  vil1_rgb_cell grey = (unsigned char)190;

  // Conversely, a vil1_rgb_cell can be converted to its ubyte grey equivalent:
  unsigned char orange_grey = orange.grey();

  // Write to a std::ostream: the output as the format [R G B].
  std::cout << "/* XPM */\n"
           << "/* " << orange << black << white << grey
           << (int)orange_grey << " */\n";

  // And now an .xpm file using these vil1_rgb_cells, and the function as_hex:
  std::cout << "static char* example_rgb[] = {\n\"20 8 4 1\",\n"
           << "\" \tc " << as_hex(white) << "\",\n"
           << "\"*\tc " << as_hex(black) << "\",\n"
           << "\".\tc " << as_hex(orange)<< "\",\n"
           << "\"/\tc " << as_hex(grey)  << "\",\n"
           << "\"/ /.*** / /.* /.* / \",\n"
           << "\" /.* /.* / .*/.* / /\",\n"
           << "\"/.* / /.* /.*.* / / \",\n"
           << "\" .*/ / .*/ .** / / /\",\n"
           << "\"/.* / /.* /.*.* / / \",\n"
           << "\" .*/ / .*/ .*/.* / /\",\n"
           << "\"/ .*/ .*/ /.* /.* / \",\n"
           << "\" / .***/.* .*/ /.* .\"\n};\n";
  return 0;
}

// This function writes, e.g., vil1_rgb_cell(177,49,97) as "#b13161":

char* as_hex(vil1_rgb_cell const& rgb)
{
  // The data members r, g and b of a vil1_rgb_cell are public:
  unsigned char r = rgb.r;
  unsigned char g = rgb.g;
  unsigned char b = rgb.b;

  // And now some magic char manipulations, to obtain hex values:
  static char s[] = "#000000";
  s[1] = (char)('0'+(r/16)); if (s[1] > '9') s[1] += (char)('a'-'9'-1);
  s[2] = (char)('0'+(r%16)); if (s[2] > '9') s[2] += (char)('a'-'9'-1);
  s[3] = (char)('0'+(g/16)); if (s[3] > '9') s[3] += (char)('a'-'9'-1);
  s[4] = (char)('0'+(g%16)); if (s[4] > '9') s[4] += (char)('a'-'9'-1);
  s[5] = (char)('0'+(b/16)); if (s[5] > '9') s[5] += (char)('a'-'9'-1);
  s[6] = (char)('0'+(b%16)); if (s[6] > '9') s[6] += (char)('a'-'9'-1);
  return s;
}
