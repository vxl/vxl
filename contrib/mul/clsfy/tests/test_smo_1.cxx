// This is mul/clsfy/tests/test_smo_1.cxx
// Copyright: (C) 2001 British Telecommunications PLC

//:
// \file
// \brief Tests the Sequential minimum optimisation code.
// \author Ian Scott

#include <vcl_iostream.h>
#include <vcl_ios.h>
#include <vcl_iomanip.h>
#include <clsfy/clsfy_smo_1.h>
#include <clsfy/clsfy_k_nearest_neighbour.h>
#include <clsfy/clsfy_rbf_parzen.h>
#include <vpdfl/vpdfl_axis_gaussian_sampler.h>
#include <vpdfl/vpdfl_axis_gaussian.h>
#include <mbl/mbl_data_array_wrapper.h>
#include <testlib/testlib_test.h>
#include <vnl/vnl_math.h>
#include <vnl/vnl_random.h>
#include <vul/vul_timer.h>

//=======================================================================

void test_smo()
{
  vcl_cout << "\n\n\n"
           << "*************************\n"
           << " Testing clsfy_smo_1_rbf\n"
           << "*************************\n\n"

           <<"======== TESTING BUILDING ===========\n";

  vcl_vector<vpdfl_axis_gaussian_sampler *> generator(4);//
  const unsigned nDims = 2;
  vnl_vector<double> mean0(nDims), var0(nDims), mean1(nDims), var1(nDims), mean2(nDims), var2(nDims), mean3(nDims), var3(nDims);
  vpdfl_axis_gaussian PDF0, PDF1, PDF2, PDF3;

  mean0.fill(0.0); mean0(0) = 1;
  mean1.fill(0.0); mean1(0) = -1;
  mean2.fill(0.0); mean2(1) = 1;
  mean3.fill(0.0); mean3(1) = -1;

  const double width = 0.05;
  var0.fill(width/10.0); var0(0) = width;
  var1.fill(width/10.0); var1(0) = width;
  var2.fill(width/10.0); var2(1) = width;
  var3.fill(width/10.0); var3(1) = width;

  PDF0.set(mean0, var0);
  PDF1.set(mean1, var1);
  PDF2.set(mean2, var2);
  PDF3.set(mean3, var3);

  TEST("is_a() and is_class() for vpdfl_axis_gaussian",
       PDF0.is_class("vpdfl_axis_gaussian") &&
       PDF0.is_class("vpdfl_pdf_base") &&
       PDF0.is_a() == "vpdfl_axis_gaussian", true);

  generator[0] = (vpdfl_axis_gaussian_sampler *)PDF0.new_sampler();
  generator[1] = (vpdfl_axis_gaussian_sampler *)PDF1.new_sampler();
  generator[2] = (vpdfl_axis_gaussian_sampler *)PDF2.new_sampler();
  generator[3] = (vpdfl_axis_gaussian_sampler *)PDF3.new_sampler();

  TEST("is_a() and is_class() for vpdfl_axis_gaussian_sampler",
       generator[0]->is_class("vpdfl_axis_gaussian_sampler") &&
       generator[0]->is_class("vpdfl_sampler_base") &&
       generator[0]->is_a() == "vpdfl_axis_gaussian_sampler", true);

  vnl_random rng;
  rng.reseed(33323335);
  const unsigned nSamples = 200;
  vcl_vector<unsigned> labels(nSamples);
  vcl_vector<int> svlabels(nSamples);
  vcl_vector<vnl_vector<double> > data(nSamples);
  vnl_vector<double> s;
  vcl_cout << "Generating test data\n";
  vcl_vector<unsigned> labelcount(4, 0u);
#if 0
  vcl_ofstream svmtest("C:\\svmtest.txt");
#endif
  for (unsigned int i=0; i<nSamples; i++)
  {
    int c = rng.lrand32(3);
    labels[i] = c/2;
    svlabels[i] = (c/2) * 2 - 1;
    labelcount[c] ++;
    generator[c]->sample(s);
#if 0
    svmtest << s << ' ' << svlabels[i] << vcl_endl;
#endif
    data[i] = s;
  }

  delete generator[0];
  delete generator[1];
  delete generator[2];
  delete generator[3];

  mbl_data_array_wrapper<vnl_vector<double> > trainingVectors(data);

  vcl_cout << "****************The Training set****************\n"
           << "The number of labels from each generators are respectively "
           << labelcount[0] << ' '
           << labelcount[1] << ' '
           << labelcount[2] << ' '
           << labelcount[3] << vcl_endl;

  vnl_vector<double> x(nDims);
  vcl_vector<double> out(1);
  x.fill(0.0);
  vcl_cout << "x(2) varies across from -2 to + 2\n"
           << "x(1) varies down from -2 to + 2\n";

  clsfy_k_nearest_neighbour knn;
  knn.set(data, labels);
  knn.set_k(3);
  vcl_cout << vcl_endl << "KNN output\n" << vcl_setprecision(4);
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
  win.set_rbf_width(0.14);
  win.set_power(10);
  vcl_cout << vcl_endl << "Training data distribution\n"
           << vcl_setprecision(1);
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

  mbl_data_wrapper<vnl_vector<double> > &inputs = trainingVectors;

  vcl_cout << "****************Testing SVM on Training set**************\n"
           << vcl_setprecision(8) << vcl_resetiosflags(vcl_ios_floatfield);

  clsfy_smo_1_rbf svAPI;
  svAPI.set_gamma( (1.0/(2.0*0.2*0.2)));
  //svAPI.set_C(1.0);
  svAPI.set_data(inputs, svlabels  );

  vul_timer mytimer;

  svAPI.calc();
  vcl_cerr << "Optimisation took " << mytimer.real() << " milliseconds\n";

  unsigned int nSupports=0;
  for (unsigned int i=0; i<nSamples; i++)
  {
    if (svAPI.lagrange_mults()(i) != 0.0)
    {
      nSupports++;
      inputs.set_index(i);
      vcl_cout << i << ' ' << vcl_fabs(svAPI.lagrange_mults()(i)) << ' ';
      for (unsigned int j=0; j<nDims; j++)
        vcl_cout << inputs.current().operator()(j) << ' ';
      vcl_cout << labels[i] <<vcl_endl;
    }
  }
  vcl_cout << "\n\n";
  TEST_NEAR("Number of support vectors", nSupports, 29, 3);
  TEST_NEAR("Bias", svAPI.bias(), -0.064, 0.03);
  TEST_NEAR("Error", svAPI.error(), 0.0, 0.0);

  vcl_cout << vcl_setprecision(6) << vcl_resetiosflags(vcl_ios_floatfield);
}

TESTLIB_DEFINE_MAIN(test_smo);
