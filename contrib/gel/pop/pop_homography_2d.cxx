// This is gel/pop/pop_homography_2d.cxx
#include "pop_homography_2d.h"
//:
// \file

#include <pop/pop_point_2d.h>
#include <vgl/vgl_homg_point_2d.h>
#include <cassert>
#ifdef _MSC_VER
#  include <vcl_msvc_warnings.h>
#endif

//: constructor
// The parameters for this transform are
// the Euler angle r1 r2 r2 and the translation t1 t2 t3
pop_homography_2d::pop_homography_2d(std::vector<pop_parameter*> params,
                                     pop_vertex *cs1, pop_vertex *cs2) :
  pop_transform(params,cs1,cs2)
{
  this->update();
}

//: destructor
pop_homography_2d::~pop_homography_2d()
{
}

//: transform a geometric object
pop_geometric_object* pop_homography_2d::transform(pop_geometric_object *obj)
{
  // try to transform a 2d point to a 2d point
  pop_point_2d *p1 = obj->cast_to_pop_point_2d();

  if (p1)
  {
    // make sure that p1 comes from the right coordinate
    // system
    assert(p1->coordinate_system_ == cs1_);

    // transform this point
    vgl_homg_point_2d<double> hp1(p1->x(),p1->y(),1.0);

    vgl_homg_point_2d<double> hp2 = trans_ * hp1;
    double w = hp2.w();

    double x = hp2.x();
    double y = hp2.y();

    if (w) {
      x=x/w;
      y=y/w;
    }

    // make a new pop_point_3d
    pop_point_2d *p2 = new pop_point_2d(cs2_,x,y);

    return p2;
  }

  return 0;
}

//: update the transform based on the parameters
void pop_homography_2d::update()
{
  // update the rotation
  // make a 3 by 3 matrix
  vnl_matrix_fixed<double,3,3> mat;

  for (int r=0;r<3;r++)
    for (int c=0;c<3;c++)
      mat(r,c)=params_[r*3+c]->value_;
  trans_.set(mat);
}
