#ifndef btol_edge_algs_h_
#define btol_edge_algs_h_

//-----------------------------------------------------------------------------
//:
// \file
// \author J.L. Mundy
// \brief topology algorithms involving edges or edge-centric routines
//
// \verbatim
// Modifications
// Inital version November 25, 2002
// \endverbatim
//
//-----------------------------------------------------------------------------
#include <btol/dll.h>
#include <vtol/vtol_vertex_sptr.h>
#include <vtol/vtol_edge_sptr.h>
#include <vtol/vtol_vertex_2d_sptr.h>
#include <vtol/vtol_edge_2d_sptr.h>

class btol_edge_algs
{
  // use static methods
 public:
  static BTOL_DLL_DATA const double tol;
  ~btol_edge_algs();
  //:
  // Splits e at v and returns the two edges e1, e2, which are incident at v.
  // If v is not within etol of a point on edge e, then false is returned, and
  // e is not split
  static bool split_edge_2d(vtol_vertex_2d_sptr& v, vtol_edge_2d_sptr& e,
                            vtol_edge_2d_sptr& e1, vtol_edge_2d_sptr& e2);

  static bool unlink_all_inferiors_twoway(vtol_edge_2d_sptr& e);
  static bool subst_vertex_on_edge(vtol_vertex_sptr& va, vtol_vertex_sptr& vb, vtol_edge_sptr& e);
 private:
  btol_edge_algs();
};

#endif
