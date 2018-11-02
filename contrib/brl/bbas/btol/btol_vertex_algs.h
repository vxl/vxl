#ifndef btol_vertex_algs_h_
#define btol_vertex_algs_h_
//-----------------------------------------------------------------------------
//:
// \file
// \author J.L. Mundy
// \brief topology algorithms involving vertices or vertex-centric routines
// \verbatim
//  Initial version November 25, 2002
// \endverbatim
//
//-----------------------------------------------------------------------------
#include <iostream>
#include <vector>
#include <vnl/vnl_matrix_fixed.h>
#include <vtol/vtol_vertex_sptr.h>
#include <vtol/vtol_vertex_2d_sptr.h>
#ifdef _MSC_VER
#  include <vcl_msvc_warnings.h>
#endif

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

  static void vertex_erase(std::vector<vtol_vertex>& verts,
                           vtol_vertex_sptr& v);

  static vtol_vertex_2d_sptr transform(vtol_vertex_2d_sptr const& v,
                                       vnl_matrix_fixed<double, 3, 3> const& T);
 private:
  btol_vertex_algs();
};

#endif
