// This is gel/pop/pop_point_3d.h
#ifndef pop_point_3d_h_
#define pop_point_3d_h_
//:
// \file
// \brief a 2d point with a coordinate system
//
// \author
//            Peter Tu April 2003
//            General Electric
//
//-------------------------------------------------------------------------------

#include<vgl/vgl_point_3d.h>
#include<pop/pop_geometric_object.h>

//: this is a 2d point with a coordinate system

class pop_point_3d:public vgl_point_3d<double>, public pop_geometric_object
{
 public:
  //: constructor
  pop_point_3d(pop_vertex *coordinate_system,std::vector<pop_parameter*> &params);

  //: used when this is not a parametric object
  pop_point_3d(pop_vertex *coordinate_system, double x, double y, double z);

  //: destructor
  virtual ~pop_point_3d();

  //: this is the update method

  virtual void update();

  //: this is the casting which needs to be done
  virtual pop_point_3d* cast_to_pop_point_3d() {return this;}

  //: this is the square distance between two 3d points
  virtual double cost(pop_geometric_object *other);
};

#endif // pop_point_3d_h_
