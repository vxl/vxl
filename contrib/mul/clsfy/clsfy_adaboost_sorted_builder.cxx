// This is mul/clsfy/clsfy_adaboost_sorted_builder.cxx
#ifdef VCL_NEEDS_PRAGMA_INTERFACE
#pragma implementation
#endif
//:
// \file
// \brief Functions to train classifiers using AdaBoost algorithm
// \author dac
// \date   Fri Mar  1 23:49:39 2002
//
//  Functions to train classifiers using AdaBoost algorithm
//  AdaBoost combines a set of (usually simple, weak) classifiers into
//  a more powerful single classifier.  Essentially it selects the
//  classifiers one at a time, choosing the best at each step.
//  The classifiers are trained to distinguish the examples mis-classified
//  by the currently selected classifiers.

#include "clsfy_adaboost_sorted_builder.h"
#include "clsfy_simple_adaboost.h"
#include "clsfy_builder_1d.h"

#include <vcl_iostream.h>
#include <vcl_cstdlib.h> // for vcl_abort()
#include <vcl_cmath.h>
#include <vcl_ctime.h> // for clock()
#include <vcl_algorithm.h>
#include <vcl_cassert.h>
#include <vbl/vbl_triple.h>
#include <mbl/mbl_file_data_collector.h>
#include <mbl/mbl_data_collector_list.h>

//=======================================================================

clsfy_adaboost_sorted_builder::clsfy_adaboost_sorted_builder()
: save_data_to_disk_(false), bs_(-1), max_n_clfrs_(-1), weak_builder_(0)
{
}

//=======================================================================

clsfy_adaboost_sorted_builder::~clsfy_adaboost_sorted_builder()
{
}


//=======================================================================

bool clsfy_adaboost_sorted_builder::is_class(vcl_string const& s) const
{
  return s == clsfy_adaboost_sorted_builder::is_a() || clsfy_builder_base::is_class(s);
}

//=======================================================================

vcl_string clsfy_adaboost_sorted_builder::is_a() const
{
  return vcl_string("clsfy_adaboost_sorted_builder");
}


//: Build classifier composed of 1d classifiers working on individual vector elements
// Builds an n-component classifier, each component of which is a 1D classifier
// working on a single element of the input vector.
double clsfy_adaboost_sorted_builder::build(clsfy_classifier_base& model,
                                            mbl_data_wrapper<vnl_vector<double> >& inputs,
                                            unsigned /* nClasses */,
                                            const vcl_vector<unsigned> &outputs) const
{
  // N.B. ignore nClasses=1, i.e. always binary classifier

  assert( model.is_class("clsfy_simple_adaboost") );
  clsfy_simple_adaboost &strong_classifier = (clsfy_simple_adaboost&) model;


  // check parameters are OK
  if ( max_n_clfrs_ < 0 )
  {
    vcl_cout<<"Error: clsfy_adaboost_sorted_builder::build\n"
            <<"max_n_clfrs_ = "<<max_n_clfrs_<<" ie < 0\n"
            <<"set using set_max_n_clfrs()\n";
    vcl_abort();
  }
  else
  {
    vcl_cout<<"Maximum number of classifiers to be found by Adaboost = "
            <<max_n_clfrs_<<'\n';
  }

  if ( weak_builder_ == 0 )
  {
    vcl_cout<<"Error: clsfy_adaboost_sorted_builder::build\n"
            <<"weak_builder_ pointer has not been set\n"
            <<"need to provide a builder to build each weak classifier\n"
            <<"set using set_weak_builder()\n";
    vcl_abort();
  }
  else
  {
    vcl_cout<<"Weak learner used by AdaBoost = "
            <<weak_builder_->is_a()<<'\n';
  }

  if ( bs_ < 0 )
  {
    vcl_cout<<"Error: clsfy_adaboost_sorted_builder::build\n"
            <<"bs_ = "<<bs_<<" ie < 0\n"
            <<"set using set_batch_size()\n";
    vcl_abort();
  }
  else
  {
    vcl_cout<<"Batch size when sorting data =" <<bs_<<'\n';
  }


  assert(bs_>0);
  assert(bs_!=1);
  assert (max_n_clfrs_ >= 0);

  // first arrange the data in the form
  // vcl_vector< < vcl_vector< vtl_triple<double,int,int> > > > data
  // + vnl_vector wts
  // then sort all data once, then build the classifier

  // number of examples
  unsigned n= inputs.size();
  //vcl_cout<<"n= "<<n<<'\n';

  // Dimensionality of data
  inputs.reset();
  int d = inputs.current().size();

  //need file data wrapper instead of old vector
  //data stored on disk NOT ram
  //vcl_vector< vcl_vector<vbl_triple<double,int,int> > > data(d);

  vcl_string temp_path= "temp.dat";
  mbl_file_data_collector<
      vcl_vector< vbl_triple<double,int,int> >
                         >
              file_collector( temp_path );

  mbl_data_collector_list<
      vcl_vector< vbl_triple<double,int,int> >
                         >
              ram_collector;

  mbl_data_collector<vcl_vector< vbl_triple<double,int,int> >
                         >*  collector;

  if (save_data_to_disk_)
  {
    vcl_cout<<"saving data to disk!\n";
    collector= &file_collector;
  }
  else
  {
    //bs_ = n ;
    vcl_cout<<"saving data to ram!\n";
    collector= &ram_collector;
  }

  // perhaps change this so load and sort several vectors at once!!
  // far too slow at present
  // say load in and sort 100 at once?????
  // i.e. 100 features at once!

  //int bs= 100; //batch size
  vcl_vector< vcl_vector< vbl_triple<double,int,int> > >vec(bs_);
  vbl_triple<double,int,int> t;

  vcl_cout<<"d= "<<d<<'\n';
  int b=0;
  while ( b+1<d )
  {
    int r= vcl_min ( bs_, (d-b) );
    assert(r>0);

    vcl_cout<<"sorting weak classifiers = "<<b<<" to "
            <<(b+r)-1<<" of "<<d<<'\n';


    // have to resize all vectors
    for (int i=0; i< bs_; ++i)
      vec[i].resize(0);

    // add data for both classes
    inputs.reset();
    for (unsigned int j=0;j<n;++j)
    {
      for (int i=0; i< r; ++i)
      {
        t.first=inputs.current()[b+i];
        t.second=outputs[j];
        t.third = j;
        vec[i].push_back(t);
      }
      inputs.next();
    }


    for (int i=0; i< r; ++i)
    {
      // sort training data for each individual weak classifier
      assert (vec[i].size() == n);
      assert (n != 0);

      vcl_sort(vec[i].begin(), vec[i].end() );

      // store sorted vector of responses for each individual weak classifier
      collector->record(vec[i]);
    }

    b+=bs_;
  }


  mbl_data_wrapper< vcl_vector< vbl_triple<double,int,int> > >&
              wrapper=collector->data_wrapper();


  // now actually apply AdaBoost algorithm
  wrapper.reset();
  assert ( wrapper.current().size() == n );
  assert ( d == (int)wrapper.size() );


   // initialize weights
  unsigned n0=0;
  unsigned n1=0;
  for (unsigned int i=0;i<n;++i)
  {
    if ( outputs[i] == 0 )
      n0++;
    else if ( outputs[i] == 1 )
      n1++;
    else
    {
      vcl_cout<<"Error : clsfy_adaboost_sorted_builder\n"
              <<"unrecognised output value : outputs["<<i<<"]= "
              <<outputs[i]<<'\n';
      vcl_abort();
    }
  }

  assert (n0+n1==n );

  vnl_vector<double> wts(n);
  for (unsigned int i=0; i<n; ++i)
  {
    if ( outputs[i] == 0 )
      wts(i)=0.5/n0;
    else if ( outputs[i] == 1 )
      wts(i)=0.5/n1;
    else
    {
      vcl_cout<<"Error : clsfy_adaboost_sorted_builder\n"
              <<"unrecognised output value : outputs["<<i<<"]= "
              <<outputs[i]<<'\n';
      vcl_abort();
    }
  }


  // clear classifier
  // N.B. maybe shouldn't do this if going to build incrementally
  // i.e. by rebuilding the training set from false positives of the
  // current classifier
  strong_classifier.clear();
  strong_classifier.set_n_dims(d);
  // N.B. have to set builder as a member variable elsewhere
  clsfy_classifier_1d* c1d = weak_builder_->new_classifier();
  clsfy_classifier_1d* best_c1d= weak_builder_->new_classifier();

  double beta, alpha;
  long old_time = vcl_clock();
  double tot_time=0;

  for (unsigned int r=0;r<(unsigned)max_n_clfrs_;++r)
  {
    vcl_cout<<"adaboost training round = "<<r<<'\n';

    //vcl_cout<<"wts0= "<<wts0<<"\nwts1= "<<wts1<<'\n';

    int best_i=-1;
    double min_error= 100000;
    wrapper.reset();  // make sure pointing to first data vector
    for (int i=0;i<d;++i)
    {
      const vcl_vector< vbl_triple<double,int,int> >& vec = wrapper.current();

      double error = weak_builder_->build_from_sorted_data(*c1d,&vec[0],wts);
      if (i==0 || error<min_error)
      {
        min_error = error;
        delete best_c1d;
        best_c1d= c1d->clone();
        best_i = i;
      }

      wrapper.next();   // move to next data vector
    }

    assert(best_i != -1);

    vcl_cout<<"best_i= "<<best_i<<'\n'
            <<"min_error= "<<min_error<<'\n';

    if (min_error<1e-10)  // Hooray!
    {
      vcl_cout<<"min_error<1e-10 !!!\n";
      alpha  = vcl_log(2.0*n);   //is this appropriate???
      strong_classifier.add_classifier( best_c1d, alpha, best_i);

      // delete classifiers on heap, because clones taken by strong_classifier
      delete c1d;
      delete best_c1d;
      return clsfy_test_error(strong_classifier, inputs, outputs);
    }


    if (0.5-min_error<1e-10) // Oh dear, no further improvement possible
    {
      vcl_cout<<"min_error => 0.5 !!!\n";
      beta=1.0;

      // delete classifiers on heap, because clones taken by strong_classifier
      delete c1d;
      delete best_c1d;
      return clsfy_test_error(strong_classifier, inputs, outputs);
    }

    // update the classifier
    beta = min_error/(1.0-min_error);
    alpha  = -1.0*vcl_log(beta);
    strong_classifier.add_classifier( best_c1d, alpha, best_i);


    // extract the best weak classifier results
    wrapper.set_index(best_i);
    const vcl_vector< vbl_triple<double,int,int> >& vec = wrapper.current();

    // update the wts using the best weak classifier
    for (unsigned int j=0;j<n;++j)
      if ( best_c1d-> classify( vec[j].first )
            ==
           (unsigned) vec[j].second
         )
      wts[vec[j].third]*=beta;

    double w_sum= wts.mean()*n;
    wts/=w_sum;

    long new_time = vcl_clock();

    double dt = (1.0*(new_time-old_time))/CLOCKS_PER_SEC;
    vcl_cout<<"Time for AdaBoost round:      "<<dt<<" secs\n";
    tot_time+=dt;
    vcl_cout<<"Total time for rounds so far: "<<tot_time<<" secs\n";

    old_time = new_time;
  }

  delete c1d;
  delete best_c1d;

  // does clsfy_test_error balk if have too much data?
  // should be OK because just passes mbl_data_wrapper and evaluates
  // one at a time, so if using mbl_file_data_wrapper should be OK!
  vcl_cout<<"calculating training error\n";
  return clsfy_test_error(strong_classifier, inputs, outputs);
}


//: Create empty classifier
// Caller is responsible for deletion
clsfy_classifier_base* clsfy_adaboost_sorted_builder::new_classifier() const
{
  return new clsfy_simple_adaboost();
}


//=======================================================================

#if 0
    // required if data stored on the heap is present in this derived class
clsfy_adaboost_sorted_builder::clsfy_adaboost_sorted_builder(const clsfy_adaboost_sorted_builder& new_b):
  data_ptr_(0)
{
  *this = new_b;
}


//=======================================================================

    // required if data stored on the heap is present in this derived class
clsfy_adaboost_sorted_builder& clsfy_adaboost_sorted_builder::operator=(const clsfy_adaboost_sorted_builder& new_b)
{
  if (&new_b==this) return *this;

  // Copy heap member variables.
  delete data_ptr_; data_ptr_=0;

  if (new_b.data_ptr_)
    data_ptr_ = new_b.data_ptr_->clone();

  // Copy normal member variables
  data_ = new_b.data_;

  return *this;
}
#endif // 0

//=======================================================================

clsfy_builder_base* clsfy_adaboost_sorted_builder::clone() const
{
  return new clsfy_adaboost_sorted_builder(*this);
}

//=======================================================================

    // required if data is present in this base class
void clsfy_adaboost_sorted_builder::print_summary(vcl_ostream& /*os*/) const
{
#if 0
  clsfy_builder_base::print_summary(os); // Uncomment this line if it has one.
  vsl_print_summary(os, data_); // Example of data output
#endif

  vcl_cerr << "clsfy_adaboost_sorted_builder::print_summary() NYI\n";
}

//=======================================================================

  // required if data is present in this base class
void clsfy_adaboost_sorted_builder::b_write(vsl_b_ostream& /*bfs*/) const
{
#if 0
  vsl_b_write(bfs, version_no());
  clsfy_builder_base::b_write(bfs);  // Needed if base has any data
  vsl_b_write(bfs, data_);
#endif
  vcl_cerr << "clsfy_adaboost_sorted_builder::b_write() NYI\n";
}

//=======================================================================

  // required if data is present in this base class
void clsfy_adaboost_sorted_builder::b_read(vsl_b_istream& /*bfs*/)
{
  vcl_cerr << "clsfy_adaboost_sorted_builder::b_read() NYI\n";
#if 0
  if (!bfs) return;

  short version;
  vsl_b_read(bfs,version);
  switch (version)
  {
  case (1):
    //clsfy_builder_base::b_read(bfs);  // Needed if base has any data
    vsl_b_read(bfs,data_);
    break;
  default:
    vcl_cerr << "I/O ERROR: vsl_b_read(vsl_b_istream&, clsfy_adaboost_sorted_builder&) \n"
             << "           Unknown version number "<< version << "\n";
    bfs.is().clear(vcl_ios::badbit); // Set an unrecoverable IO error on stream
    return;
  }
#endif // 0
}
