// This is brl/bseg/bvpl/kernels/bvpl_create_directions.h
#ifndef  bvpl_create_directions_h_
#define  bvpl_create_directions_h_
//:
// \file
// \brief File to store different functor to sample 3D directions on a sphere.
// The sphere is modeled by two angles azimuthal($\theta$) and elevation($\phi$ from pole)
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
#include <vcl_string.h>

class bvpl_create_directions_base
{
 public:
 bvpl_create_directions_base() {}

#if 0
 //:Destructor
 virtual ~bvpl_create_directions_base() {}
#endif

 //:Accessor methods
 vcl_vector<vnl_float_3> get_axes() const { return axes_; }
 vcl_vector<float> get_angles() const { return angles_; }

 //:Print to file
 void save_directions_map(vcl_string outfile);

 protected:
  vcl_vector<vnl_float_3> axes_;
  vcl_vector<float> angles_;
};

//: Creates directions aligned the positive x,y,z axis
class bvpl_create_directions_xyz : public bvpl_create_directions_base
{
 public:
  bvpl_create_directions_xyz();
};


//:
// this class samples the
// phi=0, theta=0;
// phi=pi/4 theta = [0, 2pi)
// phi=pi/2 theta = [0, 2pi)
class bvpl_create_directions_a: public bvpl_create_directions_base
{
 public:
  //: Default constructor
  bvpl_create_directions_a();
};

//: This class samples
// phi = 0, theta= 0;
// phi = pi/4 theta =[0, 2*pi)
// phi = pi/2 theta = [0, 3*pi/2)
class bvpl_create_directions_b: public bvpl_create_directions_base
{
 public:
  //: Default constructor
  bvpl_create_directions_b();
};

//: This class samples
// phi = 0, theta= 0;
// phi = pi/4 theta =[0, 2*pi)
// phi = pi/2 theta = [0, 3*pi/2)
// phi = pi/3 theta = [0, 2pi)
// phi = pi/6 theta = [0, 2pi)
class bvpl_create_directions_c: public bvpl_create_directions_base
{
 public:
  //: Default constructor
  bvpl_create_directions_c();
};

/**********************Corner directions************************/
class bvpl_main_corner_dirs: public bvpl_create_directions_base
{
 public:
  //: Default constructor
  bvpl_main_corner_dirs();
};

class bvpl_main_plane_corner_dirs: public bvpl_create_directions_base
{
 public:
  //: Default constructor
  bvpl_main_plane_corner_dirs();
};

//: This class samples
// phi = 0, theta= 0;
// phi = pi/4 theta =[0, 2*pi)
// phi = pi/2 theta = [0, 2*pi)
// rotation around the axis with resolution pi/4
class bvpl_all_corner_dirs: public bvpl_create_directions_base
{
 public:
  //: Default constructor
  bvpl_all_corner_dirs();
};

//: This class samples
// phi = 0, theta= 0;
// phi = pi/2 theta = [0, 2*pi)
// rotation around the axis with resolution pi/2
class bvpl_pi_over_2_corner_dirs: public bvpl_create_directions_base
{
 public:
  //: Default constructor
  bvpl_pi_over_2_corner_dirs();
};


#endif //bvpl_create_directions_h_
