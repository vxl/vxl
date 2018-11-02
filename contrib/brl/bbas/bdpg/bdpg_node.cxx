#include <iostream>
#include <cmath>
#include "bdpg_node.h"
//:
// \file
#ifdef _MSC_VER
#  include <vcl_msvc_warnings.h>
#endif
#include <vnl/vnl_numeric_traits.h>

bdpg_node::bdpg_node() :
  previous_row_(-1),   previous_col_(-1), prior_prob_(0),
  log_prob_(-vnl_numeric_traits<double>::maxval)
{
}

bdpg_node::~bdpg_node()
= default;

void bdpg_node::set_prior_prob(const double p)
{
  prior_prob_ = p;
  log_prob_ = std::log(p);
}

double bdpg_node::p()
{
  return std::exp(log_prob_);
}

//: append the path probability to the current probability
void bdpg_node::update_prob(const double p)
{
  log_prob_ += std::log(p);
}
