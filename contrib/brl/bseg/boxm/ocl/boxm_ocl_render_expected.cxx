#include <boxm/ocl/boxm_ocl_render_expected.h>
#include <vil/vil_load.h>
#include <vul/vul_file.h>
#include <vil/vil_convert.h>

void boxm_opencl_ocl_scene_expected(boxm_ocl_scene &scene,
                                    vpgl_camera_double_sptr cam,
                                    vil_image_view<float> &expected,
                                    vil_image_view<float> & mask,
                                    bool /*use_black_background*/)
{
  // set up the application-specific function to be called at every cell along a ray

  const unsigned int ni = expected.ni();
  const unsigned int nj = expected.nj();
  vil_image_view<float> img0(ni,nj);
  vil_image_view<float> img1(ni,nj);
  // render the image using the opencl raytrace manager
  boxm_render_ocl_scene_manager* ray_mgr = boxm_render_ocl_scene_manager::instance();
  int bundle_dim=8;
  ray_mgr->set_bundle_ni(bundle_dim);
  ray_mgr->set_bundle_nj(bundle_dim);
  ray_mgr->init_ray_trace(&scene, cam, expected,true);

  ray_mgr->run_scene();

  // extract expected image and mask from OpenCL output data
  cl_float* results = ray_mgr->output_image();
  if (!results) {
    vcl_cerr << "Error : boxm_opencl_render_expected : ray_mgr->ray_results() returned NULL\n";
    return;
  }
  cl_float *results_p = results;
  for (unsigned j = 0; j<nj; ++j)
    for (unsigned i = 0; i<ni; ++i)
      expected(i,j) = *(results_p++); // expected intensity

#if 0 //images for debuggin
  vil_save(img0,"f:/apl/img0.tiff");
  vil_save(img1,"f:/apl/img1.tiff");
  vil_save(expected,"f:/apl/img2.tiff");
  vil_save(mask,"f:/apl/img3.tiff");
#endif
}

void boxm_ocl_scene_rerender(boxm_ocl_scene &scene,
                             vcl_vector<vcl_string> camfiles,
                             vcl_vector<vcl_string> imgfiles,
                             vcl_string out_dir)
{
  boxm_render_ocl_scene_manager* ray_mgr = boxm_render_ocl_scene_manager::instance();
  int bundle_dim=8;
  ray_mgr->set_bundle_ni(bundle_dim);
  ray_mgr->set_bundle_nj(bundle_dim);

  vpgl_perspective_camera<double>* prevcam = new vpgl_perspective_camera<double>;
  vpgl_perspective_camera<double>* currcam = new vpgl_perspective_camera<double>;
  vil_image_view<float> previmg;
  vil_image_view<float> currimg;
  for (unsigned curr_frame=0;curr_frame<camfiles.size();curr_frame++)
  {
    if (curr_frame==0)
    {
      // load camera an d image
      vcl_ifstream ifs(camfiles[curr_frame].c_str());
      if (!ifs.is_open()) {
        vcl_cerr << "Failed to open file " << camfiles[curr_frame] << vcl_endl;
        return;
      }
      ifs >> *prevcam;

      //load image from file
      vil_image_view_base_sptr loaded_image = vil_load(imgfiles[curr_frame].c_str());
      previmg.set_size(loaded_image->ni(), loaded_image->nj());
      if (vil_image_view<vxl_byte> *img_byte = dynamic_cast<vil_image_view<vxl_byte>*>(loaded_image.ptr()))
        vil_convert_stretch_range_limited(*img_byte ,previmg, vxl_byte(0), vxl_byte(255), 0.0f, 1.0f);
      else if (vil_image_view<float> *img_byte = dynamic_cast<vil_image_view<float>*>(loaded_image.ptr()))
        previmg=*img_byte;
      else {
        vcl_cerr << "Failed to load image " << imgfiles[curr_frame] << vcl_endl;
        return ;
      }
      ray_mgr->init_ray_trace(&scene, prevcam, previmg,true);
      ray_mgr->start(true);
    }
    //else
    {
      vcl_ifstream ifs(camfiles[curr_frame].c_str());
      if (!ifs.is_open()) {
        vcl_cerr << "Failed to open file " << camfiles[curr_frame] << vcl_endl;
        return;
      }
      ifs >> *currcam;

      //load image from file
      vil_image_view_base_sptr loaded_image = vil_load(imgfiles[curr_frame].c_str());
      currimg.set_size(loaded_image->ni(), loaded_image->nj());
      if (vil_image_view<vxl_byte> *img_byte = dynamic_cast<vil_image_view<vxl_byte>*>(loaded_image.ptr()))
        vil_convert_stretch_range_limited(*img_byte ,currimg, vxl_byte(0), vxl_byte(255), 0.0f, 1.0f);
      else if (vil_image_view<float> *img_byte = dynamic_cast<vil_image_view<float>*>(loaded_image.ptr()))
        currimg=*img_byte;
      else {
        vcl_cerr << "Failed to load image " << imgfiles[curr_frame] << vcl_endl;
        return;
      }
      ray_mgr->set_external_image_cam_buffers(currimg,currcam);
      ray_mgr->set_persp_camera(prevcam);
      ray_mgr->write_persp_camera_buffers();
      ray_mgr->set_args(0);
      ray_mgr->set_args(1);
      ray_mgr->run(true);
      ray_mgr->read_rerendered_image();

      vcl_string filename=vul_file::strip_directory(imgfiles[curr_frame].c_str());
      filename=vul_file::strip_extension(filename.c_str());
      vcl_cout<<"Saving to "<<filename<<vcl_endl;
      ray_mgr->save_rerender_image(out_dir+"/"+filename+".tiff");

      (*prevcam)=(*currcam);
      previmg=currimg;
    }
  }

  ray_mgr->finish();
}

void boxm_opencl_bit_scene_expected(boxm_ocl_bit_scene &scene,
                                    vpgl_camera_double_sptr cam,
                                    vil_image_view<float> &expected,
                                    vil_image_view<float> & mask,
                                    bool /*use_black_background*/)
{
  // set up the application-specific function to be called at every cell along a ray

  const unsigned int ni = expected.ni();
  const unsigned int nj = expected.nj();
  vil_image_view<float> img0(ni,nj);
  vil_image_view<float> img1(ni,nj);
  // render the image using the opencl raytrace manager
  boxm_render_bit_scene_manager* ray_mgr = boxm_render_bit_scene_manager::instance();
  int bundle_dim=8;
  ray_mgr->set_bundle_ni(bundle_dim);
  ray_mgr->set_bundle_nj(bundle_dim);
  ray_mgr->init_ray_trace(&scene, cam, expected, false);
  ray_mgr->run_scene();
  ray_mgr->save_image("test.tiff");

  // extract expected image and mask from OpenCL output data
  cl_float* results = ray_mgr->output_image();
  if (!results) {
    vcl_cerr << "Error : boxm_opencl_render_expected : ray_mgr->ray_results() returned NULL\n";
    return;
  }
  cl_float *results_p = results;
  for (unsigned j = 0; j<nj; ++j) {
    for (unsigned i = 0; i<ni; ++i) {
      expected(i,j) = *(results_p++); // expected intensity
    }
  }

#if 1 //images for debuggin
  vil_save(img0,"/media/VXL/img0.tiff");
  vil_save(img1,"/media/VXL/img1.tiff");
  vil_save(expected,"/media/VXL/expected.tiff");
  vil_save(mask,"/media/VXL/img3.tiff");
#endif

}

