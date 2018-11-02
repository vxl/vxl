#include <iostream>
#include <fstream>
#include <deque>
#include <algorithm>
#include "sdet_edgel.h"
#include "sdet_sel_utils.h"

#include <cassert>
#ifdef _MSC_VER
#  include <vcl_msvc_warnings.h>
#endif

//------------------------------------------------------------------------------
// sdet_edgel methods
//------------------------------------------------------------------------------

//: constructor
sdet_edgel::sdet_edgel(
    vgl_point_2d<double> new_pt, double tan, double conf, double der, double uncer,
    sdet_appearance* lapp, sdet_appearance* rapp)
:
  id(-1),
  pt(new_pt),
  tangent(sdet_angle0To2Pi(tan)),
  strength(conf),
  deriv(der),
  uncertainty(uncer),
  left_app(lapp),
  right_app(rapp)
{
}


//: copy constructor
sdet_edgel::sdet_edgel(const sdet_edgel& other)
  :
  id(other.id),
  pt(other.pt),
  tangent(other.tangent),
  strength(other.strength),
  deriv(other.deriv),
  gpt(other.gpt)
{
  left_app = other.left_app->clone();
  right_app = other.right_app->clone();
}

sdet_edgel &
sdet_edgel::
operator=(const sdet_edgel &rhs)
{
  id = rhs.id;

  pt = rhs.pt;
  tangent = rhs.tangent;
  strength = rhs.strength;
  deriv = rhs.deriv;

  gpt = rhs.gpt;

  //: Copy pointers, taking care for when rhs is *this
  sdet_appearance* left_app_orig = left_app;
  sdet_appearance* right_app_orig = right_app;

  left_app = rhs.left_app->clone();
  right_app = rhs.right_app->clone();

  delete left_app_orig;
  delete right_app_orig;

  return *this;
}

//: destructor
sdet_edgel::~sdet_edgel()
{
  delete left_app;
  delete right_app;
}
