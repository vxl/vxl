// <begin copyright notice>
// ---------------------------------------------------------------------------
//
//                   Copyright (c) 1999 TargetJr Consortium
//               GE Corporate Research and Development (GE CRD)
//                             1 Research Circle
//                            Niskayuna, NY 12309
//                            All Rights Reserved
//              Reproduction rights limited as described below.
//                               
//      Permission to use, copy, modify, distribute, and sell this software
//      and its documentation for any purpose is hereby granted without fee,
//      provided that (i) the above copyright notice and this permission
//      notice appear in all copies of the software and related documentation,
//      (ii) the name TargetJr Consortium (represented by GE CRD), may not be
//      used in any advertising or publicity relating to the software without
//      the specific, prior written permission of GE CRD, and (iii) any
//      modifications are clearly marked and summarized in a change history
//      log.
//       
//      THE SOFTWARE IS PROVIDED "AS IS" AND WITHOUT WARRANTY OF ANY KIND,
//      EXPRESS, IMPLIED OR OTHERWISE, INCLUDING WITHOUT LIMITATION, ANY
//      WARRANTY OF MERCHANTABILITY OR FITNESS FOR A PARTICULAR PURPOSE.
//      IN NO EVENT SHALL THE TARGETJR CONSORTIUM BE LIABLE FOR ANY SPECIAL,
//      INCIDENTAL, INDIRECT OR CONSEQUENTIAL DAMAGES OF ANY KIND OR ANY
//      DAMAGES WHATSOEVER RESULTING FROM LOSS OF USE, DATA OR PROFITS,
//      WHETHER OR NOT ADVISED OF THE POSSIBILITY OF SUCH DAMAGES, OR ON
//      ANY THEORY OF LIABILITY ARISING OUT OF OR IN CONNECTION WITH THE
//      USE OR PERFORMANCE OF THIS SOFTWARE.
//
// ---------------------------------------------------------------------------
// <end copyright notice>

// Example use of the vbl_rgb and vbl_rgb_cell classes.
//
// Author: Peter Vanroose, KULeuven/ESAT, December 1999

#include <iostream.h>
#include <vbl/vbl_rgb.txx>
typedef vbl_rgb<unsigned char> vbl_rgb_cell;

static char* as_hex(vbl_rgb_cell const&);

int main(int /*argc*/, char** /*argv*/)
{
  // The "normal" vbl_rgb class, with ubyte cells, is called vbl_rgb_cell;
  // its constructor takes the R, G and B values to be set:
  vbl_rgb_cell orange(255,140,0);
  // A ubyte (= grey pixel) value is automatically converted to an vbl_rgb_cell:
  vbl_rgb_cell black = (unsigned char)0;
  vbl_rgb_cell white = (unsigned char)255;
  vbl_rgb_cell grey = (unsigned char)190;

  // Conversely, an vbl_rgb_cell can be converted to its ubyte grey equivalent:
  unsigned char orange_grey = orange.grey();

  // Write to an ostream: the output as the format [R G B].
  cout << "/* XPM */\n/* " << orange << black << white << grey
       << (int)orange_grey << " */" << endl;

  // And now an .xpm file using these vbl_rgb_cells, and the function as_hex:
  cout << "static char* example_rgb[] = {\n\"20 8 4 1\",\n";
  cout << "\" \tc " << as_hex(white) << "\",\n";
  cout << "\"*\tc " << as_hex(black) << "\",\n";
  cout << "\".\tc " << as_hex(orange)<< "\",\n";
  cout << "\"/\tc " << as_hex(grey)  << "\",\n";
  cout << "\"/ /.*** / /.* /.* / \",\n"
       << "\" /.* /.* / .*/.* / /\",\n"
       << "\"/.* / /.* /.*.* / / \",\n"
       << "\" .*/ / .*/ .** / / /\",\n"
       << "\"/.* / /.* /.*.* / / \",\n"
       << "\" .*/ / .*/ .*/.* / /\",\n"
       << "\"/ .*/ .*/ /.* /.* / \",\n"
       << "\" / .***/.* .*/ /.* .\"\n};\n";
  return 0;
}

// This function writes, e.g., vbl_rgb_cell(177,49,97) as "#b13161":

char* as_hex(vbl_rgb_cell const& rgb) {
  // The data members R, G and B of an vbl_rgb_cell are public:
  unsigned char r = rgb.R;
  unsigned char g = rgb.G;
  unsigned char b = rgb.B;

  // And now some magic char manipulations, to obtain hex values:
  static char s[] = "#000000";
  s[1] = '0'+(r/16); if (s[1] > '9') s[1] += 'a'-'9'-1;
  s[2] = '0'+(r%16); if (s[2] > '9') s[2] += 'a'-'9'-1;
  s[3] = '0'+(g/16); if (s[3] > '9') s[3] += 'a'-'9'-1;
  s[4] = '0'+(g%16); if (s[4] > '9') s[4] += 'a'-'9'-1;
  s[5] = '0'+(b/16); if (s[5] > '9') s[5] += 'a'-'9'-1;
  s[6] = '0'+(b%16); if (s[6] > '9') s[6] += 'a'-'9'-1;
  return s;
}

