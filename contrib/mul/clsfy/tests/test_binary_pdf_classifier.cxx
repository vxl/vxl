// Copyright: (C) 2000 British Telecommunications PLC

//:
// \file
// \brief Tests the clsfy_binary_pdf_classifier class
// \author Ian Scott
// Test construction, IO etc.

#include <vcl_iostream.h>
#include <vcl_iomanip.h>
#include <vcl_ios.h>

#include <vcl_string.h>
#include <vpdfl/vpdfl_axis_gaussian.h>
#include <clsfy/clsfy_binary_pdf_classifier.h>

#include <testlib/testlib_test.h>

//: Tests the clsfy_binary_pdf_classifier class
void test_binary_pdf_classifier()
{
  vsl_add_to_binary_loader(vpdfl_axis_gaussian());

  vcl_cout << "\n\n\n"
           << "*************************************\n"
           << " Testing clsfy_binary_pdf_classifier\n"
           << "*************************************\n";

  vcl_cout<<"\n======== TESTING CONSTRUCTION ===========\n";

  vpdfl_axis_gaussian PDF0;
  unsigned nDims=2;
  vnl_vector<double> mean0(nDims), var0(nDims);
  mean0.fill(0.0);
  var0.fill(1);
  PDF0.set(mean0, var0);

  vcl_cout << "PDF model: " << PDF0;

  clsfy_binary_pdf_classifier classifier(PDF0, -2.0);

  // print input, print output

  vnl_vector<double> x(nDims);
  vcl_vector<double> out(1);
  x.fill(0.0);
  vcl_cout << "x(2) varies across from -2 to + 2\n"
           << "x(1) varies down from -2 to + 2\n";

  vcl_cout << vcl_setprecision(4);
  for (x(0) = -2; x(0) <= 2 ; x(0) += 0.25)
  {
    for (x(1) = -2; x(1) <= 2 ; x(1) += 0.25)
    {
      classifier.class_probabilities(out, x);
      vcl_cout << vcl_fixed << vcl_setw(4) << out[0] << ' ';
    }
    vcl_cout << vcl_endl;
  }

  for (x(0) = -2; x(0) <= 2 ; x(0) += 0.25)
  {
    for (x(1) = -2; x(1) <= 2 ; x(1) += 0.25)
    {
      vcl_cout << classifier.classify(x);
    }
    vcl_cout << vcl_endl;
  }

  vcl_cout<<"======== TESTING I/O ===========\n";

  vcl_string test_path = "test_binary_pdf_classifier.bvl.tmp";

  vsl_b_ofstream bfs_out(test_path);
  TEST(("Opened " + test_path + " for writing").c_str(), (!bfs_out ), false);
  classifier.b_write(bfs_out);
  bfs_out.close();

  clsfy_binary_pdf_classifier classifier2;

  vsl_b_ifstream bfs_in(test_path);
  TEST(("Opened " + test_path + " for writing").c_str(), (!bfs_out ), false);
  classifier2.b_read(bfs_in);

  bfs_in.close();

  vcl_cout<<"Saved : " << classifier << vcl_endl
          <<"Loaded: " << classifier2 << vcl_endl;

  TEST("Original KNN == Loaded KNN",
       classifier.n_classes() == classifier2.n_classes() &&
       classifier.n_dims() == classifier2.n_dims() &&
       classifier.log_prob_limit() == classifier2.log_prob_limit(),
       true);

  const vnl_vector<double> probe1(2,0.5);
  const vnl_vector<double> probe2(2,2.0);
  vcl_cout << "classifier.log_l(2.0, 2.0) = "
           << classifier.log_l(probe2) << vcl_endl;
  TEST("Original classifier(2.0, 2.0) == Loaded classifier(2.0, 2.0)",
       classifier.log_l(probe2) == classifier2.log_l(probe2) &&
       classifier.log_l(probe1) == classifier2.log_l(probe1),
       true);
  vcl_cout << vcl_setprecision(6) << vcl_resetiosflags(vcl_ios_floatfield);
}

TESTLIB_DEFINE_MAIN(test_binary_pdf_classifier);
