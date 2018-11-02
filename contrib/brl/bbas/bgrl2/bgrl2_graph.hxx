// This is brl/bbas/bgrl2/bgrl2_graph.hxx
#ifndef bgrl2_graph_hxx_
#define bgrl2_graph_hxx_
//:
// \file

#include <iostream>
#include <vector>
#include "bgrl2_graph.h"

#include <cassert>
#ifdef _MSC_VER
#  include <vcl_msvc_warnings.h>
#endif

//-------------------------------------------------------------------
// Graph building functions
//-------------------------------------------------------------------

//: Adds a new vertex to the graph
template<class V, class E>
bool bgrl2_graph<V,E>::add_vertex(V_sptr v)
{
  if (!v) return false;
  vertices_.push_back(v);

  return true; //not doing a duplicate check anymore
}

//: check existence
template<class V, class E>
bool bgrl2_graph<V,E>::exists(V_sptr v)
{
  for ( vertex_iterator v_itr = vertices_.begin();
        v_itr != vertices_.end(); ++v_itr )
  {
    if ((*v_itr) == v)
      return true;
  }
  return false;
}


//: Deletes a vertex in the graph
template<class V, class E>
bool bgrl2_graph<V,E>::remove_vertex(V_sptr v)
{
  if (!v) return false;

  vertices_.remove(v);

  //delete all the edges connected to this vertex
  while (v->out_edges().size()>0)
    remove_edge(*v->out_edges_begin());

  while (v->in_edges().size()>0)
    remove_edge(*v->in_edges_begin());

  return true;
}

//: Add an edge
template<class V, class E>
bool bgrl2_graph<V,E>::add_edge(E_sptr e)
{
  // verify that this edge is not already present
  vertex_edge_iterator e_itr = e->source()->out_edges_begin();
  for (; e_itr != e->source()->out_edges_end(); ++e_itr )
    if ((*e_itr)->target() == e->target())
      return false;

  edges_.push_back(e);
  return true;
}

//: Add an edge with no duplicate checking
template<class V, class E>
bool bgrl2_graph<V,E>::add_edge_no_check(E_sptr e)
{
  if (!e)
    return false;
  edges_.push_back(e);
  return true;
}


//: Add an edge between \p v1 and \p v2
template<class V, class E>
typename bgrl2_graph<V,E>::E_sptr bgrl2_graph<V,E>::add_edge(V_sptr v1, V_sptr v2)
{
  if (!v1 || !v2)
    return nullptr;
  if (v1==v2)
    return nullptr; //can't add the edge to itself

  //add the vertices first
  if (!this->exists(v1))
    this->add_vertex(v1);
  if (!this->exists(v2))
    this->add_vertex(v2);

  //if ( vertices_.count(v1) == 0 && !this->add_vertex(v1) )
  //  return false;
  //if ( vertices_.count(v2) == 0 && !this->add_vertex(v2) )
  //  return false;

  //set the source and target nodes
  E_sptr e = new E(v1, v2);

  if (this->add_edge(e)){
    //if this is a valid edge, add the links from the nodes to this edge
    v1->add_outgoing_edge(e);
    v2->add_incoming_edge(e);

    return e;
  }

  return nullptr; //edge wasn't added
}

//: Remove an edge
template<class V, class E>
bool bgrl2_graph<V,E>::remove_edge(E_sptr e)
{
  if (!e) return false;

  edges_.remove(e);

  //update connectivity from nodes
  bool success = e->source()->del_out_edge(e);

  //the edge might not have a target node
  if (e->target())
    success = success && e->target()->del_in_edge(e);

  return success;
}

//: Remove the edge between \p v1 and \p v2
// \todo {finish this.}
template<class V, class E>
bool bgrl2_graph<V,E>::remove_edge(V_sptr v1, V_sptr v2 )
{
  if (!v1 || !v2) return false;

  // verify that this edge is not already present
  vertex_edge_iterator e_itr = v1->out_edges_begin();
  for (; e_itr != v1->out_edges_end(); ++e_itr )
  {
    if ((*e_itr)->target() == v2){
      E_sptr e = (*e_itr);

      return remove_edge(e);
    }
  }

  return false;
}

//: delete all the vertices
template<class V, class E>
void  bgrl2_graph<V,E>::del_all_vertices()
{
  while (vertices_.size()>0){
    remove_vertex(*vertices_.begin());
  }
}

//: delete all the edges
template<class V, class E>
void  bgrl2_graph<V,E>::del_all_edges()
{
  while (edges_.size()>0){
    remove_edge(*edges_.begin());
  }
}

//: delete all vertices in the graph that are not adjacent to any edges
template<class V, class E>
void bgrl2_graph<V,E>::purge_isolated_vertices()
{
  std::vector<V_sptr> vert_to_del;
  for ( vertex_iterator v_itr = vertices_.begin();
        v_itr != vertices_.end(); ++v_itr )
  {
    if ((*v_itr)->degree()==0)
      vert_to_del.push_back(*v_itr);
  }

  //is there a better way to do this ???
  for (unsigned int i=0; i<vert_to_del.size(); i++)
    remove_vertex(vert_to_del[i]);

  vert_to_del.clear();
}

//: return all vertices with degree one
template<class V, class E>
void bgrl2_graph<V,E>::get_all_degree_one_vertices(std::vector<V_sptr>& vertices)
{
  for ( vertex_iterator v_itr = vertices_.begin();
        v_itr != vertices_.end(); ++v_itr )
  {
    if ((*v_itr)->degree()==1)
      vertices.push_back(*v_itr);
  }
}

//: clear all the nodes and edges of this graph
template<class V, class E>
void bgrl2_graph<V,E>::clear()
{
  del_all_edges();
  del_all_vertices();
}

//-------------------------------------------------------------------
// Directed graph operations
//-------------------------------------------------------------------

//:  returns the successor of edge e in the adjacency list of node source(e) (nil if it does not exist).
template<class V, class E>
typename bgrl2_graph<V,E>::E_sptr bgrl2_graph<V,E>::adj_succ(E_sptr e)
{
  return adj_succ(e, e->source());
}

//:  returns the predecessor of edge e in the adjacency list of node source(e) (nil if it does not exist).
template<class V, class E>
typename bgrl2_graph<V,E>::E_sptr bgrl2_graph<V,E>::adj_pred(E_sptr e)
{
  return adj_pred(e, e->source());
}

//:  returns the cyclic successor of edge e in the adjacency list of node source(e).
template<class V, class E>
typename bgrl2_graph<V,E>::E_sptr bgrl2_graph<V,E>::cyclic_adj_succ(E_sptr e)
{
  return cyclic_adj_succ(e, e->source());
}

//:  returns the cyclic predecessor of edge e in the adjacency list of node source(e).
template<class V, class E>
typename bgrl2_graph<V,E>::E_sptr bgrl2_graph<V,E>::cyclic_adj_pred(E_sptr e)
{
  return cyclic_adj_pred(e, e->source());
}

//: returns the first edge of in_edges(v) (nil if this list is empty).
template<class V, class E>
typename bgrl2_graph<V,E>::E_sptr bgrl2_graph<V,E>::first_in_edge(V_sptr v)
{
  if (v->in_edges().size()>0)
    return v->in_edges().front();
  else
    return nullptr;
}

//:  returns the last edge of in_edges(v) (nil if this list is empty).
template<class V, class E>
typename bgrl2_graph<V,E>::E_sptr bgrl2_graph<V,E>::last_in_edge(V_sptr v)
{
  if (v->in_edges().size()>0)
    return v->in_edges().back();
  else
    return nullptr;
}

//: returns the first edge of out_edges(v) (nil if this list is empty).
template<class V, class E>
typename bgrl2_graph<V,E>::E_sptr bgrl2_graph<V,E>::first_out_edge(V_sptr v)
{
  if (v->out_edges().size()>0)
    return v->out_edges().front();
  else
    return nullptr;
}

//:  returns the last edge of out_edges(v) (nil if this list is empty).
template<class V, class E>
typename bgrl2_graph<V,E>::E_sptr bgrl2_graph<V,E>::last_out_edge(V_sptr v)
{
  if (v->out_edges().size()>0)
    return v->out_edges().back();
  else
    return nullptr;
}

//:  returns the successor of edge e in in_edges(target(e)) (nil if it does not exist).
template<class V, class E>
typename bgrl2_graph<V,E>::E_sptr bgrl2_graph<V,E>::in_succ(E_sptr e)
{
  return adj_succ(e, e->target());
}

//:  returns the predecessor of edge e in in_edges(target(e)) (nil if it does not exist).
template<class V, class E>
typename bgrl2_graph<V,E>::E_sptr bgrl2_graph<V,E>::in_pred(E_sptr e)
{
  return adj_pred(e, e->target());
}

//:  returns the cyclic successor of edge e in in_edges(target(e)) (nil if it does not exist).
template<class V, class E>
typename bgrl2_graph<V,E>::E_sptr bgrl2_graph<V,E>::cyclic_in_succ(E_sptr e)
{
  return cyclic_adj_succ(e, e->target());
}

//:  returns the cyclic predecessor of edge e in in_edges(target(e)) (nil if it does not exist).
template<class V, class E>
typename bgrl2_graph<V,E>::E_sptr bgrl2_graph<V,E>::cyclic_in_pred(E_sptr e)
{
  return cyclic_adj_pred(e, e->target());
}

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

//:  returns the first edge in the adjacency list of v (nil if this list is empty).
template<class V, class E>
typename bgrl2_graph<V,E>::E_sptr bgrl2_graph<V,E>::first_adj_edge(V_sptr v)
{
  if (first_in_edge(v))
    return first_in_edge(v);
  else
    return first_out_edge(v);
}

//:  returns the last edge in the adjacency list of v (nil if this list is empty).
template<class V, class E>
typename bgrl2_graph<V,E>::E_sptr bgrl2_graph<V,E>::last_adj_edge(V_sptr v)
{
  if (last_out_edge(v))
    return last_out_edge(v);
  else
    return last_in_edge(v);
}

//: returns the successor of edge e in the adjacency list of v.
//  Precondition: e is incident to v.
template<class V, class E>
typename bgrl2_graph<V,E>::E_sptr bgrl2_graph<V,E>::adj_succ(E_sptr /*e*/, V_sptr /*v*/)
{
  return nullptr;
}

//: returns the predecessor of edge e in the adjacency list of v.
//  Precondition: e is incident to v.
template<class V, class E>
typename bgrl2_graph<V,E>::E_sptr bgrl2_graph<V,E>::adj_pred(E_sptr /*e*/, V_sptr /*v*/)
{
  return nullptr;
}

//: returns the cyclic successor(CW) of edge e in the adjacency list of v.
//  Precondition: e is incident to v.
//  if this is a leaf edge, it should return itself
template<class V, class E>
typename bgrl2_graph<V,E>::E_sptr bgrl2_graph<V,E>::cyclic_adj_succ(E_sptr e, V_sptr v)
{
  E_sptr adj_edge=nullptr;

  //find the edge e in the adjacency list of v

  //look in the in_edges list first
  for ( vertex_edge_iterator itr = v->in_edges_begin();
        itr != v->in_edges_end(); ++itr )
  {
    if ((*itr) == e) //edge found
    {
      itr++;
      if (itr != v->in_edges_end()){
        adj_edge = (*itr);
      }
      else { //last in_edge, check out_edges
        if (v->out_edges().size()>0)
          adj_edge = v->out_edges().front();
        else //no out_edges
          adj_edge = v->in_edges().front();
      }
      break;
    }
  }

  //if not found look in the out_edges
  if (!adj_edge)
  {
    for ( vertex_edge_iterator itr = v->out_edges_begin();
          itr != v->out_edges_end(); ++itr )
    {
      if ((*itr) == e) //edge found
      {
        itr++;
        if (itr != v->out_edges_end())
          adj_edge = (*itr);
        else  //last out_edge, check in_edges
        {
          if (v->in_edges().size()>0)
            adj_edge = v->in_edges().front();
          else  //no in_edges
            adj_edge = v->out_edges().front();
        }
        break;
      }
    }
  }

  //make sure we found it
  //assert (adj_edge);

  return adj_edge;
}

//: returns the cyclic predecessor(CW) of edge e in the adjacency list of v.
//  Precondition: e is incident to v.
//  if this is a leaf edge, it should return itself
template<class V, class E>
typename bgrl2_graph<V,E>::E_sptr bgrl2_graph<V,E>::cyclic_adj_pred(E_sptr e, V_sptr v)
{
  E_sptr adj_edge=nullptr;

  //find the edge e in the adjacency list of v

  //look in the in_edges list first
  for ( vertex_edge_iterator itr = v->in_edges_begin();
        itr != v->in_edges_end(); ++itr )
  {
    if ((*itr) == e) //edge found
    {
      if (itr == v->in_edges_begin())
      {
        //first in_edge, look in out_edges
        if (v->out_edges().size()>0)
          adj_edge = v->out_edges().back();
        else //no out_edges
          adj_edge = v->in_edges().back();
      }
      else {
        itr--;
        adj_edge = (*itr);
      }
      break;
    }
  }

  //if not found look in the out_edges
  if (!adj_edge)
  {
    for ( vertex_edge_iterator itr = v->out_edges_begin();
          itr != v->out_edges_end(); ++itr )
    {
      if ((*itr) == e) //edge found
      {
        if (itr == v->out_edges_begin())
        {
          //first out_edge, check in_edges
          if (v->in_edges().size()>0)
            adj_edge = v->in_edges().back();
          else //no in_edges
            adj_edge = v->out_edges().back();
        }
        else {
          itr--;
          adj_edge = (*itr);
        }
        break;
      }
    }
  }

  //make sure we found it
  assert (adj_edge);

  return adj_edge;
}

//-------------------------------------------------------------------
// Standard Graph functions
//-------------------------------------------------------------------

//: Return true if the graph contains a cycle
//  This method can go to bgrl2_algo
template<class V, class E>
bool bgrl2_graph<V,E>::has_cycle()
{
  //: Theorem: given an undirected graph G(V,E), there exists a cycle within the graph if |E| >= |V|
  if (edges_.size() >= vertices_.size())
    return true;
  else
    return false;
}

//-------------------------------------------------------------------
// Miscellaneous
//-------------------------------------------------------------------

//: Print an ascii summary to the stream
template<class V, class E>
void bgrl2_graph<V,E>::print_summary( std::ostream& os ) const
{
  os << this->number_of_vertices() << " vertices";
}

#if 0 // these I/O methods commented out!

//: Return IO version number
short bgrl2_graph<V,E>::version() const
{
  return 1;
}

//: Binary save self to stream.
void bgrl2_graph<V,E>::b_write(vsl_b_ostream &os) const
{
  vsl_b_write(os, version());
  vsl_b_write(os, vertices_);
  vsl_b_write(os, edges_);
}

//: Binary load self from stream.
void bgrl2_graph<V,E>::b_read(vsl_b_istream &is)
{
  if (!is) return;

  short ver;
  vsl_b_read(is, ver);
  switch (ver)
  {
  case 1:
  {
    vsl_b_read(is, vertices_);
    vsl_b_read(is, edges_);
    break;
  }
  default:
    std::cerr << "I/O ERROR: bgrl2_graph<V,E>::b_read(vsl_b_istream&)\n"
             << "           Unknown version number "<< ver << '\n';
    is.is().clear(std::ios::badbit); // Set an unrecoverable IO error on stream
  }
  return;
}
#endif // 0

#define BGRL2_GRAPH_INSTANTIATE(V, E) \
template class bgrl2_graph<V , E >

#endif // bgrl2_graph_hxx_
