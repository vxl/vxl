#ifndef vcsl_spheroid_h_
#define vcsl_spheroid_h_
//:
// \file
// \brief Reference sphere or ellipse for a geographic coordinate system
// \author François BERTEL
//
// \verbatim
//  Modifications
//   2000/06/28 François BERTEL Creation. Adapted from IUE
//   2001/04/10 Ian Scott (Manchester) Converted perceps header to doxygen
//   2004/09/17 Peter Vanroose  made a(), b(), e() and f() non-virtual - they just return a member and should not be overloaded
// \endverbatim

#include <vbl/vbl_ref_count.h>
#include <vcsl/vcsl_spheroid_sptr.h>

//: Reference sphere or ellipse for a geographic coordinate system
// The default value for a reference ellipsoid is the Clarke 1866 model, but
// this class contains a constructor that allows reference spheroids to be
// constructed with values for several different standard models. See the book
// "Map Projections Used by the U.S. Geological Survey" (Snyder, John P.,
// "Map Projections Used by the U.S. Geological Survey," Geological Survey
// Bulletin 1532, U.S. Government Printing Office, Washington, 1982.) for
// further detail on most of these coordinate systems.
class vcsl_spheroid
  : public vbl_ref_count
{
 public:
  enum vcsl_std_spheroid
  {
    airy_1830,
    australian_national,
    bessel_1841,
    clarke_1866,
    clarke_1880,
    everest_1830,
    grs_1980,
    international,
    modified_airy,
    modified_everest,
    south_american_1969,
    wgs_1972,
    wgs_1984
  };

  //***************************************************************************
  // Constructors/Destructor
  //***************************************************************************

  //: Default constructor. Clark_1866 spheroid
  vcsl_spheroid() { set_from_std(clarke_1866); }

  //: Constructor from a standard spheroid
  explicit vcsl_spheroid(const vcsl_std_spheroid s) { set_from_std(s); }

  // Copy constructor
  vcsl_spheroid(const vcsl_spheroid &other)
    : vbl_ref_count(), a_(other.a_), b_(other.b_), e_(other.e_), f_(other.f_) {}

  // Destructor
  ~vcsl_spheroid() {}

  //***************************************************************************
  // Status report
  //***************************************************************************

  //: Return the major axis of spheroid
  double a() const { return a_; }

  //: Return the minor axis of spheroid
  double b() const { return b_; }

  //: Return the eccentricity of spheroid
  double e() const { return e_; }

  //: Return the flattening of spheroid
  double f() const { return f_; }

  //***************************************************************************
  // Status setting
  //***************************************************************************

  //: Set from a standard spheroid
  void set_from_std(const vcsl_std_spheroid new_std_spheroid);

  //: Set the major axis of spheroid
  void set_a(double new_a) { a_=new_a; }

  //: Set the minor axis of spheroid
  void set_b(double new_b) { b_=new_b; }

  //: Set the eccentricity of spheroid
  void set_e(double new_e) { e_=new_e; }

  //: Set the flattening of spheroid
  void set_f(double new_f) { f_=new_f; }

  //***************************************************************************
  // Comparison
  //***************************************************************************

  //: Is `this' equal to `other' ?
  bool operator==(const vcsl_spheroid &other) const;

  //***************************************************************************
  // Duplication
  //***************************************************************************

  // Assignment
  vcsl_spheroid &operator=(const vcsl_spheroid &other);

 protected:
  //***************************************************************************
  // Implementation
  //***************************************************************************

  //: Major axis of spheroid
  double a_;
  //: Minor axis of spheroid
  double b_;
  //: Eccentricity of spheroid
  double e_;
  //: Flattening of spheroid
  double f_;
};

#endif // vcsl_spheroid_h_
