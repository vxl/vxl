// This is gel/vsrl/vsrl_dense_matcher.cxx
#include "vsrl_dense_matcher.h"

// constructor
vsrl_dense_matcher::vsrl_dense_matcher(const vil_image &im1)
{
  _image1=im1;
}

// destructor
vsrl_dense_matcher::~vsrl_dense_matcher()
{
}

void vsrl_dense_matcher::set_correlation_range(int range)
{
  _correlation_range=range;
}


int vsrl_dense_matcher::get_width()
{
  return _image1.width();
}


int vsrl_dense_matcher::get_height()
{
  return _image1.height();
}

