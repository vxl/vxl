// This is mul/clsfy/tests/test_binary_pdf_classifier.cxx
// Copyright: (C) 2000 British Telecommunications PLC
#include <iostream>
#include <iomanip>
#include <ios>
#include <string>
#include <testlib/testlib_test.h>
//:
// \file
// \brief Tests the clsfy_binary_pdf_classifier class
// \author Ian Scott
// Test construction, IO etc.

#ifdef _MSC_VER
#  include <vcl_msvc_warnings.h>
#endif
#include <vpl/vpl.h> // vpl_unlink()
#include <vsl/vsl_binary_loader.h>

#include <vpdfl/vpdfl_axis_gaussian.h>
#include <clsfy/clsfy_binary_pdf_classifier.h>

#ifndef LEAVE_FILES_BEHIND
#define LEAVE_FILES_BEHIND 0
#endif

//: Tests the clsfy_binary_pdf_classifier class
void test_binary_pdf_classifier()
{
  vsl_add_to_binary_loader(vpdfl_axis_gaussian());

  std::cout << "*************************************\n"
           << " Testing clsfy_binary_pdf_classifier\n"
           << "*************************************\n";

  std::cout<<"\n======== TESTING CONSTRUCTION ===========\n";

  vpdfl_axis_gaussian PDF0;
  unsigned nDims=2;
  vnl_vector<double> mean0(nDims), var0(nDims);
  mean0.fill(0.0);
  var0.fill(1);
  PDF0.set(mean0, var0);

  std::cout << "PDF model: " << PDF0;

  clsfy_binary_pdf_classifier classifier(PDF0, -2.0);

  // print input, print output

  vnl_vector<double> x(nDims);
  std::vector<double> out(1);
  x.fill(0.0);
  std::cout << "x(2) varies across from -2 to + 2\n"
           << "x(1) varies down from -2 to + 2\n";

  std::cout << std::setprecision(4);
  for (x(0) = -2; x(0) <= 2 ; x(0) += 0.25)
  {
    for (x(1) = -2; x(1) <= 2 ; x(1) += 0.25)
    {
      classifier.class_probabilities(out, x);
      std::cout << std::fixed << std::setw(4) << out[0] << ' ';
    }
    std::cout << std::endl;
  }

  for (x(0) = -2; x(0) <= 2 ; x(0) += 0.25)
  {
    for (x(1) = -2; x(1) <= 2 ; x(1) += 0.25)
    {
      std::cout << classifier.classify(x);
    }
    std::cout << std::endl;
  }

  std::cout<<"======== TESTING I/O ===========\n";

  std::string test_path = "test_binary_pdf_classifier.bvl.tmp";

  vsl_b_ofstream bfs_out(test_path);
  TEST(("Opened " + test_path + " for writing").c_str(), (!bfs_out ), false);
  classifier.b_write(bfs_out);
  bfs_out.close();

  clsfy_binary_pdf_classifier classifier2;

  vsl_b_ifstream bfs_in(test_path);
  TEST(("Opened " + test_path + " for reading").c_str(), (!bfs_in ), false);
  classifier2.b_read(bfs_in);

  bfs_in.close();
#if !LEAVE_FILES_BEHIND
  vpl_unlink(test_path.c_str());
#endif

  std::cout<<"Saved : " << classifier << std::endl
          <<"Loaded: " << classifier2 << std::endl;

  TEST("Original KNN == Loaded KNN",
       classifier.n_classes() == classifier2.n_classes() &&
       classifier.n_dims() == classifier2.n_dims() &&
       classifier.log_prob_limit() == classifier2.log_prob_limit(),
       true);

  const vnl_vector<double> probe1(2,0.5);
  const vnl_vector<double> probe2(2,2.0);
  std::cout << "classifier.log_l(2.0, 2.0) = "
           << classifier.log_l(probe2) << std::endl;
  TEST("Original classifier(2.0, 2.0) == Loaded classifier(2.0, 2.0)",
       classifier.log_l(probe2) == classifier2.log_l(probe2) &&
       classifier.log_l(probe1) == classifier2.log_l(probe1),
       true);
  std::cout << std::setprecision(6) << std::resetiosflags(std::ios::floatfield);
  vsl_delete_all_loaders();

}

TESTMAIN(test_binary_pdf_classifier);
