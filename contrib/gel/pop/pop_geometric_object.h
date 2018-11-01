// This is gel/pop/pop_geometric_object.h
#ifndef pop_geometric_object_h_
#define pop_geometric_object_h_
//:
// \file
// \brief this is a geometric object which has a coordinate system.
//
// \author
//            Peter Tu April 2003
//            General Electric
//
//-------------------------------------------------------------------------------

#include <pop/pop_vertex.h>
#include <pop/pop_object.h>

class pop_point_2d;
class pop_point_3d;

//: This is an object that has a coordinate system.
//  The object is based on a set of parameters that can be changed. This object
//  can be transformed into another coordinate system which is a pop_vertex.

class pop_geometric_object : public pop_object
{
 public:
  //: constructor
  pop_geometric_object(pop_vertex *coordinate_system,std::vector<pop_parameter*> &params);

  //: used when this is not a parametric object but just a geometric one
  pop_geometric_object(pop_vertex *coordinate_system);

  //: destructor
  virtual ~pop_geometric_object();

  //: this is the coordinate system
  pop_vertex *coordinate_system_;

  //: we should be able to transform to another coordinate system
  pop_geometric_object* transform(pop_vertex *destination);

  //: we need to have safe down casting

  pop_geometric_object* cast_to_pop_geometric_object() {return this;}
  virtual pop_point_2d* cast_to_pop_point_2d() {return 0;}
  virtual pop_point_3d* cast_to_pop_point_3d() {return 0;}

  //: computes a cost between this and another object
  virtual double cost(pop_geometric_object *other);

  virtual void update() =0;
};

#endif // pop_geometric_object_h_
