#ifndef BNL_ANALYTIC_INTEGRANT
#define BNL_ANALYTIC_INTEGRANT

// :
// \author Kongbin Kang
// \date Jan 13, 2005
// \brief a class to represent an analytic integrant

#include "bnl_integrant_fnct.h"

class bnl_analytic_integrant : public bnl_integrant_fnct 
{
  public:
    bnl_analytic_integrant() {}
    ~bnl_analytic_integrant() {}

    // the function every derived class has to implement
    virtual double f_(double x) { return 0;}
};

#endif

