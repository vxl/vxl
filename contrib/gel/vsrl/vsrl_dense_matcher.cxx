// This is gel/vsrl/vsrl_dense_matcher.cxx
#include "vsrl_dense_matcher.h"

// constructor
vsrl_dense_matcher::vsrl_dense_matcher(const vil_image &im1)
{
  image1_=im1;
}

// destructor
vsrl_dense_matcher::~vsrl_dense_matcher()
{
}

void vsrl_dense_matcher::set_correlation_range(int range)
{
  correlation_range_=range;
}
