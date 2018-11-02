// This is mul/clsfy/tests/test_mean_square_1d.cxx
#include <iostream>
#include <string>
#include <testlib/testlib_test.h>
//:
// \file
// \brief Tests the clsfy_mean_square_1d class
// \author dac
// Test construction, IO etc

#ifdef _MSC_VER
#  include <vcl_msvc_warnings.h>
#endif
#include <vpl/vpl.h> // vpl_unlink()
#include <clsfy/clsfy_mean_square_1d.h>
#include <clsfy/clsfy_mean_square_1d_builder.h>
#include <vsl/vsl_binary_loader.h>
#include <vsl/vsl_vector_io.h>
#include <pdf1d/pdf1d_gaussian.h>
#include <pdf1d/pdf1d_gaussian_sampler.h>

#ifndef LEAVE_FILES_BEHIND
#define LEAVE_FILES_BEHIND 0
#endif

//: Tests the clsfy_mean_square_1d class
void test_mean_square_1d()
{
  std::cout << "**************************************\n"
           << " Testing clsfy_mean_square_1d_builder\n"
           << "**************************************\n";


  // Create +ve and -ve sets of data
  pdf1d_gaussian pos_model(1,3);
  pdf1d_gaussian neg_model(6,3);

  pdf1d_gaussian_sampler pos_sampler;
  pdf1d_gaussian_sampler neg_sampler;

  pos_sampler.set_model(pos_model);
  neg_sampler.set_model(neg_model);
  pos_sampler.reseed(111333);
  neg_sampler.reseed(111333);


  // Generate lots of examples for binary threshold training set
  int n_pos = 50;
  int n_neg = 50;
  vnl_vector<double> pos_samples(n_pos), pos_wts(n_pos, 0.5/n_pos),
                     neg_samples(n_neg), neg_wts(n_neg, 0.5/n_neg);

  pos_sampler.get_samples( pos_samples );
  neg_sampler.get_samples( neg_samples );
  //std::cout<<"pos_samples= "<<pos_samples<<std::endl
  //        <<"neg_samples= "<<neg_samples<<std::endl;

  // Generate lots of examples for test set
  vnl_vector<double>  pos_samples_test(n_pos), neg_samples_test(n_neg);
  pos_sampler.get_samples( pos_samples_test );
  neg_sampler.get_samples( neg_samples_test );
  //std::cout<<"pos_samples_test= "<<pos_samples_test<<std::endl
  //        <<"neg_samples_test= "<<neg_samples_test<<std::endl;


  std::cout<<"=================test pos + neg samples ============\n";

  clsfy_mean_square_1d_builder mean_square_builder;
  clsfy_classifier_1d* mean_square_clsfr=mean_square_builder.new_classifier();
  double error1= mean_square_builder.build(*mean_square_clsfr,
                                           neg_samples, neg_wts,
                                           pos_samples, pos_wts);


  // build second classifier by combining pos and neg samples
  // + check get the same result
  int n= n_pos + n_neg;
  vnl_vector<double> egs(n), wts(n);
  std::vector<unsigned> outputs(n);
  for (int i=0; i<n_neg; ++i)
  {
    egs(i) = neg_samples(i);
    wts(i) = neg_wts(i);
    outputs[i] = 0;
  }
  for (int i=0; i<n_pos; ++i)
  {
    egs(i+n_neg) = pos_samples(i);
    wts(i+n_neg) = pos_wts(i);
    outputs[i+n_neg] = 1;
  }

  clsfy_classifier_1d* mean_square_clsfr2=mean_square_builder.new_classifier();
  double error2=mean_square_builder.build(*mean_square_clsfr2,
                                          egs,
                                          wts,
                                          outputs);

  TEST("Clsfrs the same", *mean_square_clsfr2, *mean_square_clsfr);
  TEST_NEAR("Clsfr error", error2, 0.0, 0.2);

  std::cout<<*mean_square_clsfr2<<std::endl
          <<*mean_square_clsfr<<std::endl;
  //mean_square_clsfr->print_summary(std::cout);
  std::cout<<"error1= "<<error1<<std::endl;


  //calc te (ie total error) on train set
  int tp=0, fp=0;
  for (int i=0; i<n_pos; ++i)
    if ( mean_square_clsfr->classify( pos_samples[i] ) == 1 ) tp++;

  for (int i=0; i<n_neg; ++i)
    if ( mean_square_clsfr->classify( neg_samples[i] ) == 1 ) fp++;


  double te=((n_pos-tp+fp)*1.0)/(n_pos+n_neg);
  std::cout<<"te on training set= "<<te<<std::endl;


  // calc te (ie total error) on test set
  tp=0; fp=0;
  for (int i=0; i<n_pos; ++i)
    if ( mean_square_clsfr->classify( pos_samples_test[i] ) == 1 ) tp++;

  for (int i=0; i<n_neg; ++i)
    if ( mean_square_clsfr->classify( neg_samples_test[i] ) == 1 ) fp++;

  std::cout<<"Applied to test set:\n";
  double tpr=(tp*1.0)/n_pos, fpr= (fp*1.0)/n_neg;
  std::cout<<"True positives= "<<tpr<<std::endl
          <<"False positives= "<<fpr<<std::endl;

  te=((n_pos-tp+fp)*1.0)/(n_pos+n_neg);
  std::cout<<"te= "<<te<<std::endl;

  // simple test for binary threshold
  TEST("tpr>0.7", tpr>0.7, true);
  TEST("fpr<0.3", fpr<0.3, true);


  std::cout << "******************************\n"
           << " Testing clsfy_mean_square_1d\n"
           << "******************************\n";

  // Test various parameter settings
  vnl_vector<double> p(2);
  // ie mean=1 square distance thresh= 4
  p[0]=1;
  p[1]=4;
  mean_square_clsfr->set_params(p);
  TEST("-2 not accepted", mean_square_clsfr->classify(-2), 0);
  TEST("2 accepted", mean_square_clsfr->classify(2), 1);

  // Test loading clsfy_mean_square_1d by base class pointer

  std::cout<<"======== TESTING I/O ===========\n";

   // add binary loaders
  vsl_add_to_binary_loader(clsfy_mean_square_1d());
  std::string test_path = "test_clsfy_simple_adaboost.bvl.tmp";

  vsl_b_ofstream bfs_out(test_path);
  TEST(("Opened " + test_path + " for writing").c_str(), (!bfs_out ), false);
  vsl_b_write(bfs_out, *mean_square_clsfr);
  bfs_out.close();

  clsfy_classifier_1d* classifier_in = mean_square_builder.new_classifier();;

  vsl_b_ifstream bfs_in(test_path);
  TEST(("Opened " + test_path + " for reading").c_str(), (!bfs_in ), false);
  vsl_b_read(bfs_in, *classifier_in);

  bfs_in.close();
#if !LEAVE_FILES_BEHIND
  vpl_unlink(test_path.c_str());
#endif

  std::cout<<"Saved :\n"
          << *mean_square_clsfr << std::endl
          <<"Loaded:\n"
          << classifier_in << std::endl;

  TEST("saved classifier = loaded classifier",
       mean_square_clsfr ->params(), classifier_in->params());

  delete mean_square_clsfr2;
  delete mean_square_clsfr;
  delete classifier_in;
  vsl_delete_all_loaders();
}

TESTMAIN(test_mean_square_1d);
