#ifndef _bdgl_curve_algs_h
#define _bdgl_curve_algs_h

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
#include <btol/dll.h>
#include <vdgl/vdgl_edgel_chain_sptr.h>

class bdgl_curve_algs 
{
public:
  static BTOL_DLL_DATA const double tol;
  ~bdgl_curve_algs();
  // use static methods
  //:
  // Finds the edgel index on a digital curve closest to the given 
  // point (x, y). Later this routine can become a method on 
  // vdgl_edgel_chain.
  //
  static int closest_point(vdgl_edgel_chain_sptr& dc,
                            const double x, const double y);
                       
 private:
  bdgl_curve_algs();
};

#endif
