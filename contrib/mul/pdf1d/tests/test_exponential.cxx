// This is mul/pdf1d/tests/test_exponential.cxx
#include <vcl_iostream.h>
#include <pdf1d/pdf1d_exponential.h>
#include <pdf1d/pdf1d_sampler.h>
#include <testlib/testlib_test.h>

void test_exponential()
{
  vcl_cout << "***************************\n";
  vcl_cout << " Testing pdf1d_exponential\n";
  vcl_cout << "***************************\n";

  vnl_vector<double> x(1000);
  pdf1d_exponential exp_pdf(5);

  pdf1d_sampler* sampler = exp_pdf.new_sampler();
  sampler->get_samples(x);

  vcl_cout<<"PDF mean : "<<exp_pdf.mean()<<vcl_endl;
  vcl_cout<<"Mean of samples: "<<x.mean()<<vcl_endl;
  TEST("PDF mean", exp_pdf.mean(), 0.2);
  double e = x.mean()-exp_pdf.mean(); if (e<0) e = -e;
  TEST("deviation from mean", e<0.01, true);

  delete sampler;
}

TESTLIB_DEFINE_MAIN(test_exponential);
