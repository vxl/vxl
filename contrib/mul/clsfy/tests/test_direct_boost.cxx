// This is mul/clsfy/tests/test_direct_boost.cxx
#include <iostream>
#include <string>
#include <testlib/testlib_test.h>
//:
// \file
// \brief Tests the clsfy_direct_boost and clsfy_direct_boost_builder classes
// \author dac
// Test construction, IO etc.

#ifdef _MSC_VER
#  include <vcl_msvc_warnings.h>
#endif
#include <vpl/vpl.h> // vpl_unlink()
#include <clsfy/clsfy_direct_boost.h>
#include <clsfy/clsfy_direct_boost_builder.h>
#include <clsfy/clsfy_mean_square_1d_builder.h>
#include <clsfy/clsfy_mean_square_1d.h>

#include <vsl/vsl_binary_loader.h>
#include <vsl/vsl_vector_io.h>
#include <mbl/mbl_data_array_wrapper.h>
#include <vpdfl/vpdfl_axis_gaussian.h>
#include <vpdfl/vpdfl_axis_gaussian_sampler.h>

#ifndef LEAVE_FILES_BEHIND
#define LEAVE_FILES_BEHIND 0
#endif

//: Extracts the j-th element of each vector in data and puts into v
// Defined in test_adaboost;
void get_1d_inputs(vnl_vector<double>& v,
                   mbl_data_wrapper<vnl_vector<double> >& data, int j);


//: Tests the clsfy_direct_boost and clsfy_direct_boost_builder classes
void test_direct_boost()
{
  std::cout << "****************************\n"
           << " Testing clsfy_direct_boost\n"
           << "****************************\n";

  // Generate lots of examples for adaboost algorithm
  // Each vector represents a +ve or -ve example
  // Each element is the output of a single weak classifier
  int n_pos = 100;
  int n_neg = 100;
  std::vector<vnl_vector<double> > pos_samples(n_pos);
  std::vector<vnl_vector<double> > neg_samples(n_neg);

  // Create +ve and -ve sets of data for 4 separate weak classifiers
  int n_clfrs= 4;
  std::vector< vpdfl_axis_gaussian > pos_models(n_clfrs);
  std::vector< vpdfl_axis_gaussian > neg_models(n_clfrs);
  pos_models[0].set(vnl_vector<double>(1,3), vnl_vector<double>(1,3) );
  pos_models[1].set(vnl_vector<double>(1,7), vnl_vector<double>(1,2) );
  pos_models[2].set(vnl_vector<double>(1,-2), vnl_vector<double>(1,3) );
  pos_models[3].set(vnl_vector<double>(1,-1), vnl_vector<double>(1,2) );
  //pos_models[4].set(vnl_vector<double>(1,-5), vnl_vector<double>(1,2) );

  neg_models[0].set(vnl_vector<double>(1,7), vnl_vector<double>(1,20) );
  neg_models[1].set(vnl_vector<double>(1,3), vnl_vector<double>(1,10) );
  neg_models[2].set(vnl_vector<double>(1,-8), vnl_vector<double>(1,10) );
  neg_models[3].set(vnl_vector<double>(1,7), vnl_vector<double>(1,15) );
  //neg_models[4].set(vnl_vector<double>(1,-1), vnl_vector<double>(1,15) );

  std::vector< vpdfl_axis_gaussian_sampler > pos_samplers(n_clfrs);
  std::vector< vpdfl_axis_gaussian_sampler > neg_samplers(n_clfrs);
  for (int i=0; i<n_clfrs; ++i)
  {
    pos_samplers[i].set_model(pos_models[i]);
    neg_samplers[i].set_model(neg_models[i]);
  }

  //sample the distributions to build pos+ negative examples
  //build +ve
  for (int j=0; j<n_pos; ++j)
  {
    pos_samples[j].set_size(n_clfrs);
    for (int i=0; i<n_clfrs; ++i)
    {
      vnl_vector<double> x(1);
      pos_samplers[i].sample(x);
      pos_samples[j](i)= x(0);
    }
  }

  //build -ve
  for (int j=0; j<n_neg; ++j)
  {
    neg_samples[j].set_size(n_clfrs);
    for (int i=0; i<n_clfrs; ++i)
    {
      vnl_vector<double> x(1);
      neg_samplers[i].sample(x);
      neg_samples[j](i)= x(0);
    }
  }


  //create data wrappers
  mbl_data_array_wrapper< vnl_vector<double> > egs1(&pos_samples[0],n_pos);
  mbl_data_array_wrapper< vnl_vector<double> > egs0(&neg_samples[0],n_neg);

  //create inputs and outputs (so can train using clsfy_adaboost_sorted_builder)
  std::vector< vnl_vector<double> > inputs_vec(0);
  std::vector< unsigned > outputs(0);
  for (int j=0; j<n_pos; ++j)
  {
    inputs_vec.push_back( pos_samples[j] );
    outputs.push_back( 1 );
  }

  for (int j=0; j<n_neg; ++j)
  {
    inputs_vec.push_back( neg_samples[j] );
    outputs.push_back( 0 );
  }
  mbl_data_array_wrapper< vnl_vector<double> > inputs(&inputs_vec[0],n_pos+n_neg);

  // build clsfy_direct_boost
  std::cout<<"*************normal classifier************\n";
  auto *pClassifier = new clsfy_direct_boost;
  clsfy_direct_boost_builder builder;
  int n_rounds=5;
  builder.set_max_n_clfrs(n_rounds);
  builder.set_calc_all_thresholds(true);
  builder.set_prop(0.85);
  builder.set_batch_size(100);
  clsfy_mean_square_1d_builder weak_builder;
  builder.set_weak_builder(weak_builder);
  double t_error=builder.build( *pClassifier, inputs, 1, outputs);

  std::cout<<"t_error= "<<t_error<<std::endl;


  // test positive examples from training set
  // nb egs0 are the positive training examples
  double tpr=0.0, fpr=1.0, adab_te=0.0;

  for (unsigned int k=1; k<=pClassifier->wts().size(); ++k)
  {
    int tp=0, fp=0;
    pClassifier->set_n_clfrs_used(k);
    for (int i=0; i<n_pos; ++i)
      if ( pClassifier->classify( pos_samples[i] ) == 1 ) tp++;

    for (int i=0; i<n_neg; ++i)
      if ( pClassifier->classify( neg_samples[i] ) == 1 ) fp++;

    //std::cout<<"Applied to training set:\n"
    //        <<"number of classifiers used= "<<k<<std::endl;
    tpr=(tp*1.0)/n_pos;
    std::cout<<"True positives= "<<tpr<<std::endl;
    fpr= (fp*1.0)/n_neg;
    std::cout<<"False positives= "<<fpr<<std::endl;
    adab_te= ((n_pos-tp+fp)*1.0)/(n_pos+n_neg);
    std::cout<<"total error rate= "<<adab_te<<std::endl;
  }


  //soft test
  TEST("tpr>0.8", tpr>0.8, true);
  TEST("fpr<0.2", fpr<0.2, true);


  std::cout<<"======== TESTING I/O ===========\n";

   // add binary loaders
  vsl_add_to_binary_loader(clsfy_mean_square_1d());
  std::string test_path = "test_clsfy_direct_boost.bvl.tmp";

  vsl_b_ofstream bfs_out(test_path);
  TEST(("Opened " + test_path + " for writing").c_str(), (!bfs_out ), false);
  vsl_b_write(bfs_out, *pClassifier);
  bfs_out.close();


  clsfy_direct_boost classifier_in;

  vsl_b_ifstream bfs_in(test_path);
  TEST(("Opened " + test_path + " for reading").c_str(), (!bfs_in ), false);
  vsl_b_read(bfs_in, classifier_in);

  bfs_in.close();
#if !LEAVE_FILES_BEHIND
  vpl_unlink(test_path.c_str());
  vpl_unlink("temp.dat");
#endif

  std::cout<<"Saved :\n"
          << *pClassifier << std::endl
          <<"Loaded:\n"
          << classifier_in << std::endl;

  TEST("saved classifier == loaded classifier", *pClassifier, classifier_in);

  delete pClassifier;
  vsl_delete_all_loaders();
}

TESTMAIN(test_direct_boost);
