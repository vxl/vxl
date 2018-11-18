// This is mul/clsfy/tests/test_binary_hyperplane.cxx
// Copyright: (C) 2000 British Telecommunications PLC
#include <iostream>
#include <iomanip>
#include <ios>
#include <string>
#include <algorithm>
#include <cmath>
#include <testlib/testlib_test.h>
//:
// \file
// \brief Tests the clsfy_binary_pdf_classifier class
// \author Ian Scott
// Test construction, IO etc.

#ifdef _MSC_VER
#  include <vcl_msvc_warnings.h>
#endif
#include <clsfy/clsfy_rbf_parzen.h>
#include <clsfy/clsfy_k_nearest_neighbour.h>
#include <clsfy/clsfy_binary_hyperplane.h>
#include <clsfy/clsfy_binary_hyperplane_ls_builder.h>
#include <clsfy/clsfy_binary_hyperplane_gmrho_builder.h>
#include <vpdfl/vpdfl_axis_gaussian.h>
#include <vpdfl/vpdfl_axis_gaussian_sampler.h>
#include <vnl/vnl_random.h>
#include <vnl/vnl_math.h>
#include <vsl/vsl_binary_loader.h>
#include <vsl/vsl_vector_io.h>
#include <mbl/mbl_data_array_wrapper.h>
#include <vpl/vpl.h> // vpl_unlink()

#ifndef LEAVE_FILES_BEHIND
#define LEAVE_FILES_BEHIND 0
#endif

void   test_clsfy_geman_mcclure_build();
//: Tests the clsfy_binary_hyperplane and clsfy_binary_hyperplane_builder classes
void test_binary_hyperplane()
{
  std::cout << "*****************************\n"
           << " Testing clsfy_binary_linear\n"
           << "*****************************\n";

  std::vector<vpdfl_axis_gaussian_sampler *> generator(2);//
  constexpr unsigned nDims = 2;
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
  vnl_random rng;
  rng.reseed(111333);

  constexpr unsigned nSamples = 50;
  constexpr unsigned nTestSamples = 501;
  std::vector<vnl_vector<double> > trainingVectors(nSamples);
  std::vector<vnl_vector<double> > testVectors(nTestSamples);
  std::vector<unsigned> labels(nSamples);
  std::vector<unsigned> testLabels(nTestSamples);
  vnl_vector<double> s;

  std::cout << "Generating test data\n";
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

  std::cout << "****************The Training set****************\n";

  vnl_vector<double> x(nDims);
  std::vector<double> out(1);
  x.fill(0.0);
  std::cout << "x(2) varies across from -2 to + 2\n"
           << "x(1) varies down from    2 to  -2\n";

#if 0

  clsfy_k_nearest_neighbour knn;
  knn.set(trainingVectors, labels);
  knn.set_k(3);
  std::cout << "\nKNN output\n" << std::setprecision(4);
  for (x(0) = 2; x(0) >= -2 ; x(0) -= 0.25)
  {
    for (x(1) = -2; x(1) <= 2 ; x(1) += 0.25)
    {
      knn.class_probabilities(out, x);
      std::cout << std::fixed << std::setw(3) << out[0] << ' ';
    }
    std::cout << std::endl;
  }

  for (x(0) = 2; x(0) >= -2; x(0) -= 0.25)
  {
    for (x(1) = -2; x(1) <= 2 ; x(1) += 0.25)
    {
      std::cout << knn.classify(x);
    }
    std::cout << std::endl;
  }

  clsfy_rbf_parzen win;
  win.set(trainingVectors, labels);
  win.set_rbf_width(0.2);
  win.set_power(10);
  std::cout << "x(2) varies across from -2 to +2\n"
           << "x(1) varies down from    2 to -2\n\n"
           << "Training data distribution\n"
           << std::setprecision(1);
  for (x(0) = 2; x(0) >= -2 ; x(0) -= 0.25)
  {
    for (x(1) = -2; x(1) <= 2 ; x(1) += 0.25)
    {
      double v = win.weightings(x);
      if (v < 0.05)
        std::cout << "  .  ";
      else
        std::cout << std::fixed << std::setw(4) << v << ' ';
    }
    std::cout << std::endl;
  }
#endif
  std::cout<<"======== TESTING BUILDING ===========\n" << std::setprecision(6);

  clsfy_binary_hyperplane_ls_builder builder;

  auto *pClassifier =
    (clsfy_binary_hyperplane*) builder.new_classifier();
  std::cout << "Finding Least Squares Separator using least squares\n";
  mbl_data_array_wrapper<vnl_vector<double> > training_set(trainingVectors);
  std::cout << "Error on Training set ";
  double train_error = builder.build(*pClassifier, training_set, labels);
  std::cout << train_error << std::endl;
  TEST_NEAR("Train error on classifier is good enough", train_error, 0.0, 0.05);

  std::cout << "****************Testing over test set**************\n";

  mbl_data_array_wrapper<vnl_vector<double> > test_set_inputs(testVectors);

  double test_error = clsfy_test_error(*pClassifier, test_set_inputs, testLabels);
  std::cout << "Error on Testing set " << test_error << std::endl;

  TEST_NEAR("Test error on classifier is good enough", test_error, 0.0, 0.1);

  // print input, print output

  x.fill(0.0);
  std::cout << "x(2) varies across from -2 to +2\n"
           << "x(1) varies down from    2 to -2\n" << std::setprecision(4);
  for (x(0) = 2; x(0) >= -2 ; x(0) -= 0.25)
  {
    for (x(1) = -2; x(1) <= 2 ; x(1) += 0.25)
    {
      pClassifier->class_probabilities(out, x);
      std::cout << out[0] << ' ';
    }
    std::cout << std::endl;
  }

  for (x(0) = 2; x(0) >= -2 ; x(0) -= 0.25)
  {
    for (x(1) = -2; x(1) <= 2 ; x(1) += 0.25)
    {
      std::cout << pClassifier->classify(x) << ' ';
    }
    std::cout << std::endl;
  }

  std::cout<<"======== TESTING I/O ===========\n";

  std::string test_path = "test_binary_hyperplane.bvl.tmp";

  vsl_b_ofstream bfs_out(test_path);
  TEST(("Opened " + test_path + " for writing").c_str(), (!bfs_out ), false);
  vsl_b_write(bfs_out, *pClassifier);
  bfs_out.close();

  clsfy_binary_hyperplane classifier_in;

  vsl_b_ifstream bfs_in(test_path);
  TEST(("Opened " + test_path + " for reading").c_str(), (!bfs_in ), false);
  vsl_b_read(bfs_in, classifier_in);

  bfs_in.close();
#if !LEAVE_FILES_BEHIND
  vpl_unlink(test_path.c_str());
#endif

  std::cout<<"Saved : " << *pClassifier << std::endl
          <<"Loaded: " << classifier_in << std::endl;

  TEST("saved classifier = loaded classifier",
       pClassifier->weights() == classifier_in.weights() &&
       pClassifier->bias() == classifier_in.bias(),
       true);

  TEST_NEAR("saved classifier(x) = loaded classifier(x)",
            pClassifier->log_l(vnl_vector<double>(nDims, 0.25)),
            classifier_in.log_l(vnl_vector<double>(nDims, 0.25)), 1.0e-10);

  std::cout << std::setprecision(6) << std::resetiosflags(std::ios::floatfield);

  delete pClassifier;

  test_clsfy_geman_mcclure_build();
}

void test_clsfy_geman_mcclure_build()
{
    std::vector<vpdfl_axis_gaussian_sampler *> generator(2);//
    constexpr unsigned nDims = 2;
    vnl_vector<double> meanPos(nDims), varPos(nDims), meanNeg(nDims), varNeg(nDims),origin(nDims);
    double d=1.0/vnl_math::sqrt2;
    origin.fill(0.0);
    std::vector<vnl_vector<double> > basisPos(2);
    basisPos[0].set_size(2);basisPos[0][0] = -d;basisPos[0][1] = d;
    basisPos[1].set_size(2);basisPos[1][0] = d;basisPos[1][1] = d;

    std::vector<vnl_vector<double> > basisNeg(2);
    double dx =  std::cos(0.96), dy =  std::sin(0.96); //(0.96 is a bit more than 45 degrees in radians)
    basisNeg[0].set_size(2);basisNeg[0][0] = -dx;basisNeg[0][1] = dy;
    basisNeg[1].set_size(2);basisNeg[1][0] = dx;basisNeg[1][1] = dy;

    vpdfl_axis_gaussian PDF0, PDF1;

    meanPos.fill(0.0);
    meanPos[0] = -2.75;   meanPos[1] = 2.75;
    meanNeg.fill(0.0);

    vnl_vector<double > deltaMeans = meanPos - meanNeg;
    varNeg[0]= 0.75; varNeg[1] = 3.0;
    varPos[0] = 1.0; varPos[1] = 4.0;
    PDF0.set(origin, varNeg);
    PDF1.set(origin, varPos);

    generator[0] = (vpdfl_axis_gaussian_sampler *)PDF0.new_sampler();
    generator[1] = (vpdfl_axis_gaussian_sampler *)PDF1.new_sampler();
    vnl_random rng;
    rng.reseed(111333);

    constexpr unsigned nSamples = 200;
    constexpr unsigned nTestSamples = 1000;
    std::vector<vnl_vector<double> > trainingVectors(nSamples);
    std::vector<vnl_vector<double> > testVectors(nTestSamples);
    std::vector<unsigned> labels(nSamples);
    std::vector<unsigned> testLabels(nTestSamples);
    vnl_vector<double> sbase;
    vnl_vector<double> s;

    std::cout << "Generating test data\n";
    unsigned nall = nSamples+nTestSamples;
    for (unsigned int i=0; i<nall; i++)
    {
        unsigned indicator=0;
        bool outlier=false;
        if (i%3==0)
        {
            indicator=1;
        }
        if (i<nSamples)
        {
            if ( indicator)
            {
                if (i%24==0)
                {
                    outlier = true;
                }
            }
            else if (i%20 == 0)
            {
                outlier = true;
            }
        }
        generator[indicator]->sample(sbase);
        std::vector<vnl_vector<double> >& basis = (indicator ? basisPos : basisNeg);
        vnl_vector<double>& mean=(indicator ? meanPos : meanNeg);
        s = sbase[0]*basis[0] + sbase[1]*basis[1];
        if (outlier)
        {
            if (indicator)
            {
                s += 4.0*deltaMeans; //Shift miles away from the boundary
            }
            else
            {
                s -= 2.0*deltaMeans;   // Shift inwards away from the boundary
            }
        }
        if (i<nSamples)
        {
            trainingVectors[i] = mean + s;
            labels[i] = indicator;
        }
        else
        {
            unsigned j = i-nSamples;
            testLabels[j] = indicator;
            testVectors[j] = mean + s;
        }
    }

    delete generator[0];
    delete generator[1];

    std::cout<<"======== TESTING BUILDING ===========\n" << std::setprecision(6);

    //First do an ordinary least squares build for comparison
    clsfy_binary_hyperplane_ls_builder builder;

    auto *pClassifier =
        (clsfy_binary_hyperplane*) builder.new_classifier();
    std::cout << "Finding Least Squares Separator using least squares\n";
    mbl_data_array_wrapper<vnl_vector<double> > training_set(trainingVectors);
    std::cout << "Error on Training set ";
    double train_errorLS = builder.build(*pClassifier, training_set, labels);
    std::cout << train_errorLS << std::endl;
    TEST_NEAR("Train error on classifier is good enough", train_errorLS, 0.0, 0.15);
    vnl_vector<double> weightsLS = pClassifier->weights();
    std::cout << "****************Testing over test set**************\n";

    mbl_data_array_wrapper<vnl_vector<double> > test_set_inputs(testVectors);

    double test_errorLS = clsfy_test_error(*pClassifier, test_set_inputs, testLabels);
    std::cout << "Error on Testing set " << test_errorLS << std::endl;

    TEST_NEAR("Test error on classifier is good enough", test_errorLS, 0.0, 0.25);

    //Now run the gmrho build but with sigma forced large so that the GM function approximates
    // to ordinary least square
    clsfy_binary_hyperplane_gmrho_builder builderGM;
    std::cout<<std::endl<<"Now training using Geman-McClure builder"<<std::endl
            << "Error on Training set ";
    builderGM.set_auto_estimate_sigma(false);
    builderGM.set_sigma_preset(10.0);

    #if 0 // commented out
    {
        using namespace clsfy_binary_hyperplane_gmrho_builder_helpers;
        //Now copy from the urggghh data wrapper into a sensible data structure (matrix!)
        training_set.reset();
        unsigned num_vars_ = training_set.current().size();
        unsigned num_examples_ =  training_set.size();
        vnl_matrix<double> data(num_examples_,num_vars_,0.0);
        unsigned i=0;
        do
        {
            double* row=data[i++];
            std::copy(training_set.current().begin(),training_set.current().end(),row);
        } while (training_set.next());

        Set up category regression values determined by output class
            vnl_vector<double> y(num_examples_,0.0);
        std::transform(labels.begin(),labels.end(),
                      y.begin(),
                      category_value(std::count(labels.begin(),labels.end(),1u),labels.size()));

        vnl_vector<double> weights(weightsLS);

        weights.set_size(num_vars_+1);

        weights.update(weightsLS,0);
        weights[num_vars_] = pClassifier->bias();
        gmrho_sum costFn(data,y,1.5);
        double epsilon=0.001;
        vnl_vector<double> delta(num_vars_+1,epsilon);
        vnl_vector<double> weights0(weights);
        weights0+= 2.0*delta;
        double f0 = costFn.f(weights0);
        vnl_vector<double > gradf0(num_vars_+1,0.0);
        vnl_vector<double > gradf1(num_vars_+1,0.0);
        epsilon=1.0E-6;
        for (unsigned i=0; i<weights.size();i++)
        {
            weights = weights0;
            weights[i] += epsilon;
            double f1 = costFn.f(weights);
            weights = weights0;
            weights[i] -= epsilon;
            double f2 = costFn.f(weights);
            gradf0[i] = (f1 - f2)/(2.0*epsilon);
        }
        costFn.gradf(weights0,gradf1);
        vnl_vector<double> dg = gradf1-gradf0;
        TEST_NEAR("GM cost function gradient is consistent", dg.inf_norm(), 0.0, 1.0E-4);
    }
    #endif // 0

    clsfy_builder_base* pBase = &builderGM;
    double train_error = pBase->build(*pClassifier, training_set, 1,labels);
    std::cout << train_error << std::endl;
    TEST_NEAR("Train error on classifier is good enough", train_error, 0.0, 0.15);
    vnl_vector<double > weights = pClassifier->weights();
    weights.normalize();
    weightsLS.normalize();
    vnl_vector<double> diff=weightsLS - weights;
    TEST_NEAR("Hyperplane close to LS Build at high sigma", diff.inf_norm(), 0.0, 0.02);
    std::cout << "****************Testing over test set**************\n";

    double test_error = clsfy_test_error(*pClassifier, test_set_inputs, testLabels);
    std::cout << "Error on Testing set " << test_error << std::endl;
    TEST_NEAR("Test error on classifier is good enough", test_error, 0.0, 0.2);
    double dtest=std::fabs(test_error - test_errorLS);
    TEST_NEAR("Test error is similar to LS build at high sigma", dtest, 0.0, 0.005);

    builderGM.set_auto_estimate_sigma(true);
    builderGM.set_sigma_preset(1.0);

    std::cout<<"Now doing Geman McClure build with automatic sigma scaling..."<<std::endl;

    double train_error2 = pBase->build(*pClassifier, training_set,1, labels);
    std::cout << train_error2 << std::endl;
    TEST_NEAR("Train error on classifier is good enough", train_error2, 0.0, 0.05);
    weights = pClassifier->weights();
    weights.normalize();
    diff=weightsLS - weights;
    TEST_NEAR("Hyperplane still fairly close to LS Build", diff.inf_norm(), 0.0, 0.25);
    std::cout << "****************Testing over test set**************\n";

    double test_error2 = clsfy_test_error(*pClassifier, test_set_inputs, testLabels);
    std::cout << "Error on Testing set " << test_error2 << std::endl;
    TEST("Test error on classifier has improved", test_error2<test_error, true);
    TEST_NEAR("Test error on classifier is good enough", test_error2, 0.0, 0.10);

    delete pClassifier;
}

TESTMAIN(test_binary_hyperplane);
