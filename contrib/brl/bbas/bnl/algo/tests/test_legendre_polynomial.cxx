//: 
//\file
//\author Kongbin Kang
//\date Dec. 22th, 2004
//\brief test bnl_legendre_polynomial by comparing result with math table

#include <vcl_iostream.h>
#include <vnl/vnl_real_polynomial.h>
#include <bnl/algo/bnl_legendre_polynomial.h>


int test_legendre_polynomial(int argc, char *argv[])
{
  for(int i = 0; i< 8; i++){
    vcl_cout << " p"<< i <<" = ";
    vnl_real_polynomial p = bnl_legendre_polynomial(i);
    for(int j=0; j<i+1; j++)
      vcl_cout<<p[j]<<'\t';
    vcl_cout << '\n';
  }
  return 0;
}
