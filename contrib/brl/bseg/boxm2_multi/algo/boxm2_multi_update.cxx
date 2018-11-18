#include <iostream>
#include <algorithm>
#include "boxm2_multi_update.h"
#include "boxm2_multi_store_aux.h"
#include "boxm2_multi_pre_vis_inf.h"
#include "boxm2_multi_update_cell.h"

#ifdef _MSC_VER
#  include <vcl_msvc_warnings.h>
#endif
#include <boxm2/boxm2_scene.h>
#include <boxm2/boxm2_util.h>
#include <boxm2/ocl/algo/boxm2_ocl_camera_converter.h>

#include <bocl/bocl_mem.h>
#include <bocl/bocl_device.h>
#include <bocl/bocl_kernel.h>
#include <vil/vil_image_view.h>
#include <vul/vul_timer.h>
#include <vpgl/vpgl_camera.h>

float boxm2_multi_update::update(boxm2_multi_cache& cache,
                                 vil_image_view<float>& img,
                                 vpgl_camera_double_sptr  cam)
{
  std::cout<<"------------ boxm2_multi_update ----------------"<<std::endl;

  //setup image size
  int ni=img.ni(),
      nj=img.nj();
  std::size_t lthreads[2] = {8,8};
  unsigned cl_ni = RoundUp(ni,lthreads[0]);
  unsigned cl_nj = RoundUp(nj,lthreads[1]);
  float gpu_time = 0.0f;  //time spent on the GPU in milliseconds

  //---------------------------------
  //store vars for each ocl_cache
  //---------------------------------
  std::vector<cl_command_queue> queues; //store queue for each device
  std::vector<bocl_mem_sptr> img_dims, outputs, ray_ds, ray_os, lookups,tnearfarptrs; //ray trace vars
  std::vector<std::vector<boxm2_block_id> > vis_orders; //visibility order for each dev
  std::size_t maxBlocks = 0;
  std::vector<boxm2_opencl_cache1*> ocl_caches = cache.ocl_caches();
  for (auto ocl_cache : ocl_caches) {
    //grab sub scene and it's cache
    boxm2_scene_sptr    sub_scene = ocl_cache->get_scene();
    bocl_device_sptr    device    = ocl_cache->get_device();

    // create a command queue.
    int status=0;
    cl_command_queue queue = clCreateCommandQueue( device->context(),
                                                   *(device->device_id()),
                                                   CL_QUEUE_PROFILING_ENABLE,
                                                   &status );
    queues.push_back(queue);
    if (status!=0) {
      std::cout<<"boxm2_multi_store_aux::store_aux unable to create command queue"<<std::endl;
      return 0.0f;
    }
    auto * tnearfar= new float[2];
    tnearfar[0] = 0.0;
    tnearfar[1] = 100000.0 ;
    bocl_mem_sptr tnearfar_mem_ptr = new bocl_mem(device->context(),tnearfar,sizeof(float)*2,  "tnearfar  buffer");
    tnearfar_mem_ptr->create_buffer(CL_MEM_READ_WRITE | CL_MEM_COPY_HOST_PTR);
    tnearfarptrs.push_back(tnearfar_mem_ptr);
    //create image dim buff
    int img_dim_buff[4] = {0,0,ni,nj};
    bocl_mem_sptr img_dim = new bocl_mem(device->context(), img_dim_buff, sizeof(int)*4, "image dims");
    img_dim->create_buffer(CL_MEM_READ_WRITE | CL_MEM_COPY_HOST_PTR);
    img_dims.push_back(img_dim);


    // Output Array
    auto* output_arr = new float[cl_ni*cl_nj];
    std::fill(output_arr, output_arr+cl_ni*cl_nj, 1.0f);
    bocl_mem_sptr  cl_output=new bocl_mem(device->context(), output_arr, sizeof(float)*cl_ni*cl_nj, "output buffer");
    cl_output->create_buffer(CL_MEM_READ_WRITE | CL_MEM_COPY_HOST_PTR);
    outputs.push_back(cl_output);

    //set generic cam and get visible block order
    auto* ray_origins    = new cl_float[4*cl_ni*cl_nj];
    auto* ray_directions = new cl_float[4*cl_ni*cl_nj];
    bocl_mem_sptr ray_o_buff = new bocl_mem(device->context(), ray_origins   ,  cl_ni*cl_nj * sizeof(cl_float4), "ray_origins buffer");
    bocl_mem_sptr ray_d_buff = new bocl_mem(device->context(), ray_directions,  cl_ni*cl_nj * sizeof(cl_float4), "ray_directions buffer");
    boxm2_ocl_camera_converter::compute_ray_image( device, queue, cam, cl_ni, cl_nj, ray_o_buff, ray_d_buff);
    ray_os.push_back(ray_o_buff);
    ray_ds.push_back(ray_d_buff);

    // bit lookup buffer
    cl_uchar lookup_arr[256];
    boxm2_ocl_util::set_bit_lookup(lookup_arr);
    bocl_mem_sptr lookup=new bocl_mem(device->context(), lookup_arr, sizeof(cl_uchar)*256, "bit lookup buffer");
    lookup->create_buffer(CL_MEM_READ_ONLY | CL_MEM_COPY_HOST_PTR);
    lookups.push_back(lookup);

    // set arguments
    std::vector<boxm2_block_id> vis_order = sub_scene->get_vis_blocks(cam);
    vis_orders.push_back(vis_order);
    maxBlocks = std::max(vis_order.size(), maxBlocks);
  }

  //initialize per group images (vis/pre
  std::vector<boxm2_multi_cache_group*> grp = cache.get_vis_groups(cam);
  for (auto & grpId : grp) {
    std::vector<boxm2_block_id> ids = grpId->ids();
    for (unsigned int i=0; i<ids.size(); ++i) {
      auto* visImg = new float[cl_ni*cl_nj]; std::fill(visImg, visImg+cl_ni*cl_nj, 1.0f);
      auto* preImg = new float[cl_ni*cl_nj]; std::fill(preImg, preImg+cl_ni*cl_nj, 0.0f);
      grpId->set_vis(i, visImg);
      grpId->set_pre(i, preImg);
    }
  }

  //create boxm2_multi update helper object for factored out vars
  boxm2_multi_update_helper helper(queues, ray_os, ray_ds, img_dims,tnearfarptrs, lookups, outputs, grp, vis_orders, ocl_caches, maxBlocks);

  //store aux data (cell vis, cell length)
  vul_timer stepTimer; stepTimer.mark();
  float aux_time = boxm2_multi_store_aux::store_aux(cache, img, cam, helper);
  std::cout<<"  store_aux time: "<<aux_time<<"  "<<stepTimer.all()<<std::endl;
  gpu_time += aux_time;

  //calcl pre/vis inf, and store pre/vis images along the way
  auto* norm_img = new float[cl_ni * cl_nj];
  std::map<bocl_device*, float*> pre_map, vis_map;
  stepTimer.mark();
  float pre_vis_time = boxm2_multi_pre_vis_inf::pre_vis_inf(cache, img, cam, norm_img, helper);
  std::cout<<"  pre_vis time: "<<pre_vis_time<<"  "<<stepTimer.all()<<std::endl;
  gpu_time += pre_vis_time;


  //calculate cell beta, cell vis, and finally reduce each cell to new alphas
  stepTimer.mark();
  float update_cell_time = boxm2_multi_update_cell::update_cells(cache, img, cam, norm_img, helper);
  std::cout<<"  update_cell time: "<<update_cell_time<<"  "<<stepTimer.all()<<std::endl;
  gpu_time += update_cell_time;

  //-------------------------------------
  //clean up
  //-------------------------------------
  for (auto & grpId : grp) {
    std::vector<boxm2_block_id> ids = grpId->ids();
    for (unsigned int i=0; i<ids.size(); ++i) {
      delete[] grpId->get_vis(i);
      delete[] grpId->get_pre(i);
    }
  }

  for (unsigned int i=0; i<queues.size(); ++i) {
    boxm2_opencl_cache1* ocl_cache = ocl_caches[i];

    //release generic cam
    auto* rayO = (float*) ray_os[i]->cpu_buffer();
    auto* rayD = (float*) ray_ds[i]->cpu_buffer();
    delete[] rayO;
    delete[] rayD;
    ocl_cache->unref_mem(ray_os[i].ptr());
    ocl_cache->unref_mem(ray_ds[i].ptr());

    auto* tnf = (float*) tnearfarptrs[i]->cpu_buffer();
    delete[] tnf;
    ocl_cache->unref_mem(tnearfarptrs[i].ptr() );
    //release output
    auto* output = (float*) outputs[i]->cpu_buffer();
    delete[] output;
    ocl_cache->unref_mem(outputs[i].ptr());

    //free vis mem, pre mem
    clReleaseCommandQueue(queues[i]);
  }

  //delete single norm image
  delete[] norm_img;

  //clean up pre/vis maps per device
  std::map<bocl_device*, float*>::iterator iter;
  for (iter = pre_map.begin(); iter != pre_map.end(); ++iter)
    delete[] iter->second;
  for (iter = vis_map.begin(); iter != vis_map.end(); ++iter)
    delete[] iter->second;

  //report gpu time
  return gpu_time;
}
