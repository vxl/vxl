#ifndef bdgl_region_algs_h_
#define bdgl_region_algs_h_
//-----------------------------------------------------------------------------
//:
// \file
// \author J.L. Mundy
// \brief vdgl_digital_region algorithms
//
// \verbatim
//  Modifications
//   Initial version July 17, 2004
// \endverbatim
//
//-----------------------------------------------------------------------------
#include <vcl_map.h>
#include <vnl/vnl_matrix.h>
#include <vdgl/vdgl_digital_region_sptr.h>

class bdgl_region_algs
{
 public:
  ~bdgl_region_algs();

  //:Computes the Mahanabolis distance between two regions
  static  float mahanabolis_distance(vdgl_digital_region_sptr const& r1,
                                     vdgl_digital_region_sptr const& r2);
                                         

  //:Merges two digital regions and returns false if there is failure
  static  bool merge(vdgl_digital_region_sptr const& r1,
                     vdgl_digital_region_sptr const& r2, 
                     vdgl_digital_region_sptr& rm);
                                         


 private:
  bdgl_region_algs();
};

#endif
