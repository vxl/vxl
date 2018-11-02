// This is brl/bbas/bgrl2/bgrl2_hg_hypergraph.h
#ifndef bgrl2_hypergraph_h_
#define bgrl2_hypergraph_h_
//:
// \file
// \author Ming-Ching Chang
// \date   Apr 04, 2005
//
// \verbatim
//  Modifications
//   Ozge C. Ozcanli 11/15/08  Moved up to vxl
// \endverbatim

#include <iostream>
#include <map>
#ifdef _MSC_VER
#  include <vcl_msvc_warnings.h>
#endif

#include <bgrl2/bgrl2_hg_graph.h>
#include <bgrl2/bgrl2_hg_hyperedge.h>

class bgrl2_hg_hypergraph : public bgrl2_hg_graph
{
 protected:
  std::map<int, bgrl2_hg_hyperedge*> hyperedges_;

 public:
  //: ====== Hypergraph query functions ======
  std::map<int, bgrl2_hg_hyperedge*>& hyperedges() {
    return hyperedges_;
  }
  bgrl2_hg_hyperedge* hyperedges(const int i) {
    std::map<int, bgrl2_hg_hyperedge*>::iterator H_it = hyperedges_.find(i);
    if (H_it == hyperedges_.end())
      return nullptr;

    bgrl2_hg_hyperedge* hyperedge = (*H_it).second;
    return hyperedge;
  }

  //: ===== Internal Low-level Graph operation (without handling connectivity) =====
  bool _internal_del_vertex(bgrl2_hg_vertex* vertex) override;
  bool _internal_del_edge(bgrl2_hg_edge* edge) override;

  void _internal_add_hyperedge(bgrl2_hg_hyperedge* hyperedge);
  bool _internal_del_hyperedge(bgrl2_hg_hyperedge* hyperedge);

  //: ===== Internal Mid-level Graph operation (handling connectivity) =====
  void _internal_connect_hyperedge_edge(bgrl2_hg_hyperedge* hyperedge, bgrl2_hg_edge* edge) {
    //: unordered edge list
    hyperedge->connect_edge(edge);
    edge->connect_hyperedge(hyperedge);
  }
  void _internal_disconnect_hyperedge_edge(bgrl2_hg_hyperedge* hyperedge, bgrl2_hg_edge* edge) {
    //: unordered edge list
    hyperedge->disconnect_edge(edge);
    edge->disconnect_hyperedge(hyperedge);
  }

  void _internal_connect_hyperedge_vertex(bgrl2_hg_hyperedge* hyperedge, bgrl2_hg_vertex* vertex) {
    hyperedge->connect_vertex(vertex);
    vertex->connect_hyperedge(hyperedge);
  }
  void _internal_disconnect_hyperedge_vertex(bgrl2_hg_hyperedge* hyperedge, bgrl2_hg_vertex* vertex) {
    hyperedge->disconnect_vertex(vertex);
    vertex->disconnect_hyperedge(hyperedge);
  }

  //: ===== High-level Graph operation for insert/remove/replace element =====
  bool remove_vertex(bgrl2_hg_vertex* vertex) override;
  bool remove_edge(bgrl2_hg_edge* edge) override;
  virtual bool remove_hyperedge(bgrl2_hg_hyperedge* hyperedge);
#if 0
  virtual bool remove_vertex(int id);
  virtual bool remove_edge(int id);
  virtual bool remove_hyperedge(int id);
  virtual bool topo_remove_vertex(int id);
  virtual bool topo_remove_edge(int id);
  virtual bool topo_remove_hyperedge(int id);
#endif // 0

  //: ===== High-level Graph operation that user should use =====
  //  These operations are always topologically consistent,
  //  i.e. the resulting graph is always a geometric graph.
  //  (You will not have an edge that has no end nodes.)
  bool topo_remove_vertex(bgrl2_hg_vertex* vertex) override;
  bool topo_remove_edge(bgrl2_hg_edge* edge) override;
  virtual bool topo_remove_hyperedge(bgrl2_hg_hyperedge* hyperedge);

  bgrl2_hg_hypergraph() : bgrl2_hg_graph() {}
  ~bgrl2_hg_hypergraph() override = default;
};

#endif // bgrl2_hypergraph_h_
