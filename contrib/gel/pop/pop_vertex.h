// This is gel/pop/pop_vertex.h
#ifndef pop_vertex_h_
#define pop_vertex_h_
//:
// \file
// \brief this is a vertex/coordinate system in a coordinate system graph
//
// \author
//            Peter Tu April 2003
//            General Electric
//
//-------------------------------------------------------------------------------

#include<std::list.h>
class pop_edge;

//: A parameter that can be optimized
class pop_vertex
{
 public:
  //: constructor
  pop_vertex();

  //: destructor
  ~pop_vertex();

  //: A list of edges that can lead to other vertex
  void add_edge(pop_edge*);

  //: find a path to another vertex
  // \todo should be protected and accessed by friend edge
  bool search(pop_vertex *destination, std::list<pop_edge*> &path);

  //: set all vertex to untouched
  void clear();

  //: a flag to which states whether or not the vertex has been touched during a search
  bool touched_;

  //: find a path of edges to the following vertex
  bool find_path(pop_vertex *destination, std::list<pop_edge*> &path);

 private:
  //: a list of edges that can lead out of this vertex
  std::list<pop_edge*> edges_;
};

#endif // pop_vertex_h_
