//:
// \file
// \brief Describe an abstract classifier of 1D data
// \author Tim Cootes

#include "clsfy_classifier_1d.h"
#include <vcl_iostream.h>
#include <vcl_cassert.h>
#include <vcl_vector.h>
#include <vsl/vsl_indent.h>
#include <vsl/vsl_binary_loader.h>

//=======================================================================

clsfy_classifier_1d::clsfy_classifier_1d()
{
}

//=======================================================================

clsfy_classifier_1d::~clsfy_classifier_1d()
{
}

//=======================================================================

unsigned clsfy_classifier_1d::classify(double input) const
{
  unsigned N = n_classes();

  vcl_vector<double> probs;
  class_probabilities(probs, input);

  if (N == 1) // This is a binary classifier
  {
    if (probs[0] > 0.5)
      return 1u;
    else return 0u;
  }
  else
  {
    unsigned bestIndex = 0;
    unsigned i = 1;
    double bestProb = probs[bestIndex];

    while (i < N)
    {
      if (probs[i] > bestProb)
      {
        bestIndex = i;
        bestProb = probs[i];
      }
      i++;
    }
    return bestIndex;
  }
}

//=======================================================================

void clsfy_classifier_1d::classify_many(vcl_vector<unsigned> &outputs, mbl_data_wrapper<double> &inputs) const
{
  outputs.resize(inputs.size());

  inputs.reset();
  unsigned i=0;

  do
  {
    outputs[i++] = classify(inputs.current());
  } while (inputs.next());
}

//=======================================================================

vcl_string clsfy_classifier_1d::is_a() const
{
  return vcl_string("clsfy_classifier_1d");
}

//=======================================================================

vcl_ostream& operator<<(vcl_ostream& os, clsfy_classifier_1d const& b)
{
  os << b.is_a() << ": ";
  vsl_indent_inc(os);
  b.print_summary(os);
  vsl_indent_dec(os);
  return os;
}

//=======================================================================

vcl_ostream& operator<<(vcl_ostream& os,const clsfy_classifier_1d* b)
{
  if (b)
    return os << *b;
  else
    return os << vsl_indent() << "No clsfy_classifier_1d defined.";
}

//=======================================================================

void vsl_add_to_binary_loader(const clsfy_classifier_1d& b)
{
  vsl_binary_loader<clsfy_classifier_1d>::instance().add(b);
}

//=======================================================================

void vsl_b_write(vsl_b_ostream& os, const clsfy_classifier_1d& b)
{
  b.b_write(os);
}

//=======================================================================

void vsl_b_read(vsl_b_istream& bfs, clsfy_classifier_1d& b)
{
  b.b_read(bfs);
}

//=======================================================================
//: Calculate the fraction of test samples which are classified incorrectly
double clsfy_test_error(const clsfy_classifier_1d &classifier,
                        mbl_data_wrapper<double> & test_inputs,
                        const vcl_vector<unsigned> & test_outputs)
{
  assert(test_inputs.size() == test_outputs.size());

  vcl_vector<unsigned> results;
  classifier.classify_many(results, test_inputs);
  unsigned sum_diff = 0;
  const unsigned n = results.size();
  for (unsigned i=0; i < n; ++i)
    if (results[i] != test_outputs[i]) sum_diff++;
  return ((double) sum_diff) / ((double) n);
}

