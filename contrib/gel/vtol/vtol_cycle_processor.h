#ifndef vtol_cycle_processor_h_
#define vtol_cycle_processor_h_

//:
// \file
// \brief A class for tracing boundaries and forming nested one_cycles.
//
// The input is a set of vtol_edge(s) which may or may not
// define one or more closed paths, or cycles. The output is a set of
// vtol_one_chain(s) which define a nested set of cycles.  That is, there is
// an outer, bounding, cycle and zero or more interior hole boundaries.
// The algorithm requires a tolerance which is used in the process of
// deciding if one boundary is enclosed by another.
//
// \author
//          J.L. Mundy August 13, 2000
//          GE Corporate Research and Development
//
//-----------------------------------------------------------------------------

#include <vcl_vector.h>
#include <vtol/vtol_edge_2d_sptr.h>
#include <vtol/vtol_one_chain_sptr.h>
#include <vtol/vtol_vertex_sptr.h>
class vtol_edge;

class vtol_cycle_processor
{
 public:
  vtol_cycle_processor(vcl_vector<vtol_edge_2d_sptr>& edges);
  vtol_cycle_processor(vcl_vector<vtol_edge*>& edges);
  ~vtol_cycle_processor(){};

  // PUBLIC INTERFACE----------------------------------------------------------
  bool nested_one_cycles(vcl_vector<vtol_one_chain_sptr>& one_chains,
                         const float& tolerance = 1e-03);
 protected:
  //internal utilites
  void set_bridge_vars();
  void init(vcl_vector<vtol_edge_2d_sptr>& edges);
  vtol_edge_2d_sptr search_for_next_edge(vcl_vector<vtol_edge_2d_sptr>& edges_at_last);
  void add_edge_to_path();
  bool classify_path(vcl_vector<vtol_edge_2d_sptr>& path_edges, vtol_one_chain_sptr& chain);
  void compute_cycles();
  void sort_one_cycles();
  void process();
  //members
  bool valid_;
  float tolerance_;
  bool cycle_;
  bool found_next_edge_;
  vtol_vertex_sptr first_;
  vtol_vertex_sptr last_;
  vtol_edge_2d_sptr l_;
  vtol_edge_2d_sptr next_edge_;
  vcl_vector<vtol_edge_2d_sptr> edges_;
  vcl_vector<vtol_edge_2d_sptr> e_stack_;
  vcl_vector<vtol_vertex_sptr> v_stack_;
  vcl_vector<vtol_one_chain_sptr> chains_;
  vcl_vector<vtol_one_chain_sptr> nested_one_cycles_;
};

#endif // vtol_cycle_processor_h_
