//-*- c++ -*-------------------------------------------------------------------
// Module: Find all roots of a polynomial
// Author: Peter Vanroose, KULeuven, ESAT/PSI.
// Created: February 2000
//-----------------------------------------------------------------------------

#include <vcl/vcl_iostream.h>
#include <vcl/vcl_cstdlib.h> // for atof()
#include <vnl/vnl_complex.h>
#include <vnl/vnl_vector.h>
#include <vnl/algo/vnl_rpoly_roots.h>

main(int argc, char* argv[])
{
  --argc; ++argv;

  // Read coefficients from stdin, or from command line
  vnl_vector<double> pts(argc);
  if (argc == 0) {
    cout << "Give the polynomial coefficients, and end with EOF (CTRL-Z)\n";
    cin >> pts;
  }
  else for (int i=0; i<argc; ++i) pts[i] = atof(argv[i]);
  
  cout << "Coefficients = [ " << pts << " ]\n";
  cout << "Polynomial = ";
  for (int i=0; i<pts.size()-2; ++i) if (pts[i] != 0)
    cout << pts[i] << " X^" << pts.size()-i-1 << " + ";
  cout << pts[pts.size()-2] << " X + " << pts[pts.size()-1] << endl;
  
  vnl_rpoly_roots r(pts);

  cout << "Roots = [ " << r.roots() << " ]\n";

  return 0;
}
