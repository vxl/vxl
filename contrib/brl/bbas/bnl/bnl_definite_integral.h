#ifndef BNL_DEFINITE_INTEGRAL_H_
#define BNL_DEFINITE_INTEGRAL_H_
//:
// \file
// \author Kongbin Kang
// \date Jan 12, 2005
// \brief the abstract 1D integrant function used for integration

#include "bnl_integrant_fnct.h"

class bnl_definite_integral
{
  protected:

    static bnl_integrant_fnct *pfnct_;
    
  public:

    bnl_definite_integral() { pfnct_ = 0; }

    void set_fnct(bnl_integrant_fnct* f) { pfnct_ = f; }
    
    //: integration from a to b
    virtual double integral(double a, double b) { return 0; }

    //: dector
    virtual ~bnl_definite_integral() { pfnct_ = 0; }
};

#endif
