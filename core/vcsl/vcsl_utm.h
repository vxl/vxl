#ifndef vcsl_utm_h_
#define vcsl_utm_h_
//:
// \file
// \brief Universal Transverse Mercator projection
// \author Francois BERTEL
//
// \verbatim
//  Modifications
//   2000/06/29 Francois BERTEL Creation. Adapted from IUE
//   2001/04/10 Ian Scott (Manchester) Converted perceps header to doxygen
// \endverbatim

#include <vcsl/vcsl_geographic.h>
#include <vcsl/vcsl_utm_sptr.h>

//: Universal Transverse Mercator projection
// Coordinate system in which a point is specified by a grid zone designation,
// an x and y coordinate within the grid zone, and z, the height above the
// surface of the reference ellipsoid
class vcsl_utm
  : public vcsl_geographic
{
 private:
  //***************************************************************************
  // Constructors/Destructor
  //***************************************************************************

  // Default constructor.
  vcsl_utm();

 public:
  // Destructor
  ~vcsl_utm() override = default;
};

#endif // vcsl_utm_h_
