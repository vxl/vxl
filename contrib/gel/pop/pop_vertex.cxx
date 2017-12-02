// This is gel/pop/pop_vertex.cxx
#include "pop_vertex.h"
//:
// \file
#include <pop/pop_edge.h>


// make the static
static std::list<pop_vertex*> all_vertex_;

//: constructor
pop_vertex::pop_vertex()
{
  touched_ = false;
  // add this to the list of all known vertex
  all_vertex_.push_back(this);
}


//: destructor
pop_vertex::~pop_vertex()
{
}


//: A list of edges that can lead to other vertex
void pop_vertex::add_edge(pop_edge* ed)
{
  edges_.push_back(ed);
}

//: set all vertex to untouched
void pop_vertex::clear()
{
  // mark all vertex as un touched
  std::list<pop_vertex*>::iterator it;
  for (it= all_vertex_.begin();it!=all_vertex_.end();it++) {
    (*it)->touched_ = false;
  }
}

//: find a path to another vertex
bool pop_vertex::search(pop_vertex *destination, std::list<pop_edge*> &path)
{
  // if this vertex has already been touched return false;
  if (touched_) {
    return false;
  }

  // set touched to true so we will not go through this vertex again
  touched_ = true;

  // search all edges

  std::list<pop_edge*>::iterator ei;

  // perform a depth first search

  for (ei=edges_.begin();ei!=edges_.end();ei++) {
    if ((*ei)->search(destination,path)) {
      return true;
    }
  }
  // it looks like no path exists so return false
  return false;
}

//: find a path of edges to the following vertex
bool pop_vertex::find_path(pop_vertex *destination, std::list<pop_edge*> &path)
{
  // clear all the vertex touched flags
  clear();

  // start to search for the path
  return search(destination,path);
}
