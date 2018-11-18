// This is gel/pop/pop_projective.cxx
#include "pop_projective.h"
//:
// \file
#include<pop/pop_point_2d.h>
#include<pop/pop_point_3d.h>
#include <cassert>
#ifdef _MSC_VER
#  include <vcl_msvc_warnings.h>
#endif


//: constructor
// the parameters for this transform are
// the Euler angle r1 r2 r2 and the translation t1 t2 t3

pop_projective::pop_projective(std::vector<pop_parameter*> params,
                               pop_vertex *cs1, pop_vertex *cs2) :
  pop_transform(params,cs1,cs2)
{
  this->update();
}

//: destructor
pop_projective::~pop_projective()
{
}

//: transform a geometric object
pop_geometric_object* pop_projective::transform(pop_geometric_object *obj)
{
  // try to transform a 3d point to a 2d point
  pop_point_3d *p1 = obj->cast_to_pop_point_3d();

  if (p1)
  {
    // make sure that p1 comes from the right coordinate
    // system
    assert(p1->coordinate_system_ == cs1_);

    // transform this point
    vnl_vector<double> v1(3);
    v1(0) = p1->x();
    v1(1) = p1->y();
    v1(2) = p1->z();

    vnl_vector<double> v2 = trans_ * v1;

    // normalize
    if (v2(2)) {
      v2 = v2/v2(2);
    }

    // make a new pop_point_2d
    pop_point_2d *p2 = new pop_point_2d(cs2_,v2(0),v2(1));

    return p2;
  }

  return 0;
}

//: update the transform based on the parameters
void pop_projective::update()
{
  // update the projective matrix
  // |a s u0|
  // |0 b v0|
  // |0 0 1 |

  trans_.fill(0.0);
  trans_(0,0) = params_[0]->value_; // the a value
  trans_(1,1) = params_[1]->value_; // the b value
  trans_(0,1) = params_[2]->value_; // the shear s
  trans_(0,2) = params_[3]->value_; // the u offset
  trans_(1,2) = params_[4]->value_; // the v offset
  trans_(2,2) = 1.0;
}
