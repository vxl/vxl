// This is brl/bseg/bmrf/bmrf_curvel_3d.cxx
//:
// \file

#include "bmrf_curvel_3d.h"
#include "bmrf_node.h"


//: Constructor
bmrf_curvel_3d::bmrf_curvel_3d()
 : bugl_gaussian_point_3d<double>()
{
}

//: Constructor
bmrf_curvel_3d::bmrf_curvel_3d(vgl_point_3d<double> &p, vnl_double_3x3 &s)
 : bugl_gaussian_point_3d<double>(p, s)
{
}

//: Constructor
bmrf_curvel_3d::bmrf_curvel_3d(double x, double y, double z, vnl_double_3x3 & s)
 : bugl_gaussian_point_3d<double>(x, y, z, s)
{
}


//: Access the next curvel in the list
bmrf_curvel_3d_sptr
bmrf_curvel_3d::next() const
{
  return next_;
}


//: Access the previous curvel in the list
bmrf_curvel_3d_sptr
bmrf_curvel_3d::prev() const
{
  return prev_;
}


//: Set the next curvel in the list
void
bmrf_curvel_3d::set_next(const bmrf_curvel_3d_sptr& curvel_3d)
{
  next_ = curvel_3d;
}


//: Set the previous curvel in the list
void
bmrf_curvel_3d::set_prev(const bmrf_curvel_3d_sptr& curvel_3d)
{
  prev_ = curvel_3d;
}


//: Set the projection of this curvel into \p frame as the segment in
//  \p node at the value \p alpha
void
bmrf_curvel_3d::set_proj_in_frame(unsigned int frame, double alpha, const bmrf_node_sptr& node)
{
  if (frame >= projs_2d_.size())
    projs_2d_.resize(frame+1);

  projs_2d_[frame] = vcl_pair<double, bmrf_node_sptr>(alpha, node);
}


//: Returns the 2d position of this curvel in \p frame by reference
bool
bmrf_curvel_3d::pos_in_frame(unsigned int frame, vnl_double_2& pos)
{
  double alpha = projs_2d_[frame].first;
  bmrf_node_sptr node = projs_2d_[frame].second;
  if (node.ptr() == NULL || node->epi_seg().ptr() == NULL)
    return false;
  pos[0] = node->epi_seg()->x(alpha);
  pos[1] = node->epi_seg()->y(alpha);

  return true;
}
