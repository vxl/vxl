// This is brl/bseg/bmrf/bmrf_curvel_3d.cxx
//:
// \file

#include "bmrf_curvel_3d.h"
#include "bmrf_node.h"
#include <vcl_algorithm.h>


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


//: Merge the other curvel into this curvel if there is no overlap
// \return false if merging is not possible
bool 
bmrf_curvel_3d::merge(const bmrf_curvel_3d_sptr& other)
{
  int num_frames = vcl_max(this->projs_2d_.size(), other->projs_2d_.size());
  vcl_vector<vcl_pair<double,bmrf_node_sptr> > merged_projs_2d_(num_frames);
  for ( int f=0; f<num_frames; ++f ){
    
    bmrf_node_sptr n1 = NULL, n2 = NULL;
    if( f<this->projs_2d_.size() )
      n1 = this->projs_2d_[f].second;
    if( f<other->projs_2d_.size() )
      n2 = other->projs_2d_[f].second;

    if( n1 && n2 )
      return false;

    if( n1 )
      merged_projs_2d_[f] = this->projs_2d_[f];
    else if( n2 )
      merged_projs_2d_[f] = other->projs_2d_[f];
  }
  this->projs_2d_ = merged_projs_2d_;
  return true;
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


//: Set the projection of this curvel into \p frame at an interpolated position
void 
bmrf_curvel_3d::set_psuedo_point(unsigned int frame, const vnl_double_2& pos)
{
  if (frame >= projs_2d_.size())
    projs_2d_.resize(frame+1);

  pseudo_points_[frame] = pos;
}


//: Returns the 2d position of this curvel in \p frame by reference
bool
bmrf_curvel_3d::pos_in_frame(unsigned int frame, vnl_double_2& pos) const
{
  if( frame >= projs_2d_.size() )
    return false;
  double alpha = projs_2d_[frame].first;
  bmrf_node_sptr node = projs_2d_[frame].second;
  if ( node && node->epi_seg()){
    pos[0] = node->epi_seg()->x(alpha);
    pos[1] = node->epi_seg()->y(alpha);
    return true;
  }
  vcl_map<unsigned int, vnl_double_2>::const_iterator p_itr = pseudo_points_.find(frame);
  if( p_itr != pseudo_points_.end() ){
    pos = p_itr->second;
    return true;
  }
    
  return false;
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
bmrf_curvel_3d::num_projections(bool include_pseudo) const
{
  int count = 0;
  for ( vcl_vector<vcl_pair<double,bmrf_node_sptr> >::const_iterator itr = projs_2d_.begin();
        itr != projs_2d_.end();  ++itr )
  {
    if(itr->second)
      ++count;
  }
  if (include_pseudo)
    count += pseudo_points_.size();

  return count;
}
