#ifndef bugl_curve_3d_h_
#define bugl_curve_3d_h_
//--------------------------------------------------------------------------
//:
// \file

// \brief bugl_curve_3d - a 3-d pointset with curve (1-d) neighborhood
//
//    The data structure is a matrix of 3-d points with uncertainty
//    
//     ->|              |<--- 2*num_neighbors_ + 1
//       x x x  p0  x x x  
//       x x x  p1  x x x
//             ...  |---|  <--- num_neighbors_
//             ...
//       x x x pN-1 x x x
//                    ^---neighboring points
//    N = data_.size()
//
//   There is also the notion of "fragments". A vector, index_, marks 
//   intervals between sets of 3-d points inserted by each invocation of
//   ::add_curve(..)
//   
//  
// \author Kongbin Kang
// \verbatim
// Initial version November 22, 2003
// \endverbatim
//---------------------------------------------------------------------------

//:
// \file
#include <bugl/bugl_normal_point_3d_sptr.h>
#include <bugl/bugl_normal_point_3d.h>
#include <vcl_vector.h>

class bugl_curve_3d
{
  //: number of neighbors
  unsigned int num_neighbors_;

  //: data
  vcl_vector<vcl_vector<bugl_normal_point_3d_sptr > > data_;

  //: starting position index for each fragment
  vcl_vector<int> index_;

 public:

  //: total number of points in the curve
  int get_num_points() const { return data_.size(); }

  //: how many fragments in the curve
  int get_num_fragments() const { return index_.size(); }

  //: add a vector of point as a curve
  void add_curve(vcl_vector<bugl_normal_point_3d_sptr > &pts);

  bugl_curve_3d(unsigned int neighbors = 2) : num_neighbors_(neighbors) {}

  ~bugl_curve_3d() {}

  //: get the point
  bugl_normal_point_3d_sptr get_point(unsigned int index) const;

  //: get a point with neighbors
  vcl_vector<bugl_normal_point_3d_sptr> get_neighbors(unsigned int index) const;

  //: get a neighbor point
  bugl_normal_point_3d_sptr get_neighbor(unsigned int self, int offset) const;

  //: get the global position of a point in a fragment. 0-based indices are used.
  //  Returns -1 if the arguments are invalid.
  inline int get_global_pos(unsigned int frag_index, unsigned int loc_pos)
  { return frag_index<index_.size() && loc_pos<get_fragment_size(frag_index) ?
           int(index_[frag_index]+loc_pos) : -1; }

  //: get the i-th fragment size
  unsigned int get_fragment_size(unsigned int i) const;
};

#endif
