// This is gel/pop/pop_rigid_3d.cxx
#include "pop_rigid_3d.h"
//:
// \file

#include <vgl/vgl_homg_point_3d.h>
#include <pop/pop_point_3d.h>
#include <cassert>
#ifdef _MSC_VER
#  include <vcl_msvc_warnings.h>
#endif

//: constructor
// the parameters for this transform are
// the Euler angle r1 r2 r2 and the translation t1 t2 t3


pop_rigid_3d::pop_rigid_3d(std::vector<pop_parameter*> params,
                           pop_vertex *cs1, pop_vertex *cs2) :
  pop_transform(params,cs1,cs2)
{
  this->update();
}

//: destructor
pop_rigid_3d::~pop_rigid_3d()
{
}

//: transform a geometric object
pop_geometric_object* pop_rigid_3d::transform(pop_geometric_object *obj)
{
  // try to transform a 3d point to a 3d point
  pop_point_3d *p1 = obj->cast_to_pop_point_3d();

  if (p1)
  {
    // make sure that p1 comes from the right coordinate
    // system
    assert(p1->coordinate_system_ == cs1_);

    // transform this point
    vgl_homg_point_3d<double> hp1(p1->x(),p1->y(),p1->z(),1.0);

    vgl_homg_point_3d<double> hp2 = trans_ * hp1;
    double w = hp2.w();

    double x = hp2.x();
    double y = hp2.y();
    double z = hp2.z();

    if (w) {
      x=x/w;
      y=y/w;
      z=z/w;
    }

    // make a new pop_point_3d
    pop_point_3d *p2 = new pop_point_3d(cs2_,x,y,z);

    return p2;
  }

  return 0;
}

//: update the transform based on the parameters
void pop_rigid_3d::update()
{
  // update the rotation
  trans_.set_identity();

  trans_.set_rotation_euler(params_[0]->value_,
                            params_[1]->value_,
                            params_[2]->value_);

  trans_.set_translation(params_[3]->value_,
                         params_[4]->value_,
                         params_[5]->value_);
}
