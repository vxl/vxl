//:
// \file
// \author
//   Copyright: (C) 2000 British Telecommunications plc

#include "clsfy_rbf_parzen.h"

#include <vcl_string.h>
#include <vcl_algorithm.h>

#include <vcl_cassert.h>
#include <vsl/vsl_binary_io.h>
#include <vsl/vsl_vector_io.h>
#include <vnl/io/vnl_io_vector.h>

//=======================================================================
//: Return the classification of the given probe vector.
unsigned clsfy_rbf_parzen::classify(const vnl_vector<double> &input) const
{
  const unsigned nTrainingVecs = trainInputs_.size();
  double sumWeightings=0.0, sumPredictions=0.0, weight;

  if (power_ == 2) // optimise common case
    for (unsigned i = 0; i < nTrainingVecs; i++)
    {
      weight = vcl_exp(gamma_ * vnl_vector_ssd(input, trainInputs_[i]));

      sumWeightings += weight;
      sumPredictions += weight * trainOutputs_[i];
    }
  else
  {
    double gamma = - 0.5 * vcl_pow(-2*gamma_, 0.5*power_);
    double p = power_ / 2.0;
    for (unsigned i = 0; i < nTrainingVecs; i++)
    {
      weight = vcl_exp(gamma * vcl_pow(vnl_vector_ssd(input, trainInputs_[i]), p) );

      sumWeightings += weight;
      sumPredictions += weight * trainOutputs_[i];
    }
  }
  return sumPredictions * 2 > sumWeightings ? 1 : 0;
}

//=======================================================================
//: Set the training data.
void clsfy_rbf_parzen::set(const vcl_vector<vnl_vector<double> > &inputs,
  const vcl_vector<unsigned> &outputs)
{
  assert(*vcl_max_element(outputs.begin(), outputs.end()) <= 1); // The class labels must be 0 or 1.
  assert(inputs.size() == outputs.size());
  trainInputs_ = inputs;
  trainOutputs_ = outputs;
}

//=======================================================================
//: Return a probability like value that the input being in each class.
// output(i) i<<nClasses, contains the probability that the input is in class i
void clsfy_rbf_parzen::class_probabilities(vcl_vector<double> &outputs, const vnl_vector<double> &input) const
{
  const unsigned nTrainingVecs = trainInputs_.size();
  double sumWeightings=0.0, sumPredictions=0.0, weight;

  if (power_ == 2) // optimise common case
    for (unsigned i = 0; i < nTrainingVecs; i++)
    {
      weight = vcl_exp(gamma_ * vnl_vector_ssd(input, trainInputs_[i]));

      sumWeightings += weight;
      sumPredictions += weight * trainOutputs_[i];
    }
  else
  {
    double gamma = - 0.5 * vcl_pow(-2*gamma_, 0.5*power_);
    double p = power_ / 2.0;
    for (unsigned i = 0; i < nTrainingVecs; i++)
    {
      weight = vcl_exp(gamma * vcl_pow(vnl_vector_ssd(input, trainInputs_[i]), p) );

      sumWeightings += weight;
      sumPredictions += weight * trainOutputs_[i];
    }
  }
  outputs.resize(1);
  outputs[0] = sumPredictions / sumWeightings;
}

//=======================================================================
//: Return the number of training vectors weighted by the windowing function.
double clsfy_rbf_parzen::weightings(const vnl_vector<double> &input) const
{
  const unsigned nTrainingVecs = trainInputs_.size();
  double sumWeightings=0.0;

  if (power_ == 2) // optimise common case
    for (unsigned i = 0; i < nTrainingVecs; i++)
      sumWeightings += vcl_exp(gamma_ * vnl_vector_ssd(input, trainInputs_[i]));
  else
  {
    double gamma = - 0.5 * vcl_pow(-2*gamma_, 0.5*power_);
    double p = power_ / 2.0;
    for (unsigned i = 0; i < nTrainingVecs; i++)
      sumWeightings += vcl_exp(gamma * vcl_pow(vnl_vector_ssd(input, trainInputs_[i]), p) );
  }

  return sumWeightings;
}

//=======================================================================
//: The dimensionality of input vectors.
unsigned clsfy_rbf_parzen::n_dims() const
{
  if (trainInputs_.size() == 0)
    return 0;
  else
    return trainInputs_[0].size();
}

//=======================================================================
//: This value has properties of a Log likelyhood of being in class (binary classifiers only)
// class probability = exp(logL) / (1+exp(logL))
double clsfy_rbf_parzen::log_l(const vnl_vector<double> &input) const
{
  vcl_vector<double> outputs(1);
  class_probabilities(outputs, input);
  double prob = outputs[0];
  return vcl_log(prob/(1-prob));
}

//=======================================================================
//: Set the 1st standard deviation width of the RBF window.
// The default value is 1. Really this could be better named as the RBF radius.
void clsfy_rbf_parzen::set_rbf_width(double sigma)
{
  assert(sigma > 0.0);
  width_ = sigma;
  gamma_ = -0.5/(sigma*sigma);
}


//=======================================================================
//: The value p in the window function exp(-1/(2*sigma^p) * |x-y|^p).
// The value p affects the kurtosis, or peakyness of the window.
// Towards 0 gives a more peaked central spike, and longer tail.
// Toward +inf gives a broader peak, and shorter tail.
// The default value is 2, giving a Gaussian distribution.
void clsfy_rbf_parzen::set_power(double p)
{
  assert(p > 0.0);
  power_ = p;
}


//=======================================================================

vcl_string clsfy_rbf_parzen::is_a() const
{
  return vcl_string("clsfy_rbf_parzen");
}

//=======================================================================

bool clsfy_rbf_parzen::is_class(vcl_string const& s) const
{
  return s == clsfy_rbf_parzen::is_a() || clsfy_classifier_base::is_class(s);
}

//=======================================================================

short clsfy_rbf_parzen::version_no() const
{
  return 2;
}

//=======================================================================

clsfy_classifier_base* clsfy_rbf_parzen::clone() const
{
  return new clsfy_rbf_parzen(*this);
}

//=======================================================================

void clsfy_rbf_parzen::print_summary(vcl_ostream& os) const
{
  os << trainInputs_.size() << " training samples, "
     << "Gaussian Width=" << rbf_width() << ", power=" << power_;
}

//=======================================================================

void clsfy_rbf_parzen::b_write(vsl_b_ostream& bfs) const
{
  vsl_b_write(bfs,version_no());
  vsl_b_write(bfs,width_);
  vsl_b_write(bfs,power_);
  vsl_b_write(bfs,trainOutputs_);
  vsl_b_write(bfs,trainInputs_);
}

//=======================================================================

void clsfy_rbf_parzen::b_read(vsl_b_istream& bfs)
{
  if (!bfs) return;

  short version;
  vsl_b_read(bfs,version);
  switch (version)
  {
  case (1):
    vsl_b_read(bfs,gamma_);
    width_ = vcl_sqrt(-0.5/gamma_);
    vsl_b_read(bfs,power_);
    vsl_b_read(bfs,trainOutputs_);
    vsl_b_read(bfs,trainInputs_);
    break;
  case(2):
    vsl_b_read(bfs,width_);
    gamma_ = -0.5/(width_*width_);
    vsl_b_read(bfs,power_);
    vsl_b_read(bfs,trainOutputs_);
    vsl_b_read(bfs,trainInputs_);
    break;
  default:
    vcl_cerr << "I/O ERROR: clsfy_rbf_parzen::b_read(vsl_b_istream&)\n"
             << "           Unknown version number "<< version << "\n";
    bfs.is().clear(vcl_ios::badbit); // Set an unrecoverable IO error on stream
  }
}
