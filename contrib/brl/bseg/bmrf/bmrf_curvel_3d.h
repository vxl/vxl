// This is brl/bseg/bmrf/bmrf_curvel_3d.h
#ifndef bmrf_curvel_3d_h_
#define bmrf_curvel_3d_h_
//:
// \file
// \brief An uncertain 3D curve element
// \author Matt Leotta, (mleotta@lems.brown.edu)
// \date 3/23/04
//
//  The curvels are nodes in a doubly-linked list which makes up the curve
//
// \verbatim
//  Modifications
// \endverbatim

#include <bmrf/bmrf_curvel_3d_sptr.h>
#include <bmrf/bmrf_node_sptr.h>
#include <bugl/bugl_gaussian_point_3d.h>
#include <vgl/vgl_point_3d.h>
#include <vnl/vnl_double_3x3.h>
#include <vnl/vnl_double_2.h>
#include <vbl/vbl_ref_count.h>
#include <vcl_vector.h>
#include <vcl_utility.h>

//: An uncertain 3D curve element
class bmrf_curvel_3d : public bugl_gaussian_point_3d<double>, public vbl_ref_count
{
 public:
  //: Constructor
  bmrf_curvel_3d();
  bmrf_curvel_3d(vgl_point_3d<double> &p, vnl_double_3x3 &s);
  bmrf_curvel_3d(double x, double y, double z, vnl_double_3x3 & s);

  //: Destructor
  ~bmrf_curvel_3d() {}

  //: Access the next curvel in the list
  bmrf_curvel_3d_sptr next() const;
  //: Access the previous curvel in the list
  bmrf_curvel_3d_sptr prev() const;

  //: Set the next curvel in the list
  void set_next(const bmrf_curvel_3d_sptr& curvel_3d);
  //: Set the previous curvel in the list
  void set_prev(const bmrf_curvel_3d_sptr& curvel_3d);

  //: Set the projection of this curvel into \p frame as the segment in \p node at the value \p alpha
  void set_proj_in_frame(unsigned int frame, double alpha, const bmrf_node_sptr& node);

  //: Returns the 2d position of this curvel in frame \p frame by reference
  // \retval true if a correspondence exists at this frame
  // \retval false if no correspondence exists at this frame
  bool pos_in_frame(unsigned int frame, vnl_double_2& pos);

 protected:
  //: The next curvel along the curve
  bmrf_curvel_3d_sptr next_;
  //: The previous curvel along the curve
  bmrf_curvel_3d_sptr prev_;

  //: A vector of alpha/node pairs which represent the projection of this curvel into image i.
  vcl_vector<vcl_pair<double,bmrf_node_sptr> > projs_2d_;
};

#endif // bmrf_curvel_3d_h_
