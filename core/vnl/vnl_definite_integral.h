#ifndef VNL_DEFINITE_INTEGRAL_H_
#define VNL_DEFINITE_INTEGRAL_H_
//:
// \file
// \author Kongbin Kang at Brown
// \date Jan 12, 2005
// \brief the abstract 1D integrant function used for integration

#include "vnl_integrant_fnct.h"

class vnl_definite_integral
{
  protected:

    static vnl_integrant_fnct *pfnct_;
    
  public:

    vnl_definite_integral() { pfnct_ = 0; }

    void set_fnct(vnl_integrant_fnct* f) { pfnct_ = f; }
    
    //: integration from a to b
    virtual double integral(double a, double b) { return 0; }

    //: dector
    virtual ~vnl_definite_integral() { pfnct_ = 0; }
};

#endif
