// This is mul/clsfy/clsfy_knn_builder.cxx
// Copyright (c) 2001: British Telecommunications plc
#ifdef VCL_NEEDS_PRAGMA_INTERFACE
#pragma implementation
#endif
//:
// \file
// \brief Implement a knn classifier builder
// \author Ian Scott
// \date 2001/10/07

#include "clsfy_knn_builder.h"

#include <vcl_iostream.h>
#include <vcl_string.h>
#include <vcl_cassert.h>
#include <vsl/vsl_binary_loader.h>
#include <clsfy/clsfy_k_nearest_neighbour.h>

//=======================================================================

clsfy_knn_builder::clsfy_knn_builder():
k_(1)
{
}


//=======================================================================

short clsfy_knn_builder::version_no() const
{
  return 1;
}

//=======================================================================

vcl_string clsfy_knn_builder::is_a() const
{
  return vcl_string("clsfy_knn_builder");
}

//=======================================================================

bool clsfy_knn_builder::is_class(vcl_string const& s) const
{
  return s == clsfy_knn_builder::is_a() || clsfy_builder_base::is_class(s);
}

//=======================================================================

clsfy_builder_base* clsfy_knn_builder::clone() const
{
  return new clsfy_knn_builder(*this);
}

//=======================================================================

void clsfy_knn_builder::print_summary(vcl_ostream& os) const
{
  os << "k = " << k_;
}

//=======================================================================

void clsfy_knn_builder::b_write(vsl_b_ostream& bfs) const
{
  vsl_b_write(bfs, version_no());
  vsl_b_write(bfs, k_);
  vcl_cerr << "clsfy_knn_builder::b_write() NYI\n";
}

//=======================================================================

void clsfy_knn_builder::b_read(vsl_b_istream& bfs)
{
  if (!bfs) return;

  short version;
  vsl_b_read(bfs,version);
  switch (version)
  {
  case (1):
    vsl_b_read(bfs, k_);
    break;
  default:
    vcl_cerr << "I/O ERROR: vsl_b_read(vsl_b_istream&, clsfy_knn_builder&)\n"
             << "           Unknown version number "<< version << "\n";
    bfs.is().clear(vcl_ios::badbit); // Set an unrecoverable IO error on stream
  }
}

//=======================================================================

//: Build model from data
// return the mean error over the training set.
// For many classifiers, you may use nClasses==1 to
// indicate a binary classifier
double clsfy_knn_builder::build(clsfy_classifier_base& model,
                                mbl_data_wrapper<vnl_vector<double> >& inputs,
                                unsigned /* nClasses */,
                                const vcl_vector<unsigned> &outputs) const
{
  const unsigned n = inputs.size();
  assert(model.is_class("clsfy_k_nearest_neighbour")); // equiv to dynamic_cast<> != 0
  assert(n==outputs.size());

  clsfy_k_nearest_neighbour &knn = (clsfy_k_nearest_neighbour&) model;

  vcl_vector<vnl_vector<double> > vin(inputs.size());

  inputs.reset();
  unsigned i=0;
  do
  {
    vin[i++] = inputs.current();
  } while (inputs.next());

  assert(i==n);

  knn.set(vin, outputs);
  knn.set_k(k_);
  return clsfy_test_error(model, inputs, outputs);
}

//=======================================================================

unsigned clsfy_knn_builder::k() const
{
  return k_;
}

//=======================================================================

void clsfy_knn_builder::set_k(unsigned k)
{
  k_ = k;
}

//=======================================================================
//: Create empty classifier
// Caller is responsible for deletion
clsfy_classifier_base* clsfy_knn_builder::new_classifier() const
{
  return new clsfy_k_nearest_neighbour();
}

