//*****************************************************************************
//:
// \file
// \brief Smart pointer on a vsol_digital_curve_2d
//*****************************************************************************
#ifndef vsol_digital_curve_2d_sptr_h
#define vsol_digital_curve_2d_sptr_h

class vsol_digital_curve_2d;
#include <vbl/vbl_smart_ptr.h>
#include <iostream>
#include <map>
#include <vector>
#ifdef _MSC_VER
#  include <vcl_msvc_warnings.h>
#endif

typedef vbl_smart_ptr<vsol_digital_curve_2d> vsol_digital_curve_2d_sptr;
typedef std::vector<vsol_digital_curve_2d_sptr> curves;
#endif // vsol_digital_curve_2d_sptr_h

