// This is//projects/vxl/src/contrib/brl/bseg/bvpl/bvpl_corner_search_kernel.h
#ifndef bvpl_corner_search_kernel_h
#define bvpl_corner_search_kernel_h

//:
// \file
// \brief  A class to determine how to find a corner according to bvpl_weighted_cubel_kernel
// \author Isabel Restrepo mir@lems.brown.edu
// \date  9/23/09
//
// \verbatim
//  Modifications
//   <none yet>
// \endverbatim

#include <bvpl/bvpl_kernel_factory.h>
#include <bvpl/bvpl_create_directions.h>
#include <bvpl/bvpl_kernel_iterator.h>
#include <vcl_iostream.h>

class bvpl_corner_search_functor 
{

public:
  //:Default Constructor
  bvpl_corner_search_functor():this_id_(-1){}

  //:Constructor
  bvpl_corner_search_functor(int this_id, vcl_vector<vnl_float_3> axes, vcl_vector<float> angles );
  
  //:Destructor
  ~bvpl_corner_search_functor(){}
  
  //: Apply a given operation to value val, depending on the dispatch character
  void apply(int& val, bvpl_kernel_dispatch& d);
  
  //: Returns the final operation of this functor
  float result();
  

private:
  int this_id;
  vcl_vector<vnl_float_3> axes_;
  vcl_vector<float> angles_;
  vcl_vector<vgl_line_3d<int> > lines_;

};


bvpl_corner_search_functor::bvpl_corner_search_functor(int this_id, vcl_vector<vnl_float_3> axes, vcl_vector<float> angles)
{
  this_id_ = this_id;
  axes_ =  axes;
  angles_= angles;
  
}


void bvpl_corner_search_functor::apply(int& id, bvpl_kernel_dispatch& d, vgl_point_3d<int> p1, vgl_point_3d<int> p2)
{
  if(axes_(id)==axes(this_id_)) //same plane now look for opossite diagonal corners
    if(vcl_abs(vcl_abs(angles(id) - angles(this_id)) - vnl_math::pi_over_2) > 1e-7)
       lines_.push_back(vgl_line_3d<int>(p1, p2);
}

#endif
