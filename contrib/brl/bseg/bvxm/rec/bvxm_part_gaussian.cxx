//:
// \file
// \author Ozge C Ozcanli (ozge@lems.brown.edu)
// \date Oct. 16, 2008

#include "bvxm_part_gaussian.h"
#include <rec/bvxm_part_gaussian_sptr.h>

#include <vil/vil_convert.h>
#include <vil/vil_save.h>
#include <vil/algo/vil_threshold.h>
#include <vil/vil_new.h>
#include <brip/brip_vil_float_ops.h>
#include <vnl/vnl_math.h>

#include <bxml/bxml_find.h>
#include <vcl_cmath.h>
#include <vcl_iostream.h>

//strength_threshold in [0,1] - min strength to declare the part as detected
bool extract_gaussian_primitives(vil_image_resource_sptr img, float lambda0, float lambda1, float theta, bool bright, float cutoff_percentage, float strength_threshold, unsigned type, vcl_vector<bvxm_part_instance_sptr>& parts)
{
  vil_image_view<float> fimg = brip_vil_float_ops::convert_to_float(img);
  vil_image_view<float> extr = brip_vil_float_ops::extrema(fimg, lambda0, lambda1, theta, bright, true);
  if (extr.nplanes() != 2)
    return false;

  unsigned ni = fimg.ni();
  unsigned nj = fimg.nj();

  vil_image_view<float> res(ni, nj), mask(ni, nj);
  for (unsigned j = 0; j<nj; ++j)
    for (unsigned i = 0; i<ni; ++i)
    {
      res(i,j) = extr(i,j,0);
      mask(i,j) = extr(i,j,1);
    }

  float min, max;
  vil_math_value_range(res, min, max);

#if 1
  vcl_cout << "res min: " << min << " max: " << max << vcl_endl;
  vil_image_view<vxl_byte> res_o(ni, nj);
  vil_convert_stretch_range_limited(res, res_o, min, max);
  vil_save(res_o, "./temp.png");
#endif

#if 0
  // find the top 10 percentile of the output map and convert it into a prob map (scale to [0,1] range) accordingly
  float val;
  vil_math_value_range_percentile(res, 1.0, val);
  vcl_cout << "res top 10 percentile value: " << val << vcl_endl;
#endif // 0
  vil_image_view<float> strength_map(ni, nj);
//vil_convert_stretch_range_limited(res, strength_map, 0.0f, val, 0.0f, 1.0f);
  vil_convert_stretch_range_limited(res, strength_map, 0.0f, max, 0.0f, 1.0f);
#if 1
  vil_math_value_range(strength_map, min, max);
  vcl_cout << "strength_map min: " << min << " max: " << max << vcl_endl;
  vil_convert_stretch_range_limited(strength_map, res_o, min, max);
  vil_save(res_o, "./strength_map.png");
#endif

  //: extract all the parts from the responses
  for (unsigned j = 0; j<nj; ++j)
    for (unsigned i = 0; i<ni; ++i)
    {
      if (strength_map(i,j) > strength_threshold) {
        bvxm_part_gaussian_sptr dp = new bvxm_part_gaussian((float)i, (float)j, strength_map(i,j), lambda0, lambda1, theta, bright, type);
        dp->cutoff_percentage_ = cutoff_percentage;
        parts.push_back(dp->cast_to_instance());
      }
    }

#if 0
  vil_image_resource_sptr img_resc = vil_new_image_resource_of_view(img);
  vil_image_resource_sptr res_resc = vil_new_image_resource_of_view(res);
  vil_image_resource_sptr msk_resc = vil_new_image_resource_of_view(mask);
  vil_image_view<vil_rgb<vxl_byte> > rgb =
      brip_vil_float_ops::combine_color_planes(img_resc, res_resc, msk_resc);
    vil_save(rgb, "./temp.png");
  vil_math_value_range(fimg, min, max);
  vcl_cout << "img min: " << min << " max: " << max << vcl_endl;
  vil_math_value_range(mask, min, max);
  vcl_cout << "mask min: " << min << " max: " << max << vcl_endl;
#endif

  vil_image_view<bool> res_bool;
  vil_threshold_above(res, res_bool, max/2);
  vil_image_view<float> res_bool_f;
  vil_convert_cast(res_bool, res_bool_f);
  vil_convert_stretch_range_limited(res_bool_f, res_o, 0.0f, 1.0f);
  vil_save(res_o, "./temp_thresholded.png");

  return true;
}

bvxm_part_gaussian* bvxm_part_gaussian::cast_to_gaussian(void)
{
  return this;
}

bool bvxm_part_gaussian::mark_receptive_field(vil_image_view<vxl_byte>& img, unsigned plane)
{
  if (img.nplanes() <= plane)
    return false;

  vbl_array_2d<bool> mask;
  vbl_array_2d<float> kernel;
  brip_vil_float_ops::extrema_kernel_mask(lambda0_, lambda1_, theta_, kernel, mask);

  unsigned nrows = mask.rows();
  unsigned ncols = mask.cols();

  int js = (int)vcl_floor(y_ - (float)nrows/2.0f + 0.5f);
  int is = (int)vcl_floor(x_ - (float)ncols/2.0f + 0.5f);
  int je = (int)vcl_floor(y_ + (float)nrows/2.0f + 0.5f);
  int ie = (int)vcl_floor(x_ + (float)ncols/2.0f + 0.5f);

  int ni = (int)img.ni();
  int nj = (int)img.nj();
  for (int i = is; i < ie; i++)
    for (int j = js; j < je; j++) {
      int mask_i = i - is;
      int mask_j = j - js;
      if (mask[mask_j][mask_i] && i >= 0 && j >= 0 && i < ni && j < nj) {
        if (((int)img(i, j, plane) + strength_*255) > 255)
          img(i, j, plane) = 255;
        else
          img(i, j, plane) += (vxl_byte)(strength_*255);
      }
    }

  return true;
}

bool bvxm_part_gaussian::mark_receptive_field(vil_image_view<float>& img, float val)
{
  vbl_array_2d<bool> mask;
  vbl_array_2d<float> kernel;
  brip_vil_float_ops::extrema_kernel_mask(lambda0_, lambda1_, theta_, kernel, mask, cutoff_percentage_);

  unsigned nrows = mask.rows();
  unsigned ncols = mask.cols();

  int js = (int)vcl_floor(y_ - (float)nrows/2.0f + 0.5f);
  int is = (int)vcl_floor(x_ - (float)ncols/2.0f + 0.5f);
  int je = (int)vcl_floor(y_ + (float)nrows/2.0f + 0.5f);
  int ie = (int)vcl_floor(x_ + (float)ncols/2.0f + 0.5f);

  int ni = (int)img.ni();
  int nj = (int)img.nj();
  for (int i = is; i < ie; i++)
    for (int j = js; j < je; j++) {
      int mask_i = i - is;
      int mask_j = j - js;
      if (mask[mask_j][mask_i] && i >= 0 && j >= 0 && i < ni && j < nj) {
        if ((img(i, j) + val) > 1.0f)
          img(i, j) = 1.0f;
        else
          img(i, j) += val;
      }
    }
  return true;
}

bool bvxm_part_gaussian::mark_center(vil_image_view<vxl_byte>& img, unsigned plane)
{
  if (img.nplanes() <= plane)
    return false;

  int ni = (int)img.ni();
  int nj = (int)img.nj();

  int ic = (int)vcl_floor(x_ + 0.5f);
  int jc = (int)vcl_floor(y_ + 0.5f);
  if (ic >= 0 && jc >= 0 && ic < ni && jc < nj)
    img(ic, jc, plane) = (vxl_byte)(strength_*255);

  return true;
}

vnl_vector_fixed<float,2>
bvxm_part_gaussian::direction_vector(void)  // return a unit vector that gives direction of this instance in the image
{
  vnl_vector_fixed<float,2> v;
  double theta_rad = theta_*vnl_math::pi/180.0;
  v(0) = (float)vcl_cos(theta_rad);
  v(1) = (float)vcl_sin(theta_rad);
  return v;
}


bxml_data_sptr bvxm_part_gaussian::xml_element()
{
  bxml_data_sptr data_super = bvxm_part_instance::xml_element();

  bxml_element* data = new bxml_element("gaussian");

  data->set_attribute("lambda0",lambda0_);
  data->set_attribute("lambda1",lambda1_);
  data->set_attribute("theta",theta_);
  if (bright_)
    data->set_attribute("bright",1);
  else
    data->set_attribute("bright",0);

  data->set_attribute("cutoff_perc", cutoff_percentage_);

  data->append_text("\n ");
  data->append_data(data_super);
  data->append_text("\n ");
  //((bxml_element*)data_super.ptr())->append_data(data);
  //((bxml_element*)data_super.ptr())->append_text("\n ");

  return data;
}

bool bvxm_part_gaussian::xml_parse_element(bxml_data_sptr data)
{
  bxml_element query("gaussian");
  bxml_data_sptr g_root = bxml_find_by_name(data, query);

  if (!g_root)
    return false;

  if (g_root->type() == bxml_data::ELEMENT) {
    bool found = (((bxml_element*)g_root.ptr())->get_attribute("lambda0", lambda0_) &&
                  ((bxml_element*)g_root.ptr())->get_attribute("lambda1", lambda1_) &&
                  ((bxml_element*)g_root.ptr())->get_attribute("theta", theta_) &&
                  ((bxml_element*)g_root.ptr())->get_attribute("cutoff_perc", cutoff_percentage_));

    int bright_int;
    found = found && ((bxml_element*)g_root.ptr())->get_attribute("bright", bright_int);

    if (!found)
      return false;

    bright_ = bright_int == 0 ? false : true;

    return bvxm_part_instance::xml_parse_element(g_root);
  } else
    return false;

  return true;
}

