// This is brl/bbas/bgrl/bgrl_search_func.h
#ifndef bgrl_search_func_h_
#define bgrl_search_func_h_
//:
// \file
// \brief A set of search functions to search through a graph
// \author Matt Leotta, (mleotta@lems.brown.edu)
// \date 3/24/04
//
// \verbatim
//  Modifications
// \endverbatim

#include <vbl/vbl_ref_count.h>
#include <bgrl/bgrl_vertex_sptr.h>
#include <bgrl/bgrl_edge_sptr.h>
#include <vcl_deque.h>
#include <vcl_set.h>


//: The abstract base class for search functions
class bgrl_search_func : public vbl_ref_count
{
 public:
  //: Constructor
  bgrl_search_func(const bgrl_vertex_sptr& init_vertex = NULL)
    : curr_vertex_(init_vertex) {}

  //: Destructor
  ~bgrl_search_func(){}

  bgrl_vertex_sptr curr_vertex() const { return curr_vertex_; }

  //: Returns the edge to the next vertex in the search
  virtual bgrl_edge_sptr next_vertex() = 0;

 protected:
  bgrl_vertex_sptr curr_vertex_;
};

//================================================================

//: A search function for breadth first search
class bgrl_breadth_search : public bgrl_search_func
{
 public:
  //: Constructor
  bgrl_breadth_search(const bgrl_vertex_sptr& init_vertex = NULL)
    : bgrl_search_func(init_vertex) {visited_.insert(init_vertex);}

  //: Destructor
  ~bgrl_breadth_search(){}

  //: Returns the edge to the next vertex in the search
  virtual bgrl_edge_sptr next_vertex();

 protected:
  //: The queue of nodes to be evaluated
  vcl_deque<bgrl_edge_sptr> eval_queue_;
  //: The set of visited nodes
  vcl_set<bgrl_vertex_sptr> visited_;
};

//================================================================

//: A search function for depth first search
class bgrl_depth_search : public bgrl_search_func
{
 public:
  //: Constructor
  bgrl_depth_search(const bgrl_vertex_sptr& init_vertex = NULL)
    : bgrl_search_func(init_vertex) {visited_.insert(init_vertex);}

  //: Destructor
  ~bgrl_depth_search(){}

  //: Returns the edge to the next vertex in the search
  virtual bgrl_edge_sptr next_vertex();
  
 protected:
  //: The queue of nodes to be evaluated
  vcl_deque<bgrl_edge_sptr> eval_queue_;
  //: The set of visited nodes
  vcl_set<bgrl_vertex_sptr> visited_;
};



#endif // bgrl_search_func_h_
