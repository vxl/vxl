// This is oxl/mvl/ProjStructure.h
#ifndef ProjStructure_h_
#define ProjStructure_h_
//:
//  \file
// \author awf@robots.ox.ac.uk
// \date   17 Mar 00

#include <iostream>
#include <vector>
#ifdef _MSC_VER
#  include <vcl_msvc_warnings.h>
#endif

#include <mvl/ImageMetric.h>
#include <mvl/PMatrix.h>
#include <mvl/HomgPoint2D.h>
#include <mvl/HomgPoint3D.h>

class HomgInterestPointSet;

//: Not documented FIXME
class ProjStructure
{
 public:

  std::vector<HomgInterestPointSet*> points_2d_;
  std::vector<PMatrix> P_;
  std::vector<HomgPoint3D> X_;
  ImageMetric const* metric_;

  ProjStructure();

  ImageMetric const* get_image_metric(int) { return metric_; }

  PMatrix& P(int i) { return P_[i]; }
  PMatrix const& P(int i) const { return P_[i]; }

  HomgPoint3D& X(int i) { return X_[i]; }
  HomgPoint3D const& X(int i) const { return X_[i]; }

  int get_min_view() const { return 0; }
  int get_max_view() const { return P_.size() - 1; }
  int npoints() const { return X_.size(); }

  HomgPoint2D& get_point_2d(int view, int c);
  int get_point_match(int view, int c);
};

#endif // ProjStructure_h_
