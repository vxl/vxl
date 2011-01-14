// This is mul/clsfy/tests/test_binary_1d_wrapper.cxx
#include <testlib/testlib_test.h>
//:
// \file
// \brief Tests the clsfy_binary_1d_wrapper class
// \author DAC, Ian Scott
// Test construction, IO etc

#include <vcl_iostream.h>
#include <vcl_sstream.h>
#include <vcl_string.h>
#include <vpl/vpl.h> // vpl_unlink()
#include <vsl/vsl_binary_loader.h>
#include <vsl/vsl_vector_io.h>
#include <mbl/mbl_data_collector_list.h>
#include <mbl/mbl_test.h>
#include <clsfy/clsfy_binary_threshold_1d.h>
#include <clsfy/clsfy_binary_threshold_1d_builder.h>
#include <clsfy/clsfy_binary_1d_wrapper.h>
#include <clsfy/clsfy_binary_1d_wrapper_builder.h>
#include <clsfy/clsfy_add_all_loaders.h>
#include <vpdfl/vpdfl_axis_gaussian.h>
#include <vpdfl/vpdfl_axis_gaussian_sampler.h>

#ifndef LEAVE_FILES_BEHIND
#define LEAVE_FILES_BEHIND 0
#endif

//: Tests the clsfy_binary_1d_wrapper and builder using the clsfy_binary_threshold_1d class
void test_binary_1d_wrapper()
{
  vcl_cout << "*****************************************\n"
           << " Testing clsfy_binary_1d_wrapper_builder\n"
           << "*****************************************\n";

  // Create +ve and -ve sets of data
  vpdfl_axis_gaussian pos_model;
  vpdfl_axis_gaussian neg_model;
  pos_model.set(vnl_vector<double>(1,3), vnl_vector<double>(1,3) );
  neg_model.set(vnl_vector<double>(1,10), vnl_vector<double>(1,20) );


  vpdfl_axis_gaussian_sampler pos_sampler;
  vpdfl_axis_gaussian_sampler neg_sampler;

  pos_sampler.set_model(pos_model);
  neg_sampler.set_model(neg_model);
  pos_sampler.reseed(111333);
  neg_sampler.reseed(111333);


  // Generate lots of examples for binary threshold training set
  unsigned n_pos = 50;
  unsigned n_neg = 50;


  mbl_data_collector_list <vnl_vector<double> > training_samples;
  vcl_vector<unsigned> training_outputs;

  for (unsigned i=0; i<n_pos; ++i)
  {
    vnl_vector<double> x(1);
    pos_sampler.sample( x );
    training_samples.record(x);
    training_outputs.push_back(1);
    neg_sampler.sample( x );
    training_samples.record(x);
    training_outputs.push_back(0);
  }

  for (unsigned i=0; i<n_neg; ++i)
  {
    vnl_vector<double> x(1);
  }

  //vcl_cout<<"pos_samples= "<<pos_samples<<vcl_endl
  //        <<"neg_samples= "<<neg_samples<<vcl_endl;

  // Generate lots of examples for test set
  vcl_vector<vnl_vector<double> > pos_samples_test, neg_samples_test;
  for (unsigned i=0; i<n_pos; ++i)
  {
    vnl_vector<double> x(1);
    pos_sampler.sample( x );
    pos_samples_test.push_back(x);
  }

  for (unsigned i=0; i<n_neg; ++i)
  {
    vnl_vector<double> x(1);
    neg_sampler.sample( x );
    neg_samples_test.push_back(x);
  }

  //vcl_cout<<"pos_samples_test= "<<pos_samples_test<<vcl_endl
  //        <<"neg_samples_test= "<<neg_samples_test<<vcl_endl;


  vcl_cout<<"=================test pos + neg samples ============\n";

  clsfy_binary_1d_wrapper_builder b_thresh_builder;
  b_thresh_builder.set_builder_1d(clsfy_binary_threshold_1d_builder());

  vcl_auto_ptr<clsfy_classifier_base> b_thresh_clsfr(
    b_thresh_builder.new_classifier());

  double error1= b_thresh_builder.build(*b_thresh_clsfr,
                                        training_samples.data_wrapper(),
                                        training_outputs);

  vcl_cout << *b_thresh_clsfr<<vcl_endl;
  //b_thresh_clsfr->print_summary(vcl_cout);
  vcl_cout<<"error1= "<<error1<<vcl_endl;


  // calc te (i.e. total error) on test set
  int tp=0, fp=0;
  for (unsigned i=0; i<n_pos; ++i)
    if ( b_thresh_clsfr->classify( pos_samples_test[i] ) == 1 ) tp++;

  for (unsigned i=0; i<n_neg; ++i)
    if ( b_thresh_clsfr->classify( neg_samples_test[i] ) == 1 ) fp++;

  vcl_cout<<"Applied to test set:\n";
  double tpr=(tp*1.0)/n_pos, fpr= (fp*1.0)/n_neg;
  vcl_cout<<"True positives= "<<tpr<<vcl_endl
          <<"False positives= "<<fpr<<vcl_endl;

  double te=((n_pos-tp+fp)*1.0)/(n_pos+n_neg);
  vcl_cout<<"te= "<<te<<vcl_endl;

  // simple test for binary threshold
  TEST("tpr>0.7", tpr>0.7, true);
  TEST("fpr<0.3", fpr<0.3, true);

  vcl_cout<<"======== TESTING I/O ===========\n";

   // add binary loaders
  clsfy_add_all_loaders();
  vcl_string test_path = "test_clsfy_binary_1d_wrapper.bvl.tmp";

  vsl_b_ofstream bfs_out(test_path);
  TEST(("Opened " + test_path + " for writing").c_str(), (!bfs_out ), false);
  vsl_b_write(bfs_out, b_thresh_clsfr.get());
  bfs_out.close();

  clsfy_classifier_base *classifier_in = 0;

  vsl_b_ifstream bfs_in(test_path);
  TEST(("Opened " + test_path + " for reading").c_str(), (!bfs_in ), false);
  vsl_b_read(bfs_in, classifier_in);

  bfs_in.close();
#if !LEAVE_FILES_BEHIND
  vpl_unlink(test_path.c_str());
#endif

  vcl_cout<<"Saved :\n"
          << *b_thresh_clsfr << vcl_endl
          <<"Loaded:\n"
          << classifier_in << vcl_endl;

  TEST("saved classifier = loaded classifier",
       mbl_test_summaries_are_equal(b_thresh_clsfr.get(), classifier_in), true);

  vcl_istringstream ss(
    "clsfy_binary_1d_wrapper_builder { builder_1d: clsfy_binary_threshold_1d_builder }\n" );
  vcl_auto_ptr<clsfy_builder_base> builder_configged =
    clsfy_builder_base::new_builder(ss);

  vcl_cout<<"Builder Constructed :\n"
          << static_cast<const clsfy_builder_base*>(&b_thresh_builder) << vcl_endl
          <<"Builder Configged:\n"
          << *builder_configged << vcl_endl;

  TEST("saved classifier = loaded classifier",
       mbl_test_summaries_are_equal(
          static_cast<clsfy_builder_base*>(&b_thresh_builder),
          builder_configged.get()), true );

  delete classifier_in;
  vsl_delete_all_loaders();
}

TESTMAIN(test_binary_1d_wrapper);
