// This is contrib/brl/bseg/bvpl/bvpl_kernel_base.h
#ifndef bvpl_kernel_base_h
#define bvpl_kernel_base_h

//:
// \file
// \brief 
//
// \author Isabel Restrepo mir@lems.brown.edu
//
// \date  5/29/09
//
// \verbatim
//  Modifications
//   <none yet>
// \endverbatim

#include "bvpl_kernel_iterator.h"
#include <vcl_string.h>
#include <vcl_map.h>
#include <vcl_utility.h>
#include <vnl/vnl_matrix_fixed.h>

class bvpl_kernel_base
{
public:
  unsigned angular_resolution_x()  {return angular_resolution_x_;}
  unsigned angular_resolution_y()  {return angular_resolution_y_;}
  unsigned angular_resolution_z()  {return angular_resolution_z_;}
  virtual vcl_string name()=0;
  
  //:Return a map of symbols and it's 3d position
  bvpl_kernel_iterator iterator(){return kernel_;}

  
protected:
  //The map of 3d positions and their symbols
  vcl_vector<vcl_pair<vgl_point_3d<int>, bvpl_kernel_dispatch> > kernel_;
  //The rotation matrix
  vnl_matrix_fixed<double,3,3> R_;
  
  //A map containing symbols and their total_number
  //vcl_map<char, unsigned> symbols_map_;
  
  //:Interpolation to find 3d integer position 
  bool warp_nearest_neighbor();
  
  // Angular resolutions
  unsigned angular_resolution_x_;
  unsigned angular_resolution_y_;
  unsigned angular_resolution_z_;
};


#endif

