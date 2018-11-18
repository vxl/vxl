#ifndef VPGL_EARTH_CONSTANTS_H
#define VPGL_EARTH_CONSTANTS_H
//-----------------------------------------------------------------------------
//
// \file
// \brief Spheroid definitions for various standards
// \author J. L. Mundy
// \date December 31, 2005
//
//   JLM - note the Earth constants are in meters
//===========================================================================
#include <vnl/vnl_math.h>

/* MATH CONSTANTS */
#define DEGREES_TO_RADIANS (vnl_math::pi_over_180)
#define RADIANS_TO_DEGREES (vnl_math::deg_per_rad)

/* EARTH CONSTANTS */
/* GRS-80 */
#define GRS80_a 6378137.0
#define GRS80_b 6356752.3
#define GRS80_e 0.08181921805

/* NAD 1927; Same as Clarke 1866 */
#define NAD27_a 6378206.4
#define NAD27_e 8.227185424e-02

#define EARTH_GM 3.98601e+14
#define SIDEREAL_DAY 86400.0

#define METERS_TO_FEET 3.280839895
#define FEET_TO_METERS (1.0/3.280839895)

#endif //VPGL_EARTH_CONSTANTS_H
