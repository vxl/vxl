// This is gel/pop/pop_rigid_3d.h
#ifndef pop_rigid_3d_h_
#define pop_rigid_3d_h_
//:
// \file
// \brief this is a vertex/coordinate system in a coordinate system graph
//
// \author
//            Peter Tu April 2003
//            General Electric
//
//-------------------------------------------------------------------------------

#include<pop/pop_object.h>
#include<vgl/algo/vgl_h_matrix_3d.h>
#include<pop/pop_transform.h>

//: A parameter that can be optimized
class pop_rigid_3d : public pop_transform
{
 public:
  //: constructor
  // the parameters for this transform are
  // the Euler angle r1 r2 r2 and the translation t1 t2 t3
  pop_rigid_3d(std::vector<pop_parameter*> params,
               pop_vertex *cs1, pop_vertex *cs2);

  //: destructor
  virtual ~pop_rigid_3d();

  //: transform a geometric object
  virtual pop_geometric_object* transform(pop_geometric_object *obj);

  //: update the transform based on the parameters
  virtual void update();

 private:
  vgl_h_matrix_3d<double> trans_;
};

#endif // pop_rigid_3d_h_
