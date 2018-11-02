//  Copyright: (C) 2000 British Telecommunications plc

//:
// \file

#include <iostream>
#include <string>
#include <utility>
#include "clsfy_k_nearest_neighbour.h"

#ifdef _MSC_VER
#  include <vcl_msvc_warnings.h>
#endif
#include <cassert>

#include <vnl/vnl_math.h>
#include <vsl/vsl_binary_io.h>
#include <vsl/vsl_vector_io.h>

#include <mbl/mbl_priority_bounded_queue.h>

//: Set the training data.
void clsfy_k_nearest_neighbour::set(const std::vector<vnl_vector<double> > &inputs,
                                    const std::vector<unsigned> &outputs)
{
  assert(inputs.size() == outputs.size());
  trainInputs_ = inputs;
  trainOutputs_ = outputs;
}

// stuff to get the priority queue to work happily
typedef std::pair<double, unsigned> pairDV;
struct first_lt { bool operator()(const pairDV &x, const pairDV &y)
{ return x.first < y.first;} };
struct second_eq_one { bool operator()(const pairDV &x) {return x.second == 1;}};

//: Return the classification of the given probe vector.
unsigned clsfy_k_nearest_neighbour::classify(const vnl_vector<double> &input) const
{
  const unsigned nTrainingVecs = trainInputs_.size();
  const unsigned k = std::min(k_, nTrainingVecs-1 + (nTrainingVecs%2));
  mbl_priority_bounded_queue<pairDV, std::vector<pairDV>, first_lt >  pq(k);
  unsigned i;

  for (i = 0; i < nTrainingVecs; i++)
    pq.push(std::make_pair(vnl_vector_ssd(input, trainInputs_[i]), trainOutputs_[i]));

  unsigned count = 0;
  for (i = 0; i < k; i++)
  {
    count += pq.top().second;
    pq.pop();
  }
  return count *2 > k;
}


//: Return a probability like value that the input being in each class.
// output(i) i<<nClasses, contains the probability that the input
// is in class i;
void clsfy_k_nearest_neighbour::class_probabilities(std::vector<double> &outputs,
                                                    const vnl_vector<double> &input) const
{
  const unsigned nTrainingVecs = trainInputs_.size();
  const unsigned k = std::min(k_, nTrainingVecs-1 + (nTrainingVecs%2));
  mbl_priority_bounded_queue<pairDV, std::vector<pairDV>, first_lt >  pq(k);
  unsigned i;

  for (i = 0; i < nTrainingVecs; i++)
    pq.push(std::make_pair(vnl_vector_ssd(input, trainInputs_[i]), trainOutputs_[i]));

  unsigned count = 0;
  for (i = 0; i < k; i++)
  {
    count += pq.top().second;
    pq.pop();
  }
  outputs.resize(1);
  outputs[0] = ((double)count)/ ((double) k);
}

//: The dimensionality of input vectors.
unsigned clsfy_k_nearest_neighbour::n_dims() const
{
  if (trainInputs_.size() == 0)
    return 0;
  else
    return trainInputs_[0].size();
}


//=======================================================================

//: This value has properties of a Log likelihood of being in class (binary classifiers only)
// class probability = exp(logL) / (1+exp(logL))
double clsfy_k_nearest_neighbour::log_l(const vnl_vector<double> &input) const
{
  std::vector<double> outputs(1);
  class_probabilities(outputs, input);
  double prob = outputs[0];
  return std::log(prob/(1-prob));
}

//=======================================================================

std::string clsfy_k_nearest_neighbour::is_a() const
{
  return std::string("clsfy_k_nearest_neighbour");
}

//=======================================================================

bool clsfy_k_nearest_neighbour::is_class(std::string const& s) const
{
  return s == clsfy_k_nearest_neighbour::is_a() || clsfy_classifier_base::is_class(s);
}

//=======================================================================

short clsfy_k_nearest_neighbour::version_no() const
{
  return 1;
}

//=======================================================================

clsfy_classifier_base* clsfy_k_nearest_neighbour::clone() const
{
  return new clsfy_k_nearest_neighbour(*this);
}

//=======================================================================

void clsfy_k_nearest_neighbour::print_summary(std::ostream& os) const
{
  os << trainInputs_.size() << " training samples, k=" << k_;
}

//=======================================================================

void clsfy_k_nearest_neighbour::b_write(vsl_b_ostream& bfs) const
{
  vsl_b_write(bfs,version_no());
  vsl_b_write(bfs,k_);
  vsl_b_write(bfs,trainOutputs_);
  vsl_b_write(bfs,trainInputs_);
}

//=======================================================================

void clsfy_k_nearest_neighbour::b_read(vsl_b_istream& bfs)
{
  if (!bfs) return;

  short version;
  vsl_b_read(bfs,version);
  switch (version)
  {
  case (1):
    vsl_b_read(bfs,k_);
    vsl_b_read(bfs,trainOutputs_);
    vsl_b_read(bfs,trainInputs_);
    break;
  default:
    std::cerr << "I/O ERROR: clsfy_k_nearest_neighbour::b_read(vsl_b_istream&)\n"
             << "           Unknown version number "<< version << '\n';
    bfs.is().clear(std::ios::badbit); // Set an unrecoverable IO error on stream
    return;
  }
}
