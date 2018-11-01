// This is core/vil/vil_crop.cxx
//:
// \file
// \author Ian Scott.
//
//-----------------------------------------------------------------------------

#include "vil_crop.h"
#include <cassert>
#ifdef _MSC_VER
#  include <vcl_msvc_warnings.h>
#endif
#include <vil/vil_exception.h>


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

vil_image_view_base_sptr vil_crop_image_resource::get_copy_view(unsigned i0, unsigned n_i,
                                                                unsigned j0, unsigned n_j) const
{
  if (i0 + n_i > ni() || j0 + n_j > nj())
  {
     vil_exception_warning(vil_exception_out_of_bounds(
        "vil_crop_image_resource::get_copy_view") );
    return nullptr;
  }
  return src_->get_copy_view(i0+i0_, n_i, j0+j0_, n_j);
}

vil_image_view_base_sptr vil_crop_image_resource::get_view(unsigned i0, unsigned n_i,
                                                           unsigned j0, unsigned n_j) const
{
  if (i0 + n_i > ni() || j0 + n_j > nj())
  {
     vil_exception_warning(vil_exception_out_of_bounds(
        "vil_crop_image_resource::get_view") );
    return nullptr;
  }
  return src_->get_view(i0+i0_, n_i, j0+j0_, n_j);
}
