#ifndef vsl_conic_points_h_
#define vsl_conic_points_h_
#ifdef __GNUC__
#pragma interface
#endif
//:
//  \file
// \brief Point sampling of 2D conic
//
//   This class represents the point sampling of a 2D conic section
//   which has been clipped to a viewport.
//
// \author
//     Andrew W. Fitzgibbon, Oxford RRG, 12 Nov 99
//-----------------------------------------------------------------------------

#include <vcl_vector.h>

#include <vnl/vnl_double_2.h>

class vsl_conic_as_matrix;

class vsl_conic_points {
public:
  typedef vnl_double_2 point_type;
  typedef vcl_vector< point_type >::const_iterator point_iterator;

  //: Constructor makes the point samples.
  vsl_conic_points(vsl_conic_as_matrix const& conic, double xmin, double xmax, double ymin, double ymax);

  //: Number of connected components.
  int n() const { return segment_ends.size(); }

  //: Beginning of segment segment_index
  point_iterator begin(int segment_index) const { return points.begin() + segment_begin(segment_index); }

  //: End of segment segment_index
  point_iterator end(int segment_index) const { return points.begin() + segment_ends[segment_index]; }

private:
  vcl_vector< point_type > points;
  vcl_vector< int > segment_ends;
  int segment_begin(int i) const { return (i==0)? 0 : segment_ends[i-1]; }

  static vcl_vector<double> find_angles(vnl_matrix<double> const& b, vnl_matrix<double> const& Hinv);
};

#endif // vsl_conic_points_h_
