//:
// \file
// \brief Classifier using adaboost on combinations of simple 1D classifiers
// \author dac

//=======================================================================

#include "clsfy_direct_boost.h"
#include <vcl_string.h>
#include <vcl_iostream.h>
#include <vcl_vector.h>
#include <vcl_cassert.h>
#include <vcl_cmath.h>
#include <vsl/vsl_binary_io.h>
#include <vsl/vsl_binary_loader.h>
#include <vsl/vsl_vector_io.h>
#include <vnl/io/vnl_io_matrix.h>
#include <vnl/io/vnl_io_vector.h>

//=======================================================================
//: Default constructor
clsfy_direct_boost::clsfy_direct_boost()
: n_clfrs_used_(-1) , n_dims_(-1)
{
}

clsfy_direct_boost::clsfy_direct_boost(const clsfy_direct_boost& c)
  : clsfy_classifier_base()
{
  *this = c;
}

//: Copy operator
clsfy_direct_boost& clsfy_direct_boost::operator=(const clsfy_direct_boost& c)
{
  delete_stuff();

  int n = c.classifier_1d_.size();
  classifier_1d_.resize(n);
  for (int i=0;i<n;++i)
    classifier_1d_[i] = c.classifier_1d_[i]->clone();

  threshes_ = c.threshes_;
  wts_ = c.wts_;
  index_ = c.index_;
  return *this;
}

//: Delete objects on heap
void clsfy_direct_boost::delete_stuff()
{
  for (unsigned int i=0;i<classifier_1d_.size();++i)
    delete classifier_1d_[i];

  classifier_1d_.resize(0);

  threshes_.resize(0);
  wts_.resize(0);
  index_.resize(0);
  n_clfrs_used_= -1;
}

//: Destructor
clsfy_direct_boost::~clsfy_direct_boost()
{
  delete_stuff();
}


//: Comparison
bool clsfy_direct_boost::operator==(const clsfy_direct_boost& x) const
{
  if (x.classifier_1d_.size() != classifier_1d_.size() ) return false;
  int n= x.classifier_1d_.size();
  for (int i=0; i<n; ++i)
    if (!(*(x.classifier_1d_[i]) == *(classifier_1d_[i]) )) return false;

  return  x.threshes_ == threshes_ &&
          x.wts_ == wts_ &&
          x.index_ == index_;
}


//: Set parameters.  Clones taken of *classifier[i]
void clsfy_direct_boost::set_parameters(
                      const vcl_vector<clsfy_classifier_1d*>& classifier,
                      const vcl_vector<double>& threshes,
                      const vcl_vector<double>& wts,
                      const vcl_vector<int>& index)
{
  delete_stuff();

  int n = classifier.size();
  classifier_1d_.resize(n);
  for (int i=0;i<n;++i)
    classifier_1d_[i] = classifier[i]->clone();

  threshes_ = threshes;
  wts_ = wts;
  index_= index;
}


//: Clear all wts and classifiers
void clsfy_direct_boost::clear()
{
  delete_stuff();
}


//: Add weak classifier and alpha value
// nb also changes n_clfrs_used to use all current weak classifiers
// nb calc total threshold (ie threshes_ separately, see below)
void clsfy_direct_boost::add_one_classifier(clsfy_classifier_1d* c1d,
                                            double wt,
                                            int index)
{
  classifier_1d_.push_back(c1d->clone());
  wts_.push_back(wt);
  index_.push_back(index);
  n_clfrs_used_=wts_.size();
}


//: Add one threshold
void clsfy_direct_boost::add_one_threshold(double thresh)
{
  threshes_.push_back(thresh);
}


//: Add final threshold
void clsfy_direct_boost::add_final_threshold(double thresh)
{
  int n= threshes_.size();
  threshes_[n-1]= thresh;
}


//: Classify the input vector.
// Returns either 1 (for positive class) or 0 (for negative class)
unsigned clsfy_direct_boost::classify(const vnl_vector<double> &v) const
{
  //vcl_cout<<"wts_.size()= "<<wts_.size()<<vcl_endl
  //        <<"n_clfrs_used_= "<<n_clfrs_used_<<vcl_endl;
  assert ( n_clfrs_used_ >= 0);
  assert ( (unsigned)n_clfrs_used_ <= wts_.size() );
  assert ( n_dims_ >= 0);
  assert ( v.size() == (unsigned)n_dims_ );


  double sum = 0.0;
  for (int i=0;i<n_clfrs_used_;++i)
    sum+= wts_[i]* classifier_1d_[i]->log_l( v[ index_[i] ] );
    //sum += wts_[i]*classifier_1d_[i]->classify(v[ index_[i] ]);

  if (sum < threshes_[n_clfrs_used_-1] ) return 1;
  return 0;
}

//=======================================================================

//: Find the posterior probability of the input being in the positive class.
// The result is is outputs(0)
void clsfy_direct_boost::class_probabilities(vcl_vector<double> &outputs,
  const vnl_vector<double> &input) const
{
  outputs.resize(1);
  outputs[0] = 1.0 / (1.0 + vcl_exp(-log_l(input)));
}

//=======================================================================

//: Log likelihood of being in the positive class.
// Class probability = 1 / (1+exp(-log_l))
double clsfy_direct_boost::log_l(const vnl_vector<double> &v) const
{
  assert ( n_clfrs_used_ >= 0);
  assert ( (unsigned)n_clfrs_used_ <= wts_.size() );
  //assert ( n_dims_ != -1);
  //assert ( v.size() == n_dims_ );
  double sum = 0.0;
  for (int i=0;i<n_clfrs_used_;++i)
    sum+= wts_[i]* classifier_1d_[i]->log_l( v[ index_[i] ] );
    //sum += wts_[i]*classifier_1d_[i]->classify(v[ index_[i] ]);

  return sum;  // this isn't really a log likelihood, because the lower this
               // value the more likely a vector is to be a pos example
}

//=======================================================================

vcl_string clsfy_direct_boost::is_a() const
{
  return vcl_string("clsfy_direct_boost");
}

//=======================================================================

bool clsfy_direct_boost::is_class(vcl_string const& s) const
{
  return s == clsfy_direct_boost::is_a() || clsfy_classifier_base::is_class(s);
}

//=======================================================================

// required if data is present in this class
void clsfy_direct_boost::print_summary(vcl_ostream& os) const
{
  int n = wts_.size();
  assert( wts_.size() == index_.size() );
  os<<vcl_endl;
  for (int i=0;i<n;++i)
  {
    os<<" Weights: "<<wts_[i]
      <<" Index: "<<index_[i]
      <<" Total Threshold: "<<threshes_[i]
      <<" Classifier: "<<classifier_1d_[i]<<vcl_endl;
  }
}

//=======================================================================

short clsfy_direct_boost::version_no() const
{
  return 1;
}

//=======================================================================

void clsfy_direct_boost::b_write(vsl_b_ostream& bfs) const
{
  vsl_b_write(bfs,version_no());
  vsl_b_write(bfs,classifier_1d_);
  vsl_b_write(bfs,threshes_);
  vsl_b_write(bfs,wts_);
  vsl_b_write(bfs,index_);
}

//=======================================================================

void clsfy_direct_boost::b_read(vsl_b_istream& bfs)
{
  if (!bfs) return;

  delete_stuff();

  short version;
  vsl_b_read(bfs,version);
  switch (version)
  {
    case (1):
      vsl_b_read(bfs,classifier_1d_);
      vsl_b_read(bfs,threshes_);
      vsl_b_read(bfs,wts_);
      vsl_b_read(bfs,index_);

      // set default number of classifiers used to be the maximimum number
      n_clfrs_used_= index_.size();

      break;
    default:
      vcl_cerr << "I/O ERROR: clsfy_direct_boost::b_read(vsl_b_istream&)\n"
               << "           Unknown version number "<< version << '\n';
      bfs.is().clear(vcl_ios::badbit); // Set an unrecoverable IO error on stream
  }
}
