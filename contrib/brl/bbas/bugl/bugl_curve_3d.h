#ifndef bugl_curve_3d_h_
#define bugl_curve_3d_h_
#include "bugl_normal_point_3d_sptr.h"
#include "bugl_normal_point_3d.h"
#include "bugl_normal_point_3d_sptr.h"

class bugl_curve_3d
{
  //: number of neighbors
	int num_neighbors_;

  //: data
	vcl_vector<vcl_vector<bugl_normal_point_3d_sptr > > data_;

  //: starting position index for each fragment
  vcl_vector<int> index_;

public:

  //: total number of points in the curve
	int get_num_points() const;

  //: how many fragments in the curve
  int get_num_fragments() const;

  //: add a vector of point as a curve
	int add_curve(vcl_vector<bugl_normal_point_3d_sptr > &pts);

  bugl_curve_3d(int neighbors = 2);

  ~bugl_curve_3d();

  //: get the point
	bugl_normal_point_3d_sptr  get_point(const int index) const;

  //: get a point with neighbors 
  vcl_vector<bugl_normal_point_3d_sptr> get_neighbors(const int index) const;

  //: get a neighbor point
  bugl_normal_point_3d_sptr get_neighbor(const int self, const int offset ) const;

  //: get the global position of a point in a fragment. 0-based indice are used 
  inline int get_global_pos(int frag_index, int loc_pos ) 
  {return index_[frag_index]+loc_pos;}
};

#endif
