#ifndef btol_edge_algs_h_
#define btol_edge_algs_h_

//-----------------------------------------------------------------------------
//:
// \file
// \author J.L. Mundy
// \brief topology algorithms involving edges or edge-centric routines
//
// \verbatim
// Initial version November 25, 2002
// \endverbatim
//
//-----------------------------------------------------------------------------
#include <vcl_vector.h>
#include "dll.h"
#include <vtol/vtol_vertex_sptr.h>
#include <vtol/vtol_edge_sptr.h>
#include <vtol/vtol_vertex_2d_sptr.h>
#include <vtol/vtol_edge_2d_sptr.h>
#include <vsol/vsol_box_2d.h>
class btol_edge_algs
{
 public:
  static BTOL_DLL_DATA const double tol;
  ~btol_edge_algs();
  //: use static methods

  //:Splits e at v and returns the two edges e1, e2, which are incident at v.
  static bool split_edge_2d(vtol_vertex_2d_sptr const& v,
                            vtol_edge_2d_sptr const& e,
                            vtol_edge_2d_sptr& e1, vtol_edge_2d_sptr& e2);

  //:Removes e's inferiors from e and removes e from superiors of e's inferiors
  static bool unlink_all_inferiors_twoway(vtol_edge_2d_sptr const& e);

  //:Replace va on edge e by vb
  static bool subst_vertex_on_edge(vtol_vertex_sptr const& va,
                                   vtol_vertex_sptr const& vb,
                                   vtol_edge_sptr const& e);

  //:A convenient erase interface
  static void edge_2d_erase(vcl_vector<vtol_edge_2d_sptr>& edges,
                            vtol_edge_2d_sptr const& e);

  //:Get the bounding box for a set of edges
  static vsol_box_2d bounding_box(vcl_vector<vtol_edge_2d_sptr>& edges);
  
  //:Find the vertex closest to the specified position
  static vtol_vertex_2d_sptr closest_vertex(vtol_edge_2d_sptr const& e,
                                            const double x, const double y);
 private:
  btol_edge_algs();
};

#endif
