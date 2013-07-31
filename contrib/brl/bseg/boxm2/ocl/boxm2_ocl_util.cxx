#include "boxm2_ocl_util.h"
#include <boxm2/boxm2_util.h>
#include <boxm2/boxm2_data_traits.h>
//:
// \file
#include <vgl/vgl_point_3d.h>
#include <vnl/vnl_vector.h>
#include <vnl/vnl_matrix.h>
#include <vnl/algo/vnl_svd.h>
#include <vil/vil_image_view.h>
#include <vil/vil_math.h>
#include <vpgl/vpgl_perspective_camera.h>
#include <vpgl/vpgl_calibration_matrix.h>
#include <bsta/bsta_histogram.h>

//for root dir
#include <vcl_where_root_dir.h>
#include "boxm2_ocl_where_root_dir.h"

void boxm2_ocl_util::set_dodecahedron_dir_lookup(cl_float4* dodecahedron_dir)
{
  float golden = 0.8507f; //the golden ratio
  float other = 0.5257f;

  float data[] = {  0,     -golden, -other,  0,
                    0,     -golden,  other,  0,
                    0,      golden, -other,  0,
                    0,      golden,  other,  0,
                   -other,  0,      -golden, 0,
                   -other,  0,       golden, 0,
                    other,  0,      -golden, 0,
                    other,  0,       golden, 0,
                   -golden, -other,  0,      0,
                   -golden,  other,  0,      0,
                    golden, -other,  0,      0,
                    golden,  other,  0,      0 };

#ifdef CL_ALIGNED
  for (int i=0; i<12; ++i)
    for (int j=0; j<4; ++j)
      dodecahedron_dir[i].s[j] = data[4*i+j];
#else // assuming cl_float4 is a typedef for float[4]
  for (int i=0; i<12; ++i) {
    float* d = static_cast<float*>(dodecahedron_dir[i]);
    for (int j=0; j<4; ++j)
      d[j] = data[4*i+j];
  }
#endif // CL_ALIGNED
}

void boxm2_ocl_util::set_dodecahedron_orientations_lookup(cl_float4* dodecahedron_dir)
{
  float golden = 0.8507f; //the golden ratio
  float other = 0.5257f;

  float data[] = { 0,      golden, -other,  0,
                   0,      golden,  other,  0,
                   other,  0,      -golden, 0,
                   other,  0,       golden, 0,
                   golden, -other, 0,       0,
                   golden,  other, 0,       0 };

#ifdef CL_ALIGNED
  for (int i=0; i<6; ++i)
    for (int j=0; j<4; ++j)
      dodecahedron_dir[i].s[j] = data[4*i+j];
#else // assuming cl_float4 is a typedef for float[4]
  for (int i=0; i<6; ++i) {
    float* d = static_cast<float*>(dodecahedron_dir[i]);
    for (int j=0; j<4; ++j)
      d[j] = data[4*i+j];
  }
#endif // CL_ALIGNED
}


//returns path to opencl src (cl files)
vcl_string boxm2_ocl_util::ocl_src_root()
{
  vcl_string boxm2_ocl_src_dir(BOXM2_OPENCL_SOURCE_DIR);
  if ( boxm2_ocl_src_dir == "" )
    boxm2_ocl_src_dir = vcl_string(VCL_SOURCE_ROOT_DIR) + "/contrib/brl/bseg/boxm2/ocl/cl/";
  return boxm2_ocl_src_dir;
}

// fills a float buffer (should be 16*3 floats) with a perspective cam to be sent
void boxm2_ocl_util::set_ocl_camera(vpgl_camera_double_sptr& cam, cl_float* ocl_cam)
{
  if (vpgl_perspective_camera<double>* pcam =
      dynamic_cast<vpgl_perspective_camera<double>* >(cam.ptr()))
  {
    set_persp_camera(pcam, ocl_cam);
  }
  else if (vpgl_proj_camera<double>* pcam =
           dynamic_cast<vpgl_proj_camera<double>* >(cam.ptr()))
  {
    set_proj_camera(pcam, ocl_cam);
  }
  else {
    vcl_cerr << "Error set_persp_camera() : unsupported camera type\n";
  }
}

// fills a float buffer (should be 16*3 floats) with a perspective cam to be sent
void boxm2_ocl_util::set_persp_camera(vpgl_perspective_camera<double> * pcam, cl_float* persp_cam)
{
  //vcl_cout<<"CAM: "<<(*pcam)<<vcl_endl;
  vnl_svd<double>* svd=pcam->svd();
  vnl_matrix<double> Ut=svd->U().conjugate_transpose();
  vnl_matrix<double> V=svd->V();
  vnl_vector<double> Winv=svd->Winverse().diagonal();

  //U matrix stored here (16 floats)
  int cnt=0;
  for (unsigned i=0;i<Ut.rows();++i)
  {
    for (unsigned j=0;j<Ut.cols();++j)
      persp_cam[cnt++]=(cl_float)Ut(i,j);

    persp_cam[cnt++]=0;
  }

  //store V matrix (16 floats)
  for (unsigned i=0;i<V.rows();++i)
    for (unsigned j=0;j<V.cols();++j)
      persp_cam[cnt++]=(cl_float)V(i,j);

  //store W matrix (4 floats)
  for (unsigned i=0;i<Winv.size();++i)
    persp_cam[cnt++]=(cl_float)Winv(i);

  //store cam center (4 floats)
  vgl_point_3d<double> cam_center=pcam->camera_center();
  persp_cam[cnt++]=(cl_float)cam_center.x();
  persp_cam[cnt++]=(cl_float)cam_center.y();
  persp_cam[cnt++]=(cl_float)cam_center.z();
  persp_cam[cnt++]=(cl_float) 0.0f;

  //store [focal length, focal length, principal point] (4 floats)
  const vpgl_calibration_matrix<double>& K = pcam->get_calibration();
  persp_cam[cnt++] = K.x_scale()*K.focal_length();
  persp_cam[cnt++] = K.y_scale()*K.focal_length();
  persp_cam[cnt++] = K.principal_point().x();
  persp_cam[cnt++] = K.principal_point().y();
}
// fills a float buffer (should be 16*3 floats) with a perspective cam to be sent
void boxm2_ocl_util::set_proj_camera(vpgl_proj_camera<double> * pcam, cl_float* cam)
{
  //vcl_cout<<"CAM: "<<(*pcam)<<vcl_endl;
  vnl_svd<double>* svd=pcam->svd();
  vnl_matrix<double> Ut=svd->U().conjugate_transpose();
  vnl_matrix<double> V=svd->V();
  vnl_vector<double> Winv=svd->Winverse().diagonal();

  //U matrix stored here (16 floats)
  int cnt=0;
  for (unsigned i=0;i<Ut.rows();++i)
  {
    for (unsigned j=0;j<Ut.cols();++j)
      cam[cnt++]=(cl_float)Ut(i,j);
    cam[cnt++]=0;
  }

  //store V matrix (16 floats)
  for (unsigned i=0;i<V.rows();++i)
    for (unsigned j=0;j<V.cols();++j)
      cam[cnt++]=(cl_float)V(i,j);

  //store W matrix (4 floats)
  for (unsigned i=0;i<Winv.size();++i)
    cam[cnt++]=(cl_float)Winv(i);

  //store cam center (4 floats)
  vgl_point_3d<double> cam_center=pcam->camera_center();
  cam[cnt++]=(cl_float)cam_center.x();
  cam[cnt++]=(cl_float)cam_center.y();
  cam[cnt++]=(cl_float)cam_center.z();
  cam[cnt++]=(cl_float) 0.0f;
#if 0 // not valiud for projective camer
  //store [focal length, focal length, principal point] (4 floats)
  const vpgl_calibration_matrix<double>& K = pcam->get_calibration();
  cam[cnt++] = K.x_scale()*K.focal_length();
  cam[cnt++] = K.y_scale()*K.focal_length();
  cam[cnt++] = K.principal_point().x();
  cam[cnt++] = K.principal_point().y();
#endif
}

void boxm2_ocl_util::set_generic_camera(vpgl_camera_double_sptr& cam, cl_float* ray_origins, cl_float* ray_directions, unsigned cl_ni, unsigned cl_nj)
{
  if (vpgl_generic_camera<double>* gcam =
      dynamic_cast<vpgl_generic_camera<double>* >(cam.ptr()))
  {
    // iterate through each ray and record origin/direction
    int count = 0;
    for (unsigned int v=0; v<cl_nj; ++v) {
      for (unsigned int u=0; u<cl_ni; ++u, ++count) {
        vgl_ray_3d<double> ray;
        if (u>=gcam->cols()||v>=gcam->rows()) {
          vgl_point_3d<double> p(0,0,0); // dummy
          vgl_vector_3d<double> t(0,0,0);   // dummy
          ray = vgl_ray_3d<double>(p, t);
        }
        else {
          // the ray corresponding to a given pixel
          ray = gcam->ray(u, v);
        }

        // origin
        ray_origins[4*count  ] = static_cast<float>(ray.origin().x());
        ray_origins[4*count+1] = static_cast<float>(ray.origin().y());
        ray_origins[4*count+2] = static_cast<float>(ray.origin().z());
        ray_origins[4*count+3] = 1.0f;

        // direction
        ray_directions[4*count  ] = static_cast<float>(ray.direction().x());
        ray_directions[4*count+1] = static_cast<float>(ray.direction().y());
        ray_directions[4*count+2] = static_cast<float>(ray.direction().z());
        ray_directions[4*count+3] = 0.0f;
      }
    }
  }
  else {
    vcl_cerr << "Error set_generic_camera() : unsupported camera type\n";
  }
}


// fills in a 256 char array with number of BITS for each value (255 = 8, 254 = 7 etc)
void boxm2_ocl_util::set_bit_lookup(cl_uchar* lookup)
{
  unsigned char bits[] = { 0,   1,   1,   2,   1,   2,   2,   3,   1,   2,   2,   3,   2,   3,   3,   4,
                           1,   2,   2,   3,   2,   3,   3,   4,   2,   3,   3,   4,   3,   4,   4,   5,
                           1,   2,   2,   3,   2,   3,   3,   4,   2,   3,   3,   4,   3,   4,   4,   5,
                           2,   3,   3,   4,   3,   4,   4,   5,   3,   4,   4,   5,   4,   5,   5,   6,
                           1,   2,   2,   3,   2,   3,   3,   4,   2,   3,   3,   4,   3,   4,   4,   5,
                           2,   3,   3,   4,   3,   4,   4,   5,   3,   4,   4,   5,   4,   5,   5,   6,
                           2,   3,   3,   4,   3,   4,   4,   5,   3,   4,   4,   5,   4,   5,   5,   6,
                           3,   4,   4,   5,   4,   5,   5,   6,   4,   5,   5,   6,   5,   6,   6,   7,
                           1,   2,   2,   3,   2,   3,   3,   4,   2,   3,   3,   4,   3,   4,   4,   5,
                           2,   3,   3,   4,   3,   4,   4,   5,   3,   4,   4,   5,   4,   5,   5,   6,
                           2,   3,   3,   4,   3,   4,   4,   5,   3,   4,   4,   5,   4,   5,   5,   6,
                           3,   4,   4,   5,   4,   5,   5,   6,   4,   5,   5,   6,   5,   6,   6,   7,
                           2,   3,   3,   4,   3,   4,   4,   5,   3,   4,   4,   5,   4,   5,   5,   6,
                           3,   4,   4,   5,   4,   5,   5,   6,   4,   5,   5,   6,   5,   6,   6,   7,
                           3,   4,   4,   5,   4,   5,   5,   6,   4,   5,   5,   6,   5,   6,   6,   7,
                           4,   5,   5,   6,   5,   6,   6,   7,   5,   6,   6,   7,   6,   7,   7,   8 };
  vcl_memcpy(lookup, bits, 256);
}

void
boxm2_ocl_util::load_perspective_camera(vcl_string filename, vpgl_perspective_camera<double> & pcam)
{
  vcl_ifstream ifile(filename.c_str());
  if (!ifile)
  {
      vcl_cout<<"Error Opening Camera File "<<filename<<vcl_endl;
      return ;
  }
  ifile>>pcam;
  ifile.close();
}

void boxm2_ocl_util::get_render_transfer_function(vcl_vector<vcl_string> imgfilenames, float & min, float & max, vbl_array_1d<float> & tf)
{
  min= 3e38f;
  max=-3e38f;
  unsigned int interval=1;

  if (imgfilenames.size()>10)
    interval=imgfilenames.size()/10;

  // this loop obtains min and max over all images.
  for (unsigned i=0;i<imgfilenames.size();i+=interval)
  {
    vil_image_view_base_sptr img_ptr=boxm2_util::prepare_input_image(imgfilenames[i]);
    if (vil_image_view<float> * imgf=dynamic_cast<vil_image_view<float>*>(img_ptr.ptr()))
    {
      float mini, maxi;
      vil_math_value_range<float>(*imgf, mini,maxi);
      if (min>mini) min=mini;
      if (max<maxi) max=maxi;
    }
  }

  // create  histogram over intensities over all images.
  bsta_histogram<float> hist(min,max,255);
  vil_image_view<float>::iterator iter;
  for (unsigned i=0;i<imgfilenames.size();i+=interval)
  {
    vil_image_view_base_sptr img_ptr=boxm2_util::prepare_input_image(imgfilenames[i]);
    if ( vil_image_view<float> * imgf=dynamic_cast<vil_image_view<float>*>(img_ptr.ptr()))
      for (iter=imgf->begin();iter!=imgf->end();++iter)
        hist.upcount(*iter,1);
  }
  float sum=0.0f;
  for (unsigned i=0;i<hist.nbins();++i)
  {
    sum+=hist.p(i);
    tf[i]=sum;
  }
}

vcl_string boxm2_ocl_util::mog_options(vcl_string data_type)
{
  if ( data_type == boxm2_data_traits<BOXM2_MOG3_GREY>::prefix() )
    return "-D MOG_TYPE_8 ";
  else if ( data_type == boxm2_data_traits<BOXM2_MOG3_GREY_16>::prefix() )
    return "-D MOG_TYPE_16 ";
  else if ( data_type == boxm2_data_traits<BOXM2_GAUSS_GREY>::prefix() )
    return "-D GAUSS_TYPE_2 ";
  else if ( data_type == boxm2_data_traits<BOXM2_LABEL_SHORT>::prefix() )
    return "-D SHORT ";
  else if ( data_type == boxm2_data_traits<BOXM2_MOG6_VIEW>::prefix() )
    return "-D MOG_VIEW_DEP ";
  else if ( data_type == boxm2_data_traits<BOXM2_MOG6_VIEW_COMPACT>::prefix() )
    return "-D MOG_VIEW_DEP_COMPACT ";
  else if ( data_type == boxm2_data_traits<BOXM2_GAUSS_RGB_VIEW>::prefix() )
     return "-D MOG_VIEW_DEP_COLOR ";
  else if ( data_type == boxm2_data_traits<BOXM2_GAUSS_RGB_VIEW_COMPACT>::prefix() )
     return "-D MOG_VIEW_DEP_COLOR_COMPACT ";
  else
    return "";
}
