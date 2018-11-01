// This is oxl/mvl/LineSegSet.h
#ifndef LineSegSet_h_
#define LineSegSet_h_
//
// \author
//     Andrew W. Fitzgibbon, Oxford RRG, 19 Sep 96
//
//-----------------------------------------------------------------------------

#include <vector>
#include <iostream>
#include <iosfwd>
#ifdef _MSC_VER
#  include <vcl_msvc_warnings.h>
#endif
#include <mvl/HomgLineSeg2D.h>
#include <mvl/HomgMetric.h>

class ImageMetric;

class LineSegSet
{
  // Data Members--------------------------------------------------------------
  std::vector<HomgLineSeg2D> hlines_;
  HomgMetric conditioner_;

 public:
  // Constructors/Destructors--------------------------------------------------

  LineSegSet();
  LineSegSet(const HomgMetric& c, const std::vector<HomgLineSeg2D>& lines, bool is_conditioned = true);
  LineSegSet(const char* filename, const HomgMetric& c = nullptr);
  LineSegSet(const LineSegSet& that);
 ~LineSegSet();

  LineSegSet& operator=(const LineSegSet& that);

  // Operations----------------------------------------------------------------
  unsigned size() const { return hlines_.size(); }

  // Computations--------------------------------------------------------------
  int pick_line_index(double x, double y);
  HomgLineSeg2D* pick_line(double x, double y);

  // Data Access---------------------------------------------------------------
        HomgLineSeg2D& get_homg(int i)       { return hlines_[i]; }
  const HomgLineSeg2D& get_homg(int i) const { return hlines_[i]; }

  std::vector<HomgLineSeg2D>& get_homg() { return hlines_; }

  int FindNearestLineIndex(double x, double y);

  // Data Control--------------------------------------------------------------
  bool load_ascii(std::istream&, HomgMetric const& c);
  bool save_ascii(std::ostream&) const;

  bool set_iuline(int i, void* l);

  void compute_homglines_from_rawlines(const HomgMetric& );
};

#endif // LineSegSet_h_
