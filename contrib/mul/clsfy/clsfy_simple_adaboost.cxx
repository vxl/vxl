//:
// \file
// \brief Classifier using adaboost on combinations of simple 1D classifiers
// \author Tim Cootes

//=======================================================================

#include <vcl_string.h>
#include <vcl_iostream.h>
#include <vcl_vector.h>
#include <vnl/io/vnl_io_vector.h>
#include <mbl/mbl_matxvec.h>
#include <mbl/mbl_matrix_products.h>
#include <vcl_cassert.h>
#include <vcl_cmath.h>
#include <clsfy/clsfy_simple_adaboost.h>
#include <vnl/io/vnl_io_vector.h>
#include <vnl/io/vnl_io_matrix.h>
#include <vsl/vsl_binary_io.h>
#include <vnl/io/vnl_io_vector.h>
#include <vsl/vsl_binary_loader.h>
#include <vsl/vsl_vector_io.h>

//=======================================================================
//: Default constructor
clsfy_simple_adaboost::clsfy_simple_adaboost()
{
}

clsfy_simple_adaboost::clsfy_simple_adaboost(const clsfy_simple_adaboost& c)
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

  wts_ = c.wts_;
  bias_ = c.bias_;
  index_ = c.index_;
  n_dims_ = c.n_dims_;

  return *this;
}

//: Delete objects on heap
void clsfy_simple_adaboost::delete_stuff()
{
  for (int i=0;i<classifier_1d_.size();++i)
    delete classifier_1d_[i];

  classifier_1d_.resize(0);
}

//: Destructor
clsfy_simple_adaboost::~clsfy_simple_adaboost()
{
  delete_stuff();
}

//: Set parameters.  Clones taken of *classifier[i]
void clsfy_simple_adaboost::set_parameters(
                      const vcl_vector<clsfy_classifier_1d*>& classifier,
                      const vcl_vector<vnl_vector<double> >& params,
                      const vnl_vector<double>& wts,
                      const vcl_vector<int>& index, int n_dims)
{
  delete_stuff();

  int n = classifier.size();
  classifier_1d_.resize(n);
  for (int i=0;i<n;++i)
    classifier_1d_[i] = classifier[i]->clone();

  wts_ = wts;
  bias_ = 0.5*wts.mean()*wts.size();
  index_ = index;
  n_dims_ = n_dims;
}


//: Classify the input vector
// Returns either 0 (for negative class) or 1 (for positive class)
unsigned clsfy_simple_adaboost::classify(const vnl_vector<double> &v) const
{
  int n = index_.size();
  double sum = 0.0;
  for (int i=0;i<n;++i)
    sum += wts_[i]*classifier_1d_[i]->classify(v[index_[i]]);

  if (sum<bias_) return 0;
  return 1;
}


//=======================================================================

//: Find the posterior probability of the input being in the positive class.
// The result is is outputs(0)
void clsfy_simple_adaboost::class_probabilities(vcl_vector<double> &outputs,
  const vnl_vector<double> &input) const
{
  outputs.resize(1);
  outputs[0] = 1.0 / (1.0 + vcl_exp(-log_l(input)));
}


//=======================================================================

//: Log likelyhood of being in the positive class
// class probability = 1 / (1+exp(-log_l))
double clsfy_simple_adaboost::log_l(const vnl_vector<double> &v) const
{
  int n = index_.size();
  double sum = 0.0;
  for (int i=0;i<n;++i)
    sum += wts_[i]*classifier_1d_[i]->classify(v[index_[i]]);

  return sum - bias_;
}


//=======================================================================


vcl_string clsfy_simple_adaboost::is_a() const
{
  return vcl_string("clsfy_simple_adaboost");
}

//=======================================================================

bool clsfy_simple_adaboost::is_class(vcl_string const& s) const
{
  static const vcl_string s_ = "clsfy_simple_adaboost";
  return s == s_ || clsfy_classifier_base::is_class(s);
}

//=======================================================================

// required if data is present in this class
void clsfy_simple_adaboost::print_summary(vcl_ostream& os) const
{
  int n = index_.size();
  for (int i=0;i<n;++i)
  {
    os<<"Element "<<index_[i]<<" weight: "<<wts_[i]
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
  vsl_b_write(bfs,wts_);
  vsl_b_write(bfs,index_);
  vsl_b_write(bfs,bias_);
  vsl_b_write(bfs,n_dims_);
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
      vsl_b_read(bfs,wts_);
      vsl_b_read(bfs,index_);
      vsl_b_read(bfs,bias_);
      vsl_b_read(bfs,n_dims_);
      break;
    default:
      vcl_cerr << "I/O ERROR: clsfy_simple_adaboost::b_read(vsl_b_istream&)\n"
               << "           Unknown version number "<< version << "\n";
      bfs.is().clear(vcl_ios::badbit); // Set an unrecoverable IO error on stream
  }
}

