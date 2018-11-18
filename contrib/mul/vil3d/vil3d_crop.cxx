// This is mul/vil3d/vil3d_crop.cxx
//:
// \file
// \author Ian Scott.
//
//-----------------------------------------------------------------------------

#include "vil3d_crop.h"
#include <cassert>
#ifdef _MSC_VER
#  include <vcl_msvc_warnings.h>
#endif

vil3d_image_resource_sptr vil3d_crop(
  const vil3d_image_resource_sptr &src,
  unsigned i0, unsigned n_i,
  unsigned j0, unsigned n_j,
  unsigned k0, unsigned n_k)
{
  return new vil3d_crop_image_resource(src, i0, n_i, j0, n_j, k0, n_k);
}


vil3d_crop_image_resource::vil3d_crop_image_resource(
  vil3d_image_resource_sptr const& gi,
  unsigned i0, unsigned n_i,
  unsigned j0, unsigned n_j,
  unsigned k0, unsigned n_k):
  src_(gi),
  i0_(i0),
  ni_(n_i),
  j0_(j0),
  nj_(n_j),
  k0_(k0),
  nk_(n_k)
{
  assert(i0 + n_i <= src_->ni() &&
         j0 + n_j <= src_->nj() &&
         k0 + n_k <= src_->nk() );
}
