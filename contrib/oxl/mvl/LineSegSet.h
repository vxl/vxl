#ifndef LineSegSet_h_
#define LineSegSet_h_
#ifdef __GNUC__
#pragma interface
#endif
//
// .NAME	LineSegSet - Undocumented class FIXME
// .LIBRARY	MViewBasics
// .HEADER	MultiView package
// .INCLUDE	mvl/LineSegSet.h
// .FILE	LineSegSet.cxx
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
};

#endif // LineSegSet_h_
