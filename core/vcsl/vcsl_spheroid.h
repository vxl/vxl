#ifndef VCSL_SPHEROID_H
#define VCSL_SPHEROID_H
//*****************************************************************************
//
// .NAME vcsl_spheroid - Reference sphere or ellipse for a geographic
//                       coordinate system
// .LIBRARY vcsl
// .HEADER  vcsl/vcsl__spheroid.h
// .INCLUDE vcsl/vcsl_spheroid_ref.h
// .INCLUDE vbl/vbl_ref_count.h
// .FILE    vcsl/vcsl_spheroid.cxx
//
// The default value for a reference ellipsoid is the Clarke 1866 model, but
// this class contains a constructor that allows reference spheroids to be
// constructed with values for several different standard models. See the book
// "Map Projections Used by the U.S. Geological Survey" (Snyder, John P.,
// "Map Projections Used by the U.S. Geological Survey," Geological Survey
// Bulletin 1532, U.S. Government Printing Office, Washington, 1982.) for
// further detail on most of these coordinate systems.
//
// .SECTION Author
// François BERTEL
//
// .SECTION Modifications
// 2000/06/28 François BERTEL Creation. Adapted from IUE
//*****************************************************************************

#include <vcsl/vcsl_spheroid_ref.h>

#include <vbl/vbl_ref_count.h>

class vcsl_spheroid
  :public vbl_ref_count
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
  explicit vcsl_spheroid(void);
  
  //: Constructor from a standard spheroid
  explicit vcsl_spheroid(const vcsl_std_spheroid new_std_spheroid);

  //: Copy constructor
  vcsl_spheroid(const vcsl_spheroid &other);

  //: Destructor
  virtual ~vcsl_spheroid();

  //***************************************************************************
  // Status report
  //***************************************************************************

  //: Return the major axis of spheroid
  virtual double a(void) const;

  //: Return the minor axis of spheroid
  virtual double b(void) const;

  //: Return the eccentricity of spheroid
  virtual double e(void) const;

  //: Return the flattening of spheroid
  virtual double f(void) const;

  //***************************************************************************
  // Status setting
  //***************************************************************************

  //: Set from a standard spheroid
  virtual void set_from_std(const vcsl_std_spheroid new_std_spheroid);

  //: Set the major axis of spheroid
  virtual void set_a(const double new_a);

  //: Set the minor axis of spheroid
  virtual void set_b(const double new_b);

  //: Set the eccentricity of spheroid
  virtual void set_e(const double new_e);

  //: Set the flattening of spheroid
  virtual void set_f(const double new_f);


  //***************************************************************************
  // Comparison
  //***************************************************************************

  //: Is `this' equal to `other' ?
  virtual bool operator==(const vcsl_spheroid &other) const;

  //***************************************************************************
  // Duplication
  //***************************************************************************
  
  //: Assignement
  virtual vcsl_spheroid &operator=(const vcsl_spheroid &other);

protected:
  //***************************************************************************
  // Implementation
  //***************************************************************************

  //: Major axis of spheroid
  double _a;
  //: Minor axis of spheroid
  double _b;
  //: Eccentricity of spheroid
  double _e;
  //: Flattening of spheroid
  double _f;
};

#endif // #ifndef VCSL_SPHEROID_H
