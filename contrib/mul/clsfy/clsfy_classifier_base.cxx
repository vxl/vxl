// Copyright: (C) 2000 British Telecommunications plc

//:
// \file
// \brief Implement bits of an abstract classifier
// \author Ian Scott
// \date 2000/05/10
// \verbatim
//  Modifications
//  2 May 2001 IMS Converted to VXL
// \endverbatim

#include "clsfy_classifier_base.h"
#include <vcl_iostream.h>
#include <vcl_cassert.h>
#include <vcl_vector.h>
#include <vsl/vsl_indent.h>
#include <vsl/vsl_binary_loader.h>

//=======================================================================

clsfy_classifier_base::clsfy_classifier_base()
{
}

//=======================================================================

clsfy_classifier_base::~clsfy_classifier_base()
{
}

//=======================================================================

unsigned clsfy_classifier_base::classify(const vnl_vector<double> &input) const
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

void clsfy_classifier_base::classify_many(vcl_vector<unsigned> &outputs, mbl_data_wrapper<vnl_vector<double> > &inputs) const
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

vcl_string clsfy_classifier_base::is_a() const
{
  return vcl_string("clsfy_classifier_base");
}

//=======================================================================

bool clsfy_classifier_base::is_class(vcl_string const& s) const
{
  return s == clsfy_classifier_base::is_a();
}

//=======================================================================

vcl_ostream& operator<<(vcl_ostream& os, clsfy_classifier_base const& b)
{
  os << b.is_a() << ": ";
  vsl_indent_inc(os);
  b.print_summary(os);
  vsl_indent_dec(os);
  return os;
}

//=======================================================================

vcl_ostream& operator<<(vcl_ostream& os,const clsfy_classifier_base* b)
{
  if (b)
    return os << *b;
  else
    return os << vsl_indent() << "No clsfy_classifier_base defined.";
}

//=======================================================================

void vsl_add_to_binary_loader(const clsfy_classifier_base& b)
{
  vsl_binary_loader<clsfy_classifier_base>::instance().add(b);
}

//=======================================================================

void vsl_b_write(vsl_b_ostream& os, const clsfy_classifier_base& b)
{
  b.b_write(os);
}

//=======================================================================

void vsl_b_read(vsl_b_istream& bfs, clsfy_classifier_base& b)
{
  b.b_read(bfs);
}

//=======================================================================
//: Calculate the fraction of test samples which are classified incorrectly
double clsfy_test_error(const clsfy_classifier_base &classifier,
                        mbl_data_wrapper<vnl_vector<double> > & test_inputs,
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

//=======================================================================
//: Calculate the fraction of test samples of a particular class which are classified incorrectly
// \return -1 if there are no samples of test_class. 
double clsfy_test_error(const clsfy_classifier_base &classifier,
                        mbl_data_wrapper<vnl_vector<double> > & test_inputs,
                        const vcl_vector<unsigned> & test_outputs,
                        unsigned test_class)
{
  assert(test_inputs.size() == test_outputs.size());
  if (test_inputs.size()==0) return -1;
  test_inputs.reset();
  unsigned n_class=0, n_bad=0, i=0;
  do
  {
    if (test_outputs[i] == test_class)
    {
      if (test_outputs[i] != classifier.classify(test_inputs.current()))
        n_bad ++;
      n_class ++;
    }
    i++;
  } while (test_inputs.next());

  if (n_class==0) return -1.0;
  return ((double) n_bad) / ((double) n_class);
}

