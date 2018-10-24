// This is mul/vimt3d/vimt3d_save.cxx
#include "vimt3d_save.h"
//:
// \file
// \author Ian Scott

#include <mbl/mbl_log.h>
#include <vil3d/vil3d_image_resource.h>
#include <vil3d/vil3d_new.h>
#include <vil3d/vil3d_save.h>
#include <vil3d/file_formats/vil3d_meta_image_format.h>
#include <vimt3d/vimt3d_transform_3d.h>
#include <vimt3d/vimt3d_image_3d.h>
#include <vimt3d/vimt3d_vil3d_v3i.h>
#include <vimt3d/vimt3d_vil3d_v3m.h>


static mbl_logger& logger()
{
  static mbl_logger l("mul.vimt3d.save");
  return l;
}

//: Create a transform from the properties of image resource.
// The transform will be from world co-ordinates in metres to image co-ordinates (or mm if requested).
void vimt3d_save_transform(vil3d_image_resource_sptr &ir,
                           const vimt3d_transform_3d& trans,
                           bool use_millimetres)
{
  vimt3d_transform_3d i2w=trans.inverse();
  double width_x = (i2w(1,0,0)-i2w(0,0,0)).length();
  double width_y = (i2w(0,1,0)-i2w(0,0,0)).length();
  double width_z = (i2w(0,0,1)-i2w(0,0,0)).length();

  if (dynamic_cast<vimt3d_vil3d_v3i_image *>(ir.ptr()) ||
    dynamic_cast<vimt3d_vil3d_v3m_image *>(ir.ptr()) )
  {
    vgl_point_3d<double> origin = trans.origin();

    // get the translation component
    double tx =  origin.x();
    double ty =  origin.y();
    double tz =  origin.z();

    // Transform saved in metres
    vimt3d_transform_3d tr;
    const double units_scaling = use_millimetres ? 1000.0 : 1.0;
    tr.set_zoom_only (units_scaling/width_x,
                      units_scaling/width_y,
                      units_scaling/width_z, tx,ty,tz );

    if (dynamic_cast<vimt3d_vil3d_v3i_image *>(ir.ptr()))
      static_cast<vimt3d_vil3d_v3i_image &>(*ir).set_world2im(tr);
    else
      static_cast<vimt3d_vil3d_v3m_image &>(*ir).set_world2im(tr);

  }
  else if (dynamic_cast<vil3d_meta_image *>(ir.ptr()) )
  {
    double ox =  trans.origin().x();
    double oy =  trans.origin().y();
    double oz =  trans.origin().z();
    const double units_scaling = use_millimetres ? 1.0f:1000.0f;
    static_cast<vil3d_meta_image *>(ir.ptr())->set_offset(ox,oy,oz,
                                                    units_scaling*width_x,
                                                    units_scaling*width_y,
                                                    units_scaling*width_z);
  }
  else
  {
    vimt3d_transform_3d i2w=trans.inverse();
    vgl_vector_3d<double> dp = i2w.delta(vgl_point_3d<double> (0,0,0),
                                         vgl_vector_3d<double> (1.0, 1.0, 1.0));
    const double units_scaling = use_millimetres ? 1.0f:1000.0f;
    if (!ir->set_voxel_size_mm(float(units_scaling*width_x),
                               float(units_scaling*width_y),
                               float(units_scaling*width_z)))
      MBL_LOG(WARN, logger(), "vimt3d_save_transform(): Unable to include voxel sizes:"
              <<dp.x()<<','<<dp.y()<<','<<dp.z() );
  }
}


bool vimt3d_save(const std::string& path,
                 const vimt3d_image_3d& image,
                 bool use_millimetres /*=false*/)
{
  const vimt3d_image_3d & iv = image;
  const vil3d_image_view_base & ib = iv.image_base();

  vil3d_image_resource_sptr ir = vil3d_new_image_resource(
    path.c_str(), ib.ni(), ib.nj(), ib.nk(), ib.nplanes(), ib.pixel_format(),
    vil3d_save_guess_file_format(path.c_str()));

  if (!ir)
    return false;

  ir->put_view(ib);

  vimt3d_save_transform(ir, image.world2im(), use_millimetres);
  return true;
}
