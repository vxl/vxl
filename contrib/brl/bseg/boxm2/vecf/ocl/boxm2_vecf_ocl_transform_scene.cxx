#include "boxm2_vecf_ocl_transform_scene.h"
//:
// \file
#include <vcl_stdexcept.h>
#include <vnl/vnl_vector_fixed.h>
#include <bocl/bocl_cl.h>
#include <vgl/vgl_box_3d.h>
#include <vgl/vgl_vector_3d.h>
#include <vgl/algo/vgl_rotation_3d.h>
#include <vgl/vgl_intersection.h>
#include <boxm2/boxm2_data_traits.h>
#include <boxm2/ocl/boxm2_opencl_cache.h>
#include <boxm2/boxm2_block_metadata.h>
#include <boxm2/boxm2_block.h>
#include <boxm2/boxm2_data_base.h>
#include <boxm2/boxm2_util.h>
#include <boxm2/ocl/boxm2_ocl_util.h>
#include <boxm2/ocl/algo/boxm2_ocl_expected_image_renderer.h>
#include <boct/boct_bit_tree.h>

#include <bocl/bocl_kernel.h>
#include <vcl_where_root_dir.h>
#include <vcl_algorithm.h>

typedef vnl_vector_fixed<unsigned char,16> uchar16;
bool boxm2_vecf_ocl_transform_scene::get_scene_appearance( boxm2_scene_sptr scene,
          vcl_string&      options)
{
    vcl_vector<vcl_string> apps = scene->appearances();
    bool foundDataType = false;
    for (unsigned int i=0; i<apps.size(); ++i) {
      boxm2_data_type app_type = boxm2_data_info::data_type(apps[i]);
        if ( app_type == BOXM2_MOG3_GREY )
        {
            app_type_ = BOXM2_MOG3_GREY;
            foundDataType = true;
            options=" -D MOG_TYPE_8 ";
        }
        else if ( app_type == BOXM2_MOG3_GREY_16 )
        {
            app_type_ = BOXM2_MOG3_GREY_16;
            foundDataType = true;
            options=" -D MOG_TYPE_16 ";
        }
    }
    if (!foundDataType) {
        vcl_cout<<"ERROR: boxm2_vecf_ocl_transform_scene: unsupported appearance type" << vcl_endl;
        return false;
    }
    //set apptype size
    apptypesize_ = (int) boxm2_data_info::datasize(app_type_);
    return true;
}


boxm2_vecf_ocl_transform_scene::boxm2_vecf_ocl_transform_scene(boxm2_scene_sptr& source_scene,
              boxm2_scene_sptr& target_scene,
              boxm2_opencl_cache_sptr ocl_cache, 
              unsigned ni, unsigned nj)
  : source_scene_(source_scene),
     target_scene_(target_scene),
     opencl_cache_(ocl_cache),
     ni_(ni), nj_(nj),
     renderer_(target_scene, ocl_cache)
{
  device_=opencl_cache_->get_device();
  this->compile_trans_interp_kernel();
  this->compile_depth_kernel();
  this->compile_depth_norm_kernel();
  this->init_ocl_trans();

  if (!init_render_args()) {
    throw vcl_runtime_error("init_render_args returned false");
  }
}

boxm2_vecf_ocl_transform_scene::~boxm2_vecf_ocl_transform_scene()
{
  delete trans_interp_kern;
  //delete trans_kern;
  delete depth_kern_;
  delete depth_norm_kern_;
  //opencl_cache_->clear_cache();


  // destroy render args
  if ((ni_ > 0) && (nj_ > 0)) {
    delete [] img_buff_;
    delete [] vis_buff_;
    delete [] max_omega_buff_;
    delete [] ray_origins_;
    delete [] ray_directions_;
    opencl_cache_->unref_mem(ray_o_buff_.ptr());
    opencl_cache_->unref_mem(ray_d_buff_.ptr());
    opencl_cache_->unref_mem(tnearfar_mem_ptr_.ptr());
    opencl_cache_->unref_mem(exp_image_.ptr());
    opencl_cache_->unref_mem(vis_image_.ptr());
    opencl_cache_->unref_mem(max_omega_image_.ptr());
    // depth stuff
    delete[] depth_buff_;
    delete[] var_buff_;
    delete[] prob_image_buff_;
    delete[] t_infinity_buff_;
    opencl_cache_->unref_mem(depth_image_.ptr());
    opencl_cache_->unref_mem(var_image_.ptr());
    opencl_cache_->unref_mem(prob_image_.ptr());
    opencl_cache_->unref_mem(t_infinity_.ptr());
    opencl_cache_->unref_mem(subblk_dim_.ptr());
  }
  // common stuff
  opencl_cache_->unref_mem( output_.ptr() );
  opencl_cache_->unref_mem( blk_info_target_.ptr() );
  opencl_cache_->unref_mem( octree_depth_.ptr() );
  opencl_cache_->unref_mem( lookup_.ptr() );

  // transform stuff
  opencl_cache_->unref_mem( centerX_.ptr() );
  opencl_cache_->unref_mem( centerY_.ptr() );
  opencl_cache_->unref_mem( centerZ_.ptr() );
}


bool boxm2_vecf_ocl_transform_scene::init_render_args()
{
  lthreads_[0]=8;
  lthreads_[1]=8;

  if ((ni_ == 0) || (nj_ == 0)) {
    vcl_cerr << "Error: invalid image dimensions " << ni_ << ", " << nj_ << vcl_endl;
    return false;
  }
  cl_ni_=RoundUp(ni_,lthreads_[0]);
  cl_nj_=RoundUp(nj_,lthreads_[1]);


  bool good_buffers = true;

  img_buff_ = new float[cl_ni_*cl_nj_];
  vcl_fill(img_buff_, img_buff_ + cl_ni_*cl_nj_, 0.0f);
  exp_image_=opencl_cache_->alloc_mem(cl_ni_*cl_nj_*sizeof(float), img_buff_,"exp image buffer");
  good_buffers &= exp_image_->create_buffer(CL_MEM_READ_WRITE | CL_MEM_COPY_HOST_PTR);

  vis_buff_ = new float[cl_ni_*cl_nj_];
  vcl_fill(vis_buff_, vis_buff_ + cl_ni_*cl_nj_, 1.0f);
  vis_image_ = opencl_cache_->alloc_mem(cl_ni_*cl_nj_*sizeof(float), vis_buff_,"vis image buffer");
  good_buffers &= vis_image_->create_buffer(CL_MEM_READ_WRITE | CL_MEM_COPY_HOST_PTR);

  max_omega_buff_ = new float[cl_ni_*cl_nj_];
  vcl_fill(max_omega_buff_, max_omega_buff_ + cl_ni_*cl_nj_, 0.0f);
  max_omega_image_ = opencl_cache_->alloc_mem(cl_ni_*cl_nj_*sizeof(float), max_omega_buff_,"vis image buffer");
  good_buffers &= max_omega_image_->create_buffer(CL_MEM_READ_WRITE | CL_MEM_COPY_HOST_PTR);

  tnearfar_buff_[0] = 0.0f;
  tnearfar_buff_[1] = 1000000.0f;

  tnearfar_mem_ptr_ = opencl_cache_->alloc_mem(2*sizeof(float), tnearfar_buff_, "tnearfar  buffer");
  good_buffers &= tnearfar_mem_ptr_->create_buffer(CL_MEM_READ_ONLY | CL_MEM_COPY_HOST_PTR);  
  
  img_dim_buff_[0] = 0;   img_dim_buff_[2] = (int)ni_;
  img_dim_buff_[1] = 0;   img_dim_buff_[3] = (int)nj_;

  exp_img_dim_ = new bocl_mem(device_->context(), img_dim_buff_, sizeof(int)*4, "image dims");
  good_buffers &= exp_img_dim_->create_buffer(CL_MEM_READ_ONLY | CL_MEM_COPY_HOST_PTR);

  // create all buffers
  ray_origins_ = new cl_float[4*cl_ni_*cl_nj_];
  ray_directions_ = new cl_float[4*cl_ni_*cl_nj_];
  ray_o_buff_ = opencl_cache_->alloc_mem(cl_ni_*cl_nj_*sizeof(cl_float4), ray_origins_, "ray_origins buffer");
  ray_d_buff_ = opencl_cache_->alloc_mem(cl_ni_*cl_nj_*sizeof(cl_float4), ray_directions_, "ray_directions buffer");
  good_buffers &= ray_o_buff_->create_buffer(CL_MEM_READ_WRITE | CL_MEM_COPY_HOST_PTR);
  good_buffers &= ray_d_buff_->create_buffer(CL_MEM_READ_WRITE | CL_MEM_COPY_HOST_PTR);

  // depth args
  depth_buff_ = new float[cl_ni_*cl_nj_];
  vcl_fill(depth_buff_, depth_buff_ + cl_ni_*cl_nj_, 0.0f);
  depth_image_ = opencl_cache_->alloc_mem(cl_ni_*cl_nj_*sizeof(float),depth_buff_,"exp depth buffer");
  good_buffers &= depth_image_->create_buffer(CL_MEM_READ_WRITE | CL_MEM_COPY_HOST_PTR);

  var_buff_ = new float[cl_ni_*cl_nj_];
  vcl_fill(var_buff_, var_buff_ + cl_ni_*cl_nj_, 0.0f);
  var_image_ = opencl_cache_->alloc_mem(cl_ni_*cl_nj_*sizeof(float),var_buff_,"var image buffer");
  good_buffers &= var_image_->create_buffer(CL_MEM_READ_WRITE | CL_MEM_COPY_HOST_PTR);

  prob_image_buff_ = new float[cl_ni_*cl_nj_];
  vcl_fill(prob_image_buff_, prob_image_buff_ + cl_ni_*cl_nj_, 0.0f);
  prob_image_ = opencl_cache_->alloc_mem(cl_ni_*cl_nj_*sizeof(float),prob_image_buff_,"vis x omega image buffer");
  good_buffers &= prob_image_->create_buffer(CL_MEM_READ_WRITE | CL_MEM_COPY_HOST_PTR);

  t_infinity_buff_ = new float[cl_ni_*cl_nj_];
  vcl_fill(t_infinity_buff_, t_infinity_buff_ + cl_ni_*cl_nj_, 0.0f);
  t_infinity_ = opencl_cache_->alloc_mem(cl_ni_*cl_nj_*sizeof(float),t_infinity_buff_,"t infinity buffer");
  good_buffers &= t_infinity_->create_buffer(CL_MEM_READ_WRITE | CL_MEM_COPY_HOST_PTR);

  subblk_dim_buff_ = 1.0f;
  subblk_dim_ = opencl_cache_->alloc_mem(sizeof(cl_float), &(subblk_dim_buff_), "sub block dim buffer");
  good_buffers &= subblk_dim_->create_buffer(CL_MEM_READ_ONLY | CL_MEM_COPY_HOST_PTR );

  // common stuff
  vcl_vector<boxm2_block_id> blocks_target = target_scene_->get_block_ids();
  vcl_vector<boxm2_block_id> blocks_source = source_scene_->get_block_ids();
  if(blocks_target.size()!=1||blocks_source.size()!=1) {
    std::cerr << "Error: boxm2_vecf_ocl_transform_scene: number of blocks > 1" << std::endl;
    return false;
  }
  vcl_vector<boxm2_block_id>::iterator iter_blk_target = blocks_target.begin();
  vcl_vector<boxm2_block_id>::iterator iter_blk_source = blocks_source.begin();
#if 0
  trans_interp_kern->set_arg(centerX_.ptr());
  trans_interp_kern->set_arg(centerY_.ptr());
  trans_interp_kern->set_arg(centerZ_.ptr());
  trans_interp_kern->set_arg(lookup_.ptr());
#endif

  octree_depth_buff_ = 0;
  octree_depth_ = new bocl_mem(device_->context(), &(octree_depth_buff_), sizeof(int), "  depth of octree " );
  good_buffers &= octree_depth_->create_buffer(CL_MEM_READ_ONLY | CL_MEM_COPY_HOST_PTR );

  //Gather information about the target and setup target data buffers
  // trans_interp_kernel arguments set the first time the function is called

  blk_target_       = opencl_cache_->get_block(target_scene_, *iter_blk_target);
  alpha_target_     = opencl_cache_->get_data<BOXM2_ALPHA>(target_scene_, *iter_blk_target,0, false);
  info_buffer_ = target_scene_->get_blk_metadata(*iter_blk_target);
  int alphaTypeSize = (int)boxm2_data_info::datasize(boxm2_data_traits<BOXM2_ALPHA>::prefix());
  info_buffer_->data_buffer_length = (int) (alpha_target_->num_bytes()/alphaTypeSize);
  int data_size = info_buffer_->data_buffer_length;
  blk_info_target_  = new bocl_mem(device_->context(), info_buffer_, sizeof(boxm2_scene_info), " Scene Info" );   
  good_buffers &= blk_info_target_->create_buffer(CL_MEM_READ_ONLY | CL_MEM_COPY_HOST_PTR);       

  if (!good_buffers) {
    std::cerr << "Error allocating one or more opencl buffers" << std::endl;
    std::cerr << "cl_ni_ = " << cl_ni_ << " cl_nj_ = " << cl_nj_ << std::endl;
    return false;
  }

  if(app_type_ == BOXM2_MOG3_GREY) {
    mog_target_       = opencl_cache_->get_data<BOXM2_MOG3_GREY>(target_scene_, *iter_blk_target,0,false);
  }
  else if(app_type_ == BOXM2_MOG3_GREY_16) {
    mog_target_       = opencl_cache_->get_data<BOXM2_MOG3_GREY_16>(target_scene_, *iter_blk_target,0,false);
  }
  else if (app_type_ == BOXM2_GAUSS_RGB) {
    mog_target_ = opencl_cache_->get_data<BOXM2_GAUSS_RGB>(target_scene_, *iter_blk_target,0,false);
  }
  else {
    vcl_cout << "Unknown appearance type for target_scene " << app_type_ << '\n';
    return false;
  }

  return true;
}


#if 0
bool boxm2_vecf_ocl_transform_scene::compile_trans_kernel()
{
  vcl_string options;
  // sets apptypesize_ and app_type
  get_scene_appearance(source_scene_, options);
  vcl_vector<vcl_string> src_paths;
  vcl_string source_dir = vcl_string(VCL_SOURCE_ROOT_DIR) + "/contrib/brl/bseg/boxm2/ocl/cl/";
  vcl_string vecf_source_dir = vcl_string(VCL_SOURCE_ROOT_DIR)+ "/contrib/brl/bseg/boxm2/vecf/ocl/cl/";
  src_paths.push_back(source_dir     + "scene_info.cl");
  src_paths.push_back(source_dir     + "bit/bit_tree_library_functions.cl");
  src_paths.push_back(vecf_source_dir + "transform_scene_blockwise.cl");
  this->trans_kern = new bocl_kernel();
  return trans_kern->create_kernel(&device_->context(),device_->device_id(), src_paths, "transform_scene_blockwise", options, "trans_scene");
}
#endif

bool boxm2_vecf_ocl_transform_scene::compile_trans_interp_kernel()
{
  vcl_string options;
  // sets apptypesize_ and app_type
  get_scene_appearance(source_scene_, options);
  vcl_vector<vcl_string> src_paths;
  vcl_string source_dir = vcl_string(VCL_SOURCE_ROOT_DIR) + "/contrib/brl/bseg/boxm2/ocl/cl/";
  vcl_string vecf_source_dir = vcl_string(VCL_SOURCE_ROOT_DIR)+ "/contrib/brl/bseg/boxm2/vecf/ocl/cl/";
  src_paths.push_back(source_dir     + "scene_info.cl");
  src_paths.push_back(source_dir     + "bit/bit_tree_library_functions.cl");
  src_paths.push_back(vecf_source_dir + "transform_scene_with_interpolation.cl");
  this->trans_interp_kern = new bocl_kernel();
  return trans_interp_kern->create_kernel(&device_->context(),device_->device_id(), src_paths, "transform_scene_interpolate", options, "trans_interp_scene");
}

bool boxm2_vecf_ocl_transform_scene::compile_depth_kernel()
{
    vcl_vector<vcl_string> src_paths;
    vcl_string source_dir = boxm2_ocl_util::ocl_src_root();
    src_paths.push_back(source_dir + "scene_info.cl");
    src_paths.push_back(source_dir + "pixel_conversion.cl");
    src_paths.push_back(source_dir + "bit/bit_tree_library_functions.cl");
    src_paths.push_back(source_dir + "backproject.cl");
    src_paths.push_back(source_dir + "statistics_library_functions.cl");
    src_paths.push_back(source_dir + "expected_functor.cl");
    src_paths.push_back(source_dir + "ray_bundle_library_opt.cl");
    src_paths.push_back(source_dir + "bit/render_bit_scene.cl");
    src_paths.push_back(source_dir + "bit/cast_ray_bit.cl");

    //set kernel options
    vcl_string options = " -D RENDER_DEPTH ";
    options +=  "-D DETERMINISTIC";
    options += " -D STEP_CELL=step_cell_render_depth2(tblock,linfo->block_len,aux_args.alpha,data_ptr,d*linfo->block_len,aux_args.vis,aux_args.expdepth,aux_args.expdepthsqr,aux_args.probsum,aux_args.t)";

    //have kernel construct itself using the context and device
    this->depth_kern_ = new bocl_kernel();

    bool good = depth_kern_->create_kernel( &device_->context(),
                                            device_->device_id(),
                                            src_paths,
                                            "render_depth",   //kernel name
                                            options,              //options
                                            "boxm2 opencl render depth image"); //kernel identifier (for error checking)
    return good;
}

bool boxm2_vecf_ocl_transform_scene::compile_depth_norm_kernel()
{
    vcl_vector<vcl_string> norm_src_paths;
    vcl_string source_dir = boxm2_ocl_util::ocl_src_root();
    norm_src_paths.push_back(source_dir + "scene_info.cl");

    norm_src_paths.push_back(source_dir + "pixel_conversion.cl");
    norm_src_paths.push_back(source_dir + "bit/normalize_kernels.cl");
    depth_norm_kern_ = new bocl_kernel();

    vcl_string options = " -D RENDER_DEPTH ";

    bool good = depth_norm_kern_->create_kernel( &device_->context(),
                                                 device_->device_id(),
                                                 norm_src_paths,
                                                 "normalize_render_depth_kernel",   //kernel name
                                                 options,              //options
                                                 "normalize render depth kernel"); //kernel identifier (for error checking)
    return good;
}

bool boxm2_vecf_ocl_transform_scene::init_ocl_trans()
{
    centerX_ = new bocl_mem(device_->context(), boct_bit_tree::centerX, sizeof(cl_float)*585, "centersX lookup buffer");
    centerY_ = new bocl_mem(device_->context(), boct_bit_tree::centerY, sizeof(cl_float)*585, "centersY lookup buffer");
    centerZ_ = new bocl_mem(device_->context(), boct_bit_tree::centerZ, sizeof(cl_float)*585, "centersZ lookup buffer");
  bocl_mem_sptr output;
    centerX_->create_buffer(CL_MEM_READ_ONLY | CL_MEM_COPY_HOST_PTR);
    centerY_->create_buffer(CL_MEM_READ_ONLY | CL_MEM_COPY_HOST_PTR);
    centerZ_->create_buffer(CL_MEM_READ_ONLY | CL_MEM_COPY_HOST_PTR);

    // output buffer for debugging
    output_ = new bocl_mem(device_->context(), output_buff_, sizeof(float)*1000, "output" );
    output_->create_buffer(CL_MEM_READ_WRITE | CL_MEM_COPY_HOST_PTR );

    // bit lookup buffer
    boxm2_ocl_util::set_bit_lookup(lookup_arr_);
    lookup_=new bocl_mem(device_->context(), lookup_arr_, sizeof(cl_uchar)*256, "bit lookup buffer");
    lookup_->create_buffer(CL_MEM_READ_ONLY | CL_MEM_COPY_HOST_PTR);

    int status = 0;
    queue_ = clCreateCommandQueue(device_->context(),*(device_->device_id()),CL_QUEUE_PROFILING_ENABLE,&status);

    octree_depth_ = 0;
    blk_info_target_ = 0;
    blk_info_source = 0;
    info_buffer_ = 0;
    blk_target_ = 0;
    alpha_target_ = 0;
    mog_target_ = 0;
    info_buffer_source = 0;
    blk_source = 0;
    alpha_source = 0;
    mog_source = 0;
    return true;
}
// the transform is the inverse, i.e. a map from the target to the source
// an arbitrary number of blocks are handled, no interpolation
bool boxm2_vecf_ocl_transform_scene::transform(vgl_rotation_3d<double> rot,
            vgl_vector_3d<double> trans,
            vgl_vector_3d<double> scale)

{
#if 0
   float translation_buff[4];
   translation_buff[0] = trans.x();        
   translation_buff[1] = trans.y();        
   translation_buff[2] = trans.z();
   translation_buff[3] = 0.0;
   bocl_mem_sptr translation = new bocl_mem(device_->context(), translation_buff, sizeof(float)*4, " translation " );
   translation->create_buffer(CL_MEM_READ_ONLY | CL_MEM_COPY_HOST_PTR );

   float scale_buff[4];
   scale_buff[0] = scale.x();        
   scale_buff[1] = scale.y();        
   scale_buff[2] = scale.z();
   scale_buff[3] = 0.0;
   bocl_mem_sptr scalem = new bocl_mem(device_->context(), scale_buff, sizeof(float)*4, " scale " );
   scalem->create_buffer(CL_MEM_READ_ONLY | CL_MEM_COPY_HOST_PTR );

   float rotation_buff[9];
   vnl_matrix_fixed<double, 3, 3> R = rot.as_matrix();
   rotation_buff[0] = R(0,0);  rotation_buff[3] = R(1,0);  rotation_buff[6] = R(2,0);
   rotation_buff[1] = R(0,1);  rotation_buff[4] = R(1,1);  rotation_buff[7] = R(2,1);
   rotation_buff[2] = R(0,2);  rotation_buff[5] = R(1,2);  rotation_buff[8] = R(2,2);
   bocl_mem_sptr rotation = new bocl_mem(device_->context(), rotation_buff, sizeof(float)*9, " rotation " );
   rotation->create_buffer(CL_MEM_READ_ONLY | CL_MEM_COPY_HOST_PTR );
    
   bocl_mem_sptr ocl_depth = new bocl_mem(device_->context(), &(depth), sizeof(int), "  depth of octree " );
   ocl_depth->create_buffer(CL_MEM_READ_ONLY | CL_MEM_COPY_HOST_PTR );

   vcl_vector<boxm2_block_id> blocks_target = target_scene_->get_block_ids();
   vcl_vector<boxm2_block_id> blocks_source = source_scene_->get_block_ids();
   vcl_vector<boxm2_block_id>::iterator iter_blks_target = blocks_target.begin();
   vcl_vector<boxm2_block_id>::iterator iter_blks_source = blocks_source.begin();
   vcl_size_t local_threads[1]={64};
   vcl_size_t global_threads[1]={1};
   int status=0;    float gpu_time = 0.0;
   vcl_cout<<"Cache size "<<opencl_cache_->bytes_in_cache()/1024/1024<<" MB"<<vcl_endl;
   for (;iter_blks_target!=blocks_target.end(); iter_blks_target++)
     {
       //Gather information about the target and setup target data buffers
       bocl_mem* blk_target       = opencl_cache_->get_block(target_scene_, *iter_blks_target);
       bocl_mem* alpha_target     = opencl_cache_->get_data<BOXM2_ALPHA>(target_scene_, *iter_blks_target,0,false);
       boxm2_scene_info* info_buffer = target_scene_->get_blk_metadata(*iter_blks_target);
       int alphaTypeSize = (int)boxm2_data_info::datasize(boxm2_data_traits<BOXM2_ALPHA>::prefix());
       info_buffer->data_buffer_length = (int) (alpha_target->num_bytes()/alphaTypeSize);
       int data_size = info_buffer->data_buffer_length;
       bocl_mem* blk_info_target  = new bocl_mem(device_->context(), info_buffer, sizeof(boxm2_scene_info), " Scene Info" );   
       blk_info_target->create_buffer(CL_MEM_READ_ONLY | CL_MEM_COPY_HOST_PTR);       
       bocl_mem * mog_target = 0;
       if(app_type_ == "boxm2_mog3_grey")
      mog_target       = opencl_cache_->get_data<BOXM2_MOG3_GREY>(target_scene_, *iter_blks_target,0,false);
       else if(app_type_ == "boxm2_mog3_grey_16")
      mog_target       = opencl_cache_->get_data<BOXM2_MOG3_GREY_16>(target_scene_, *iter_blks_target,0,false);
       else {
      vcl_cout << "Unknown appearance type for target_scene " << app_type_ << '\n';
      return false;
       }
       vgl_box_3d<float> box_target(info_buffer->scene_origin[0],info_buffer->scene_origin[1],info_buffer->scene_origin[2],
        info_buffer->scene_origin[0]+info_buffer->scene_dims[0]*info_buffer->block_len,
        info_buffer->scene_origin[1]+info_buffer->scene_dims[1]*info_buffer->block_len,
        info_buffer->scene_origin[2]+info_buffer->scene_dims[2]*info_buffer->block_len);
       
       vgl_box_3d<float> box_target_xformed;
       for(unsigned int k = 0 ; k<box_target.vertices().size(); k++)
  {
    vgl_point_3d<float> p = box_target.vertices()[k];
    vgl_point_3d<float> px(scale.x()*(p.x()*R(0,0)+p.y()*R(0,1)+p.z()*R(0,2) + trans.x()),
      scale.y()*(p.x()*R(1,0)+p.y()*R(1,1)+p.z()*R(1,2) + trans.y()),
      scale.z()*(p.x()*R(2,0)+p.y()*R(2,1)+p.z()*R(2,2) + trans.z()));
    box_target_xformed.add(px);
  }

       global_threads[0] = (unsigned) RoundUp(info_buffer->scene_dims[0]*info_buffer->scene_dims[1]*info_buffer->scene_dims[2],(int)local_threads[0]);
       // for each target block iterate over source blocks
       for (iter_blks_source = blocks_source.begin();iter_blks_source!=blocks_source.end(); iter_blks_source++)
  {
    //Gather information about the source and setup source data buffers
    boxm2_scene_info* info_buffer_source = source_scene_->get_blk_metadata(*iter_blks_source);
    vgl_box_3d<float> box_source(info_buffer_source->scene_origin[0],info_buffer_source->scene_origin[1],info_buffer_source->scene_origin[2],
     info_buffer_source->scene_origin[0]+info_buffer_source->scene_dims[0]*info_buffer_source->block_len,
     info_buffer_source->scene_origin[1]+info_buffer_source->scene_dims[1]*info_buffer_source->block_len,
     info_buffer_source->scene_origin[2]+info_buffer_source->scene_dims[2]*info_buffer_source->block_len);

    // If the bounding box of the reverse transformed target doesn't intersect the source box
    // then there is nothing to do.
    if(!vgl_intersection<float>(box_target_xformed,box_source).is_empty())
      {
        // get more information about the source block since it will actually be used.
               bocl_mem* blk_source       = opencl_cache_->get_block(source_scene_, *iter_blks_source);
               bocl_mem* alpha_source     = opencl_cache_->get_data<BOXM2_ALPHA>(source_scene_, *iter_blks_source,0,true);
               int alphaTypeSize = (int)boxm2_data_info::datasize(boxm2_data_traits<BOXM2_ALPHA>::prefix());
               info_buffer_source->data_buffer_length = (int) (alpha_source->num_bytes()/alphaTypeSize);

               bocl_mem* blk_info_source  = new bocl_mem(device_->context(), info_buffer_source, sizeof(boxm2_scene_info), " Scene Info" );   
               blk_info_source->create_buffer(CL_MEM_READ_ONLY | CL_MEM_COPY_HOST_PTR);
        bocl_mem* mog_source = 0;
        if(app_type_ == "boxm2_mog3_grey")
          mog_source       = opencl_cache_->get_data<BOXM2_MOG3_GREY>(source_scene_, *iter_blks_source,0,true);
        else if(app_type_ == "boxm2_mog3_grey_16")
          mog_source       = opencl_cache_->get_data<BOXM2_MOG3_GREY_16>(source_scene_, *iter_blks_source,0,true);
        else {
   vcl_cout << "Unknown appearance type for source_scene " << app_type_ << '\n';
   return false;
        }
               trans_kern->set_arg(centerX.ptr());
               trans_kern->set_arg(centerY.ptr());
               trans_kern->set_arg(centerZ.ptr());
               trans_kern->set_arg(lookup_.ptr());
               trans_kern->set_arg(blk_info_target);
               trans_kern->set_arg(blk_info_source);
               trans_kern->set_arg(blk_target);
               trans_kern->set_arg(alpha_target);
               trans_kern->set_arg(mog_target);
               trans_kern->set_arg(blk_source);
               trans_kern->set_arg(alpha_source);
            trans_kern->set_arg(mog_source);
               trans_kern->set_arg(translation.ptr());
               trans_kern->set_arg(rotation.ptr());
               trans_kern->set_arg(scalem.ptr());
               trans_kern->set_arg(octree_depth_.ptr());
               trans_kern->set_arg(output_.ptr());
            trans_kern->set_local_arg(local_threads[0]*10*sizeof(cl_uchar) );    // cumsum buffer,
               trans_kern->set_local_arg(16*local_threads[0]*sizeof(unsigned char)); // local trees target
               trans_kern->set_local_arg(16*local_threads[0]*sizeof(unsigned char)); // local trees source
               if(!trans_kern->execute(queue_, 1, local_threads, global_threads))
               {
                   vcl_cout<<"Kernel Failed to Execute "<<vcl_endl;
                   return false;
               }
               int status = clFinish(queue_);
               check_val(status, MEM_FAILURE, "MIFO EXECUTE FAILED: " + error_to_string(status));
               gpu_time += trans_kern->exec_time();
               //clear kernel args so it can reset them on next execution
               trans_kern->clear_args();
               clFinish(queue_);
               blk_info_source->release_memory();
               delete info_buffer_source;
            }
    mog_target->read_to_buffer(queue_);
    alpha_target->read_to_buffer(queue_);
  }
       blk_info_target->release_memory();
       delete info_buffer;
     }
   clFinish(queue_);
   opencl_cache_->unref_mem(translation.ptr());
   opencl_cache_->unref_mem(rotation.ptr());
   opencl_cache_->unref_mem(scalem.ptr());
   opencl_cache_->unref_mem(octree_depth_.ptr());
   boxm2_lru_cache::instance()->write_to_disk(target_scene_);
#endif
   return true;
}
bool boxm2_vecf_ocl_transform_scene::transform_1_blk(vgl_rotation_3d<double>  rot,
           vgl_vector_3d<double> trans,
           vgl_vector_3d<double> scale,
           bool finish){
#if 0
  static bool first = true;
  int depth = 0;
  // set up the buffers the first time the function is called
  // subsequent calls don't need to recreate the buffers
    if(first){
    translation_buff = new float[4];
    rotation_buff = new float[9];
    scale_buff = new float[4];
  }
   translation_buff[0] = trans.x();        
   translation_buff[1] = trans.y();        
   translation_buff[2] = trans.z();
   translation_buff[3] = 0.0;

   vnl_matrix_fixed<double, 3, 3> R = rot.as_matrix();
   rotation_buff[0] = R(0,0);  rotation_buff[3] = R(1,0);  rotation_buff[6] = R(2,0);
   rotation_buff[1] = R(0,1);  rotation_buff[4] = R(1,1);  rotation_buff[7] = R(2,1);
   rotation_buff[2] = R(0,2);  rotation_buff[5] = R(1,2);  rotation_buff[8] = R(2,2);
   
   scale_buff[0] = scale.x();        
   scale_buff[1] = scale.y();        
   scale_buff[2] = scale.z();
   scale_buff[3] = 0.0;
   int statusw =0;
   // just copy the transformation parameters to gpu memory 
   if(!first){
     translation->write_to_buffer(queue_);
     rotation->write_to_buffer(queue_);
     scalem->write_to_buffer(queue_);
     statusw = clFinish(queue_);
     bool good_write = check_val(statusw, CL_SUCCESS, "WRITE TO GPU FAILED " + error_to_string(statusw));
     if(!good_write)
       return false;
   }
   vcl_size_t local_threads[1]={64};
   static vcl_size_t global_threads[1]={1};   
  // set up all the kernel arguments 
  // subsequent calls don't need to do this initialization
   if(first){
     translation = new bocl_mem(device_->context(), translation_buff, sizeof(float)*4, " translation " );
     translation->create_buffer(CL_MEM_READ_ONLY | CL_MEM_COPY_HOST_PTR );

     rotation = new bocl_mem(device_->context(), rotation_buff, sizeof(float)*9, " rotation " );
     rotation->create_buffer(CL_MEM_READ_ONLY | CL_MEM_COPY_HOST_PTR );
     
     scalem = new bocl_mem(device_->context(), scale_buff, sizeof(float)*4, " scale " );
     scalem->create_buffer(CL_MEM_READ_ONLY | CL_MEM_COPY_HOST_PTR );

     vcl_vector<boxm2_block_id> blocks_target = target_scene_->get_block_ids();
     vcl_vector<boxm2_block_id> blocks_source = source_scene_->get_block_ids();
     if(blocks_target.size()!=1||blocks_source.size()!=1)
       return false;
     vcl_vector<boxm2_block_id>::iterator iter_blk_target = blocks_target.begin();
     vcl_vector<boxm2_block_id>::iterator iter_blk_source = blocks_source.begin();
     trans_kern->set_arg(centerX.ptr());
     trans_kern->set_arg(centerY.ptr());
     trans_kern->set_arg(centerZ.ptr());
     trans_kern->set_arg(lookup_.ptr());

     octree_depth_ = new bocl_mem(device_->context(), &(depth), sizeof(int), "  depth of octree " );
     octree_depth_->create_buffer(CL_MEM_READ_ONLY | CL_MEM_COPY_HOST_PTR );

     //Gather information about the target and setup target data buffers
     // trans_kernel arguments set the first time the function is called
   
     blk_target_       = opencl_cache_->get_block(target_scene_, *iter_blk_target);
     alpha_target_     = opencl_cache_->get_data<BOXM2_ALPHA>(target_scene_, *iter_blk_target,0,true);
     info_buffer = target_scene_->get_blk_metadata(*iter_blk_target);
     int alphaTypeSize = (int)boxm2_data_info::datasize(boxm2_data_traits<BOXM2_ALPHA>::prefix());
     info_buffer->data_buffer_length = (int) (alpha_target_->num_bytes()/alphaTypeSize);
     int data_size = info_buffer->data_buffer_length;
     blk_info_target_  = new bocl_mem(device_->context(), info_buffer, sizeof(boxm2_scene_info), " Scene Info" );   
     blk_info_target_->create_buffer(CL_MEM_READ_ONLY | CL_MEM_COPY_HOST_PTR);       
   
     if(app_type_ == "boxm2_mog3_grey")
       mog_target_       = opencl_cache_->get_data<BOXM2_MOG3_GREY>(target_scene_, *iter_blk_target,0,true);
     else if(app_type_ == "boxm2_mog3_grey_16")
       mog_target_       = opencl_cache_->get_data<BOXM2_MOG3_GREY_16>(target_scene_, *iter_blk_target,0,true);
     else {
       vcl_cout << "Unknown appearance type for target_scene " << app_type_ << '\n';
       return false;
     }
     vgl_box_3d<float> box_target(info_buffer->scene_origin[0],info_buffer->scene_origin[1],info_buffer->scene_origin[2],
      info_buffer->scene_origin[0]+info_buffer->scene_dims[0]*info_buffer->block_len,
      info_buffer->scene_origin[1]+info_buffer->scene_dims[1]*info_buffer->block_len,
      info_buffer->scene_origin[2]+info_buffer->scene_dims[2]*info_buffer->block_len);
       
     vgl_box_3d<float> box_target_xformed;//note the transformation is the inverse
     for(unsigned int k = 0 ; k<box_target.vertices().size(); k++)
       {
  vgl_point_3d<float> p = box_target.vertices()[k];
  vgl_point_3d<float> px(scale.x()*(p.x()*R(0,0)+p.y()*R(0,1)+p.z()*R(0,2)) + trans.x(),
    scale.y()*(p.x()*R(1,0)+p.y()*R(1,1)+p.z()*R(1,2)) + trans.y(),
    scale.z()*(p.x()*R(2,0)+p.y()*R(2,1)+p.z()*R(2,2)) + trans.z());
  box_target_xformed.add(px);
       }

   global_threads[0] = (unsigned) RoundUp(info_buffer->scene_dims[0]*info_buffer->scene_dims[1]*info_buffer->scene_dims[2],(int)local_threads[0]);
       // for each target block iterate over source blocks
   
   //Gather information about the source and setup source data buffers
   info_buffer_source = source_scene_->get_blk_metadata(*iter_blk_source);
   
   vgl_box_3d<float> box_source(info_buffer_source->scene_origin[0],info_buffer_source->scene_origin[1],info_buffer_source->scene_origin[2],
    info_buffer_source->scene_origin[0]+info_buffer_source->scene_dims[0]*info_buffer_source->block_len,
    info_buffer_source->scene_origin[1]+info_buffer_source->scene_dims[1]*info_buffer_source->block_len,
    info_buffer_source->scene_origin[2]+info_buffer_source->scene_dims[2]*info_buffer_source->block_len);

   // If the bounding box of the reverse transformed target doesn't intersect the source box
   // then there is nothing to do.
   if(vgl_intersection<float>(box_target_xformed,box_source).is_empty())
     return false;
   // get more information about the source block since it will actually be used.
   blk_source       = opencl_cache_->get_block(source_scene_, *iter_blk_source);
   alpha_source     = opencl_cache_->get_data<BOXM2_ALPHA>(source_scene_, *iter_blk_source,0,false);
   info_buffer_source->data_buffer_length = (int) (alpha_source->num_bytes()/alphaTypeSize);
   
   blk_info_source  = new bocl_mem(device_->context(), info_buffer_source, sizeof(boxm2_scene_info), " Scene Info" );   
   blk_info_source->create_buffer(CL_MEM_READ_ONLY | CL_MEM_COPY_HOST_PTR);
   if(app_type_ == "boxm2_mog3_grey")
     mog_source       = opencl_cache_->get_data<BOXM2_MOG3_GREY>(source_scene_, *iter_blk_source,0,false);
   else if(app_type_ == "boxm2_mog3_grey_16")
     mog_source       = opencl_cache_->get_data<BOXM2_MOG3_GREY_16>(source_scene_, *iter_blk_source,0,false);
   else {
     vcl_cout << "Unknown appearance type for source_scene " << app_type_ << '\n';
     return false;
   }
   trans_kern->set_arg(blk_info_target);
   trans_kern->set_arg(blk_info_source);
   trans_kern->set_arg(blk_target);
   trans_kern->set_arg(alpha_target);
   trans_kern->set_arg(mog_target);
   trans_kern->set_arg(blk_source);
   trans_kern->set_arg(alpha_source);
   trans_kern->set_arg(mog_source);
   trans_kern->set_arg(translation);
   trans_kern->set_arg(rotation);
   trans_kern->set_arg(scalem);
   trans_kern->set_arg(ocl_depth);
   trans_kern->set_arg(output.ptr());
   trans_kern->set_local_arg(local_threads[0]*10*sizeof(cl_uchar) );    // cumsum buffer,
   trans_kern->set_local_arg(16*local_threads[0]*sizeof(unsigned char)); // local trees target
   trans_kern->set_local_arg(16*local_threads[0]*sizeof(unsigned char)); // local trees source
   first = false;
   }
   if(!trans_kern->execute(queue_, 1, local_threads, global_threads))
     {
       vcl_cout<<"Kernel Failed to Execute "<<vcl_endl;
       return false;
     }
   int status = clFinish(queue_);
   bool good_kern = check_val(status, CL_SUCCESS, "TRANSFORMATION KERNEL FAILED: " + error_to_string(status));
   if(!good_kern)
     return false;
   mog_target->read_to_buffer(queue_);
   alpha_target->read_to_buffer(queue_);
   status = clFinish(queue_);
   bool good_read = check_val(status, CL_SUCCESS, "READ FROM GPU FAILED: " + error_to_string(status));
   if(!good_read)
     return false;
   if(finish){
     trans_kern->clear_args();
     boxm2_lru_cache::instance()->write_to_disk(target_scene_);
     ocl_depth->release_memory();
     blk_info_source->release_memory();
     delete info_buffer_source;
     blk_info_target->release_memory();
     delete info_buffer;
     opencl_cache_->unref_mem(translation);
     opencl_cache_->unref_mem(rotation);
     opencl_cache_->unref_mem(scalem);
     translation = 0; rotation = 0; scalem = 0;
     delete [] translation_buff;
     delete [] rotation_buff;
     delete [] scale_buff;
     scale_buff = 0;   rotation_buff = 0;   translation_buff = 0;
   }
#endif
   return true;
}
bool boxm2_vecf_ocl_transform_scene::transform_1_blk_interp(vgl_rotation_3d<double>  rot,
                                                            vgl_vector_3d<double> trans,
                                                            vgl_vector_3d<double> scale,
                                                            bool finish){
#if 1
 static bool first = true;
  int depth = 0;
  // set up the buffers the first time the function is called
  // subsequent calls don't need to recreate the buffers
    if(first){
    translation_buff = new float[4];
    rotation_buff = new float[9];
    scale_buff = new float[4];
  }
   translation_buff[0] = trans.x();        
   translation_buff[1] = trans.y();        
   translation_buff[2] = trans.z();
   translation_buff[3] = 0.0;

   vnl_matrix_fixed<double, 3, 3> R = rot.as_matrix();
   rotation_buff[0] = R(0,0);  rotation_buff[3] = R(1,0);  rotation_buff[6] = R(2,0);
   rotation_buff[1] = R(0,1);  rotation_buff[4] = R(1,1);  rotation_buff[7] = R(2,1);
   rotation_buff[2] = R(0,2);  rotation_buff[5] = R(1,2);  rotation_buff[8] = R(2,2);
   
   scale_buff[0] = scale.x();        
   scale_buff[1] = scale.y();        
   scale_buff[2] = scale.z();
   scale_buff[3] = 0.0;
   int statusw =0;
   // just copy the transformation parameters to gpu memory 
   if(!first){
     translation->write_to_buffer(queue_);
     rotation->write_to_buffer(queue_);
     scalem->write_to_buffer(queue_);
     statusw = clFinish(queue_);
     bool good_write = check_val(statusw, CL_SUCCESS, "WRITE TO GPU FAILED " + error_to_string(statusw));
     if(!good_write)
       return false;
   }
   vcl_size_t local_threads[1]={64};
   static vcl_size_t global_threads[1]={1};   
  // set up all the kernel arguments 
  // subsequent calls don't need to do this initialization
   if(first){
     translation = new bocl_mem(device_->context(), translation_buff, sizeof(float)*4, " translation " );
     translation->create_buffer(CL_MEM_READ_ONLY | CL_MEM_COPY_HOST_PTR );

     rotation = new bocl_mem(device_->context(), rotation_buff, sizeof(float)*9, " rotation " );
     rotation->create_buffer(CL_MEM_READ_ONLY | CL_MEM_COPY_HOST_PTR );
     
     scalem = new bocl_mem(device_->context(), scale_buff, sizeof(float)*4, " scale " );
     scalem->create_buffer(CL_MEM_READ_ONLY | CL_MEM_COPY_HOST_PTR );

     trans_interp_kern->set_arg(centerX_.ptr());
     trans_interp_kern->set_arg(centerY_.ptr());
     trans_interp_kern->set_arg(centerZ_.ptr());
     trans_interp_kern->set_arg(lookup_.ptr());

     vcl_vector<boxm2_block_id> blocks_target = target_scene_->get_block_ids();
     vcl_vector<boxm2_block_id> blocks_source = source_scene_->get_block_ids();
     if(blocks_target.size()!=1||blocks_source.size()!=1)
       return false;
     vcl_vector<boxm2_block_id>::iterator iter_blk_target = blocks_target.begin();
     vcl_vector<boxm2_block_id>::iterator iter_blk_source = blocks_source.begin();

#if 0
     ocl_depth = new bocl_mem(device_->context(), &(depth), sizeof(int), "  depth of octree " );
     ocl_depth->create_buffer(CL_MEM_READ_ONLY | CL_MEM_COPY_HOST_PTR );

     //Gather information about the target and setup target data buffers
     // trans_interp_kernel arguments set the first time the function is called
   
     blk_target       = opencl_cache_->get_block(target_scene_, *iter_blk_target);
     alpha_target     = opencl_cache_->get_data<BOXM2_ALPHA>(target_scene_, *iter_blk_target,0, false);
     info_buffer = target_scene_->get_blk_metadata(*iter_blk_target);
     int alphaTypeSize = (int)boxm2_data_info::datasize(boxm2_data_traits<BOXM2_ALPHA>::prefix());
     info_buffer->data_buffer_length = (int) (alpha_target->num_bytes()/alphaTypeSize);
     int data_size = info_buffer->data_buffer_length;
     blk_info_target  = new bocl_mem(device_->context(), info_buffer, sizeof(boxm2_scene_info), " Scene Info" );   
     blk_info_target->create_buffer(CL_MEM_READ_ONLY | CL_MEM_COPY_HOST_PTR);       
   
     if(app_type_ == "boxm2_mog3_grey")
       mog_target       = opencl_cache_->get_data<BOXM2_MOG3_GREY>(target_scene_, *iter_blk_target,0,false);
     else if(app_type_ == "boxm2_mog3_grey_16")
       mog_target       = opencl_cache_->get_data<BOXM2_MOG3_GREY_16>(target_scene_, *iter_blk_target,0,false);
     else {
       vcl_cout << "Unknown appearance type for target_scene " << app_type_ << '\n';
       return false;
     }
#endif
     vgl_box_3d<float> box_target(info_buffer_->scene_origin[0],info_buffer_->scene_origin[1],info_buffer_->scene_origin[2],
      info_buffer_->scene_origin[0]+info_buffer_->scene_dims[0]*info_buffer_->block_len,
      info_buffer_->scene_origin[1]+info_buffer_->scene_dims[1]*info_buffer_->block_len,
      info_buffer_->scene_origin[2]+info_buffer_->scene_dims[2]*info_buffer_->block_len);
       
     vgl_box_3d<float> box_target_xformed;//note the transformation is the inverse
     for(unsigned int k = 0 ; k<box_target.vertices().size(); k++)
       {
  vgl_point_3d<float> p = box_target.vertices()[k];
  vgl_point_3d<float> px(scale.x()*(p.x()*R(0,0)+p.y()*R(0,1)+p.z()*R(0,2)) + trans.x(),
    scale.y()*(p.x()*R(1,0)+p.y()*R(1,1)+p.z()*R(1,2)) + trans.y(),
    scale.z()*(p.x()*R(2,0)+p.y()*R(2,1)+p.z()*R(2,2)) + trans.z());
  box_target_xformed.add(px);
       }

   global_threads[0] = (unsigned) RoundUp(info_buffer_->scene_dims[0]*info_buffer_->scene_dims[1]*info_buffer_->scene_dims[2],(int)local_threads[0]);
       // for each target block iterate over source blocks
   
   //Gather information about the source and setup source data buffers
   info_buffer_source = source_scene_->get_blk_metadata(*iter_blk_source);
   
   vgl_box_3d<float> box_source(info_buffer_source->scene_origin[0],info_buffer_source->scene_origin[1],info_buffer_source->scene_origin[2],
    info_buffer_source->scene_origin[0]+info_buffer_source->scene_dims[0]*info_buffer_source->block_len,
    info_buffer_source->scene_origin[1]+info_buffer_source->scene_dims[1]*info_buffer_source->block_len,
    info_buffer_source->scene_origin[2]+info_buffer_source->scene_dims[2]*info_buffer_source->block_len);

   // If the bounding box of the reverse transformed target doesn't intersect the source box
   // then there is nothing to do.
   if(vgl_intersection<float>(box_target_xformed,box_source).is_empty())
     return false;
   // get more information about the source block since it will actually be used.
   blk_source       = opencl_cache_->get_block(source_scene_, *iter_blk_source);
   alpha_source     = opencl_cache_->get_data<BOXM2_ALPHA>(source_scene_, *iter_blk_source,0,true);
   int alphaTypeSize = (int)boxm2_data_info::datasize(boxm2_data_traits<BOXM2_ALPHA>::prefix());
   info_buffer_source->data_buffer_length = (int) (alpha_source->num_bytes()/alphaTypeSize);
   
   blk_info_source  = new bocl_mem(device_->context(), info_buffer_source, sizeof(boxm2_scene_info), " Scene Info" );   
   blk_info_source->create_buffer(CL_MEM_READ_ONLY | CL_MEM_COPY_HOST_PTR);
   if(app_type_ == BOXM2_MOG3_GREY) {
     mog_source       = opencl_cache_->get_data<BOXM2_MOG3_GREY>(source_scene_, *iter_blk_source,0,true);
   }
   else if(app_type_ == BOXM2_MOG3_GREY_16) {
     mog_source       = opencl_cache_->get_data<BOXM2_MOG3_GREY_16>(source_scene_, *iter_blk_source,0,true);
   }
   else if (app_type_ == BOXM2_GAUSS_RGB) {
     mog_source = opencl_cache_->get_data<BOXM2_GAUSS_RGB>(source_scene_, *iter_blk_source,0,true);
   }
   else {
     vcl_cout << "ERROR: boxm2_vecf_ocl_transform_scene: Unsupported appearance type for source_scene " << boxm2_data_info::prefix(app_type_) << '\n';
     return false;
   }
   //   nobs_source = opencl_cache_->get_data<BOXM2_NUM_OBS>(source_scene_, *iter_blk_source,0,false);
   nbr_exint = opencl_cache_->get_data<BOXM2_CHAR8>(source_scene_, *iter_blk_source,0,true, "nbr_exint");
   nbr_exists = opencl_cache_->get_data<BOXM2_CHAR8>(source_scene_, *iter_blk_source,0,true, "nbr_exist");
   nbr_prob   = opencl_cache_->get_data<BOXM2_FLOAT8>(source_scene_, *iter_blk_source,0,true, "nbr_prob");
   trans_interp_kern->set_arg(blk_info_target_.ptr());
   trans_interp_kern->set_arg(blk_info_source);
   trans_interp_kern->set_arg(blk_target_);
   trans_interp_kern->set_arg(alpha_target_);
   trans_interp_kern->set_arg(mog_target_);
   trans_interp_kern->set_arg(blk_source);
   trans_interp_kern->set_arg(alpha_source);
   trans_interp_kern->set_arg(mog_source);
   //   trans_interp_kern->set_arg(nobs_source);
   trans_interp_kern->set_arg(nbr_exint);
   trans_interp_kern->set_arg(nbr_exists);
   trans_interp_kern->set_arg(nbr_prob);
   trans_interp_kern->set_arg(translation);
   trans_interp_kern->set_arg(rotation);
   trans_interp_kern->set_arg(scalem);
   trans_interp_kern->set_arg(octree_depth_.ptr());
   trans_interp_kern->set_arg(output_.ptr());
   trans_interp_kern->set_local_arg(local_threads[0]*10*sizeof(cl_uchar) );    // cumsum buffer,
   trans_interp_kern->set_local_arg(16*local_threads[0]*sizeof(unsigned char)); // local trees target
   trans_interp_kern->set_local_arg(16*local_threads[0]*sizeof(unsigned char)); // local trees source
   first = false;
   }
   if(!trans_interp_kern->execute(queue_, 1, local_threads, global_threads))
     {
       vcl_cout<<"Kernel Failed to Execute "<<vcl_endl;
       return false;
    }
   int status = clFinish(queue_);
   bool good_kern = check_val(status, CL_SUCCESS, "TRANSFORMATION KERNEL (INTERP) FAILED: " + error_to_string(status));
   if(!good_kern)
     return false;
   mog_target_->read_to_buffer(queue_);
   alpha_target_->read_to_buffer(queue_);
   output_->read_to_buffer(queue_);
   status = clFinish(queue_);
   bool good_read = check_val(status, CL_SUCCESS, "READ FROM GPU FAILED: " + error_to_string(status));
   if(!good_read)
     return false;
#if 0
   if(finish){
     trans_interp_kern->clear_args();
     boxm2_lru_cache::instance()->write_to_disk(target_scene_);
     octree_depth_->release_memory();
     blk_info_source->release_memory();
     delete info_buffer_source;
     blk_info_target_->release_memory();
     delete info_buffer;
     opencl_cache_->unref_mem(translation);
     opencl_cache_->unref_mem(rotation);
     opencl_cache_->unref_mem(scalem);
     translation = 0; rotation = 0; scalem = 0;
     delete [] translation_buff;
     delete [] rotation_buff;
     delete [] scale_buff;
     scale_buff = 0;   rotation_buff = 0;   translation_buff = 0;
   }
#endif
#if 0
  for(int i = 0; i<1; i++)
    vcl_cout << output_buff[i] << ' ' << output_buff[i+1] << ' '
      << output_buff[i+2] << ' ' << output_buff[i+3] << ' '
      << output_buff[i+4] << ' ' << output_buff[i+5] << '\n';
#endif
#endif
   return true;
}
bool boxm2_vecf_ocl_transform_scene::
render_scene_appearance(vpgl_camera_double_sptr const& cam, vil_image_view<float>& expected_img,
                        vil_image_view<float>& vis_img)
{

  bool status = renderer_.render(cam, ni_, nj_);
  renderer_.get_last_vis(vis_img);
  renderer_.get_last_rendered(expected_img);
  
  return status;
}

bool boxm2_vecf_ocl_transform_scene::render_scene_depth(vpgl_camera_double_sptr const & cam,
                                                        vil_image_view<float>& expected_depth,
                                                        vil_image_view<float>& vis_img)
{
  // It is assumed that this function will be called multiple times
  // where the scene changes between calls and possibly the camera
  // It is also assumed that the scene is one block
  // and the data is already avaiable in the cache and required buffers
  // have been allocated by a previous function call.
  vpgl_camera_double_sptr & nccam = const_cast<vpgl_camera_double_sptr &>(cam);

  //camera check
  const vcl_string cam_type( cam->type_name() );
  if (cam_type != "vpgl_perspective_camera" &&
      cam_type != "vpgl_generic_camera" &&
      cam_type != "vpgl_affine_camera" ) {
    vcl_cout<<"Cannot render with camera of type " << cam_type << vcl_endl;
    return false;
  }

  // intialize the render image planes
  vcl_fill(depth_buff_, depth_buff_ + cl_ni_*cl_nj_, 0.0f);
  vcl_fill(vis_buff_, vis_buff_ + cl_ni_*cl_nj_, 1.0f);
  vcl_fill(var_buff_, var_buff_ + cl_ni_*cl_nj_, 0.0f);
  vcl_fill(prob_image_buff_, prob_image_buff_ + cl_ni_*cl_nj_, 0.0f);
  vcl_fill(t_infinity_buff_, t_infinity_buff_ + cl_ni_*cl_nj_, 0.0f);

  depth_image_->write_to_buffer(queue_);
  vis_image_->write_to_buffer(queue_);
  var_image_->write_to_buffer(queue_);
  prob_image_->write_to_buffer(queue_);
  t_infinity_->write_to_buffer(queue_);

  // assumes that the camera may be changing between calls
  boxm2_ocl_camera_converter::compute_ray_image( device_, queue_, nccam, cl_ni_, cl_nj_, ray_o_buff_, ray_d_buff_, 0, 0, false);
  ray_o_buff_->write_to_buffer(queue_);
  ray_d_buff_->write_to_buffer(queue_);

  int statusw = clFinish(queue_);
  bool good_write = check_val(statusw, CL_SUCCESS, "WRITE TO GPU FAILED " + error_to_string(statusw));
  if(!good_write) {
    return false;
  }

  depth_kern_->set_arg( blk_info_target_.ptr() );
  depth_kern_->set_arg( blk_target_);
  depth_kern_->set_arg( alpha_target_  );
  depth_kern_->set_arg( ray_o_buff_.ptr() );
  depth_kern_->set_arg( ray_d_buff_.ptr() );
  depth_kern_->set_arg( depth_image_.ptr() );
  depth_kern_->set_arg( var_image_.ptr() );
  depth_kern_->set_arg( exp_img_dim_.ptr() );
  depth_kern_->set_arg( output_.ptr() );
  depth_kern_->set_arg( lookup_.ptr() );
  depth_kern_->set_arg( vis_image_.ptr() );
  depth_kern_->set_arg( prob_image_.ptr() );
  depth_kern_->set_arg( t_infinity_.ptr() );

  //local tree , cumsum buffer, imindex buffer
  depth_kern_->set_local_arg( lthreads_[0]*lthreads_[1]*sizeof(cl_uchar16) );
  depth_kern_->set_local_arg( lthreads_[0]*lthreads_[1]*10*sizeof(cl_uchar) );
  depth_kern_->set_local_arg( lthreads_[0]*lthreads_[1]*sizeof(cl_int) );

  //execute kernel
  vcl_size_t gThreads[] = {cl_ni_,cl_nj_};
  depth_kern_->execute(queue_, 2, lthreads_, gThreads);
  statusw = clFinish(queue_);
  bool good_run = check_val(statusw, CL_SUCCESS, "EXECUTION OF DEPTH KERNEL FAILED " + error_to_string(statusw));
  if (!good_run) {
    return false;
  }

  depth_norm_kern_->set_arg( depth_image_.ptr() );
  depth_norm_kern_->set_arg( var_image_.ptr() );
  depth_norm_kern_->set_arg( vis_image_.ptr() );
  depth_norm_kern_->set_arg( exp_img_dim_.ptr());
  depth_norm_kern_->set_arg( t_infinity_.ptr());
  depth_norm_kern_->set_arg( subblk_dim_.ptr() );
  depth_norm_kern_->execute( queue_, 2, lthreads_, gThreads);
  statusw = clFinish(queue_);
  good_run = check_val(statusw, CL_SUCCESS, "EXECUTION OF DEPTH NORM KERNEL FAILED " + error_to_string(statusw));
  if (!good_run) {
    return false;
  }

  depth_image_->read_to_buffer(queue_);
  var_image_->read_to_buffer(queue_);
  vis_image_->read_to_buffer(queue_);
  statusw = clFinish(queue_);
  bool good_read = check_val(statusw, CL_SUCCESS, "READ OF DEPTH BUFFERS FAILED " + error_to_string(statusw));
  if (!good_read) {
    return false;
  }

  depth_kern_->clear_args();
  depth_norm_kern_->clear_args();

  expected_depth.set_size(ni_, nj_);
  expected_depth.fill(0.0f);
  for (unsigned r=0;r<nj_;r++)
    for (unsigned c=0;c<ni_;c++)
      expected_depth(c,r)=depth_buff_[r*cl_ni_+c];
  vis_img.set_size(ni_, nj_);
  vis_img.fill(0.0f);
  for (unsigned r=0;r<nj_;r++)
    for (unsigned c=0;c<ni_;c++)
      vis_img(c,r)=vis_buff_[r*cl_ni_+c];

  return true;
}
