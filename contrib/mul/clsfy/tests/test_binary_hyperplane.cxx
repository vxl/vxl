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
#include <clsfy/clsfy_rbf_parzen.h>
#include <clsfy/clsfy_k_nearest_neighbour.h>
#include <clsfy/clsfy_binary_hyperplane.h>
#include <clsfy/clsfy_binary_hyperplane_ls_builder.h>
#include <vsl/vsl_binary_loader.h>
#include <mbl/mbl_mz_random.h>
#include <vpdfl/vpdfl_axis_gaussian.h>
#include <vpdfl/vpdfl_axis_gaussian_sampler.h>
#include <vsl/vsl_vector_io.h>
#include <mbl/mbl_data_array_wrapper.h>
#include <vcl_cmath.h>
#include <vnl/vnl_test.h>

//: Tests the clsfy_binary_hyperplane and clsfy_binary_hyperplane_builder classes
void test_binary_hyperplane()
{
  vcl_cout << "\n\n\n"
           << "*****************************\n"
           << " Testing clsfy_binary_linear\n"
           << "*****************************\n";

  vcl_vector<vpdfl_axis_gaussian_sampler *> generator(2);//
  const unsigned nDims = 2;
  vnl_vector<double> mean0(nDims), var0(nDims), mean1(nDims), var1(nDims);
  vpdfl_axis_gaussian PDF0, PDF1;

  mean0.fill(-1.0); mean0(1) = -0.5;
  mean1.fill(1.0);
  var0.fill(0.6); var0(0) = 0.8;
  var1.fill(0.1); var1(0) = 0.8;
  PDF0.set(mean0, var0);
  PDF1.set(mean1, var1);

  generator[0] = (vpdfl_axis_gaussian_sampler *)PDF0.new_sampler();
  generator[1] = (vpdfl_axis_gaussian_sampler *)PDF1.new_sampler();
  mbl_mz_random rng;
  rng.reseed(111333);

  const unsigned nSamples = 50;
  const unsigned nTestSamples = 501;
  vcl_vector<vnl_vector<double> > trainingVectors(nSamples);
  vcl_vector<vnl_vector<double> > testVectors(nTestSamples);
  vcl_vector<unsigned> labels(nSamples);
  vcl_vector<unsigned> testLabels(nTestSamples);
  vnl_vector<double> s;

  vcl_cout << "Generating test data" << vcl_endl;
  for (unsigned int i=0; i<nSamples; i++)
  {
    int c = rng.lrand32(0,1);
    labels[i] = c;
    generator[c]->sample(s);
    trainingVectors[i] = s;
  }
  for (unsigned i=0; i<nTestSamples; i++)
  {
    int c = rng.lrand32(0, 1);
    testLabels[i] = c;
    generator[c]->sample(s);
    testVectors[i] = s;
  }
  delete generator[0];
  delete generator[1];

  vcl_cout << "****************The Training set****************" <<vcl_endl;

  vnl_vector<double> x(nDims);
  vcl_vector<double> out(1);
  x.fill(0.0);
  vcl_cout << "x(2) varies across from -2 to + 2" << vcl_endl
           << "x(1) varies down from    2 to  -2" << vcl_endl;

#if 0

  clsfy_k_nearest_neighbour knn;
  knn.set(trainingVectors, labels);
  knn.set_k(3);
  vcl_cout << vcl_endl << "KNN output"<<vcl_endl
           << vcl_setprecision(4);
  for (x(0) = 2; x(0) >= -2 ; x(0) -= 0.25)
  {
    for (x(1) = -2; x(1) <= 2 ; x(1) += 0.25)
    {
      knn.class_probabilities(out, x);
      vcl_cout << vcl_fixed << vcl_setw(3) << out[0] << ' ';
    }
    vcl_cout << vcl_endl;
  }

  for (x(0) = 2; x(0) >= -2; x(0) -= 0.25)
  {
    for (x(1) = -2; x(1) <= 2 ; x(1) += 0.25)
    {
      vcl_cout << knn.classify(x);
    }
    vcl_cout << vcl_endl;
  }

  clsfy_rbf_parzen win;
  win.set(trainingVectors, labels);
  win.set_rbf_width(0.2);
  win.set_power(10);
  vcl_cout << "x(2) varies across from -2 to +2" << vcl_endl
           << "x(1) varies down from    2 to -2" << vcl_endl
           << vcl_endl << "Training data distribution"<<vcl_endl
           << vcl_setprecision(1);
  for (x(0) = 2; x(0) >= -2 ; x(0) -= 0.25)
  {
    for (x(1) = -2; x(1) <= 2 ; x(1) += 0.25)
    {
      double v = win.weightings(x);
      if (v < 0.05)
        vcl_cout << "  .  ";
      else
        vcl_cout << vcl_fixed << vcl_setw(4) << v << ' ';
    }
    vcl_cout << vcl_endl;
  }
#endif
  vcl_cout<<"======== TESTING BUILDING ==========="<<vcl_endl;


  vcl_cout << vcl_setprecision(6);

  clsfy_binary_hyperplane_ls_builder builder;

  clsfy_binary_hyperplane *pClassifier =
    (clsfy_binary_hyperplane*) builder.new_classifier();
  vcl_cout << "Finding Least Squares Separator using least squares" << vcl_endl;
  mbl_data_array_wrapper<vnl_vector<double> > training_set(trainingVectors);
  vcl_cout << "Error on Training set ";
  double train_error = builder.build(*pClassifier, training_set, labels);
  vcl_cout << train_error << vcl_endl;
  TEST("Train error on classifier is good enough", train_error < 0.05, true);

  vcl_cout << "****************Testing over test set**************" <<vcl_endl;

  mbl_data_array_wrapper<vnl_vector<double> > test_set_inputs(testVectors);

  double test_error = clsfy_test_error(*pClassifier, test_set_inputs, testLabels);
  vcl_cout << "Error on Testing set " << test_error << vcl_endl;

  TEST("Test error on classifier is good enough", test_error < 0.1, true);

  // print input, print output

  x.fill(0.0);
  vcl_cout << "x(2) varies across from -2 to +2" << vcl_endl
           << "x(1) varies down from    2 to -2" << vcl_endl;

  vcl_cout << vcl_setprecision(4);
  for (x(0) = 2; x(0) >= -2 ; x(0) -= 0.25)
  {
    for (x(1) = -2; x(1) <= 2 ; x(1) += 0.25)
    {
      pClassifier->class_probabilities(out, x);
      vcl_cout << out[0] << ' ';
    }
    vcl_cout << vcl_endl;
  }

  for (x(0) = 2; x(0) >= -2 ; x(0) -= 0.25)
  {
    for (x(1) = -2; x(1) <= 2 ; x(1) += 0.25)
    {
      vcl_cout << pClassifier->classify(x) << ' ';
    }
    vcl_cout << vcl_endl;
  }

  vcl_cout<<"======== TESTING I/O ==========="<<vcl_endl;

  vcl_string test_path = "test_binary_hyperplane.bvl.tmp";

  vsl_b_ofstream bfs_out(test_path);
  TEST(("Opened " + test_path + " for writing").c_str(), (!bfs_out ), false);
  vsl_b_write(bfs_out, *pClassifier);
  bfs_out.close();


  clsfy_binary_hyperplane classifier_in;

  vsl_b_ifstream bfs_in(test_path);
  TEST(("Opened " + test_path + " for writing").c_str(), (!bfs_out ), false);
  vsl_b_read(bfs_in, classifier_in);

  bfs_in.close();

  vcl_cout<<"Saved : " << *pClassifier << vcl_endl
          <<"Loaded: " << classifier_in << vcl_endl;

  TEST("saved classifier = loaded classifier",
       pClassifier->weights() == classifier_in.weights() &&
       pClassifier->bias() == classifier_in.bias(),
       true);

  TEST("saved classifier(x) = loaded classifier(x)",
       vcl_fabs(pClassifier->log_l(vnl_vector<double>(nDims, 0.25)) -
                classifier_in.log_l(vnl_vector<double>(nDims, 0.25))) < 1.0e-10,
       true);
  vcl_cout << vcl_setprecision(6) << vcl_resetiosflags(vcl_ios_floatfield);

  delete pClassifier;
}

TESTMAIN(test_binary_hyperplane);
