//:
// \file
// \brief Tests the clsfy_binary_threshold_1d class
// \author dac
// Test construction, IO etc

#include <vcl_iostream.h>
#include <vcl_strstream.h>
#include <vcl_iomanip.h>
#include <vcl_ios.h>
#include <vcl_string.h>
#include <clsfy/clsfy_binary_threshold_1d.h>
#include <clsfy/clsfy_binary_threshold_1d_builder.h>
#include <clsfy/clsfy_binary_threshold_1d_sorted_builder.h>
#include <vsl/vsl_binary_loader.h>
#include <mbl/mbl_mz_random.h>
#include <vpdfl/vpdfl_axis_gaussian.h>
#include <vpdfl/vpdfl_axis_gaussian_sampler.h>
#include <vsl/vsl_vector_io.h>
#include <mbl/mbl_data_array_wrapper.h>
#include <vcl_cmath.h>
#include <vnl/vnl_test.h>
#include <vpdfl/vpdfl_axis_gaussian.h>
#include <vpdfl/vpdfl_axis_gaussian_sampler.h>



//: Tests the clsfy_binary_threshold_1d class
void test_adaboost()
{
  vcl_cout << "*******************************************" << vcl_endl
           << " Testing clsfy_binary_threshold_1d_builder " << vcl_endl
           << "*******************************************" << vcl_endl;


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
  int n_pos = 50;
  int n_neg = 50;
  vnl_vector<double> pos_samples(n_pos), neg_samples(n_neg),
                     pos_wts(n_pos,0.5/n_pos), neg_wts(n_neg, 0.5/n_neg);
                     

  for (int i=0; i<n_pos; ++i)
  {
    vnl_vector<double> x(1);
    pos_sampler.sample( x );
    pos_samples[i]=x(0);
  }

  for (int i=0; i<n_neg; ++i)
  {
    vnl_vector<double> x(1);
    neg_sampler.sample( x );
    neg_samples[i]=x(0);
  }

  
  //vcl_cout<<"pos_samples= "<<pos_samples<<vcl_endl;
  //vcl_cout<<"neg_samples= "<<neg_samples<<vcl_endl;

  // Generate lots of examples for test set
  vnl_vector<double>  pos_samples_test(n_pos), neg_samples_test(n_neg);
  for (int i=0; i<n_pos; ++i)
  {
    vnl_vector<double> x(1);
    pos_sampler.sample( x );
    pos_samples_test[i]=x(0);
  }

  for (int i=0; i<n_neg; ++i)
  {
    vnl_vector<double> x(1);
    neg_sampler.sample( x );
    neg_samples_test[i]=x(0);
  }

  //vcl_cout<<"pos_samples_test= "<<pos_samples_test<<vcl_endl;
  //vcl_cout<<"neg_samples_test= "<<neg_samples_test<<vcl_endl;


  vcl_cout<<"=================test pos + neg samples ============"<<vcl_endl;
  
  clsfy_binary_threshold_1d_builder b_thresh_builder;
  clsfy_classifier_1d* b_thresh_clsfr=b_thresh_builder.new_classifier();
  double error1= b_thresh_builder.build(*b_thresh_clsfr, 
                                        neg_samples, neg_wts,
                                        pos_samples, pos_wts
                                        );


  vcl_cout<<*b_thresh_clsfr<<vcl_endl;
  //b_thresh_clsfr->print_summary(vcl_cout);  
  vcl_cout<<"error1= "<<error1<<vcl_endl;
  

  //calc te (ie total error) on train set
  int tp=0, fp=0;
  for (int i=0; i<n_pos; ++i)
    if ( b_thresh_clsfr->classify( pos_samples[i] ) == 1 ) tp++;
  
  for (int i=0; i<n_neg; ++i)
    if ( b_thresh_clsfr->classify( neg_samples[i] ) == 1 ) fp++;

 
  double tpr=(tp*1.0)/n_pos, fpr= (fp*1.0)/n_neg;
  double te=((n_pos-tp+fp)*1.0)/(n_pos+n_neg);
  vcl_cout<<"te on training set= "<<te<<vcl_endl;



  // calc te (ie total error) on test set
  tp=0; fp=0;
  for (int i=0; i<n_pos; ++i)
    if ( b_thresh_clsfr->classify( pos_samples_test[i] ) == 1 ) tp++;
  
  for (int i=0; i<n_neg; ++i)
    if ( b_thresh_clsfr->classify( neg_samples_test[i] ) == 1 ) fp++;

  vcl_cout<<"Applied to test set:"<<vcl_endl;
  tpr=(tp*1.0)/n_pos, fpr= (fp*1.0)/n_neg;
  vcl_cout<<"True positives= "<<tpr<<vcl_endl;
  vcl_cout<<"False positives= "<<fpr<<vcl_endl;


  te=((n_pos-tp+fp)*1.0)/(n_pos+n_neg);
  vcl_cout<<"te= "<<te<<vcl_endl;


  // simple test for binary threshold
  TEST( "tpr>0.7", tpr>0.7, true );
  TEST( "fpr<0.3", fpr<0.3, true );

  

  vcl_cout<<"=========swap pos and neg samples round==========="<<vcl_endl;

  // Train again with +ve and -ve data swapped round + see if get same error
  clsfy_classifier_1d* b_thresh_clsfr2=b_thresh_builder.new_classifier();
  double error2= b_thresh_builder.build(*b_thresh_clsfr2, 
                                        pos_samples, pos_wts,
                                        neg_samples, neg_wts
                                        );

  b_thresh_clsfr2->print_summary(vcl_cout);

  
  vcl_cout<<"error2= "<<error2<<vcl_endl;

  TEST ("error1==error2", vcl_fabs(error1-error2)<0.001, true);

  tp=0;
  fp=0;
  for (int i=0; i<n_neg; ++i)
    if ( b_thresh_clsfr2->classify( neg_samples_test[i] ) == 1 ) tp++;
  
  for (int i=0; i<n_pos; ++i)
    if ( b_thresh_clsfr2->classify( pos_samples_test[i] ) == 1 ) fp++;

  vcl_cout<<"Applied to test set (with +ve and -ve other way round:"<<vcl_endl;
  tpr=(tp*1.0)/n_neg, fpr= (fp*1.0)/n_pos;
  vcl_cout<<"True positives= "<<tpr<<vcl_endl;
  vcl_cout<<"False positives= "<<fpr<<vcl_endl;

  te= ((n_neg-tp+fp)*1.0)/(n_pos+n_neg);
  vcl_cout<<"te= "<<te<<vcl_endl;

  // simple test for binary threshold
  TEST( "tpr>0.7", tpr>0.7, true );
  TEST( "fpr<0.3", fpr<0.3, true );


  vcl_cout << "**************************************************" << vcl_endl
           << " Testing clsfy_binary_threshold_1d_sorted_builder " << vcl_endl
           << "**************************************************" << vcl_endl;


  // Train again with +ve and -ve data, but using sorted builder
  clsfy_binary_threshold_1d_sorted_builder b_thresh_sorted_builder;
  clsfy_classifier_1d* b_thresh_clsfr3=b_thresh_sorted_builder.new_classifier();
  double error3= b_thresh_sorted_builder.build(*b_thresh_clsfr3, 
                                        neg_samples, neg_wts,
                                        pos_samples, pos_wts
                                        );

  vcl_cout<<"error3= "<<error3<<vcl_endl;

  TEST( "sorted builder clsfr == normal builder clsfr",
         b_thresh_clsfr3->params()==b_thresh_clsfr->params(),
         true);
    

  vcl_cout << "***********************************" << vcl_endl
           << " Testing clsfy_binary_threshold_1d " << vcl_endl
           << "***********************************" << vcl_endl;


  // Test various parameter settings
  vnl_vector<double> p(2);
  // ie x=>5
  p[0]=1;
  p[1]=5;
  b_thresh_clsfr->set_params(p);
  TEST( "10 > 5", b_thresh_clsfr->classify(10)==1, true );
  TEST( "2 < 5", b_thresh_clsfr->classify(2)==0, true );


  // ie x=<5
  p[0]=-1;
  p[1]=-5;
  b_thresh_clsfr->set_params(p);
  TEST( "10 > 5", b_thresh_clsfr->classify(10)==0, true );
  TEST( "2 < 5", b_thresh_clsfr->classify(2)==1, true );

  // Test loading clsfy_binary_threshold_1d by base class pointer
  
  vcl_cout<<"======== TESTING I/O ==========="<<vcl_endl;
 
   // add binary loaders
  vsl_add_to_binary_loader(clsfy_binary_threshold_1d());
  vcl_string test_path = "test_clsfy_simple_adaboost.bvl.tmp";

  vsl_b_ofstream bfs_out(test_path);
  TEST(("Opened " + test_path + " for writing").c_str(), (!bfs_out ), false);
  vsl_b_write(bfs_out, *b_thresh_clsfr);
  bfs_out.close();

  clsfy_classifier_1d* classifier_in = b_thresh_builder.new_classifier();;

  vsl_b_ifstream bfs_in(test_path);
  TEST(("Opened " + test_path + " for writing").c_str(), (!bfs_out ), false);
  vsl_b_read(bfs_in, *classifier_in);

  bfs_in.close();

  vcl_cout<<"Saved : "<<vcl_endl;
  vcl_cout<< *b_thresh_clsfr << vcl_endl;
  vcl_cout<<"Loaded: "<<vcl_endl;
  vcl_cout<< classifier_in << vcl_endl;

  TEST("saved classifier = loaded classifier",
   b_thresh_clsfr ->params() == classifier_in->params() ,
   true);
  

  delete b_thresh_clsfr;
  delete classifier_in;
 
 
}

TESTMAIN(test_adaboost);
