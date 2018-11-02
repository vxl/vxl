// This is brl/bbas/bgrl/bgrl_search_func.h
#ifndef bgrl_search_func_h_
#define bgrl_search_func_h_
//:
// \file
// \brief A set of search functions to search through a graph
// \author Matt Leotta, (mleotta@lems.brown.edu)
// \date March 24, 2004
//
// \verbatim
//  Modifications
//   10-sep-2004 Peter Vanroose Added copy ctor with explicit vbl_ref_count init
// \endverbatim

#include <iostream>
#include <deque>
#include <set>
#include <vbl/vbl_ref_count.h>
#include <bgrl/bgrl_vertex_sptr.h>
#include <bgrl/bgrl_edge_sptr.h>
#ifdef _MSC_VER
#  include <vcl_msvc_warnings.h>
#endif


//: The abstract base class for search functions
class bgrl_search_func : public vbl_ref_count
{
 public:
  // Constructor
  bgrl_search_func(const bgrl_vertex_sptr& init_vertex = nullptr)
    : curr_vertex_(init_vertex) {}

  // Copy constructor
  bgrl_search_func(bgrl_search_func const& f)
    : vbl_ref_count(), curr_vertex_(f.curr_vertex_) {}

  // Destructor
  ~bgrl_search_func() override = default;

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
  bgrl_breadth_search(const bgrl_vertex_sptr& init_vertex = nullptr)
    : bgrl_search_func(init_vertex) {visited_.insert(init_vertex);}

  //: Destructor
  ~bgrl_breadth_search() override= default;

  //: Returns the edge to the next vertex in the search
  bgrl_edge_sptr next_vertex() override;

 protected:
  //: The queue of nodes to be evaluated
  std::deque<bgrl_edge_sptr> eval_queue_;
  //: The set of visited nodes
  std::set<bgrl_vertex_sptr> visited_;
};

//================================================================

//: A search function for depth first search
class bgrl_depth_search : public bgrl_search_func
{
 public:
  //: Constructor
  bgrl_depth_search(const bgrl_vertex_sptr& init_vertex = nullptr)
    : bgrl_search_func(init_vertex) {visited_.insert(init_vertex);}

  //: Destructor
  ~bgrl_depth_search() override= default;

  //: Returns the edge to the next vertex in the search
  bgrl_edge_sptr next_vertex() override;

 protected:
  //: The queue of nodes to be evaluated
  std::deque<bgrl_edge_sptr> eval_queue_;
  //: The set of visited nodes
  std::set<bgrl_vertex_sptr> visited_;
};

#endif // bgrl_search_func_h_
