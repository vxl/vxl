#include "bvxm_util.h"

#include <vcl_iostream.h>
#include <vcl_fstream.h>
#include <vcl_cmath.h>
#include <vcl_string.h>
#include <vcl_vector.h>
#include <vpgl/vpgl_camera.h>
#include <vnl/vnl_double_3x3.h>
#include <vnl/vnl_double_3x1.h>
#include <vnl/algo/vnl_gaussian_kernel_1d.h>
#include <vil/vil_resample_bilin.h>
#include <vpgl/vpgl_local_rational_camera.h>
#include <sdet/sdet_detector.h>
#include <vtol/vtol_edge_2d.h>
#include <vil/vil_new.h>
#include <vdgl/vdgl_digital_curve.h>
#include <vdgl/vdgl_edgel.h>
#include <vdgl/vdgl_edgel_chain.h>
#include <vdgl/vdgl_interpolator.h>
#include <bil/algo/bil_edt.h>
#include <vgl/vgl_box_2d.h>
#include <vgl/vgl_box_3d.h>

#include "bvxm_voxel_slab.h"
#include "bvxm_world_params.h"


bool bvxm_util::read_cameras(const vcl_string filename, vcl_vector<vnl_double_3x3> &Ks, vcl_vector<vnl_double_3x3> &Rs, vcl_vector<vnl_double_3x1> &Ts)
{
  vcl_ifstream file_inp(filename.c_str());
  if (!file_inp.good()) {
    vcl_cerr << "error opening file "<< filename <<vcl_endl;
    return false;
  }
  unsigned ncameras;

  file_inp >> ncameras;
  for (unsigned i=0; i < ncameras; i++) {
    vnl_double_3x3 K,R;
    vnl_double_3x1 T;
    file_inp >> K;
    file_inp >> R;
    file_inp >> T;
    Ks.push_back(K);
    Rs.push_back(R);
    Ts.push_back(T);
  }
  file_inp.close();

  return true;
}


bool bvxm_util::write_cameras(const vcl_string filename, vcl_vector<vnl_double_3x3> &Ks, vcl_vector<vnl_double_3x3> &Rs, vcl_vector<vnl_double_3x1> &Ts)
{
  vcl_ofstream file_out(filename.c_str());
  if (!file_out.good()) {
    vcl_cerr << "error opening file "<< filename <<vcl_endl;
    return false;
  }
  unsigned ncameras = Ks.size();

  file_out << ncameras << vcl_endl << vcl_endl;
  for (unsigned i=0; i < ncameras; i++) {
    file_out << Ks[i] << vcl_endl
             << Rs[i] << vcl_endl
             << Ts[i] << vcl_endl
             << vcl_endl;
  }
  file_out.close();

  return true;
}


void bvxm_util::bilinear_weights(vgl_h_matrix_2d<double> invH, unsigned nx_out, unsigned ny_out, vnl_matrix<unsigned> &xvals, vnl_matrix<unsigned> &yvals, vnl_matrix<float> &weights)
{
  // perform bilinear interpolation.
  vnl_matrix_fixed<float,3,3> H;
  vnl_matrix_fixed<double,3,3> Hd = invH.get_matrix();
  // convert H to a float matrix
  vnl_matrix_fixed<float,3,3>::iterator Hit = H.begin();
  vnl_matrix_fixed<double,3,3>::iterator Hdit = Hd.begin();
  for (; Hit != H.end(); ++Hit, ++Hdit)
    *Hit = (float)(*Hdit);

  unsigned npix_out = nx_out * ny_out;

  vnl_matrix<float> pix_out_homg(3,npix_out);
  unsigned col_idx = 0;
  for (unsigned y=0; y<ny_out; ++y) {
    for (unsigned x=0; x<nx_out; ++x, ++col_idx) {
      pix_out_homg.set_column(col_idx,vnl_vector_fixed<float,3>((float)x,(float)y,1.0f));
    }
  }
  vnl_matrix<float> pix_in_homg = H*pix_out_homg;
  xvals = vnl_matrix<unsigned>(4,npix_out);
  yvals = vnl_matrix<unsigned>(4,npix_out);
  weights = vnl_matrix<float>(4,npix_out);

  for (unsigned n=0; n<npix_out; ++n) {
    // normalize homogeneous coordinate
    float pix_in_x = pix_in_homg[0][n] / pix_in_homg[2][n];
    float pix_in_y = pix_in_homg[1][n] / pix_in_homg[2][n];
    // calculate weights and pixel values
    unsigned x0 = (unsigned)vcl_floor(pix_in_x);
    unsigned x1 = (unsigned)vcl_ceil(pix_in_x);
    float x0_weight = (float)(x1 - pix_in_x);
    float x1_weight = (float)(1.0f - x0_weight);
    unsigned y0 = (unsigned)vcl_floor(pix_in_y);
    unsigned y1 = (unsigned)vcl_ceil(pix_in_y);
    float y0_weight = (float)(y1 - pix_in_y);
    float y1_weight = (float)(1.0f - y0_weight);
    xvals.set_column(n,vnl_vector_fixed<unsigned,4>(x0,x0,x1,x1));
    yvals.set_column(n,vnl_vector_fixed<unsigned,4>(y0,y1,y0,y1));
    weights.set_column(n,vnl_vector_fixed<float,4>(x0_weight*y0_weight,
                                                   x0_weight*y1_weight,
                                                   x1_weight*y0_weight,
                                                   x1_weight*y1_weight));
  }
}


vil_image_view_base_sptr bvxm_util::downsample_image_by_two(vil_image_view_base_sptr image)
{
  vil_image_view<float>*img_view_float = new vil_image_view<float>(image->ni(),image->nj(),image->nplanes());

  if (image->pixel_format() == VIL_PIXEL_FORMAT_BYTE)
  {
    vil_image_view<unsigned char> *img_view_char = dynamic_cast<vil_image_view<unsigned char>*>(image.ptr());
    vil_convert_cast<unsigned char,float>(*img_view_char,*img_view_float);
  }
  else if (image->pixel_format() == VIL_PIXEL_FORMAT_FLOAT)
  {
    img_view_float = dynamic_cast<vil_image_view<float>*>(image.ptr());
  }

  vil_image_view<float> output((int)vcl_floor((float)img_view_float->ni()/2),
                               (int)vcl_floor((float)img_view_float->nj()/2),
                               img_view_float->nplanes());

#if 0
  vil_image_view<unsigned char>* img_view_out
    = bvxm_multiscale_util::half_resolution(*img_view_float,0.359375f);

  float min_b,max_b;
  vil_math_value_range(*img_view_float,min_b,max_b);
#endif // 0

  vil_resample_bilin<float,float>(*img_view_float,output,output.ni(),output.nj());
  vil_image_view<unsigned char>* img_view_out = new vil_image_view<unsigned char>(output.ni(),output.nj(),output.nplanes());
  img_view_out->fill(0);

  for (unsigned p=0; p<output.nplanes(); p++)
    for (unsigned i=0;i<output.ni();i++)
      for (unsigned j=0;j<output.nj();j++)
      {
        if (output(i,j,p)>255.0)
          (*img_view_out)(i,j,p)=255;
        else if (output(i,j,p)<0.0)
          (*img_view_out)(i,j,p)=0;
        else
          (*img_view_out)(i,j,p)=static_cast<vxl_byte>(output(i,j)+0.5);

        //vcl_cout<<(*img_view_out)(i,j);
      }
  ////vil_convert_stretch_range_limited<float>(output,*img_view_out,min_b,max_b);
  vil_image_view_base_sptr return_img=img_view_out;
  return return_img;
}


vpgl_camera_double_sptr bvxm_util::downsample_camera(vpgl_camera_double_sptr camera,unsigned int scale)
{
  if ( vpgl_local_rational_camera<double>* rat_camera = dynamic_cast<vpgl_local_rational_camera<double>*> (camera.as_pointer()))
  {
    vpgl_local_rational_camera<double>* new_rat_camera=new vpgl_local_rational_camera<double>(*rat_camera);
    double u_s,v_s;
    float factor=  vcl_pow(2.0f,-((float)scale));
    rat_camera->image_scale(u_s,v_s);
    new_rat_camera->set_image_scale(u_s*factor,v_s*factor);

    double u_off,v_off;
    rat_camera->image_offset(u_off,v_off);
    new_rat_camera->set_image_offset(u_off*factor,v_off*factor);

    return new_rat_camera;
  }
  else
  {
    return NULL;
  }
}

vpgl_camera_double_sptr bvxm_util::downsample_persp_camera(vpgl_camera_double_sptr camera,unsigned int scale)
{
  if ( vpgl_perspective_camera<double>* persp_camera =    dynamic_cast<vpgl_perspective_camera<double>*> (camera.as_pointer()))
  {
    vpgl_perspective_camera<double>* new_persp_camera=new vpgl_perspective_camera<double>(*persp_camera);
    vnl_matrix_fixed<double,3,4> camera_matrix = persp_camera->get_matrix();
    vnl_matrix_fixed<double,3,3> scale_matrix;
    scale_matrix.fill(0.0);
    float factor=  vcl_pow(2.0f,-((float)scale));

    scale_matrix.put(0,0,1*factor);
    scale_matrix.put(1,1,1*factor);
    scale_matrix.put(2,2,1.0);

    vnl_matrix_fixed<double,3,4> new_camera_matrix = scale_matrix*camera_matrix;

    new_persp_camera->set_matrix(new_camera_matrix);
    return new_persp_camera;
  }
  else
  {
    return NULL;
  }
}

void bvxm_util::logical_and(bvxm_voxel_slab<bool> const& s1, bvxm_voxel_slab<bool> const& s2, bvxm_voxel_slab<bool> &result)
{
  // check sizes
  if ( (result.nx() != s1.nx()) || (result.nx() != s2.nx()) ||
       (result.ny() != s1.ny()) || (result.ny() != s2.ny()) ||
       (result.nz() != s1.nz()) || (result.nz() != s2.nz()) ) {
    vcl_cerr << "error: sizes of slabs to AND do not match.\n";
    return;
  }

  bvxm_voxel_slab<bool>::const_iterator s1_it = s1.begin(), s2_it = s2.begin();
  bvxm_voxel_slab<bool>::iterator result_it = result.begin();
  for (; result_it != result.end(); ++s1_it, ++s2_it, ++result_it) {
    *result_it = *s1_it && *s2_it;
  }
  return;
}

vil_image_view<float> bvxm_util::multiply_image_with_gaussian_kernel(vil_image_view<float> img, double gaussian_sigma)
{
  vil_image_view<float> ret_img(img.ni(),img.nj(),1);

  vnl_gaussian_kernel_1d gaussian(gaussian_sigma);

  for (unsigned i=0; i<img.ni(); i++) {
    for (unsigned j=0; j<img.nj(); j++) {
      ret_img(i,j) = (float)gaussian.G((double)img(i,j));
    }
  }

  return ret_img;
}

vil_image_view<vxl_byte> bvxm_util::detect_edges(vil_image_view<vxl_byte> img,
                                                 double noise_multiplier,
                                                 double smooth,
                                                 bool automatic_threshold,
                                                 bool junctionp,
                                                 bool aggressive_junction_closure)
{
  if ( img.nplanes() >= 3 )
  {
    vil_image_view<vxl_byte> img_rgb;
    img_rgb.deep_copy(img);
    vil_convert_planes_to_grey(img_rgb,img);
  }

  // set parameters for the edge detector
  sdet_detector_params dp;
  dp.noise_multiplier = (float)noise_multiplier;
  dp.smooth = (float)smooth;
  dp.automatic_threshold = automatic_threshold;
  dp.junctionp = junctionp;
  dp.aggressive_junction_closure = aggressive_junction_closure;

  // detect edgels from the input image
  sdet_detector detector(dp);
  vil_image_resource_sptr img_res_sptr = vil_new_image_resource_of_view(img);
  detector.SetImage(img_res_sptr);
  detector.DoContour();
  vcl_vector<vtol_edge_2d_sptr> * edges = detector.GetEdges();

  // initialize the output edge image
  vil_image_view<vxl_byte> img_edge(img.ni(),img.nj(),1);
  img_edge.fill(0);

  // iterate over each connected edge component
  for (vcl_vector<vtol_edge_2d_sptr>::iterator eit = edges->begin(); eit != edges->end(); eit++)
  {
    vsol_curve_2d_sptr c = (*eit)->curve();
    vdgl_digital_curve_sptr dc = c->cast_to_vdgl_digital_curve();
    if (!dc)
      continue;
    vdgl_interpolator_sptr intp = dc->get_interpolator();
    vdgl_edgel_chain_sptr ec = intp->get_edgel_chain();

    // iterate over each point in the connected edge component
    for (unsigned j=0; j<ec->size(); j++) {
      vdgl_edgel curr_edgel = ec->edgel(j);
      int cr_x = (int)curr_edgel.x();
      int cr_y = (int)curr_edgel.y();

      // set the current edge pixel in the edge image
      img_edge(cr_x,cr_y) = 255;
    }
  }

  // Following loop removes the edges in the image boundary
  int temp_index = img_edge.nj()-1;
  for (unsigned i=0; i<img_edge.ni(); i++) {
    img_edge(i,0) = 0;
    img_edge(i,temp_index) = 0;
  }
  temp_index = img_edge.ni()-1;
  for (unsigned j=0; j<img_edge.nj(); j++) {
    img_edge(0,j) = 0;
    img_edge(temp_index,j) = 0;
  }

  return img_edge;
}

void bvxm_util::edge_distance_transform(vil_image_view<vxl_byte>& inp_image, vil_image_view<float>& out_edt)
{
  vil_image_view<vxl_byte> edge_image_negated(inp_image);
  vil_math_scale_and_offset_values(edge_image_negated,-1.0,255);

  unsigned ni = edge_image_negated.ni();
  unsigned nj = edge_image_negated.nj();

  vil_image_view<vxl_uint_32> curr_image_edt(ni,nj,1);
  for (unsigned i=0; i<ni; i++) {
    for (unsigned j=0; j<nj; j++) {
      curr_image_edt(i,j) = edge_image_negated(i,j);
    }
  }

  bil_edt_maurer(curr_image_edt);

  out_edt.set_size(ni,nj,1);
  for (unsigned i=0; i<ni; i++) {
    for (unsigned j=0; j<nj; j++) {
      out_edt(i,j) = vcl_sqrt((float)curr_image_edt(i,j));
    }
  }
}

int bvxm_util::convert_uncertainty_from_meters_to_pixels(float uncertainty, bgeo_lvcs_sptr lvcs, vpgl_camera_double_sptr camera)
{
  // estimate the offset search size in the image space
  vgl_box_3d<double> box_uncertainty(-uncertainty,-uncertainty,-uncertainty,uncertainty,uncertainty,uncertainty);
  vcl_vector<vgl_point_3d<double> > box_uncertainty_corners = bvxm_util::corners_of_box_3d<double>(box_uncertainty);
  vgl_box_2d<double>* roi_uncertainty = new vgl_box_2d<double>();

  for (unsigned i=0; i<box_uncertainty_corners.size(); i++) {
    vgl_point_3d<double> curr_corner = box_uncertainty_corners[i];
    vgl_point_3d<double> curr_pt;
    if (camera->type_name()=="vpgl_local_rational_camera") {
      curr_pt.set(curr_corner.x(),curr_corner.y(),curr_corner.z());
    }
    else if (camera->type_name()=="vpgl_rational_camera") {
      double lon, lat, gz;
      lvcs->local_to_global(curr_corner.x(), curr_corner.y(), curr_corner.z(),
                          bgeo_lvcs::wgs84, lon, lat, gz, bgeo_lvcs::DEG, bgeo_lvcs::METERS);
      curr_pt.set(lon, lat, gz);
    }
    else // dummy initialisation, to avoid compiler warning
      curr_pt.set(-1e99, -1e99, -1.0);

    double curr_u,curr_v;
    camera->project(curr_pt.x(),curr_pt.y(),curr_pt.z(),curr_u,curr_v);
    vgl_point_2d<double> p2d_uncertainty(curr_u,curr_v);
    roi_uncertainty->add(p2d_uncertainty);
  }

  return vnl_math_ceil(0.5*vnl_math_max(roi_uncertainty->width(),roi_uncertainty->height()));
}
