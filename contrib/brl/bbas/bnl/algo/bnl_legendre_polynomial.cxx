//: 
//\file
//\brief Evalue a real Legendre polynomail by recursive equation
//\author Kongbin Kang, Brown University
//\date Dec. 21th, 2004.
//

#include <vnl/vnl_real_polynomial.h>
#include "bnl_legendre_polynomial.h"

//: generate a n-th degree legendre polynomail
vnl_real_polynomial bnl_legendre_polynomial(int n)
{
  if(n == 0 )
    return vnl_real_polynomial(1.0); // return 1
  else{ 
    if(n == 1){
      // return p = x
      vnl_real_polynomial p(1);
      p[0] = 1; 
      p[1] = 0; 
      return p;
    }
    else{
      // p = x
      vnl_real_polynomial p(1);
      p[0] = 1; p[1] = 0;
      vnl_real_polynomial res = (2 - 1.0/n) * p * bnl_legendre_polynomial(n -1) - vnl_real_polynomial((1 - 1.0/n)) * bnl_legendre_polynomial(n-2); 
      return res;
    }
  }
}


