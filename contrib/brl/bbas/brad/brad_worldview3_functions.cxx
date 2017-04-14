#include <string>
#include <vector>
#include <iostream>
#include <sstream>
#include <fstream>

#include <vul/vul_file.h>
#include <vil/vil_convert.h>
#include <vil/vil_load.h>
#include <vil/vil_crop.h>
#include <vil/vil_resample_bilin.h>
#include <vil/vil_save.h>
#include <vgl/algo/vgl_h_matrix_2d_compute_4point.h>
#include <vnl/vnl_math.h>
#include <vnl/vnl_inverse.h>
#include <vpgl/vpgl_local_rational_camera.h>
#include <vpgl/vpgl_lvcs.h>

#include "brad_worldview3_functions.h"


//-------------------------------------------------------------------
// Issues:
// scaled image is off by a pixel...
// needs bluring before scaling
bool brad_compose_16band_wv3_img(
  const vil_image_view<vxl_uint_16>& mul_img,
  const vpgl_rational_camera<double>& mul_rpc,
  const brad_image_metadata& mul_meta,
  const vil_image_view<vxl_uint_16>& swir_img,
  const vpgl_rational_camera<double>& swir_rpc,
  const brad_image_metadata& swir_meta,
  vil_image_view<float>& comp_img,
  float scale,
  vgl_box_2d<int> mul_region,
  bool calibrate_radiometrically )
{
  float border_val = 0.0f;
  double tol = 0.0000001;

  // Check inputs
  if (mul_img.nplanes() != 8) {
    std::cerr << "ERROR: MUL image should have 8 bands, has "
      << mul_img.nplanes() << '\n';
    return false;
  }
  if (swir_img.nplanes() != 8) {
    std::cerr << "ERROR: SWIR image should have 8 bands, has "
      << swir_img.nplanes() << '\n';
    return false;
  }

  if (mul_region.is_empty())
    mul_region = vgl_box_2d<int>(0, mul_img.ni(), 0, mul_img.nj());

  // Compute size of the scaled composite image
  int comp_width = mul_region.width(), comp_height = mul_region.height();
  if (scale > 0.0f) {
    comp_width = (int)(comp_width*scale);
    comp_height = (int)(comp_height*scale);
  }


  //
  // STEP 1: Compute transformation from SWIR to composite image
  //

  // Compute the average lat/lon/el of the MUL image
  float avg_lat = 0.0f, avg_lon = 0.0f;
  for (int c = 0; c < 4; c++) {
    avg_lat += mul_meta.footprint_[0][c].y();
    avg_lon += mul_meta.footprint_[0][c].x();
  }
  avg_lat /= 4.0f; avg_lon /= 4.0f;

  float avg_el = (mul_meta.lower_left_.z() + mul_meta.upper_right_.z()) / 2.0f;

  // Construct an LVCS at the center of the MUL image
  vpgl_lvcs lvcs(avg_lat, avg_lon, avg_el);

  // Convert cameras 
  vpgl_local_rational_camera<double> mul_cam(lvcs, mul_rpc);
  vpgl_local_rational_camera<double> swir_cam(lvcs, swir_rpc);

  // Project a planar neighborhood of the LVCS origin into the images
  double nbhd = 1000.0;
  std::vector< vgl_homg_point_2d<double> > swir_pt(4), mul_pt(4);

  for (int c = 0; c < 4; c++) {
    double x, y, u, v;
    if (c == 0 || c == 1) x = -nbhd;
    else x = nbhd;
    if (c == 0 || c == 3) y = -nbhd;
    else y = nbhd;

    mul_cam.project(x, y, avg_el, u, v);
    mul_pt[c].set(u, v, 1.0);
    swir_cam.project(x, y, avg_el, u, v);
    swir_pt[c].set(u, v, 1.0);
  }

  // Compute homography from SWIR to MUL
  vgl_h_matrix_2d<double> swir_to_mul;
  vgl_h_matrix_2d_compute_4point hc;
  hc.compute(swir_pt, mul_pt, swir_to_mul);

  // Compute homography from composite to SWIR
  vgl_h_matrix_2d<double> mul_to_comp;
  mul_to_comp.set_identity();
  mul_to_comp.set_translation(
    (double)-mul_region.min_x(), (double)-mul_region.min_y());
  if (scale > 0.0f)
    mul_to_comp.set_scale(scale);
  vnl_matrix_fixed<double, 3, 3> comp_to_swir =
    (mul_to_comp*swir_to_mul).get_inverse().get_matrix();


  //
  // STEP 2: Fill in all bands
  //

  // Allocate output image
  comp_img.set_size(comp_width, comp_height, 16);

  vil_image_view<float> band;

  // Resample the MUL bands if necessary, or just copy
  for (int p = 0; p < 8; p++) {

    //vil_convert_cast( vil_plane(mul_img,p), band);
    vil_convert_cast(vil_crop(vil_plane(mul_img, p),
      mul_region.min_x(), mul_region.width(),
      mul_region.min_y(), mul_region.height()), band);

    if (calibrate_radiometrically) vil_math_scale_and_offset_values(
      band, mul_meta.gains_[p + 1].first, mul_meta.gains_[p + 1].second);

    vil_image_view<float> comp_band = vil_plane(comp_img, p);
    if (scale > 0.0f)
      vil_resample_bilin(band, comp_band,
        comp_width, comp_height);
    else
      comp_band.deep_copy(band);
  }

  int swir_width_safe = swir_img.ni() - 2;
  int swir_height_safe = swir_img.nj() - 2;

  // Warp the swir image into the composite coordinate system
  for (int p = 0; p < 8; p++) {

    vil_convert_cast(vil_plane(swir_img, p), band);
    if (calibrate_radiometrically) vil_math_scale_and_offset_values(
      band, swir_meta.gains_[p + 1].first, swir_meta.gains_[p + 1].second);

    vil_image_view<float> comp_band = vil_plane(comp_img, p + 8);
    comp_band.fill(border_val);

    for (int y = 0; y < comp_height; y++) {

      // Speed up coordinate transform by pre-computing last two column 
      // multiplications since they don't change with x.
      vnl_vector_fixed<double, 3> col23 =
        comp_to_swir*vnl_vector_fixed<double, 3>(0.0, (double)y, 1.0);
      vnl_vector_fixed<double, 3> col1(
        comp_to_swir(0, 0), comp_to_swir(1, 0), comp_to_swir(2, 0));

      for (int x = 0; x < comp_width; x++) {

        // Compute the back-projection of pixel x,y in image 1
        vnl_vector_fixed<double, 3> wxy = (double)x*col1 + col23;

        // Convert homogeneous coords, do not warp if at infinity
        if (std::fabs(wxy[2]) < tol) continue;
        double wx = wxy[0] / wxy[2];
        double wy = wxy[1] / wxy[2];

        // Check coordinates are safe for interpolation
        if (wx < 0.0 || wx > swir_width_safe ||
          wy < 0.0 || wy > swir_height_safe) continue;

        // Interpolate
        // The below is code from vil_bilin_interp_safe
        int p1x = int(wx);
        double normx = wx - p1x;
        int p1y = int(wy);
        double normy = wy - p1y;

        double i1 = band(p1x, p1y) +
          (band(p1x, p1y + 1) - (double)band(p1x, p1y))*normy;
        double i2 = band(p1x + 1, p1y) +
          (band(p1x + 1, p1y + 1) - (double)band(p1x + 1, p1y))*normy;

        comp_band(x, y) = (float)(i1 + (i2 - i1)*normx);
      } //x
    } //y

  } //p
  return true;
};


//------------------------------------------------------------------
bool brad_compose_16band_wv3_img(
  const std::string& mul_file,
  const std::string& swir_file,
  vil_image_view<float>& comp_img,
  float scale,
  vgl_box_2d<int> mul_region )
{
  int num_bands = 8;

  // Load metadata
  brad_image_metadata mul_meta(vul_file::strip_extension(mul_file) + ".IMD");
  brad_image_metadata swir_meta(vul_file::strip_extension(swir_file) + ".IMD");

  // Load cameras
  vpgl_rational_camera<double>* mul_rpc =
    read_rational_camera<double>(std::ifstream((
      vul_file::strip_extension(mul_file) + ".RPB").c_str()));
  vpgl_rational_camera<double>* swir_rpc =
    read_rational_camera<double>(std::ifstream((
      vul_file::strip_extension(swir_file) + ".RPB").c_str()));

  // Load the images
  vil_image_resource_sptr mul_rsc = vil_load_image_resource(mul_file.c_str());
  std::cerr << "Found a " << mul_rsc->ni() << 'x' << mul_rsc->nj()
    << " image with " << mul_rsc->nplanes() << " channels\n";
  vil_image_resource_sptr swir_rsc = vil_load_image_resource(swir_file.c_str());
  std::cerr << "Found a " << swir_rsc->ni() << 'x' << swir_rsc->nj()
    << " image with " << swir_rsc->nplanes() << " channels\n";

  std::cerr << "Loading images\n";
  vil_image_view<vxl_uint_16> mul_img = mul_rsc->get_view();
  vil_image_view<vxl_uint_16> swir_img = swir_rsc->get_view();

  std::cerr << "Generating 16 band image\n";
  bool success = brad_compose_16band_wv3_img(
    mul_img, *mul_rpc, mul_meta, swir_img, *swir_rpc, swir_meta,
    comp_img, scale, mul_region);

  delete mul_rpc; delete swir_rpc;
  return success;
};


/*int main(int argc, char * argv[])
{
  // Set arguments
  std::string mul_file(
    "D:/data/core3d/sevastopol2/104001001E0AA000_P005_MUL/16JUL21091539-M1BS-056339611010_01_P005.NTF");
  std::string swir_file(
    "D:/data/core3d/sevastopol2/104A01001E0AA000_P001_SWR/16JUL21091536-A1BS-056339460010_01_P001.NTF");

  //float mean_albedo = 0.15f;

  std::string out_dir("D:/results/ms/");

  vil_image_view<float> comp_img;
  brad_compose_16band_wv3_img(
    mul_file, swir_file, comp_img, 0.33f);// , vgl_box_2d<int>(2000, 3000, 3000, 4500) );

                                          // Write bands
  std::cerr << "Writing output\n";
  for (int p = 0; p < 16; p++) {
    vil_image_view<float> plane = vil_plane(comp_img, p);
    std::stringstream ss;
    ss << "D:/results/ms/" << p << ".png";
    vil_image_view<vxl_byte> vis;
    vil_convert_stretch_range_limited(
      plane, vis, 0.0f, 100.0f);
    vil_save(vis, ss.str().c_str());
  }

  return 0;
};
*/
