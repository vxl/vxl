// <begin copyright notice>
// ---------------------------------------------------------------------------
//
//                   Copyright (c) 1997 TargetJr Consortium
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
//-*- c++ -*-------------------------------------------------------------------
#ifndef LineSegSet_h_
#define LineSegSet_h_
#ifdef __GNUC__
#pragma interface
#endif
//
// Class : LineSegSet
//
// .SECTION Description
//    LineSegSet is a class that awf hasn't documented properly. FIXME
//
// .NAME        LineSegSet - Undocumented class FIXME
// .LIBRARY MViewBasics
// .HEADER MultiView package
// .INCLUDE mvl/LineSegSet.h
// .FILE        LineSegSet.cxx
// .SECTION Author
//     Andrew W. Fitzgibbon, Oxford RRG, 19 Sep 96
//
// .SECTION Modifications:
//     <none yet>
//
//-----------------------------------------------------------------------------

////////////////#include <cool/decls.h>
#include <vcl/vcl_vector.h>
#include <vcl/vcl_vector.h>
#include <mvl/HomgLineSeg2D.h>
#include <mvl/HomgMetric.h>
////////////////#include <Geometry/IUPoint.h>

class ImageMetric;

class LineSegSet {
public:
  // Constructors/Destructors--------------------------------------------------
  
  LineSegSet();
  LineSegSet(const HomgMetric& c, const vcl_vector<HomgLineSeg2D>& lines, bool is_conditioned = true);
  LineSegSet(const char* filename, const HomgMetric& c = 0);
  LineSegSet(const LineSegSet& that);
 ~LineSegSet();

  LineSegSet& operator=(const LineSegSet& that);

  // Operations----------------------------------------------------------------
  unsigned size() const { return _hlines.size(); }

  // Computations--------------------------------------------------------------
  int pick_line_index(double x, double y);
  HomgLineSeg2D* pick_line(double x, double y);

  // Data Access---------------------------------------------------------------
        HomgLineSeg2D& get_homg(int i)       { return _hlines[i]; }
  const HomgLineSeg2D& get_homg(int i) const { return _hlines[i]; }

  vcl_vector<HomgLineSeg2D>& get_homg() { return _hlines; }

  int FindNearestLineIndex(double x, double y);
  
  // Data Control--------------------------------------------------------------
  bool load_ascii(istream&, HomgMetric const& c);
  bool save_ascii(ostream&) const;

  bool set_iuline(int i, void* l);

  void compute_homglines_from_rawlines(const HomgMetric& );
  
protected:
  // Data Members--------------------------------------------------------------
  vcl_vector<HomgLineSeg2D> _hlines;
  HomgMetric _conditioner;
  
private:
  // Helpers-------------------------------------------------------------------
};

#endif   // DO NOT ADD CODE AFTER THIS LINE! END OF DEFINITION FOR CLASS LineSegSet.

