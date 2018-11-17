// This is brl/bseg/boxm2/ocl/pro/processes/boxm2_ocl_ingest_buckeye_process.cxx
#include <iostream>
#include <algorithm>
#include <cmath>
#include <fstream>
#include <bprb/bprb_func_process.h>
//:
// \file
// \brief  A process for ingesting a buckeye-style dem (geotiff first return, last return) in a 3-d scene.
//
// \author Daniel Crispell
// \date November 8, 2011

#ifdef _MSC_VER
#  include <vcl_msvc_warnings.h>
#endif
#include <boct/boct_bit_tree.h>
#include <boxm2/ocl/boxm2_opencl_cache.h>
#include <boxm2/boxm2_scene.h>
#include <boxm2/boxm2_block.h>
#include <boxm2/boxm2_data_base.h>
#include <boxm2/ocl/boxm2_ocl_util.h>
#include <boxm2/cpp/algo/boxm2_data_serial_iterator.h>
#include <vil/vil_image_view.h>
#include <vil/vil_image_resource.h>
#include <vil/vil_load.h>
//brdb stuff
#include <brdb/brdb_value.h>

//directory utility
#include <vul/vul_timer.h>
#include <vcl_where_root_dir.h>
#include <bocl/bocl_device.h>
#include <bocl/bocl_kernel.h>

#include <vgl/vgl_box_3d.h>
#include <vgl/vgl_box_2d.h>
#include <vpgl/file_formats/vpgl_geo_camera.h>


// Helper class which updates alpha values based on belief and uncertainty computed from DEM
class alpha_update_from_opinion_functor
{
 public:
  alpha_update_from_opinion_functor(boxm2_data_base* alpha, boxm2_data_base* aux0, boxm2_data_base* aux1, double sb_side_len)
   : subblock_side_len_(sb_side_len)
   {
     // TEMPORARY WORK AROUND: These objects are never deleted!
     alpha_data_ = new boxm2_data<BOXM2_ALPHA>(alpha->data_buffer(), alpha->buffer_length(), alpha->block_id());
     aux0_data_ = new boxm2_data<BOXM2_AUX0>(aux0->data_buffer(), aux0->buffer_length(), aux0->block_id());
     aux1_data_ = new boxm2_data<BOXM2_AUX1>(aux1->data_buffer(), aux1->buffer_length(), aux1->block_id());
  }

  bool process_cell(int index, bool is_leaf, float side_len)
  {
     if (!is_leaf) {
       return true;
     }

     boxm2_data<BOXM2_ALPHA>::datatype &alpha = alpha_data_->data()[index];
     float belief = aux0_data_->data()[index];
     float uncertainty = aux1_data_->data()[index];
#ifdef DEBUG
     std::cout << "index " << index << ": belief = " << belief << ", uncertainty = " << uncertainty << ", alpha = " << alpha << std::endl;
#endif
     //if(belief< 0)
     {

     float ray_len = side_len;

     float PQ_prior = 1.0f - float(std::exp(-alpha*ray_len));
     float PQ = belief + uncertainty*PQ_prior;

     alpha =  float(-std::log(1.0 - PQ)/ray_len);

     if (alpha < 0) {
       std::cerr << "ERROR: alpha = " << alpha << ",  PQ = " << PQ << " ray_len = " << ray_len << '\n'
                << "    belief = " << belief << " uncertainty = " << uncertainty << "  PQ_prior = " << PQ_prior << '\n';
       alpha = 0.0f;
     }
     }
     return true;
  }

 protected:
  boxm2_data<BOXM2_ALPHA> *alpha_data_;
  boxm2_data<BOXM2_AUX0> *aux0_data_;
  boxm2_data<BOXM2_AUX1> *aux1_data_;
  double subblock_side_len_;
};

namespace boxm2_ocl_ingest_buckeye_dem_process_globals
{
  constexpr unsigned n_inputs_ = 7;
  constexpr unsigned n_outputs_ = 0;
  std::size_t local_threads[2]={8,8};
  void compile_kernel(const bocl_device_sptr& device,std::vector<bocl_kernel*> & vec_kernels, std::string options)
  {
    //gather all render sources... seems like a lot for rendering...
    std::vector<std::string> src_paths;
    std::string source_dir = boxm2_ocl_util::ocl_src_root();
    src_paths.push_back(source_dir + "scene_info.cl");
    src_paths.push_back(source_dir + "bit/bit_tree_library_functions.cl");
    src_paths.push_back(source_dir + "backproject.cl");
    src_paths.push_back(source_dir + "bit/render_height_map.cl");
    src_paths.push_back(source_dir + "update_functors.cl");
    src_paths.push_back(source_dir + "bit/cast_ray_bit.cl");

    //set kernel options
    options += " -D INGEST_BUCKEYE_DEM ";
    options += " -D STEP_CELL=step_cell_ingest_buckeye_dem(aux_args,data_ptr,(t_vox_exit-d)*linfo->block_len,t_vox_exit*linfo->block_len)";
    std::cout << "Kernel Options = [" << options << ']' << std::endl;
    //have kernel construct itself using the context and device
    auto * ray_trace_kernel=new bocl_kernel();

    ray_trace_kernel->create_kernel( &device->context(),
                                     device->device_id(),
                                     src_paths,
                                     "ingest_buckeye_dem",   //kernel name
                                     options,               //options
                                     "boxm2 opencl ingest buckeye dem"); //kernel identifier (for error checking)
    vec_kernels.push_back(ray_trace_kernel);
  }
  static std::map<std::string,std::vector<bocl_kernel*> > kernels;
}

bool boxm2_ocl_ingest_buckeye_dem_process_cons(bprb_func_process& pro)
{
  using namespace boxm2_ocl_ingest_buckeye_dem_process_globals;

  //process takes 6 inputs
  std::vector<std::string> input_types_(n_inputs_);
  input_types_[0] = "bocl_device_sptr";
  input_types_[1] = "boxm2_scene_sptr";
  input_types_[2] = "boxm2_opencl_cache_sptr";
  input_types_[3] = "vcl_string"; // first return geotiff filename
  input_types_[4] = "vcl_string"; // last return geotiff filename
  input_types_[5] = "float"; // geoid height
  input_types_[6] = "vpgl_camera_double_sptr"; // external geo cam

  // process has no outputs
  std::vector<std::string> output_types_(n_outputs_);

  bool good =  pro.set_input_types(input_types_) && pro.set_output_types(output_types_);

 //set defaults inputs
 brdb_value_sptr cam = new brdb_value_t<vpgl_camera_double_sptr>(nullptr);

  pro.set_input(6, cam);

  return good;
}

bool boxm2_ocl_ingest_buckeye_dem_process(bprb_func_process& pro)
{
  using namespace boxm2_ocl_ingest_buckeye_dem_process_globals;

  if ( pro.n_inputs() < n_inputs_ ) {
    std::cout << pro.name() << ": The input number should be " << n_inputs_<< std::endl;
    return false;
  }
  float transfer_time=0.0f;
  float gpu_time=0.0f;

  //get the inputs
  bocl_device_sptr device= pro.get_input<bocl_device_sptr>(0);
  boxm2_scene_sptr scene =pro.get_input<boxm2_scene_sptr>(1);
  boxm2_opencl_cache_sptr opencl_cache= pro.get_input<boxm2_opencl_cache_sptr>(2);

  std::string a1_fname = pro.get_input<std::string>(3);
  std::string a2_fname = pro.get_input<std::string>(4);

  double geoid_height = pro.get_input<float>(5);
  vpgl_camera_double_sptr geocam_in = pro.get_input<vpgl_camera_double_sptr>(6);
  // load the images as resources
  vil_image_resource_sptr a1_res = vil_load_image_resource(a1_fname.c_str());
  vil_image_resource_sptr a2_res = vil_load_image_resource(a2_fname.c_str());

  vpgl_lvcs_sptr lvcs = new vpgl_lvcs(scene->lvcs());

  vpgl_geo_camera* geocam = nullptr;
  if (geocam_in->is_a()=="vpgl_geo_camera")
  {
    std::cout<<"LOADING EXTERNAL CAMERA"<<std::endl;
    geocam = static_cast<vpgl_geo_camera*>(geocam_in.ptr());
  }
  else
  {
    vpgl_geo_camera::init_geo_camera(a1_res, lvcs, geocam);
  }
  // crop relevant image data into a view
  vgl_box_3d<double> scene_bbox = scene->bounding_box();
  vgl_box_2d<double> proj_bbox;
  double u,v;
  std::cout<<"Scene BBox "<<scene_bbox<<std::endl;
  geocam->project(scene_bbox.min_x(), scene_bbox.min_y(), scene_bbox.min_z() + geoid_height, u, v);
  proj_bbox.add(vgl_point_2d<double>(u,v));
  geocam->project(scene_bbox.max_x(), scene_bbox.max_y(), scene_bbox.max_z() + geoid_height, u, v);
  proj_bbox.add(vgl_point_2d<double>(u,v));

  std::cout<<"Proj Box "<<proj_bbox<<std::endl;
  int min_i = int(std::max(0.0, std::floor(proj_bbox.min_x())));
  int min_j = int(std::max(0.0, std::floor(proj_bbox.min_y())));
  int max_i = int(std::min(a1_res->ni()-1.0, std::ceil(proj_bbox.max_x())));
  int max_j = int(std::min(a1_res->nj()-1.0, std::ceil(proj_bbox.max_y())));

  if ((min_i > max_i) || (min_j > max_j)) {
      std::cerr << "Error: boxm2_ocl_ingest_buckeye_dem_process: No overlap between scene and DEM image.\n";
      return false;
  }

  unsigned int ni = max_i - min_i + 1;
  unsigned int nj = max_j - min_j + 1;

  vil_image_view_base_sptr a1_view_base = a1_res->get_view((unsigned int)min_i, ni, (unsigned int)min_j, nj);
  vil_image_view_base_sptr a2_view_base = a2_res->get_view((unsigned int)min_i, ni, (unsigned int)min_j, nj);

  auto* a1_view = dynamic_cast<vil_image_view<float>*>(a1_view_base.ptr());
  if (!a1_view) {
      std::cerr << "Error: boxm2_ocl_ingest_buckeye_dem_process: could not cast first return image to a vil_image_view<float>\n";
      return false;
  }
  auto* a2_view = dynamic_cast<vil_image_view<float>*>(a2_view_base.ptr());
  if (!a2_view) {
      std::cerr << "Error: boxm2_ocl_ingest_buckeye_dem_process: could not cast last return image to a vil_image_view<float>\n";
      return false;
  }

  unsigned int cl_ni = RoundUp(ni,8);
  unsigned int cl_nj = RoundUp(nj,8);

  // form the ray buffer
  auto* ray_origins = new float[4*cl_ni*cl_nj];
  auto* a1_img = new float[cl_ni*cl_nj];
  auto* a2_img = new float[cl_ni*cl_nj];
  //cl_float* outimg = new float[cl_ni*cl_nj];

  // initialize ray origin buffer, first and last return buffers
  int count=0;
  for (unsigned int j=0;j<cl_nj;++j) {
    for (unsigned int i=0;i<cl_ni;++i) {
      if ( i < ni && j < nj ) {
        int count4 = count*4;
        double full_i = min_i + i + 0.25;
        double full_j = min_j + j + 0.25;
        double lat,lon, x, y, z_first, z_last;
        double el_first = (*a1_view)(i,j) + geoid_height;
        double el_last = (*a2_view)(i,j) + geoid_height;
        geocam->img_to_global(full_i, full_j,  lon, lat);
        lvcs->global_to_local(lon,lat,el_first, vpgl_lvcs::wgs84, x, y, z_first);
        lvcs->global_to_local(lon,lat,el_last, vpgl_lvcs::wgs84, x, y, z_last);

        // start rays slightly above maximum height of model
        float z_origin = float(scene_bbox.max_z()) + 1.0f;

        ray_origins[count4+0] = float(x);
        ray_origins[count4+1] = float(y);
        // ray will begin just above "top" of scene, with direction pointing in negative z direction
        ray_origins[count4+2] = z_origin;
        ray_origins[count4+3] = 0.0;
        // first return: convert elevation to distance from ray origin
        a1_img[count] = z_origin - z_first;
        // last return: convert elevation to distance from ray origin
        a2_img[count] = z_origin - z_last;
      }
      ++count;
    }
  }

  bocl_mem_sptr ray_o_buff = new bocl_mem(device->context(),
                                          ray_origins,
                                          cl_ni*cl_nj * sizeof(cl_float4),
                                          "ray_origins buffer");
  ray_o_buff->create_buffer(CL_MEM_READ_WRITE | CL_MEM_COPY_HOST_PTR);

  bocl_mem_sptr a1_buff = new bocl_mem(device->context(), a1_img, cl_ni*cl_nj*sizeof(cl_float),"buckeye a1 buffer");
  a1_buff->create_buffer(CL_MEM_READ_WRITE | CL_MEM_COPY_HOST_PTR);

  bocl_mem_sptr a2_buff = new bocl_mem(device->context(), a2_img, cl_ni*cl_nj*sizeof(cl_float),"buckeye a2 buffer");
  a2_buff->create_buffer(CL_MEM_READ_WRITE | CL_MEM_COPY_HOST_PTR);

  //bocl_mem_sptr out_buff = new bocl_mem(device->context(),outimg, cl_ni*cl_nj * sizeof(cl_float),"out image buffer");
  //out_buff->create_buffer(CL_MEM_READ_WRITE | CL_MEM_COPY_HOST_PTR);

  //get x and y size from scene
  std::vector<boxm2_block_id> vis_order = scene->get_block_ids();
  std::vector<boxm2_block_id>::iterator id;
  // create a command queue.
  int status=0;
  cl_command_queue queue = clCreateCommandQueue(device->context(),
                                                *(device->device_id()),
                                                CL_QUEUE_PROFILING_ENABLE,&status);
  if (status!=0)
    return false;

  // compile the kernel
  std::string identifier=device->device_identifier();

  if (kernels.find(identifier)==kernels.end())
  {
    std::cout<<"===========Compiling kernels==========="<<std::endl;
    std::vector<bocl_kernel*> ks;
    compile_kernel(device,ks,"");
    kernels[identifier]=ks;
  }
  // Image Dimensions
  int img_dim_buff[4];
  img_dim_buff[0] = 0;
  img_dim_buff[1] = 0;
  img_dim_buff[2] = ni;
  img_dim_buff[3] = nj;
  bocl_mem_sptr exp_img_dim=new bocl_mem(device->context(), img_dim_buff, sizeof(int)*4, "image dims");
  exp_img_dim->create_buffer(CL_MEM_READ_WRITE | CL_MEM_COPY_HOST_PTR);

  // bit lookup buffer
  cl_uchar lookup_arr[256];
  boxm2_ocl_util::set_bit_lookup(lookup_arr);
  bocl_mem_sptr lookup=new bocl_mem(device->context(), lookup_arr, sizeof(cl_uchar)*256, "bit lookup buffer");
  lookup->create_buffer(CL_MEM_READ_ONLY | CL_MEM_COPY_HOST_PTR);

  //2. set workgroup size
  std::size_t lThreads[] = {8, 8};
  std::size_t gThreads[] = {cl_ni,cl_nj};

  std::cout<<"Ingesting DEM"<<std::endl;
  // set arguments

  for (id = vis_order.begin(); id != vis_order.end(); ++id)
  {
    std::cout<<"Block # "<<*id<<std::endl;

    //choose correct render kernel
    boxm2_block_metadata mdata = scene->get_block_metadata(*id);
    bocl_kernel* kern =  kernels[identifier][0];

    //write the image values to the buffer
    vul_timer transfer;
    bocl_mem * blk = opencl_cache->get_block(scene, *id);
    bocl_mem * blk_info = opencl_cache->loaded_block_info();
    bocl_mem* alpha = opencl_cache->get_data<BOXM2_ALPHA>(scene, *id,0,false);
    auto* info_buffer = (boxm2_scene_info*) blk_info->cpu_buffer();
    int alphaTypeSize = (int)boxm2_data_info::datasize(boxm2_data_traits<BOXM2_ALPHA>::prefix());
    info_buffer->data_buffer_length = (int) (alpha->num_bytes()/alphaTypeSize);

    //grab an appropriately sized AUX data buffer
    blk_info->write_to_buffer((queue));
    // aux0 for occupancy "belief"
    int auxTypeSize = (int)boxm2_data_info::datasize(boxm2_data_traits<BOXM2_AUX0>::prefix());
    //bocl_mem *aux0 = opencl_cache->get_data<BOXM2_AUX0>(scene,*id, info_buffer->data_buffer_length*auxTypeSize, false);
    bocl_mem *aux0 = opencl_cache->get_data(scene, *id, boxm2_data_traits<BOXM2_AUX0>::prefix(), info_buffer->data_buffer_length*auxTypeSize, false);
    // aux1 for occupancy "uncertainty"
    auxTypeSize = (int)boxm2_data_info::datasize(boxm2_data_traits<BOXM2_AUX1>::prefix());
    //bocl_mem *aux1 = opencl_cache->get_data<BOXM2_AUX1>(scene,*id, info_buffer->data_buffer_length*auxTypeSize, false);
    bocl_mem *aux1 = opencl_cache->get_data(scene, *id, boxm2_data_traits<BOXM2_AUX1>::prefix(),info_buffer->data_buffer_length*auxTypeSize, false);

    // initialize belief values to 0.0
    //aux0->zero_gpu_buffer(queue);
    // initialize uncertainty values to 1.0
    cl_float aux1_init_val = 1.0f;
    aux1->init_gpu_buffer(&aux1_init_val, sizeof(cl_float), queue);

    transfer_time += (float) transfer.all();
#if 1
    ////3. SET args
    kern->set_arg(blk_info );
    kern->set_arg(exp_img_dim.ptr() );
    kern->set_arg(ray_o_buff.ptr() );
    kern->set_arg(a1_buff.ptr() );
    kern->set_arg(a2_buff.ptr() );
    kern->set_arg(blk );
    kern->set_arg(aux0 );
    kern->set_arg(aux1 );
    kern->set_arg(lookup.ptr() );
    //local tree , cumsum buffer, imindex buffer
    kern->set_local_arg( lThreads[0]*lThreads[1]*sizeof(cl_uchar16) );
    kern->set_local_arg( lThreads[0]*lThreads[1]*10*sizeof(cl_uchar) );
    kern->set_local_arg( lThreads[0]*lThreads[1]*sizeof(cl_int) );
    std::cout<<"Setting arguments"<<std::endl;

    //execute kernel
    if (!kern->execute(queue, 2, lThreads, gThreads)) {
      std::cerr << "boxm2_ocl_ingest_buckeye_dem_process: kern->execute() returned error. exiting process.\n";
      clReleaseCommandQueue(queue);
      return false;
    }

    std::cout << "Calling clFinish" << std::endl;
    status = clFinish(queue);
    if ( !check_val(status,CL_SUCCESS,"clFinish failed (" + kern->id() + ") " +error_to_string(status)) ) {
      std::cerr << "boxm2_ocl_ingest_dem_process: clFinish returned error. exiting process.\n";
      clReleaseCommandQueue(queue);
      return false;
    }
    std::cout << "clFinish returned success" << std::endl;
    gpu_time += kern->exec_time();
    std::cout<<" Time "<<gpu_time<<std::endl;
#endif
    std::cout << "Reading back AUX buffers" << std::endl;
    aux0->read_to_buffer(queue );
    aux1->read_to_buffer(queue );

    //clear render kernel args so it can reset em on next execution
    kern->clear_args();

    // update alpha values based on belief and uncertainty stored in aux0 and aux1

    boxm2_cache_sptr cpu_cache = opencl_cache->get_cpu_cache();
    boxm2_data_base *alpha_data = cpu_cache->get_data_base(scene, *id,boxm2_data_traits<BOXM2_ALPHA>::prefix(),0,false);
    boxm2_data_base *aux0_data = cpu_cache->get_data_base(scene, *id, boxm2_data_traits<BOXM2_AUX0>::prefix(),0,false);
    boxm2_data_base *aux1_data = cpu_cache->get_data_base(scene, *id, boxm2_data_traits<BOXM2_AUX1>::prefix(),0,false);

    boxm2_block* block = cpu_cache->get_block(scene,*id);

    double subblock_side_len = block->sub_block_dim().x();
    alpha_update_from_opinion_functor update_func(alpha_data, aux0_data, aux1_data, subblock_side_len);

    int buff_len = alpha_data->buffer_length();
    std::cout << "buffer length = " << buff_len << std::endl;
    int datasize = boxm2_data_info::datasize(boxm2_data_traits<BOXM2_ALPHA>::prefix());
    std::cout << "datasize = " << datasize << std::endl;
    int data_len = int(alpha_data->buffer_length() / boxm2_data_info::datasize(boxm2_data_traits<BOXM2_ALPHA>::prefix()));
    std::cout << "data length = " << data_len << '\n'
             << "Updating Alpha values.." << std::endl;
    boxm2_data_leaves_serial_iterator<alpha_update_from_opinion_functor>(block, data_len, update_func);
    std::cout << "Done updating Alpha values." << std::endl;
  }

  clReleaseCommandQueue(queue);
  return true;
}
