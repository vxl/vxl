// This is core/vcsl/vcsl_spheroid.cxx
#include "vcsl_spheroid.h"
#include <vcl_cmath.h> // for sqrt()
#include <vcl_cassert.h>

//---------------------------------------------------------------------------
// Set from a standard spheroid
//---------------------------------------------------------------------------
void vcsl_spheroid::set_from_std(const vcsl_std_spheroid new_std_spheroid)
{
  switch (new_std_spheroid)
  {
   case airy_1830:
    a_=6377563.396;
    b_=6356256.910;
    break;
   case australian_national:
   case south_american_1969:
    a_=6378160;
    b_=6356774.7192;
    break;
   case bessel_1841:
    a_=6377397.155;
    b_=6356078.9629;
    break;
   case clarke_1866:
    a_=6378206.4;
    b_=6356583.8;
    break;
   case clarke_1880:
    a_=6378249.145;
    b_=6356514.8696;
    break;
   case everest_1830:
    a_=6377276.34518;
    b_=6356075.41511;
    break;
   case grs_1980:
    a_=6378137;
    b_=6356752.3141;
    break;
   case international:
    a_=6378388;
    b_=6356911.9462;
    break;
   case modified_airy:
    a_=6377340.189;
    b_=6356034.446;
    break;
   case modified_everest:
    a_=6377304.063;
    b_=6356103.039;
    break;
   case wgs_1972:
    a_=6378135;
    b_=6356750.5;
    break;
   case wgs_1984:
    a_=6378137;
    b_=6356752.3142;
    break;
   default:
    assert(!"impossible");
  }

  f_=(a_-b_)/a_;
  e_=vcl_sqrt(2*f_-f_*f_);
}

//---------------------------------------------------------------------------
// Is `this' equal to `other' ?
//---------------------------------------------------------------------------
bool vcsl_spheroid::operator==(const vcsl_spheroid &other) const
{
  if (this==&other)
    return true;
  else
    return a_==other.a_ && b_==other.b_ && e_==other.e_ && f_==other.f_;
}

//---------------------------------------------------------------------------
// Assignment
//---------------------------------------------------------------------------
vcsl_spheroid &vcsl_spheroid::operator=(const vcsl_spheroid &other)
{
  if (this!=&other)
  {
    a_=other.a_;
    b_=other.b_;
    e_=other.e_;
    f_=other.f_;
  }
  return *this;
}
