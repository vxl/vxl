#ifndef BNL_INTEGRANT_FNCT_H_
#define BNL_INTEGRANT_FNCT_H_

//:
// \file
// \author Kongbin Kang
// \date Jan 12, 2005
// \brief the abstract class of 1D integrant function used in integral 
// 

class bnl_integrant_fnct{
  public:
    bnl_integrant_fnct() {}
    ~bnl_integrant_fnct() {}

    virtual double f_(double x) { return 0;}
};


#endif
  
