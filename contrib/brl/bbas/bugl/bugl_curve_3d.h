#ifndef bugl_curve_3d_h_
#define bugl_curve_3d_h_
#include <bbas/bugl/bugl_normal_point_3d_sptr.h>
#include <bugl/bugl_normal_point_3d.h>
#include <bugl/bugl_normal_point_3d_sptr.h>

class bugl_curve_3d
{
  //: number of neighbors
	int num_neighbors_;

  //: data
	vcl_vector<vcl_vector<bugl_normal_point_3d_sptr > > data_;

public:
	int get_num_points() const;
  //: add a vector of point as a curve
	int add_curve(vcl_vector<bugl_normal_point_3d_sptr > &pts);

  bugl_curve_3d(int neighbors = 2);

  ~bugl_curve_3d();

  //: add a new segment
  void add_point( vcl_vector<bugl_normal_point_3d_sptr > & newneigh);

  //: get the point
	bugl_normal_point_3d_sptr  get_point(const int index) const;

  //: get a point with neighbors 
  vcl_vector<bugl_normal_point_3d_sptr> get_neighbors(const int index) const;

  //: get a neighbor point
  bugl_normal_point_3d_sptr get_neighbor(const int self, const int offset ) const;

};

#endif
