// This is mul/clsfy/tests/test_rbf_svm_smo.cxx
// Copyright: (C) 2001 British Telecommunications PLC
#include <testlib/testlib_test.h>
//:
// \file
// \brief Tests clsfy_rbf_svm and clsfy_rbf_svm_smo_1_builder
// \author Ian Scott
// Test construction, IO etc.

#include <vcl_vector.h>
#include <vcl_iostream.h>
#include <vcl_iomanip.h>
#include <vcl_ios.h>
#include <vcl_string.h>
#include <clsfy/clsfy_rbf_svm.h>
#include <clsfy/clsfy_rbf_svm_smo_1_builder.h>
#include <clsfy/clsfy_k_nearest_neighbour.h>
#include <clsfy/clsfy_rbf_parzen.h>
#include <vpdfl/vpdfl_axis_gaussian.h>
#include <vpdfl/vpdfl_axis_gaussian_sampler.h>
#include <vnl/vnl_random.h>
#include <vsl/vsl_binary_loader.h>
#include <mbl/mbl_data_array_wrapper.h>
#include <vul/vul_timer.h>
#include <vpl/vpl.h> // vpl_unlink()

#ifndef LEAVE_FILES_BEHIND
#define LEAVE_FILES_BEHIND 0
#endif

//=======================================================================
void test_rbf_svm()
{
  vcl_cout << "*************************************\n"
           << " Testing clsfy_rbf_svm_smo_1_builder\n"
           << "*************************************\n";

  vcl_cout<<"======== TESTING BUILDING ===========\n";

  vcl_vector<vpdfl_axis_gaussian_sampler *> generator(4);//
  const unsigned nDims = 2;
  vnl_vector<double> mean0(nDims), var0(nDims), mean1(nDims), var1(nDims), mean2(nDims), var2(nDims), mean3(nDims), var3(nDims);
  vpdfl_axis_gaussian PDF0, PDF1, PDF2, PDF3;

  mean0.fill(0.0); mean0(0) = 1.5;
  mean1.fill(0.0); mean1(0) = -1.5;
  mean2.fill(0.0); mean2(1) = 1.5;
  mean3.fill(0.0); mean3(1) = -1.5;

  const double trainPrincipleVariance = 0.5;
  var0.fill(trainPrincipleVariance/10.0); var0(0) = trainPrincipleVariance;
  var1.fill(trainPrincipleVariance/10.0); var1(0) = trainPrincipleVariance;
  var2.fill(trainPrincipleVariance/10.0); var2(1) = trainPrincipleVariance;
  var3.fill(trainPrincipleVariance/10.0); var3(1) = trainPrincipleVariance;

  PDF0.set(mean0, var0);
  PDF1.set(mean1, var1);
  PDF2.set(mean2, var2);
  PDF3.set(mean3, var3);

  generator[0] = (vpdfl_axis_gaussian_sampler *)PDF0.new_sampler();
  generator[1] = (vpdfl_axis_gaussian_sampler *)PDF1.new_sampler();
  generator[2] = (vpdfl_axis_gaussian_sampler *)PDF2.new_sampler();
  generator[3] = (vpdfl_axis_gaussian_sampler *)PDF3.new_sampler();
  vnl_random rng;
  rng.reseed(333248);
  const unsigned nSamples = 200;
  vcl_vector<unsigned> labels(nSamples);
  vcl_vector<vnl_vector<double> > data(nSamples);
  vnl_vector<double> s;
  vcl_cout << "Generating test data\n";
  vcl_vector<unsigned> labelcount(4, 0u);
  for (unsigned int i=0; i<nSamples; i++)
  {
    int c = rng.lrand32(3);
    labels[i] = c/2;
    labelcount[c] ++;
    generator[c]->sample(s);
    data[i] = s;
  }

  const unsigned nTestSamples = 1000;
  vcl_vector<unsigned> testLabels(nTestSamples);
  vcl_vector<vnl_vector<double> > testingVectors(nTestSamples);
  for (unsigned int i=0; i<nTestSamples; i++)
  {
    int c = rng.lrand32(3);
    testLabels[i] = c/2;
    generator[c]->sample(s);
    testingVectors[i]=s;
  }

  delete generator[0];
  delete generator[1];
  delete generator[2];
  delete generator[3];

  mbl_data_array_wrapper<vnl_vector<double> > trainingVectors(data);


  vcl_cout << "****************The Training set****************\n";
  vcl_cout << "The number of labels from each generators are respectively ";
  vcl_cout << labelcount[0] << ' ' << labelcount[1] << ' ' << labelcount[2] << ' ' << labelcount[3] <<  vcl_endl;

  vnl_vector<double> x(nDims);
  vcl_vector<double> out(1);
  x.fill(0.0);
  vcl_cout << "x(1) varies across from -2 to + 2\n";
  vcl_cout << "x(0) varies down from -2 to + 2\n";

  clsfy_k_nearest_neighbour knn;
  knn.set(data, labels);
  knn.set_k(3);
  vcl_cout  << vcl_endl << "KNN output\n";
  vcl_cout << vcl_setprecision(4);
  for (x(0) = -2; x(0) <= 2 ; x(0) += 0.25)
  {
    for (x(1) = -2; x(1) <= 2 ; x(1) += 0.25)
    {
      knn.class_probabilities(out, x);
      vcl_cout << vcl_fixed << vcl_setw(3) << out[0] << ' ';
    }
    vcl_cout << vcl_endl;
  }

  for (x(0) = -2; x(0) <= 2 ; x(0) += 0.25)
  {
    for (x(1) = -2; x(1) <= 2 ; x(1) += 0.25)
    {
      vcl_cout << knn.classify(x);
    }
    vcl_cout << vcl_endl;
  }

  clsfy_rbf_parzen win;
  win.set(data, labels);
  win.set_rbf_width(0.08);
  win.set_power(10);
  vcl_cout << vcl_endl << "Training data distribution\n";
  vcl_cout << vcl_setprecision(1);
  for (x(0) = -2; x(0) <= 2 ; x(0) += 0.25)
  {
    for (x(1) = -2; x(1) <= 2 ; x(1) += 0.25)
    {
      double v = win.weightings(x);
      if (v < 0.01)
        vcl_cout << " 0   ";
      else
        vcl_cout << vcl_fixed << vcl_setw(4) << v << ' ';
    }
    vcl_cout << vcl_endl;
  }

  vcl_cout << "\n*********Testing Support Vector Training*********\n";
  vcl_cout << vcl_setprecision(6) << vcl_resetiosflags(vcl_ios_floatfield);

  clsfy_rbf_svm_smo_1_builder builder;
  builder.set_bound_on_multipliers(1e2);
  builder.set_rbf_width(1.05);

  clsfy_rbf_svm classifier3;

  vul_timer mytimer;

  double error = builder.build(classifier3, trainingVectors, labels);

  long realtime = mytimer.real();
  vcl_cout << "\nOptimisation took " << realtime/1000.0 << " seconds\n";
  vcl_cout << "\nLagrangians ";
  for (unsigned i=0; i < classifier3.n_support_vectors(); ++i)
    vcl_cout << classifier3.lagrangians()[i] << " ";
  vcl_cout << vcl_endl;

  vcl_cout << "Training Error " << error << vcl_endl;
#if 0
  mbl_data_wrapper<vnl_vector<double> > &data = trainingVectors.vectorData();
  data.first();
  for (unsigned int i=0; i<nSamples; i++)
  {
    vcl_cout << i << '\t' << vcl_setw(8) << pClassifier->log_l(data.current()) << '\t' << pClassifier->classify(data.current()) <<
      " should be " << labels(i) <<vcl_endl;
    data.next();
  }
#endif
  // print input, print output

  vcl_cout << vcl_setprecision(4);
  for (x(0) = -2; x(0) <= 2 ; x(0) += 0.25)
  {
    for (x(1) = -2; x(1) <= 2 ; x(1) += 0.25)
    {
      classifier3.class_probabilities(out, x);
      vcl_cout << vcl_fixed << vcl_setw(3) << out[0] << ' ';
    }
    vcl_cout << vcl_endl;
  }

  for (x(0) = -2; x(0) <= 2 ; x(0) += 0.25)
  {
    for (x(1) = -2; x(1) <= 2 ; x(1) += 0.25)
    {
      vcl_cout << classifier3.classify(x);
    }
    vcl_cout << vcl_endl;
  }

  vcl_cout << "There are " << classifier3.n_support_vectors() << " Support Vectors\n";

  TEST_NEAR("Training Error < 0.05", error, 0.0, 0.05);

  vcl_cout << "\nError on Testing set ";
  mbl_data_array_wrapper<vnl_vector<double> > test_vector_data(testingVectors);
  double testError = clsfy_test_error(classifier3, test_vector_data, testLabels);
  vcl_cout << testError << vcl_endl;

  TEST_NEAR("Test Error < 0.1", testError, 0.0, 0.1);

  vcl_cout << "\n****************Testing classifier IO**************\n";
  vsl_add_to_binary_loader(clsfy_rbf_svm());
  vcl_string test_path = "test_rbf_svm.bvl.tmp";

  vsl_b_ofstream bfs_out(test_path);
  TEST(("Opened " + test_path + " for writing").c_str(), (!bfs_out ), false);
  vsl_b_write(bfs_out, classifier3);
  vsl_b_write(bfs_out, (clsfy_classifier_base *) &classifier3);
  bfs_out.close();

  clsfy_rbf_svm svmi;
  clsfy_classifier_base *pClassifier2=0;

  vsl_b_ifstream bfs_in(test_path);
  TEST(("Opened " + test_path + " for reading").c_str(), (!bfs_in ), false);
  vsl_b_read(bfs_in, svmi);
  vsl_b_read(bfs_in, pClassifier2);
  bfs_in.close();
#if !LEAVE_FILES_BEHIND
  vpl_unlink(test_path.c_str());
#endif

  vcl_cout<<"Saved : " << classifier3 << vcl_endl;
  vcl_cout<<"Loaded: " << svmi << vcl_endl;
  vcl_cout<<"Loaded: " << pClassifier2 << vcl_endl;

  clsfy_rbf_svm &svmo = *(clsfy_rbf_svm *)&classifier3;

  TEST("Saved Classifier = Loaded Classifier",
       svmo.bias() == svmi.bias() &&
       svmo.lagrangians() == svmi.lagrangians() &&
       svmo.support_vectors() == svmi.support_vectors(),
       true);

  double ll_o = svmo.log_l(vnl_vector<double>(nDims, 0.3));
  double ll_i = svmi.log_l(vnl_vector<double>(nDims, 0.3));
  TEST_NEAR("Saved Classifier.log_l() = Loaded Classifier.log_l()",
            ll_o, ll_i, 1e-11);

  TEST("Saved Classifier = Classifier Loaded by Base Class Ptr",
       svmo.is_a(), pClassifier2->is_a());

  TEST("Loaded Classifier type = Original Classifier type",
       pClassifier2->is_class(svmo.is_a()), true);

  TEST("Loaded Classifier has base class type",
       pClassifier2->is_class("clsfy_classifier_base"), true);

  vcl_cout << vcl_setprecision(6) << vcl_resetiosflags(vcl_ios_floatfield);

  delete pClassifier2;
  vsl_delete_all_loaders();

  vcl_cout << vcl_setprecision(6) << vcl_resetiosflags(vcl_ios_floatfield);
}

TESTLIB_DEFINE_MAIN(test_rbf_svm);
