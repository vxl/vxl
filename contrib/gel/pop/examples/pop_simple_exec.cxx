#include <iostream>
#include <vector>
#ifdef _MSC_VER
#  include <vcl_msvc_warnings.h>
#endif

#include <pop/pop_vertex.h>
#include <pop/pop_edge.h>
#include <pop/pop_manager.h>
#include <pop/pop_parameter.h>
#include <pop/pop_rigid_3d.h>
#include <pop/pop_point_3d.h>

int main(int argc, char** argv)
{
  // this demonstration will show the process of
  // determining the appropriate transform between
  // two coordinate systems v1 and v2

  // make the two coordinate systems

  pop_vertex cs1;
  pop_vertex cs2;

  //  make an edge between these two coordinate systems

  pop_edge e;
  e.set_vertex(&cs1,&cs2);

  // get six parameters for a rigid transform
  pop_manager pm;

  std::vector<pop_parameter*> params = pm.new_parameters(6);

  // set the rotation to non changeable and just test the translation component.
  // the first 3 parameters are rotation and the second 3 are translation.

  for (int i=0;i<3;i++) {
    params[i]->value_=0.0;
    params[i]->is_changeable_ = false;
  }

  // put them a little off of their true values

  for (int i=3;i<6;i++) {
    params[i]->value_=0.05;
    params[i]->is_changeable_ = true;
  }

  params[3]->value_ = 0.95;
  params[3]->is_changeable_ = true;

  // generate a rigid transform

  pop_rigid_3d rt(params,&cs1,&cs2);

  // let the manager know about the transform since
  // we will want to update this object during the LM process

  pm.add_object(&rt);

  // place this transform on the edge

  e.set_transform(&rt);

  // now make 10 observables, 10 observations and
  // 10 cost functions

  std::vector<pop_geometric_cost_function*> cfs(10);

  int i;
  double x;
  for (x =0,i = 0 ;x < 10;x++,i++)
  {
    // make an observable

    params = pm.new_parameters(3);
    params[0]->value_ = x;
    params[0]->is_changeable_ = false;

    params[1]->value_ = 3*x -2;
    params[1]->is_changeable_ = false;

    params[2]->value_ = x*x +4;
    params[2]->is_changeable_ = false;

    pop_point_3d *observable = new pop_point_3d(&cs1,params);

    // make the observation which is in the second coordinate system
    // and is translated by 1,0,0

    params = pm.new_parameters(3);
    params[0]->value_ = x + 1;  // this is a translation
    params[0]->is_changeable_ = false;

    params[1]->value_ = 3*x -2 ;
    params[1]->is_changeable_ = false;

    params[2]->value_ = x*x + 4;
    params[2]->is_changeable_ = false;

    // make the observation
    pop_point_3d *observation = new pop_point_3d(&cs2,params);

    // now make a cost function between the observable and the observation

    if (!(observable->cast_to_pop_point_3d())) {
        std::cout << "cannot safely downcast\n";
    }

    pop_geometric_cost_function *cf = new pop_geometric_cost_function(observable,observation);

    cfs[i] = cf;
  }

  // now optimize the network

  pm.optimize(cfs);

  // now print out the new transform
  // it should be 0 rotation and a translation in the x direction

  params = e.get_transform()->get_parameters();
  std::cout << "The new transform parameters are\n";
  for (i=0;i<params.size();i++) {
    std::cout << params[i]->value_ << std::endl;
  }

  std::cout << "We want something like 0 0 0 1 0 0\n";

  return 0;
}
