// This is oxl/mvl/LineSegSet.cxx
//:
//  \file

#include <iostream>
#include <fstream>
#include <vector>
#include "LineSegSet.h"

#ifdef _MSC_VER
#  include <vcl_msvc_warnings.h>
#endif

#include <mvl/ImageMetric.h>
#include <mvl/HomgPoint2D.h>

// Default ctor
LineSegSet::LineSegSet():
  hlines_(0)
{
  conditioner_ = nullptr;
}

// Copy ctor
LineSegSet::LineSegSet(const LineSegSet& that):
  hlines_(0)
{
  operator=(that);
}

// Assignment
LineSegSet& LineSegSet::operator=(const LineSegSet& that) = default;

// Destructor
LineSegSet::~LineSegSet() = default;

//: Construct from ascii file
LineSegSet::LineSegSet(const char* filename, const HomgMetric& c)
{
  std::ifstream f(filename);
  load_ascii(f, c);
}

//: Load lines from ASCII file
bool LineSegSet::load_ascii(std::istream& f, HomgMetric const& c)
{
  vnl_matrix<double> L;
  f >> L;

  int cols = L.columns();
  if (cols != 6 && cols != 4) {
    std::cerr << "Load failed -- there are " << L.columns() << " data per row\n";
    return false;
  }

  conditioner_ = c;
  hlines_.resize(0);
  for (unsigned i = 0; i < L.rows(); ++i) {
    double x1 = L(i,0);
    double y1 = L(i,1);
    double x2 = L(i,2);
    double y2 = L(i,3);
#if 0
    double theta;
    double avemag;
    if (cols == 6) {
      theta = L(i,4);
      avemag = L(i,5);
    } else {
      theta = 0;
      avemag = 0;
    }
#endif
    HomgPoint2D p1(x1, y1);
    HomgPoint2D p2(x2, y2);
    HomgLineSeg2D line(p1, p2);
    hlines_.push_back(c.image_to_homg_line(line));
  }

  std::cerr << "Loaded " << size() << " line segments\n";
  return true;
}

int LineSegSet::FindNearestLineIndex(double /*x*/, double /*y*/)
{
  std::cerr <<"LineSegSet::FindNearestLineIndex not yet implemented\n";
  return -1;
#if 0 // commented out
  double mindist=-1.0f;
  int mini=-1;
  for (unsigned int i=0; i<size(); ++i)
  {
    HomgLineSeg2D& dl = get_homg(i);
    double t = ( dl.get_point1().x() - x ) * ( dl.get_point2() - x )
             + ( dl.get_point1().y() - y ) * ( dl.get_point2() - y );
    // i.e.: t = dot_product ( startpt - pt , endpt - pt ) ;

    double dist;
    if (t<0)     // P lies inbetween the two end points
      dist = vgl_distance(dl.get_line(),vgl_point_2d<double>(x,y)); // distance to the support line
    else
      dist = std::min(// closest distance with endpoints
                     vgl_distance(dl.get_point1(),vgl_point_2d<double>(x,y)),
                     vgl_distance(dl.get_point2(),vgl_point_2d<double>(x,y)));
    if (mini<0 || dist<mindist){ mindist = dist; mini = i; }
  }
  return mini;
#endif
}

//: Save lines to ASCII file
bool LineSegSet::save_ascii(std::ostream& f) const
{
  for (const auto & l : hlines_) {
    vnl_double_2 p1 = conditioner_.homg_to_image(l.get_point1());
    vnl_double_2 p2 = conditioner_.homg_to_image(l.get_point2());

    f << p1[0] << " " << p1[1] << "\t"
      << p2[0] << " " << p2[1] << std::endl;
  }
  std::cerr << "LineSegSet: Saved " << hlines_.size() << " line segments\n";
  return true;
}

//: Return line selected by mouse click at (x,y) in image coordinates.
int LineSegSet::pick_line_index(double x, double y)
{
  HomgPoint2D p(x, y);
  HomgMetric metric(conditioner_);

  double dmin = 1e20;
  int imin = -1;
  int nlines = hlines_.size();
  for (int i = 0; i < nlines; ++i) {
    const HomgLineSeg2D& l = hlines_[i];
    HomgLineSeg2D l_decond = metric.homg_line_to_image(l);

    double d = l_decond.picking_distance(p);

    if (d < dmin) {
      dmin = d;
      imin = i;
    }
  }

  return imin;
}

//: Return line selected by mouse click at (x,y) in image coordinates.
HomgLineSeg2D* LineSegSet::pick_line(double x, double y)
{
  int i = pick_line_index(x,y);
  if (i >= 0)
    return &hlines_[i];
  else
    return nullptr;
}
