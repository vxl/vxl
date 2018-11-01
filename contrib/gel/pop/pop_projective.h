// This is gel/pop/pop_projective.h
#ifndef pop_projective_h_
#define pop_projective_h_
//:
// \file
// \brief A projection of a point in 3d to a point in 2D.
//        This models the intrinsic parameters of a pinhole camera.
//
// \author
//            Peter Tu April 2003
//            General Electric
//
//-------------------------------------------------------------------------------

#include<pop/pop_object.h>
#include<vnl/vnl_matrix_fixed.h>
#include<pop/pop_transform.h>
#include<pop/pop_geometric_object.h>

//: A parameter that can be optimized
class pop_projective:public pop_transform
{
 public:
  //: constructor
  // We have an upper triangular 3 by 3 matrix
  // \verbatim
  // |a  s  u0|
  // |0  b  v0|
  // |0  0  1 |
  // \endverbatim
  // where a and b are the scaling of the image u and v axis.
  // s is the shear and (u0,v0) is the piercing point
  //
  // the params are a b s u0 v0

  pop_projective(std::vector<pop_parameter*> params,
                 pop_vertex *cs1, pop_vertex *cs2);

  //: destructor
  ~pop_projective();

  //: transform a geometric object
  virtual pop_geometric_object* transform(pop_geometric_object *obj);

  //: update the transform based on the parameters
  virtual void update();

 private:
  // use a simple matrix rep until I find a better class
  vnl_matrix_fixed<double,3,3>  trans_;
};

#endif // pop_projective_h_
