// This is mul/clsfy/tests/test_adaboost.cxx
#include <testlib/testlib_test.h>
//:
// \file
// \brief Tests the clsfy_adaboost_trainer class
// \author dac
// Test construction, IO etc.

#include <vcl_iostream.h>
#include <vcl_string.h>
#include <vcl_algorithm.h>
#include <vpl/vpl.h> // vpl_unlink()
#include <clsfy/clsfy_simple_adaboost.h>
#include <clsfy/clsfy_binary_threshold_1d_builder.h>
#include <clsfy/clsfy_binary_threshold_1d.h>
#include <clsfy/clsfy_adaboost_trainer.h>
#include <clsfy/clsfy_adaboost_sorted_builder.h>
#include <vsl/vsl_binary_loader.h>
#include <vsl/vsl_vector_io.h>
#include <mbl/mbl_data_array_wrapper.h>
#include <vpdfl/vpdfl_axis_gaussian.h>
#include <vpdfl/vpdfl_axis_gaussian_sampler.h>

#ifndef LEAVE_FILES_BEHIND
#define LEAVE_FILES_BEHIND 0
#endif

//: Extracts the j-th element of each vector in data and puts into v
void get_1d_inputs(vnl_vector<double>& v,
                   mbl_data_wrapper<vnl_vector<double> >& data, int j)
{
  unsigned long n = data.size();
  v.set_size(n);
  data.reset();
  for (unsigned long i=0;i<n;++i)
  {
    v[i] = data.current()[j];
    data.next();
  }
}


//: Tests the clsfy_simple_adaboost and clsfy_adaboost_trainer classes
void test_adaboost()
{
  vcl_cout << "********************************\n"
           << " Testing clsfy_adaboost_trainer\n"
           << "********************************\n";

  // Generate lots of examples for adaboost algorithm
  // Each vector represents a +ve or -ve example
  // Each element is the output of a single weak classifier
  int n_pos = 100;
  int n_neg = 100;
  vcl_vector<vnl_vector<double> > pos_samples(n_pos);
  vcl_vector<vnl_vector<double> > neg_samples(n_neg);

  // Create +ve and -ve sets of data for 3 separate weak classifiers
  int n_clfrs= 4;
  vcl_vector< vpdfl_axis_gaussian > pos_models(n_clfrs);
  vcl_vector< vpdfl_axis_gaussian > neg_models(n_clfrs);
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

  vcl_vector< vpdfl_axis_gaussian_sampler > pos_samplers(n_clfrs);
  vcl_vector< vpdfl_axis_gaussian_sampler > neg_samplers(n_clfrs);
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
  vcl_vector< vnl_vector<double> > inputs_vec(0);
  vcl_vector< unsigned > outputs(0);
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

  // build clsfy_simple_adaboost
  vcl_cout<<"*************normal classifier************\n";
  clsfy_simple_adaboost *pClassifier = new clsfy_simple_adaboost;
  clsfy_binary_threshold_1d_builder b_thresh_builder;
  clsfy_adaboost_trainer adab_trainer;
  int n_rounds=100; // was 5; increased to 100 to better see the timing (since vcl_clock() is rather inaccurate...) - PVr
  adab_trainer.build_strong_classifier(*pClassifier, n_rounds, b_thresh_builder,
                                       egs0, egs1 );


  // build clsfy_simple_adaboost using sorted method 3
  vcl_cout<<"*************sorted classifier4************\n";
  clsfy_simple_adaboost *pClassifier4 = new clsfy_simple_adaboost;
  clsfy_binary_threshold_1d_builder b_thresh_sorted_builder4;
  clsfy_adaboost_sorted_builder adab_sorted_builder;
  adab_sorted_builder.set_batch_size( 20 );
  adab_sorted_builder.set_max_n_clfrs( n_rounds );
  adab_sorted_builder.set_save_data_to_disk( false );
  adab_sorted_builder.set_weak_builder( b_thresh_sorted_builder4 );
  adab_sorted_builder.build( *pClassifier4, inputs, 1, outputs);

  pClassifier4->print_summary(vcl_cout);


   // compare alpha values for classifier4 (with classifier1)
  double diff=0;
  unsigned na= vcl_min(pClassifier->alphas().size(), pClassifier4->alphas().size() );
  na = 3; // rounding errors can cause major differences in the values of some of the later weightings.
  for (unsigned k=0; k<na; ++k)
  {
    TEST_NEAR("sorted classifier4 == normal classifier",
              pClassifier->alphas()[k], pClassifier4->alphas()[k], 0.001);
  }

  vcl_cout<<"diff= "<<diff<<vcl_endl;


  // test positive examples from training set
  // N.B. egs0 are the positive training examples
  double tpr=0.0, fpr=1.0, adab_te=0.0, te; // initialise to make compiler happy

  for (unsigned int k=1; k<=pClassifier->alphas().size(); ++k)
  {
    int tp=0, fp=0;
    pClassifier->set_n_clfrs_used(k);
    for (int i=0; i<n_pos; ++i)
      if ( pClassifier->classify( pos_samples[i] ) == 1 ) tp++;

    for (int i=0; i<n_neg; ++i)
      if ( pClassifier->classify( neg_samples[i] ) == 1 ) fp++;

    //vcl_cout<<"Applied to training set:\n"
    //        <<"number of classifiers used= "<<k<<vcl_endl;
    tpr=(tp*1.0)/n_pos;
    vcl_cout<<"True positives= "<<tpr<<vcl_endl;
    fpr= (fp*1.0)/n_neg;
    vcl_cout<<"False positives= "<<fpr<<vcl_endl;
    adab_te= ((n_pos-tp+fp)*1.0)/(n_pos+n_neg);
    vcl_cout<<"total error rate= "<<adab_te<<vcl_endl;
  }


  //soft test
  TEST("tpr>0.8", tpr>0.8, true);
  TEST("fpr<0.2", fpr<0.2, true);


  //Train individual classifiers on each of the 4 Gaussian data sets
  for (int i=0; i<n_clfrs; ++i)
  {
    vnl_vector<double> pos_egs, neg_egs;
    get_1d_inputs(pos_egs,egs1,i);
    get_1d_inputs(neg_egs,egs0,i);
    int n_pos=pos_egs.size();
    int n_neg=neg_egs.size();
    vnl_vector<double> pos_wts(n_pos, 0.5/n_pos), neg_wts(n_neg, 0.5/n_neg);

#if 0
    vcl_cout<<"pos_egs= "<<pos_egs<<vcl_endl
            <<"pos_wts= "<<pos_wts<<vcl_endl
            <<"neg_egs= "<<neg_egs<<vcl_endl
            <<"neg_wts= "<<neg_wts<<vcl_endl;
#endif

    clsfy_classifier_1d* c1d2 = b_thresh_builder.new_classifier();
    double error=b_thresh_builder.build(*c1d2,neg_egs,neg_wts,pos_egs,pos_wts);

    vcl_cout<<"error= "<<error<<vcl_endl;
    c1d2->print_summary(vcl_cout);


    int tp=0, fp=0;
    for (int i=0; i<n_pos; ++i)
      if ( c1d2->classify( pos_egs[i] ) == 1 ) tp++;

    for (int i=0; i<n_neg; ++i)
      if ( c1d2->classify( neg_egs[i] ) == 1 ) fp++;

    tpr= (tp*1.0)/n_pos;
    vcl_cout<<"tpr= "<<tpr<<vcl_endl;
    fpr= (fp*1.0)/n_neg;
    vcl_cout<<"fpr= "<<fpr<<vcl_endl;
    te= ((n_pos-tp+fp)*1.0)/(n_pos+n_neg);
    vcl_cout<<"total error rate= "<<te<<vcl_endl;

    TEST("Weak clfr te not less than strong clfr te", te<adab_te, false);

    delete c1d2;
  }


  vcl_cout<<"======== TESTING I/O ===========\n";

   // add binary loaders
  vsl_add_to_binary_loader(clsfy_binary_threshold_1d());
  vcl_string test_path = "test_clsfy_simple_adaboost.bvl.tmp";

  vsl_b_ofstream bfs_out(test_path);
  TEST(("Opened " + test_path + " for writing").c_str(), (!bfs_out ), false);
  vsl_b_write(bfs_out, *pClassifier);
  bfs_out.close();


  clsfy_simple_adaboost classifier_in;

  vsl_b_ifstream bfs_in(test_path);
  TEST(("Opened " + test_path + " for reading").c_str(), (!bfs_in ), false);
  vsl_b_read(bfs_in, classifier_in);

  bfs_in.close();
#if !LEAVE_FILES_BEHIND
  vpl_unlink(test_path.c_str());
  vpl_unlink("temp.dat");
#endif

  vcl_cout<<"Saved :\n"
          << *pClassifier << vcl_endl
          <<"Loaded:\n"
          << classifier_in << vcl_endl;

  TEST("saved classifier == loaded classifier", *pClassifier, classifier_in);

  delete pClassifier;

  vsl_delete_all_loaders();
}

TESTLIB_DEFINE_MAIN(test_adaboost);
