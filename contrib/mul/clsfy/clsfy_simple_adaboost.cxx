//:
// \file
// \brief Classifier using adaboost on combinations of simple 1D classifiers
// \author Tim Cootes

//=======================================================================

#include "clsfy_simple_adaboost.h"
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
clsfy_simple_adaboost::clsfy_simple_adaboost()
: n_clfrs_used_(-1) , n_dims_(-1)
{
}

clsfy_simple_adaboost::clsfy_simple_adaboost(const clsfy_simple_adaboost& c)
  : clsfy_classifier_base()
{
  *this = c;
}

//: Copy operator
clsfy_simple_adaboost& clsfy_simple_adaboost::operator=(const clsfy_simple_adaboost& c)
{
  delete_stuff();

  int n = c.classifier_1d_.size();
  classifier_1d_.resize(n);
  for (int i=0;i<n;++i)
    classifier_1d_[i] = c.classifier_1d_[i]->clone();

  alphas_ = c.alphas_;
  index_ = c.index_;
  return *this;
}

//: Delete objects on heap
void clsfy_simple_adaboost::delete_stuff()
{
  for (unsigned int i=0;i<classifier_1d_.size();++i)
    delete classifier_1d_[i];

  classifier_1d_.resize(0);

  alphas_.resize(0);
  index_.resize(0);
  n_clfrs_used_= -1;
}

//: Destructor
clsfy_simple_adaboost::~clsfy_simple_adaboost()
{
  delete_stuff();
}


//: Comparison
bool clsfy_simple_adaboost::operator==(const clsfy_simple_adaboost& x) const
{
  if (x.classifier_1d_.size() != classifier_1d_.size() ) return false;
  int n= x.classifier_1d_.size();
  for (int i=0; i<n; ++i)
    if (!(*(x.classifier_1d_[i]) == *(classifier_1d_[i]) )) return false;

  return x.alphas_ == alphas_ &&
         x.index_ == index_;
}


//: Set parameters.  Clones taken of *classifier[i]
void clsfy_simple_adaboost::set_parameters(
                      const vcl_vector<clsfy_classifier_1d*>& classifier,
                      const vcl_vector<double>& alphas,
                      const vcl_vector<int>& index)
{
  delete_stuff();

  int n = classifier.size();
  classifier_1d_.resize(n);
  for (int i=0;i<n;++i)
    classifier_1d_[i] = classifier[i]->clone();

  alphas_ = alphas;
  index_= index;
}


//: Clear all alphas and classifiers
void clsfy_simple_adaboost::clear()
{
  delete_stuff();
}


//: Add weak classifier and alpha value
// nb also changes n_clfrs_used to use all current weak classifiers
void clsfy_simple_adaboost::add_classifier(clsfy_classifier_1d* c1d,
                                           double alpha,
                                           int index)
{
  classifier_1d_.push_back(c1d->clone());
  alphas_.push_back(alpha);
  index_.push_back(index);
  n_clfrs_used_=alphas_.size();
}


//: Classify the input vector.
// Returns either 1 (for positive class) or 0 (for negative class)
unsigned clsfy_simple_adaboost::classify(const vnl_vector<double> &v) const
{
  //vcl_cout<<"alphas_.size()= "<<alphas_.size()<<vcl_endl;
  //vcl_cout<<"n_clfrs_used_= "<<n_clfrs_used_<<vcl_endl;
  assert ( n_clfrs_used_ >= 0);
  assert ( (unsigned)n_clfrs_used_ <= alphas_.size() );
  assert ( n_dims_ >= 0);
  assert ( v.size() == (unsigned)n_dims_ );


  double sum1 = 0.0, sum2 =0.0;
  for (int i=0;i<n_clfrs_used_;++i)
  {
    //vcl_cout<<"index_["<<i<<"]= "<<index_[i]<<vcl_endl;
    sum1 += alphas_[i]*classifier_1d_[i]->classify(v[ index_[i] ]);
    sum2 += alphas_[i];
  }

  if (sum1 - sum2 * 0.5 > 0.0) return 1;
  return 0;
}

//=======================================================================

//: Find the posterior probability of the input being in the positive class.
// The result is outputs(0)
void clsfy_simple_adaboost::class_probabilities(vcl_vector<double> &outputs,
  const vnl_vector<double> &input) const
{
  outputs.resize(1);
  outputs[0] = 1.0 / (1.0 + vcl_exp(-log_l(input)));
}

//=======================================================================

//: Log likelihood of being in the positive class.
// Class probability = 1 / (1+exp(-log_l))
double clsfy_simple_adaboost::log_l(const vnl_vector<double> &v) const
{
  assert ( n_clfrs_used_ >= 0);
  assert ( (unsigned)n_clfrs_used_ <= alphas_.size() );
  //assert ( n_dims_ != -1);
  //assert ( v.size() == n_dims_ );
  double sum1 = 0.0, sum2= 0.0;
  for (int i=0;i<n_clfrs_used_;++i)
  {
    sum1 += alphas_[i]*classifier_1d_[i]->classify(v[ index_[i] ]);
    sum2 += alphas_[i];
  }

  return sum1 - sum2 * 0.5;
}

//=======================================================================

vcl_string clsfy_simple_adaboost::is_a() const
{
  return vcl_string("clsfy_simple_adaboost");
}

//=======================================================================

bool clsfy_simple_adaboost::is_class(vcl_string const& s) const
{
  return s == clsfy_simple_adaboost::is_a() || clsfy_classifier_base::is_class(s);
}

//=======================================================================

// required if data is present in this class
void clsfy_simple_adaboost::print_summary(vcl_ostream& os) const
{
  int n = alphas_.size();
  assert( alphas_.size() == index_.size() );
  os<<vcl_endl;
  for (int i=0;i<n;++i)
  {
    os<<" Alpha: "<<alphas_[i]
      <<" Index: "<<index_[i]
      <<" Classifier: "<<classifier_1d_[i]<<vcl_endl;
  }
}

//=======================================================================

short clsfy_simple_adaboost::version_no() const
{
  return 1;
}

//=======================================================================

void clsfy_simple_adaboost::b_write(vsl_b_ostream& bfs) const
{
  vsl_b_write(bfs,version_no());
  vsl_b_write(bfs,classifier_1d_);
  vsl_b_write(bfs,alphas_);
  vsl_b_write(bfs,index_);
}

//=======================================================================

void clsfy_simple_adaboost::b_read(vsl_b_istream& bfs)
{
  if (!bfs) return;

  delete_stuff();

  short version;
  vsl_b_read(bfs,version);
  switch (version)
  {
    case (1):
      vsl_b_read(bfs,classifier_1d_);
      vsl_b_read(bfs,alphas_);
      vsl_b_read(bfs,index_);

      // set default number of classifiers used to be the maximum number
      n_clfrs_used_= index_.size();

      break;
    default:
      vcl_cerr << "I/O ERROR: clsfy_simple_adaboost::b_read(vsl_b_istream&)\n"
               << "           Unknown version number "<< version << '\n';
      bfs.is().clear(vcl_ios::badbit); // Set an unrecoverable IO error on stream
  }
}
