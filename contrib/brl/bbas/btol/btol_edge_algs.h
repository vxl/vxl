#ifndef _btol_edge_algs_h
#define _btol_edge_algs_h

//-----------------------------------------------------------------------------
//
// \file
// \author J.L. Mundy
// \brief topology algorithms involving edges or edge-centric routines
//
// \verbatim
// Inital version November 25, 2002
// \endverbatim
//
//-----------------------------------------------------------------------------
#include <vcl_vector.h>
#include <btol/dll.h>
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
  static bool split_edge_2d(vtol_vertex_2d_sptr& v, vtol_edge_2d_sptr& e,
                            vtol_edge_2d_sptr& e1, vtol_edge_2d_sptr& e2);
  
  static bool unlink_all_inferiors_twoway(vtol_edge_2d_sptr& e);
  static bool subst_vertex_on_edge(vtol_vertex_sptr& va,
                                   vtol_vertex_sptr& vb,
                                   vtol_edge_sptr& e);
  static void edge_2d_erase(vcl_vector<vtol_edge_2d_sptr>& edges,
                            vtol_edge_2d_sptr& e);

  //:get the bounding box for a set of edges
  static vsol_box_2d bounding_box(vcl_vector<vtol_edge_2d_sptr>& edges);
 private:
  btol_edge_algs();
};

#endif
