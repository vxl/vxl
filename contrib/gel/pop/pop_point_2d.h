// This is gel/pop/pop_point_2d.h
#ifndef pop_point_2d_h_
#define pop_point_2d_h_
//:
// \file
// \brief a 2d point with a coordinate system
//
// \author
//            Peter Tu April 2003
//            General Electric
//
//-------------------------------------------------------------------------------

#include<vgl/vgl_point_2d.h>
#include<pop/pop_point_2d.h>
#include<pop/pop_geometric_object.h>
#include<pop/pop_vertex.h>
#include<std::list.h>
#include<pop/pop_parameter.h>

//: this is a 2d point with a coordinate system

class pop_point_2d:public vgl_point_2d<double>, public pop_geometric_object
{
 public:
  //: constructor
  pop_point_2d(pop_vertex *coordinate_system,std::vector<pop_parameter*> &params);

  //: constructor
  // this is used when we want to represent a non parametric result
  pop_point_2d(pop_vertex *coordinate_system, double x, double y);

  //: destructor
  virtual ~pop_point_2d();

  //: this is the update method

  virtual void update();

  //: this is the casting which needs to be done
  virtual pop_point_2d* cast_to_pop_point_2d() {return this;}

  //: compute the cost between two points
  virtual double cost(pop_geometric_object *other);
};

#endif // pop_point_2d_h_
