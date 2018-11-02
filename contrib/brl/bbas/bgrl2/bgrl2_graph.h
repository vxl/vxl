// This is brl/bbas/bgrl2/bgrl2_graph.h
#ifndef bgrl2_graph_h_
#define bgrl2_graph_h_
//:
// \file
// \brief A templated generic graph class
// \author Amir Tamrakar
// \date Feb. 30, 2005
//
// This is a templated generic graph class that works with
// vertex and edge classes subclassed from bgrl2_vertex and bgrl2_edge
// classes
//
// The clockwise and/or counterclockwise ordering of the edges are based on
// time of insertion of the edges for that node while the graph is created
//
// IMPORTANT WARNING to users of this graph class to work with DIRECTED graphs:
//                   e.g. you have the following graph with times of creation of the edges
//                        as noted by the numbers
// \verbatim
//                                 2
//                                 ^
//                                 |
//                            1 -> A <- 3
//                                 |
//                                 v
//                                 4
// \endverbatim
//                   Ideally, cyclic_adj_succ(1,A) should return 2
//                   however, in this implementation it returns 3 (the next in_edge)
//                   (When this class is used for shock_graphs because of special properties
//                    of shock_graphs, this is not a problem, i.e. the case in our example
//                    never happens in a shock graph, it is against the nature of flows
//                    that gives rise to directed shock graphs.)
//
//                   This should be noted and corrected or undirected graph operations
//                   should never be used on directed graphs where the example case can occur
//
// \verbatim
//  Modifications
//  Amir Tamrakar 02/30/05    I wanted to develop this graph class into
//                            a standardized graph like the one defined in LEDA.
//                            (Look at http://www.algorithmic-solutions.info/leda_manual/graph.html )
//                            for more detailed descriptions.
//                            So far, I have implemented the more urgent functions only.
//
//  Amir Tamrakar 06/20/05    Modified it (finally) to use smart pointer vertex and edge
//                            classes. This removes the need to delete them.
//
//  Amir Tamrakar 09/27/05    changed the containers from sets to lists
//
//  Ozge Can Ozcanli 10/06/06 Added version number and binary io functions
//  Ozge C. Ozcanli 11/15/08  Moved up to vxl, minor fixes
// \endverbatim

#include <list>
#include <vector>
#include <iostream>
#ifdef _MSC_VER
#  include <vcl_msvc_warnings.h>
#endif
#include <vbl/vbl_ref_count.h>
#include <vbl/vbl_smart_ptr.h>

//: The templated graph class
template <class V, class E>
class bgrl2_graph : public vbl_ref_count
{
 protected:
  typedef vbl_smart_ptr<V> V_sptr;
  typedef vbl_smart_ptr<E> E_sptr;

 public:

  typedef typename std::list<V_sptr>::iterator vertex_iterator;
  typedef typename std::list<E_sptr>::iterator edge_iterator;

  typedef typename V::edge_iterator vertex_edge_iterator;

  //: Constructor
  bgrl2_graph() : vbl_ref_count(), vertices_(), edges_() {}

  //: Destructor
  ~bgrl2_graph() override = default;

  //-------------------------------------------------------------------
  // Standard Graph functions
  //-------------------------------------------------------------------

  //: Return true if the graph contains a cycle
  //  This method can go to bgrl2/algo
  bool has_cycle();

  //-------------------------------------------------------------------
  // Graph building functions
  //-------------------------------------------------------------------

  //: Adds a new vertex to the graph
  // \retval true if the vertex was added
  // \retval false if the vertex could not be added
  bool add_vertex(V_sptr v);

  //: Deletes a vertex in the graph
  // \retval true if the vertex was deleted
  // \retval false if the vertex was not found in the graph
  bool remove_vertex(V_sptr v);

  //: Add an edge
  bool add_edge(E_sptr e);

  //: check existence
  bool exists(V_sptr v);

  //: Add an edge with no duplicate check (just like add_vertex() method)
  bool add_edge_no_check(E_sptr e);

  //: Add an edge between \p v1 and \p v2
  //  This function will instantiate an edge and
  //  add the links from the nodes to the edges
  //  if not already there.
  E_sptr add_edge(V_sptr v1, V_sptr v2);

  //: Remove an edge
  // \retval true if the edge was deleted
  // \retval false if the edge was not found in the graph
  bool remove_edge(E_sptr e);

  //: Remove the edge between \p v1 and \p v2
  // \retval true if the edge was deleted
  // \retval false if the edge was not found in the graph
  bool remove_edge(V_sptr v1, V_sptr v2 );

  //: delete all the vertices
  void  del_all_vertices();

  //: delete all the edges
  void  del_all_edges();

  //: delete all vertices in the graph that are not adjacent to any edges
  void purge_isolated_vertices();

  //: return all vertices with degree one
  void get_all_degree_one_vertices(std::vector<V_sptr>& vertices);

  //: clear all the nodes and edges of this graph
  virtual void clear();

  //-------------------------------------------------------------------
  // Access operations
  //-------------------------------------------------------------------

  //: returns the number of nodes in G.
  int number_of_vertices() const { return vertices_.size(); }

  //: returns the number of edges in G.
  int number_of_edges() const { return edges_.size(); }

  //: returns the list V of all nodes of G
  const std::list<V_sptr>& all_nodes() { return vertices_; }

  //:  returns the list E of all edges of the shock graph.
  const std::list<E_sptr>& all_edges() { return edges_; }

  //: Returns an iterator to the beginning of the set of vertices
  vertex_iterator  vertices_begin() { return vertices_.begin(); }

  //: Returns an iterator to the end of the set of vertices
  vertex_iterator  vertices_end() { return vertices_.end(); }

  //: Returns an iterator to the beginning of the set of edges
  edge_iterator  edges_begin() { return edges_.begin(); }

  //: Returns an iterator to the end of the set of edges
  edge_iterator  edges_end() { return edges_.end(); }

  //-------------------------------------------------------------------
  // Directed graph operations
  //-------------------------------------------------------------------

  E_sptr adj_succ(E_sptr e); ///<  returns the successor of edge e in the adjacency list of vertex source(e) (nil if it does not exist).
  E_sptr adj_pred(E_sptr e); ///<  returns the predecessor of edge e in the adjacency list of vertex source(e) (nil if it does not exist).
  E_sptr cyclic_adj_succ(E_sptr e); ///<  returns the cyclic successor of edge e in the adjacency list of vertex source(e).
  E_sptr cyclic_adj_pred(E_sptr e); ///<  returns the cyclic predecessor of edge e in the adjacency list of vertex source(e).
  E_sptr first_in_edge(V_sptr v); ///<  returns the first edge of in_edges(v) (nil if this list is empty).
  E_sptr last_in_edge(V_sptr v); ///<  returns the last edge of in_edges(v) (nil if this list is empty).
  E_sptr first_out_edge(V_sptr v); ///<  returns the first edge of out_edges(v) (nil if this list is empty).
  E_sptr last_out_edge(V_sptr v); ///<  returns the last edge of out_edges(v) (nil if this list is empty).
  E_sptr in_succ(E_sptr e); ///<  returns the successor of edge e in in_edges(target(e)) (nil if it does not exist).
  E_sptr in_pred(E_sptr e); ///<  returns the predecessor of edge e in in_edges(target(e)) (nil if it does not exist).
  E_sptr cyclic_in_succ(E_sptr e); ///<  returns the cyclic successor of edge e in in_edges(target(e)) (nil if it does not exist).
  E_sptr cyclic_in_pred(E_sptr e); ///<  returns the cyclic predecessor of edge e in in_edges(target(e)) (nil if it does not exist).

  //-------------------------------------------------------------------
  // Undirected graph operations
  //-------------------------------------------------------------------

// IMPORTANT WARNING to users of this graph class to work with DIRECTED graphs:
//                   e.g. you have the following graph with times of creation of the edges
//                        as noted by the numbers
//                                 2
//                                 ^
//                                 |
//                            1 -> A <- 3
//                                 |
//                                 v
//                                 4
//                   Ideally, cyclic_adj_succ(1,A) should return 2
//                   however, in this implementation it returns 3 (the next in_edge)
//                   (When this class is used for shock_graphs because of special properties
//                    of shock_graphs, this is not a problem, i.e. the case in our example
//                    never happens in a shock graph, it is against the nature of flows
//                    that gives rise to directed shock graphs.)
//
//                   This should be noted and corrected or undirected graph operations
//                   should never be used on directed graphs where the example case can occur

  E_sptr first_adj_edge(V_sptr v); ///<  returns the first edge in the adjacency list of v (nil if this list is empty).
  E_sptr last_adj_edge(V_sptr v); ///<  returns the last edge in the adjacency list of v (nil if this list is empty).
  E_sptr adj_succ(E_sptr e, V_sptr v); ///<  returns the successor of edge e in the adjacency list of v.
  E_sptr adj_pred(E_sptr e, V_sptr v);///<  returns the predecessor of edge e in the adjacency list of v.
  E_sptr cyclic_adj_succ(E_sptr e, V_sptr v); ///<  returns the cyclic successor of edge e in the adjacency list of v.
  E_sptr cyclic_adj_pred(E_sptr e, V_sptr v); ///<  returns the cyclic predecessor of edge e in the adjacency list of v.

  //-------------------------------------------------------------------
  // Miscellaneous
  //-------------------------------------------------------------------

  //: Print an ascii summary to the stream
  void print_summary(std::ostream &os) const;

#if 0 // these I/O methods commented out
  //: Return IO version number;
  short version() const;

  //: Binary save self to stream.
  void b_write(vsl_b_ostream &os) const;

  //: Binary load self from stream.
  void b_read(vsl_b_istream &is);
#endif // 0

 protected:

  //: The list of vertices
  std::list<V_sptr> vertices_;

  //: The list of edges
  std::list<E_sptr> edges_;
};

#endif // bgrl2_graph_h_
