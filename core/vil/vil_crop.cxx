// This is core/vil/vil_crop.cxx
#ifdef VCL_NEEDS_PRAGMA_INTERFACE
#pragma implementation
#endif
//:
// \file
// \author Ian Scott.
//
//-----------------------------------------------------------------------------

#include "vil_crop.h"
#include <vcl_cassert.h>


vil_image_resource_sptr vil_crop(const vil_image_resource_sptr &src, unsigned i0,
                                   unsigned n_i, unsigned j0, unsigned n_j)
{
  return new vil_crop_image_resource(src, i0, n_i, j0, n_j);
}


vil_crop_image_resource::vil_crop_image_resource(vil_image_resource_sptr const& gi,
                                                   unsigned i0, unsigned n_i,
                                                   unsigned j0, unsigned n_j):
  src_(gi),
  i0_(i0),
  ni_(n_i),
  j0_(j0),
  nj_(n_j)
{
  assert (i0+n_i <= src_->ni() && j0 + n_j <= src_->nj());
}

