// This is brl/bseg/bvpl/bvpl_create_directions.h
#ifndef  bvpl_create_directions_h_
#define  bvpl_create_directions_h_
//:
// \file
// \brief File to store different functor to sample 3D directions on a sphere.
//        The sphere is modeled by two angles azimuthal(\theta) and elevation(\phi)
//
// \author Vishal Jain (vj@lems.brown.edu)
// \date June 29, 2009
//
// \verbatim
//  Modifications
//   <none yet>
// \endverbatim

#include <vnl/vnl_float_3.h>
#include <vcl_iostream.h>
#include <vcl_vector.h>

//:
// this class samples the
// phi=0, theta=0;
// phi=pi/4 theta = [0, 2\pi)
// phi=pi/2 theta = [0, 2\pi)

class bvpl_create_directions_a
{
 public:
  //: Default constructor
  bvpl_create_directions_a();

  //: Destructor
  ~bvpl_create_directions_a() {}

  vcl_vector<vnl_float_3> get_axes(){ return axes_;}

 private:
  vcl_vector<vnl_float_3> axes_;
};

class bvpl_create_directions_b
{
 public:
  //: Default constructor
  bvpl_create_directions_b();

  //: Destructor
  ~bvpl_create_directions_b() {}

  vcl_vector<vnl_float_3> get_axes(){ return axes_;}

 private:
  vcl_vector<vnl_float_3> axes_;
};

#endif //bvpl_create_directions_h_
