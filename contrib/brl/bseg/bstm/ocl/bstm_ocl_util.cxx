#include "bstm_ocl_util.h"
#include <bstm/bstm_util.h>
#include <bstm/bstm_data_traits.h>
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

// fills a float buffer (should be 16*3 floats) with a perspective cam to be sent
void bstm_ocl_util::set_ocl_camera(vpgl_camera_double_sptr& cam, cl_float* ocl_cam)
{
  if (auto* pcam =
      dynamic_cast<vpgl_perspective_camera<double>* >(cam.ptr()))
  {
    set_persp_camera(pcam, ocl_cam);
  }
  else if (auto* pcam =
           dynamic_cast<vpgl_proj_camera<double>* >(cam.ptr()))
  {
    set_proj_camera(pcam, ocl_cam);
  }
  else {
    std::cerr << "Error set_persp_camera() : unsupported camera type\n";
  }
}

// fills a float buffer (should be 16*3 floats) with a perspective cam to be sent
void bstm_ocl_util::set_persp_camera(vpgl_perspective_camera<double> * pcam, cl_float* persp_cam)
{
  //std::cout<<"CAM: "<<(*pcam)<<std::endl;
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
void bstm_ocl_util::set_proj_camera(vpgl_proj_camera<double> * pcam, cl_float* cam)
{
  //std::cout<<"CAM: "<<(*pcam)<<std::endl;
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

void bstm_ocl_util::set_generic_camera(vpgl_camera_double_sptr& cam, cl_float* ray_origins, cl_float* ray_directions, unsigned cl_ni, unsigned cl_nj)
{
  if (auto* gcam =
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
    std::cerr << "Error set_generic_camera() : unsupported camera type\n";
  }
}


// fills in a 256 char array with number of BITS for each value (255 = 8, 254 = 7 etc)
void bstm_ocl_util::set_bit_lookup(cl_uchar* lookup)
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
  std::memcpy(lookup, bits, 256);
}

void
bstm_ocl_util::load_perspective_camera(const std::string& filename, vpgl_perspective_camera<double> & pcam)
{
  std::ifstream ifile(filename.c_str());
  if (!ifile)
  {
      std::cout<<"Error Opening Camera File "<<filename<<std::endl;
      return ;
  }
  ifile>>pcam;
  ifile.close();
}

void bstm_ocl_util::get_render_transfer_function(std::vector<std::string> imgfilenames, float & min, float & max, vbl_array_1d<float> & tf)
{
  min= 3e38f;
  max=-3e38f;
  unsigned int interval=1;

  if (imgfilenames.size()>10)
    interval=imgfilenames.size()/10;

  // this loop obtains min and max over all images.
  for (unsigned i=0;i<imgfilenames.size();i+=interval)
  {
    vil_image_view_base_sptr img_ptr=bstm_util::prepare_input_image(imgfilenames[i]);
    if (auto * imgf=dynamic_cast<vil_image_view<float>*>(img_ptr.ptr()))
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
    vil_image_view_base_sptr img_ptr=bstm_util::prepare_input_image(imgfilenames[i]);
    if ( auto * imgf=dynamic_cast<vil_image_view<float>*>(img_ptr.ptr()))
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

std::string bstm_ocl_util::mog_options(const std::string& data_type)
{
  if ( data_type == bstm_data_traits<BSTM_MOG3_GREY>::prefix() )
    return "-D MOG_TYPE_8 ";
  else if ( data_type == bstm_data_traits<BSTM_MOG6_VIEW>::prefix())
    return "-D MOG_VIEW_DEP ";
  else if ( data_type == bstm_data_traits<BSTM_MOG6_VIEW_COMPACT>::prefix())
    return "-D MOG_VIEW_DEP_COMPACT ";
  else if ( data_type == bstm_data_traits<BSTM_GAUSS_RGB>::prefix())
    return "-D MOG_TYPE_8 ";
  else if ( data_type == bstm_data_traits<BSTM_GAUSS_RGB_VIEW_COMPACT>::prefix())
    return "-D MOG_VIEW_DEP_COLOR_COMPACT ";
  else if ( data_type == bstm_data_traits<BSTM_CHANGE>::prefix())
    return "-D APP_CHANGE ";
  else
    return "";
}

std::string bstm_ocl_util::label_options(const std::string& data_type)
{
  if ( data_type == bstm_data_traits<BSTM_LABEL>::prefix() )
    return "-D LABEL_UCHAR ";
  else
    return "";
}
