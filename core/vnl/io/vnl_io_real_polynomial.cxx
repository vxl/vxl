// This is vxl/vnl/io/vnl_io_real_npolynomial.cxx

#include <vcl_cstdlib.h> // vcl_abort()
#include <vsl/vsl_binary_io.h>
#include <vnl/io/vnl_io_real_polynomial.h>
#include <vnl/io/vnl_io_vector.h>
#include <vnl/io/vnl_io_matrix.h>


//==============================================================================
//: Binary save self to stream.
void vsl_b_write(vsl_b_ostream & os, const vnl_real_polynomial & p)
{
  const short io_version_no = 1;
  vsl_b_write(os, io_version_no);

  vsl_b_write(os, p.coefficients());
}

//==============================================================================
//: Binary load self from stream.
void vsl_b_read(vsl_b_istream &is, vnl_real_polynomial & p)
{
  short ver;
  vnl_vector<double> coeffs;
  vnl_matrix<int> polyn;
  vsl_b_read(is, ver);
  switch(ver)
  {
  case 1:
    vsl_b_read(is, coeffs);
    p.set_coefficients(coeffs);
    break;

  default:
    vcl_cerr << "ERROR: vsl_b_read(s, vnl_real_polynomial&): Unknown version number "
            << ver << vcl_endl;
    vcl_abort();
  }
}

//==============================================================================
//: Output a human readable summary to the stream
void vsl_print_summary(vcl_ostream & os,const vnl_real_polynomial & p)
{
  os<<"Coefficients: ";
  vsl_print_summary(os, p.coefficients());
}


