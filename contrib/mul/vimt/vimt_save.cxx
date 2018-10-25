// This is mul/vimt/vimt_save.cxx
#include "vimt_save.h"
//:
// \file
// \author Ian Scott, Kevin de Souza
// \note Adapted from vimt3d_save

#include <vil/vil_image_resource.h>
#include <vil/vil_new.h>
#include <vil/vil_save.h>
#include <vimt/vimt_transform_2d.h>
#include <vimt/vimt_image_2d.h>
#include <vimt/vimt_vil_v2i.h>
#include <mbl/mbl_log.h>

static mbl_logger& logger()
{
  static mbl_logger l("mul.vimt.save");
  return l;
}


//: Create a transform from the properties of image resource.
// The transform will be from world co-ordinates in metres to image co-ordinates (or mm if requested).
void vimt_save_transform(vil_image_resource_sptr &ir,
                         const vimt_transform_2d& trans,
                         bool  /*use_millimetres*/ /*=false*/)
{
  if (dynamic_cast<vimt_vil_v2i_image *>(ir.ptr()))
  {
    vgl_vector_2d<double> pix_per_mm = trans.delta(vgl_point_2d<double>(0,0),
                                                   vgl_vector_2d<double>(1.0, 1.0));

    vimt_transform_2d tr;
    //const double units_scaling = use_millimetres ? 1000.0 : 1.0;
    tr.set_zoom_only(1000.0*pix_per_mm.x(), 1000.0*pix_per_mm.y());

    static_cast<vimt_vil_v2i_image &>(*ir).set_world2im(tr);
  }
  else
  {
    vimt_transform_2d i2w=trans.inverse();
    vgl_vector_2d<double> dp = i2w.delta(vgl_point_2d<double> (0,0),
                                         vgl_vector_2d<double> (1.0, 1.0));
    MBL_LOG(WARN, logger(), "vimt_save_transform(): function set_pixel_size()"
            " is not yet defined for vil_image_resource base class,"
            " only for vimt_vil_v2i_image derived class.");
    //if (!ir->set_pixel_size(float(dp.x()),float(dp.y())))
    MBL_LOG(WARN, logger(), "vimt_save_transform(): Unable to include pixel sizes:"
            <<dp.x()<<','<<dp.y());
  }
}


bool vimt_save(const std::string& path,
               const vimt_image_2d& image,
               bool use_millimetres /*=false*/)
{
  const vimt_image_2d & iv = image;
  const vil_image_view_base & ib = iv.image_base();

  vil_image_resource_sptr ir = vil_new_image_resource(
    path.c_str(), ib.ni(), ib.nj(), ib.nplanes(), ib.pixel_format(),
    vil_save_guess_file_format(path.c_str()));

  if (!ir)
    return false;

  ir->put_view(ib);

  vimt_save_transform(ir, image.world2im(), use_millimetres);
  return true;
}
