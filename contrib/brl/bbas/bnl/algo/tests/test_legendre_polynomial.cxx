//: 
//\file
//\author Kongbin Kang
//\date Dec. 22th, 2004
//\brief test bnl_legendre_polynomial by comparing result with math table

#include <vcl_iostream.h>
#include <vnl/vnl_real_polynomial.h>
#include <bnl/algo/bnl_legendre_polynomial.h>


static int test_legendre_polynomial()
{
  for(int i = 0; i< 8; i++){
    vcl_cout << " p"<< i <<" = ";
    vnl_real_polynomial p = bnl_legendre_polynomial(i);
    p.print(vcl_cout);
    vcl_cout << '\n';
  }
  return 0;
}

int main()
{
  return test_legendre_polynomial();
}
