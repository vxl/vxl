#include <string>
#include <vector>
#include <iostream>
#include <sstream>
#include <fstream>

#include <vul/vul_file.h>
#include <vul/vul_file_iterator.h>
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
#include "brad_image_atmospherics_est.h"


//-------------------------------------------------------------------
// Issues:
// scaled image is off by a pixel...
// needs bluring before scaling
bool brad_compose_16band_wv3_img(
  const vil_image_view<float>& mul_img,
  const vpgl_rational_camera<double>& mul_rpc,
  const brad_image_metadata& mul_meta,
  const vil_image_view<float>& swir_img,
  const vpgl_rational_camera<double>& swir_rpc,
  const brad_image_metadata&  /*swir_meta*/,
  vil_image_view<float>& comp_img,
  float scale,
  vgl_box_2d<int> mul_region )
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
    band = vil_crop(vil_plane(mul_img, p),
      mul_region.min_x(), mul_region.width(),
      mul_region.min_y(), mul_region.height());

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

    band = vil_plane(swir_img, p);

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
  vgl_box_2d<int> mul_region,
  bool  /*calibrate_radiometrically*/ )
{
  // Load metadata
  brad_image_metadata mul_meta(vul_file::strip_extension(mul_file) + ".IMD");
  brad_image_metadata swir_meta(vul_file::strip_extension(swir_file) + ".IMD");

  // Load cameras
  std::ifstream mul_ifs((vul_file::strip_extension(mul_file) + ".RPB").c_str());
  std::ifstream swir_ifs((vul_file::strip_extension(swir_file) + ".RPB").c_str());
  vpgl_rational_camera<double>* mul_rpc =
    read_rational_camera<double>(mul_ifs);
  vpgl_rational_camera<double>* swir_rpc =
    read_rational_camera<double>(swir_ifs);

  // Load the images
  vil_image_resource_sptr mul_rsc = vil_load_image_resource(mul_file.c_str());
  vil_image_resource_sptr swir_rsc = vil_load_image_resource(swir_file.c_str());
  vil_image_view<float> mul_img, swir_img;
  vil_image_view<vxl_uint_16> raw = mul_rsc->get_view();
  brad_calibrate_wv3_img(mul_meta, raw, mul_img, false);
  raw = swir_rsc->get_view();
  brad_calibrate_wv3_img(swir_meta, raw, swir_img, true);

  // Compose
  bool success = brad_compose_16band_wv3_img(
    mul_img, *mul_rpc, mul_meta, swir_img, *swir_rpc, swir_meta,
    comp_img, scale, mul_region );

  delete mul_rpc; delete swir_rpc;
  return success;
};


//-------------------------------------------------------------------
bool brad_calibrate_wv3_img(
  const brad_image_metadata& meta,
  const vil_image_view<vxl_uint_16>& wv3_raw,
  vil_image_view<float>& wv3_cal,
  bool swir)
{
  if (wv3_raw.nplanes() != 8) return false;

  wv3_cal = vil_image_view<float>(
    wv3_raw.ni(), wv3_raw.nj(), wv3_raw.nplanes());

  // Convert from digital numbers to top-of-atmosphere radiance
  for (int b = 0; b < 8; b++) {
    double abs_cal_factor = meta.abscal_[b] / meta.effect_band_width_[b];
    vil_image_view<vxl_uint_16> wv3_raw_plane = vil_plane(wv3_raw, b);
    vil_image_view<float> wv3_cal_plane = vil_plane(wv3_cal, b);
    vil_convert_cast(wv3_raw_plane, wv3_cal_plane);
    vil_math_scale_and_offset_values(wv3_cal_plane, abs_cal_factor, 0.0);
  }

  // Apply fixed gain/offset from params
  brad_apply_wv3_fixed_calibration(wv3_cal, swir);

  // Convert from radiance to reflectance
  brad_atmo_radiance_to_reflectance(wv3_cal, meta, wv3_cal);

  return true;
}


//------------------------------------------------------------------------
void brad_apply_wv3_fixed_calibration(
  vil_image_view<float>& wv3_img,
  bool swir )
{
  std::vector<double> gain(8), offset(8);

  if (!swir) {
    gain[0] = 0.863; offset[0] = -7.154;
    gain[1] = 0.905; offset[1] = -4.189;
    gain[2] = 0.907; offset[2] = -3.287;
    gain[3] = 0.938; offset[3] = -1.816;
    gain[4] = 0.945; offset[4] = -1.350;
    gain[5] = 0.980; offset[5] = -2.617;
    gain[6] = 0.982; offset[6] = -3.752;
    gain[7] = 0.954; offset[7] = -1.507;
  }
  else {
    gain[0] = 1.160; offset[0] = -4.479;
    gain[1] = 1.184; offset[1] = -2.248;
    gain[2] = 1.173; offset[2] = -1.806;
    gain[3] = 1.187; offset[3] = -1.507;
    gain[4] = 1.286; offset[4] = -0.622;
    gain[5] = 1.336; offset[5] = -0.605;
    gain[6] = 1.340; offset[6] = -0.423;
    gain[7] = 1.392; offset[7] = -0.302;
  }

  for (int p = 0; p < 8; p++) {
    vil_image_view<float> wv3_plane = vil_plane(wv3_img, p);
    vil_math_scale_and_offset_values( wv3_plane, gain[p], offset[p]);
  }
}


//---------------------------------------------------------------------------
//: The spectral bands present in a World View 3 multi-spectral image with SWIR.
//---------------------------------------------------------------------------
void brad_wv3_bands(
  std::vector<float>& bands_min,
  std::vector<float>& bands_max,
  int num_bands )
{
  bands_min.resize(16);
  bands_max.resize(16);
  bands_min[0] = 0.400f; bands_max[0] = 0.450f; // Coastal
  bands_min[1] = 0.450f; bands_max[1] = 0.510f; // Blue
  bands_min[2] = 0.510f; bands_max[2] = 0.580f; // Green
  bands_min[3] = 0.585f; bands_max[3] = 0.625f; // Yellow
  bands_min[4] = 0.630f; bands_max[4] = 0.690f; // Red
  bands_min[5] = 0.705f; bands_max[5] = 0.745f; // Red Edge
  bands_min[6] = 0.770f; bands_max[6] = 0.895f; // Near-IR1
  bands_min[7] = 0.860f; bands_max[7] = 1.040f; // Near-IR2
  bands_min[8] = 1.195f; bands_max[8] = 1.225f; // SWIR-1
  bands_min[9] = 1.550f; bands_max[9] = 1.590f; // SWIR-2
  bands_min[10] = 1.640f; bands_max[10] = 1.680f; // SWIR-3
  bands_min[11] = 1.710f; bands_max[11] = 1.750f; // SWIR-4
  bands_min[12] = 2.145f; bands_max[12] = 2.185f; // SWIR-5
  bands_min[13] = 2.185f; bands_max[13] = 2.225f; // SWIR-6
  bands_min[14] = 2.235f; bands_max[14] = 2.285f; // SWIR-7
  bands_min[15] = 2.295f; bands_max[15] = 2.365f; // SWIR-8
  if (num_bands > 0 && num_bands < 16) {
    bands_min.resize(num_bands);
    bands_max.resize(num_bands);
  }
};

void save_corrected_wv3(
  const vil_image_view<float>& corr_img,
  const std::string& save_dir)
{
  int np = corr_img.nplanes();
  vil_image_view<float> cur_plane;
  for (int i = 0; i < np; i++) {
    std::ostringstream buffer;
    cur_plane = vil_plane(corr_img, i);
    if (i < 10) buffer << save_dir << "/band0" << i << ".tif";
    else buffer << save_dir << "/band" << i << ".tif";
    vil_save(cur_plane,buffer.str().c_str());
  }
};

void load_corrected_wv3(
  const std::string& img_dir,
  vil_image_view<float>& cal_img) {

  // load first band to get the image size
  vil_image_resource_sptr cal_rsc = vil_load_image_resource((img_dir + "/band00.tif").c_str());
  cal_img = cal_rsc->get_view();
  int np = 0;
  int ni = cal_img.ni();
  int nj = cal_img.nj();
  // count the number of band images in the directory to get the number of planes
  std::string band_dir = (img_dir + "/band*.tif");
  for (vul_file_iterator fi(band_dir); fi; ++fi) { //fi((img_dir + "band*.tif").c_str()); fi; ++fi) {
    std::cout << vul_file::strip_directory(fi()) << "\n";
    np++;
  }
  // initialize the calibrated image and fill with band images
  cal_img.set_size(ni, nj, np);
  vil_image_view<float> cur_plane;
  for (int i = 0; i < np; i++) {
    std::ostringstream buffer;
    cur_plane = vil_plane(cal_img, i);
    if(i<10) buffer << img_dir << "/band0" << i << ".tif";
    else buffer << img_dir << "/band" << i << ".tif";
    cal_rsc = vil_load_image_resource(buffer.str().c_str());
    cur_plane.deep_copy(cal_rsc->get_view());
  }
}
