// This is gel/pop/pop_transform.h
#ifndef pop_transform_h_
#define pop_transform_h_
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
#include<pop/pop_parameter.h>
#include<std::vector.h>
#include<pop/pop_vertex.h>
#include<pop/pop_geometric_object.h>

//: A parameter that can be optimized
class pop_transform:public pop_object
{
 public:
  //: constructor
  pop_transform(std::vector<pop_parameter*> params,
                pop_vertex *cs1, pop_vertex *cs2);

  //: destructor
  virtual ~pop_transform();

  //: transform a geometric object
  virtual pop_geometric_object* transform(pop_geometric_object *obj)=0;

  virtual void update() = 0;

 protected:
  // the source coordinate system
  pop_vertex *cs1_;

  // the target coordinate suystem
  pop_vertex *cs2_;
};

#endif // pop_transform_h_
