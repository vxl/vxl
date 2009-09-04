// This is brl/bseg/bvpl/bvpl_create_directions.h
#ifndef  bvpl_create_directions_h_
#define  bvpl_create_directions_h_
//:
// \file
// \brief File to store different functor to sample 3D directions on a sphere.
//        The sphere is modeled by two angles azimuthal(\theta) and elevation(\phi from pole)
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
 bvpl_create_directions_base(){}
 //:Destructor
//  virtual ~bvpl_create_directions_base(){};

 //:Accessor methods
 vcl_vector<vnl_float_3> get_axes(){ return axes_;}
 vcl_vector<float> get_angles(){ return angles_;}

 //:Print to file
 void save_directions_map(vcl_string outfile);
  
 protected:
  vcl_vector<vnl_float_3> axes_;
  vcl_vector<float> angles_;
};
//:
// this class samples the
// phi=0, theta=0;
// phi=pi/4 theta = [0, 2\pi)
// phi=pi/2 theta = [0, 2\pi)
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
// phi = pi/2 theta = [0, 2*pi)
// rotation around the axis with resolution pi/4
class bvpl_create_directions_c: public bvpl_create_directions_base
{
  public:
  //: Default constructor
  bvpl_create_directions_c();

};
 
#endif //bvpl_create_directions_h_
