// This is brl/bseg/strk/strk_region_info.cxx
#include "strk_region_info.h"
//:
// \file
#include <vil1/vil1_memory_image_of.h>
#include <vgl/algo/vgl_h_matrix_2d_compute_4point.h>
#include <vtol/vtol_vertex_sptr.h>
#include <vtol/vtol_vertex_2d.h>
#include <brip/brip_vil1_float_ops.h>
#include <strk/strk_tracking_face_2d.h>

#if 0 // unused static function
//: Gives a sort on mutual information in decreasing order
static bool info_compare(strk_tracking_face_2d_sptr const f1,
                         strk_tracking_face_2d_sptr const f2)
{
  return f1->total_info() > f2->total_info();//JLM Switched
}
#endif // 0

//---------------------------------------------------------------
// Constructors
//
//----------------------------------------------------------------

//: constructor from a parameter block (the only way)
//
strk_region_info::strk_region_info(strk_region_info_params& tp)
  : strk_region_info_params(tp)
{
}

//:Default Destructor
strk_region_info::~strk_region_info()
{
}

//-------------------------------------------------------------------------
//: Set the previous frame image
//
void strk_region_info::set_image_0(vil1_image& image)
{
  if (!image)
  {
    vcl_cout <<"In strk_region_info::set_image_i(.) - null input\n";
    return;
  }

  vil1_memory_image_of<float> in, hue, sat;
  int w = image.width(), h = image.height();

  if (!color_info_||image.components()==1)
  {
    vil1_memory_image_of<float> in=brip_vil1_float_ops::convert_to_float(image);
    image_0_= brip_vil1_float_ops::gaussian(in, sigma_);
  }

  if (color_info_&&image.components()==3)
  {
    vil1_memory_image_of<vil1_rgb<unsigned char> > temp(image);
    brip_vil1_float_ops::convert_to_IHS(temp, in, hue, sat);
    image_0_= brip_vil1_float_ops::gaussian(in, sigma_);
    hue_0_ = brip_vil1_float_ops::gaussian(hue, sigma_);
    sat_0_ = brip_vil1_float_ops::gaussian(sat, sigma_);
  }

  if (gradient_info_)
  {
    Ix_0_.resize(w,h);
    Iy_0_.resize(w,h);
    brip_vil1_float_ops::gradient_3x3(image_0_, Ix_0_, Iy_0_);
  }
}

//-------------------------------------------------------------------------
//: Set the next frame image
//
void strk_region_info::set_image_i(vil1_image& image)
{
  if (!image)
  {
    vcl_cout <<"In strk_region_info::set_image_i(.) - null input\n";
    return;
  }
  vgl_h_matrix_2d<double> H;
  bool mapi = this->map_i_to_0(H);

  int w = image.width(), h = image.height();

  if (!color_info_||image.components()==1)
  {
    vil1_memory_image_of<float> in=brip_vil1_float_ops::convert_to_float(image);
    vil1_memory_image_of<float> out(w, h);
    if (mapi)
      brip_vil1_float_ops::homography(in, H, out, true);
    else
      out = in;
    image_i_= brip_vil1_float_ops::gaussian(out, sigma_);
  }

  if (color_info_&&image.components()==3)
  {
    vil1_memory_image_of<vil1_rgb<unsigned char> > temp(image);
    vil1_memory_image_of<float> in, hue, sat;
    brip_vil1_float_ops::convert_to_IHS(temp, in, hue, sat);
    vil1_memory_image_of<float> in_out(w, h), hue_out(w,h), sat_out(w,h);
    if (mapi)
    {
      brip_vil1_float_ops::homography(in, H, in_out, true);
      brip_vil1_float_ops::homography(hue, H, hue_out, true);
      brip_vil1_float_ops::homography(sat, H, sat_out, true);
    }
    else
    {
      in_out = in;
      hue_out = hue;
      sat_out = sat;
    }
    image_i_= brip_vil1_float_ops::gaussian(in_out, sigma_);
    hue_i_ = brip_vil1_float_ops::gaussian(hue_out, sigma_);
    sat_i_ = brip_vil1_float_ops::gaussian(sat_out, sigma_);
  }

  if (gradient_info_)
  {
    Ix_i_.resize(w,h);
    Iy_i_.resize(w,h);
    brip_vil1_float_ops::gradient_3x3(image_i_, Ix_i_, Iy_i_);
  }
}

//--------------------------------------------------------------------------
//: Set the polygonal region for image 0
void strk_region_info::set_face_0(vtol_face_2d_sptr const& face)
{
  face_0_ = face;
}

//--------------------------------------------------------------------------
//: Set the polygonal region for image i
void strk_region_info::set_face_i(vtol_face_2d_sptr const& face)
{
  face_i_ = face;
}
//--------------------------------------------------------------------------
//: Set the background region for image i
void strk_region_info::set_background_face(vtol_face_2d_sptr const& face)
{
  background_face_ = face;
}

static vtol_face_2d_sptr image_face(vil1_memory_image_of<float> const& image)
{
  int w = image.width()-1, h = image.height()-1;
  vtol_vertex_sptr v0 = new vtol_vertex_2d(0, 0);
  vtol_vertex_sptr v1 = new vtol_vertex_2d(w, 0);
  vtol_vertex_sptr v2 = new vtol_vertex_2d(w, h);
  vtol_vertex_sptr v3 = new vtol_vertex_2d(0, h);
  vcl_vector<vtol_vertex_sptr> verts;
  verts.push_back(v0);   verts.push_back(v1);
  verts.push_back(v2);   verts.push_back(v3);
  vtol_face_2d_sptr f = new vtol_face_2d(verts);
  return f;
}
//--------------------------------------------------------------------------
//: Evaluate information between I_0 and I_i at the initial region
//  Useful for debugging purposes.
void strk_region_info::evaluate_info()
{
  if (!image_0_||!image_i_)
    return;
  vtol_face_2d_sptr f = face_0_;
  if (!f)
    f = image_face(image_0_);

  strk_tracking_face_2d_sptr tf
    = new strk_tracking_face_2d(f, image_0_,
                                Ix_0_, Iy_0_, hue_0_, sat_0_,
                                min_gradient_,
                                parzen_sigma_);
  if (!tf->compute_mutual_information(image_i_, Ix_i_, Iy_i_, hue_i_, sat_i_))
    return;

  if (verbose_)
    vcl_cout << "Total Inf = " << tf->total_info()
             << " = IntInfo(" <<  tf->int_mutual_info()
             << ") + GradInfo(" <<  tf->grad_mutual_info()
             << ") + ColorInfo(" <<  tf->color_mutual_info()
             << ")\n" << vcl_flush;

  if (debug_)
    vcl_cout << "model_intensity_entropy = " << tf->model_intensity_entropy()
             << "\nintensity_entropy = " << tf->intensity_entropy()
             << "\nintensity_joint_entropy = " <<tf->intensity_joint_entropy()
             << "\n\nmodel_gradient_entropy = " << tf->model_gradient_entropy()
             << "\ngradient_entropy = " << tf->gradient_entropy()
             << "\ngradient_joint_entropy = " << tf->gradient_joint_entropy()
             << "\n\nmodel_color_entropy = " << tf->model_color_entropy()
             << "\ncolor_entropy = " << tf->color_entropy()
             << "\ncolor_joint_entropy = " << tf->color_joint_entropy()
             << "\n\n\n"<< vcl_flush;
}
//--------------------------------------------------------------------------
//: Evaluate background/forground information in image_0_
//  Useful for debugging purposes.
void strk_region_info::evaluate_background_info()
{
  if (!image_0_ || !face_0_)
    return;

  strk_tracking_face_2d_sptr tf
    = new strk_tracking_face_2d(face_0_, image_0_,
                                Ix_0_, Iy_0_, hue_0_, sat_0_,
                                min_gradient_,
                                parzen_sigma_);
  tf->print_intensity_histograms(image_0_);

  if (!background_face_)
    return;

  strk_tracking_face_2d_sptr bf
    = new strk_tracking_face_2d(background_face_, image_0_,
                                Ix_0_, Iy_0_, hue_0_, sat_0_,
                                min_gradient_,
                                parzen_sigma_);
  bf->print_intensity_histograms(image_0_);
  float x = tf->intensity_mutual_info_diff(bf, image_0_, true);
  vcl_cout << "intensity_mutual_info_diff = " << x << vcl_endl;
}

//: map the intensities from image i to the coordinate frame of image 0.
// assumes a homography between the images is defined by a face with
// 4 vertices
bool strk_region_info::map_i_to_0(vgl_h_matrix_2d<double>& H)
{
  if (!face_0_||!face_i_)
    return false;
  //get the vertices
  vcl_vector<vtol_vertex_sptr> verts0, vertsi;
  face_0_->vertices(verts0);
  face_i_->vertices(vertsi);
  int n0 = verts0.size(), ni = vertsi.size();
  if (n0!=4||ni!=4)
    return false;
  //convert to homogeneous coordinates
  vcl_vector<vgl_homg_point_2d<double> > pts_0, pts_i;
  for (int k = 0; k<4; k++)
  {
    vtol_vertex_2d_sptr v0 = verts0[k]->cast_to_vertex_2d(),
                        vi = vertsi[k]->cast_to_vertex_2d();
    vgl_homg_point_2d<double> hp0(v0->x(), v0->y()), hpi(vi->x(), vi->y());
    pts_0.push_back(hp0);   pts_i.push_back(hpi);
  }
  //compute the homography from image i to image 0
  vgl_h_matrix_2d_compute_4point hc;
  hc.compute(pts_i, pts_0, H);
  return true;
}

vil1_memory_image_of<unsigned char> strk_region_info::image_0()
{
  return brip_vil1_float_ops::convert_to_byte(image_0_, 0, 255);
}

vil1_memory_image_of<unsigned char> strk_region_info::image_i()
{
  return brip_vil1_float_ops::convert_to_byte(image_i_, 0, 255);
}
