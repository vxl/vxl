#ifndef bdgl_region_algs_h_
#define bdgl_region_algs_h_
//-----------------------------------------------------------------------------
//:
// \file
// \author J.L. Mundy
// \date   July 17, 2004
// \brief vdgl_digital_region algorithms
//
// \verbatim
//  Modifications
//   22-SEP-2004 - Peter Vanroose - bug fix: do not modify intensities of given
//                                  regions with qsort in earth_mover_distance()
// \endverbatim
//
//-----------------------------------------------------------------------------
#include <vdgl/vdgl_digital_region_sptr.h>

class bdgl_region_algs
{
  bdgl_region_algs() {} //!< private default constructor: all methods are static

 public:
  ~bdgl_region_algs() {}

  //:Computes the Mahalanobis distance between two regions
  // Returns -1 if the distance cannot be reliably calculated.
  static float mahalanobis_distance(vdgl_digital_region_sptr const& r1,
                                    vdgl_digital_region_sptr const& r2);
  //:Computes the intensity distance between two regions
  // Returns -1 if the distance cannot be reliably calculated.
  static float intensity_distance(vdgl_digital_region_sptr const& r1,
                                  vdgl_digital_region_sptr const& r2);

  //:Computes a variant of the earth mover's distance with respect to intensity
  // Returns -1 if the distance cannot be reliably calculated.
  static float earth_mover_distance(vdgl_digital_region_sptr const& r1,
                                    vdgl_digital_region_sptr const& r2);

  //:Merges two digital regions and returns false if there is failure
  static bool merge(vdgl_digital_region_sptr const& r1,
                    vdgl_digital_region_sptr const& r2,
                    vdgl_digital_region_sptr& rm);
};

#endif
