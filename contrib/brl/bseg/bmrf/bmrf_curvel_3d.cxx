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
bmrf_curvel_3d::pos_in_frame(unsigned int frame, vnl_double_2& pos) const
{
  if( frame >= projs_2d_.size() )
    return false;
  double alpha = projs_2d_[frame].first;
  bmrf_node_sptr node = projs_2d_[frame].second;
  if (node.ptr() == NULL || node->epi_seg().ptr() == NULL)
    return false;
  pos[0] = node->epi_seg()->x(alpha);
  pos[1] = node->epi_seg()->y(alpha);

  return true;
}


//: Returns the smart pointer to the node at the projection into \p frame
bmrf_node_sptr 
bmrf_curvel_3d::node_at_frame(unsigned int frame) const
{
  if( frame >= projs_2d_.size() )
    return NULL;
  return projs_2d_[frame].second;
}


//: Return true if \p a projection of this curvel lies on \p node
bool 
bmrf_curvel_3d::is_projection(const bmrf_node_sptr& node) const
{
  for ( vcl_vector<vcl_pair<double,bmrf_node_sptr> >::const_iterator itr = projs_2d_.begin();
        itr != projs_2d_.end();  ++itr )
  {
    if(node == itr->second)
      return true;
  }
  return false;
}


//: Return the number of projections available
int 
bmrf_curvel_3d::num_projections() const
{
  int count = 0;
  for ( vcl_vector<vcl_pair<double,bmrf_node_sptr> >::const_iterator itr = projs_2d_.begin();
        itr != projs_2d_.end();  ++itr )
    if(itr->second)
      ++count;
  
  return count;
}
