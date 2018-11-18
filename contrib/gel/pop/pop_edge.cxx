// This is gel/pop/pop_edge.cxx
#include "pop_edge.h"
//:
// \file
#include <pop/pop_transform.h>
#include <pop/pop_vertex.h>

//: constructor
pop_edge::pop_edge()
{
  v1_ = 0;
  v2_ = 0;
  transform_ = 0;
}


//: destructor
pop_edge::~pop_edge()
{
}


//: set the first and second vertex
void pop_edge::set_vertex(pop_vertex *v1, pop_vertex *v2)
{
  v1_ = v1;
  v2_ = v2;
  v1_->add_edge(this);
}


//: set the transform
void pop_edge::set_transform(pop_transform *t)
{
  transform_ = t;
}


//: get the transform
pop_transform* pop_edge::get_transform()
{
  return transform_;
}


//: help search to find a destination vertex
bool pop_edge::search(pop_vertex* destination, std::list<pop_edge*> &path)
{
  if (v2_==destination) {
    // the path is complete
    path.push_front(this);
    return true;
  }
  // we must push ahead and see if v2 leads to path;
  if (v2_->search(destination,path)) {
    // add this node to the path
    path.push_front(this);
    return true;
  }
  // it appears that this edge does not lead to the destination
  return false;
}
