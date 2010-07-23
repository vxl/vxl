#include <boxm/ocl/boxm_ocl_render_expected.h>

void boxm_opencl_ocl_scene_expected(boxm_ocl_scene &scene,
                                    vpgl_camera_double_sptr cam,
                                    vil_image_view<float> &expected,
                                    vil_image_view<float> & mask,
                                    bool use_black_background)
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
  ray_mgr->init_ray_trace(&scene, cam, expected);

  ray_mgr->run_scene();

  // extract expected image and mask from OpenCL output data
  cl_float* results = ray_mgr->output_image();
  if (!results) {
    vcl_cerr << "Error : boxm_opencl_render_expected : ray_mgr->ray_results() returned NULL\n";
    return;
  }
  cl_float *results_p = results;
  for (unsigned j = 0; j<nj; ++j)  {
    for (unsigned i = 0; i<ni; ++i) {
      img0(i,j)=*(results_p++); // vis_inf
      img1(i,j)=*(results_p++); // vis_inf
      expected(i,j) = *(results_p++); // expected intensity
      mask(i,j) = *(results_p++); // 1 - vis_inf

      if (i==266 && j==329)
      {
        vcl_cout<<img0(i,j)<<','<<img1(i,j)<<','<<expected(i,j)<<','<<mask(i,j)<<vcl_endl;
      }
    }
  }

#if 0 //images for debuggin
  vil_save(img0,"f:/apl/img0.tiff");
  vil_save(img1,"f:/apl/img1.tiff");
  vil_save(expected,"f:/apl/img2.tiff");
  vil_save(mask,"f:/apl/img3.tiff");
#endif
}
