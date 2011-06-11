#include "boxm2_ocl_util.h"
//:
// \file
#include <vgl/vgl_point_3d.h>
#include <vnl/vnl_vector.h>
#include <vnl/vnl_matrix.h>
#include <vnl/algo/vnl_svd.h>
#include <vil/vil_convert.h>
#include <vil/vil_image_view.h>
#include <vil/vil_math.h>
#include <vil/vil_load.h>
#include <vpgl/vpgl_perspective_camera.h>
#include <vpgl/vpgl_calibration_matrix.h>
#include <bsta/bsta_histogram.h>


// fills a float buffer (should be 16*3 floats) with a perspective cam to be sent
void boxm2_ocl_util::set_persp_camera(vpgl_camera_double_sptr& cam, cl_float* persp_cam)
{
  if (vpgl_perspective_camera<double>* pcam =
      dynamic_cast<vpgl_perspective_camera<double>* >(cam.ptr()))
  {
    set_persp_camera(pcam, persp_cam);
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


// private helper method prepares an input image to be processed by update
vil_image_view_base_sptr boxm2_ocl_util::prepare_input_image(vcl_string filename)
{
  //load from file
  vil_image_view_base_sptr loaded_image = vil_load(filename.c_str());
  return boxm2_ocl_util::prepare_input_image(loaded_image);
}

// private helper method prepares an input image to be processed by update
vil_image_view_base_sptr boxm2_ocl_util::prepare_input_image(vil_image_view_base_sptr loaded_image)
{
  //load from file

  //then it's an RGB image (assumes byte image...)
  if (loaded_image->nplanes() == 3 || loaded_image->nplanes() == 4)
  {
    vcl_cout<<"preparing rgb image"<<vcl_endl;
    //load image from file and format it into RGBA
    vil_image_view_base_sptr n_planes = vil_convert_to_n_planes(4, loaded_image);
    vil_image_view_base_sptr comp_image = vil_convert_to_component_order(n_planes);
    vil_image_view<vil_rgba<vxl_byte> >* rgba_view = new vil_image_view<vil_rgba<vxl_byte> >(comp_image);

    //make sure all alpha values are set to 255 (1)
    vil_image_view<vil_rgba<vxl_byte> >::iterator iter;
    for (iter = rgba_view->begin(); iter != rgba_view->end(); ++iter) {
      (*iter) = vil_rgba<vxl_byte>(iter->R(), iter->G(), iter->B(), 255);
    }
    vil_image_view_base_sptr toReturn(rgba_view);
    return toReturn;
  }

  //else if loaded planes is just one...
  if (loaded_image->nplanes() == 1)
  {
    vcl_cout<<"Preparing grey scale image"<<vcl_endl;
    vil_image_view<float>* floatimg = new vil_image_view<float>(loaded_image->ni(), loaded_image->nj(), 1);
    if (vil_image_view<vxl_byte> *img_byte = dynamic_cast<vil_image_view<vxl_byte>*>(loaded_image.ptr()))
        vil_convert_stretch_range_limited(*img_byte, *floatimg, vxl_byte(0), vxl_byte(255), 0.0f, 1.0f);
    else if (vil_image_view<unsigned short> *img_byte = dynamic_cast<vil_image_view<unsigned short>*>(loaded_image.ptr()))
        vil_convert_stretch_range_limited(*img_byte, *floatimg,(unsigned short)30500,(unsigned short)32500,  0.0f, 1.0f); // hardcoded to be fixed.
        //vil_convert_stretch_range(*img_byte, *floatimg,  0.0f, 1.0f); // hardcoded to be fixed.
    else if (vil_image_view<float> *img_float = dynamic_cast<vil_image_view<float>*>(loaded_image.ptr()))
    {
        return vil_image_view_base_sptr(img_float);
    }
    else {
        vcl_cerr << "Failed to load image\n";
        return 0;
    }
    vil_image_view_base_sptr toReturn(floatimg);
    return toReturn;
  }

  //otherwise it's messed up, return a null pointer
  vcl_cerr<<"Failed to recognize input image type\n";
  return 0;
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
  for (unsigned i=0;i<imgfilenames.size();i=i+interval)
  {
    vil_image_view_base_sptr img_ptr=boxm2_ocl_util::prepare_input_image(imgfilenames[i]);
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
    vil_image_view_base_sptr img_ptr=boxm2_ocl_util::prepare_input_image(imgfilenames[i]);
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

