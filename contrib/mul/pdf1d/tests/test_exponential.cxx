#include <vcl_iostream.h>
#include <vcl_fstream.h>
#include <vcl_utility.h>

#include <vnl/vnl_test.h>
#include <pdf1d/pdf1d_exponential.h>
#include <pdf1d/pdf1d_sampler.h>

void test_exponential()
{
  vcl_cout << "***********************" << vcl_endl;
  vcl_cout << " Testing pdf1d_exponential " << vcl_endl;
  vcl_cout << "***********************" << vcl_endl;

  vnl_vector<double> x(1000);
  pdf1d_exponential exp_pdf(5);

  pdf1d_sampler* sampler = exp_pdf.new_sampler();
  sampler->get_samples(x);

  vcl_cout<<"Mean of samples: "<<x.mean()<<vcl_endl;
  vcl_cout<<"PDF mean : "<<exp_pdf.mean()<<vcl_endl;

  delete sampler;
}

TESTMAIN(test_exponential);
