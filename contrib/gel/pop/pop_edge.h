// This is gel/pop/pop_edge.h
#ifndef pop_edge_h_
#define pop_edge_h_
//:
// \file
// \brief This is an edge/coordinate system transform in a coordinate system graph
//
// \author
//            Peter Tu April 2003
//            General Electric
//
//-------------------------------------------------------------------------------

class pop_vertex;
#include<pop/pop_transform.h>
#include<std::list.h>

//: A parameter that can be optimized
class pop_edge
{
 public:
  //: constructor
  pop_edge();

  //: destructor
  ~pop_edge();

  //: set the first and second vertex
  void set_vertex(pop_vertex *v1, pop_vertex *v2);

  //: set the transform
  void set_transform(pop_transform *t);

  //: get the transform
  pop_transform* get_transform();

  //: help search to find a destination vertex
  bool search(pop_vertex* destination, std::list<pop_edge*> &path);

 private:
  // the source vertex
  pop_vertex *v1_;

  // the sink vertex
  pop_vertex *v2_;

  // the transform
  pop_transform *transform_;
};

#endif // pop_edge_h_
