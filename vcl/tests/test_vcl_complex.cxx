
#include <iostream.h>
#include <vcl/vcl_complex.h>

int main()
{

  vcl_double_complex dc1(1.1,1.2), dc2(2.1,2.2);
  vcl_float_complex fc1(1.1,1.2), fc2(2.1,2.2);

  cout << dc1 << " + " << dc2 << " = " << dc1+dc2 << endl;
  cout << fc1 << " + " << fc2 << " = " << fc1+fc2 << endl;

  vcl_double_complex dc3(dc1.real(),dc2.imag());
  vcl_float_complex fc3(real(fc1),imag(fc2));
  
  cout << dc3 << " / " << dc1 << " = " << dc3/dc1 << endl;
  cout << fc3 << " / " << fc1 << " = " << fc3/fc1 << endl;
    
  cout << "polar representation of " << dc3 << " is [" << abs(dc3) << "," << arg(dc3) << ']' << endl;
  cout << "going back: " << dc3 << " must be = " << polar(abs(dc3),arg(dc3)) <<  endl;
  vcl_float_complex fcsr3 = sqrt(fc3);
  cout << "sqrt(" << fc3 << ") is " << fcsr3 << ", so " << fcsr3 << " * " << fcsr3 << " = " << fcsr3*fcsr3 << endl;

  return 0;
}
