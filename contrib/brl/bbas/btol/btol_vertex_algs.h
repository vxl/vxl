#ifndef _btol_vertex_algs_h
#define _btol_vertex_algs_h

//-----------------------------------------------------------------------------
//
// \file
// \author J.L. Mundy
// \brief topology algorithms involving vertices or vertex-centric routines
// The implementation defines static methods for all routines
// \verbatim
// Inital version November 25, 2002
// \endverbatim
//
//-----------------------------------------------------------------------------
#include <vtol/vtol_vertex_sptr.h>
#include <vtol/vtol_edge_sptr.h>
#include <vtol/vtol_vertex_2d_sptr.h>
#include <vtol/vtol_edge_2d_sptr.h>
#include <vcl_vector.h>

class btol_vertex_algs 
{
 public:
  ~btol_vertex_algs();

  //:
  // Replaces va by vb on all the edges connected to va.
  // The result is that vb has the union of the sets of 
  // edges incident on va and vb as superiors.
  static bool merge_superiors(vtol_vertex_sptr& va,
                              vtol_vertex_sptr& vb);
  
  static void vertex_erase(vcl_vector<vtol_vertex>& verts,
                           vtol_vertex_sptr& v);

 private:
  btol_vertex_algs();
};

#endif
