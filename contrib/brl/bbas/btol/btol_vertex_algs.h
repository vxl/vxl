#ifndef btol_vertex_algs_h_
#define btol_vertex_algs_h_

//-----------------------------------------------------------------------------
//:
// \file
// \author J.L. Mundy
// \brief topology algorithms involving vertices or vertex-centric routines
// The implementation defines static methods for all routines
//
// \verbatim
// Modifications
// Inital version November 25, 2002
// \endverbatim
//
//-----------------------------------------------------------------------------
#include <vtol/vtol_vertex_sptr.h>

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

 private:
  btol_vertex_algs();
};

#endif
