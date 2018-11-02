// This is mul/clsfy/clsfy_binary_hyperplane_ls_builder.cxx
// Copyright: (C) 2001 British Telecommunications PLC
#include <string>
#include <iostream>
#include <vector>
#include <algorithm>
#include "clsfy_binary_hyperplane_ls_builder.h"
//:
// \file
// \brief Implement a two-class output linear classifier builder
// \author Ian Scott
// \date 4 June 2001

//=======================================================================

#include <cassert>
#ifdef _MSC_VER
#  include <vcl_msvc_warnings.h>
#endif
#include <vnl/algo/vnl_svd.h>
#include <vnl/vnl_math.h>

//=======================================================================

std::string clsfy_binary_hyperplane_ls_builder::is_a() const
{
  return std::string("clsfy_binary_hyperplane_ls_builder");
}

//=======================================================================

bool clsfy_binary_hyperplane_ls_builder::is_class(std::string const& s) const
{
  return s == clsfy_binary_hyperplane_ls_builder::is_a() || clsfy_builder_base::is_class(s);
}

//=======================================================================

void clsfy_binary_hyperplane_ls_builder::print_summary(std::ostream& os) const
{
  os << is_a();
}

//=======================================================================

//: Build a multi layer perceptron classifier, with the given data.
double clsfy_binary_hyperplane_ls_builder::build(
  clsfy_classifier_base &classifier, mbl_data_wrapper<vnl_vector<double> > &inputs,
  const std::vector<unsigned> &outputs) const
{
  assert(outputs.size() == inputs.size());
  assert(* std::max_element(outputs.begin(), outputs.end()) <= 1);
  assert(classifier.is_class("clsfy_binary_hyperplane"));

  auto &hyperplane = (clsfy_binary_hyperplane &) classifier;

  inputs.reset();
  const unsigned k = inputs.current().size();
  vnl_matrix<double> XtX(k+1, k+1, 0.0);
  vnl_vector<double> XtY(k+1, 0.0);

#if 0 // The calculation is as follows
  do
  {
    // XtX += [x, -1]' * [x, -1]
    const vnl_vector<double> &x=inputs.current();
    double y = outputs[inputs.index()] ? 1.0 : -1.0;
    vnl_vector<double> xp(k+1);
    xp.update(x, 0);
    xp(k) = -1.0;
    XtX += outer_product(xp, xp);
    double y = outputs[inputs.index()] ? 1.0 : -1.0;
    XtY += y * xp;
  } while (inputs.next());
#else// However the following version is faster
  do
  {
    // XtX += [x, -1]' * [x, -1]
    const vnl_vector<double> &x=inputs.current();
    double y = outputs[inputs.index()] ? 1.0 : -1.0;
    for (unsigned i=0; i<k; ++i)
    {
      for (unsigned j=0; j<i; ++j)
        XtX(i,j) += x(i) * x(j);
      XtX(i,i) += vnl_math::sqr(x(i));
      XtX(i,k) -= x(i);
      XtY(i) += y * x(i);
    }
    XtY(k) += y * -1.0;

  } while (inputs.next());
  for (unsigned i=0; i<k; ++i)
  {
    for (unsigned j=0; j<i; ++j)
      XtX(j,i) += XtX(i,j);
    XtX(k,i) = XtX(i,k);
  }
  XtX(k, k) = (double) inputs.size();
#endif


  // Find the solution to X w = Y;
  // However it is easier to find X' X w = X' Y;
  // because X is n_train x n_elems whereas X'X is n_elems x n_elems

  vnl_svd<double> svd(XtX, 1.0e-12); // 1e-12 = zero-tolerance for singular values
  vnl_vector<double> w = svd.solve(XtY);
#if 0
  std::cerr << "XtX: " << XtX << '\n'
           << "XtY: " << XtY << '\n'
           << "w: "   << w   << std::endl;
#endif
  vnl_vector<double> weights(&w(0), k);
  hyperplane.set(weights, w(k));

  return clsfy_test_error(classifier, inputs, outputs);
}


//=======================================================================


//: Build a linear classifier, with the given data.
// Return the mean error over the training set.
// n_classes must be 1.
double clsfy_binary_hyperplane_ls_builder::build(
  clsfy_classifier_base &classifier, mbl_data_wrapper<vnl_vector<double> > &inputs,
  unsigned n_classes, const std::vector<unsigned> &outputs) const
{
  assert (n_classes == 1);
  return build(classifier, inputs, outputs);
}

//=======================================================================

void clsfy_binary_hyperplane_ls_builder::b_write(vsl_b_ostream &bfs) const
{
  constexpr short version_no = 1;
  vsl_b_write(bfs, version_no);
}

//=======================================================================

void clsfy_binary_hyperplane_ls_builder::b_read(vsl_b_istream &bfs)
{
  if (!bfs) return;

  short version;
  vsl_b_read(bfs,version);
  switch (version)
  {
    case (1):
      break;
    default:
      std::cerr << "I/O ERROR: clsfy_binary_hyperplane_ls_builder::b_read(vsl_b_istream&)\n"
               << "           Unknown version number "<< version << '\n';
      bfs.is().clear(std::ios::badbit); // Set an unrecoverable IO error on stream
  }
}
