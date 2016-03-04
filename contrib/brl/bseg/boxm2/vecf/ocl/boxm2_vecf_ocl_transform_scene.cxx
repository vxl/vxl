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
#include <boct/boct_bit_tree.h>
#include <bocl/bocl_kernel.h>
#include <vcl_where_root_dir.h>
#include <vcl_algorithm.h>
#include <vnl/vnl_vector_fixed.h>
typedef vnl_vector_fixed<unsigned char,16> uchar16;
static bool transform_scene_compiled = false;
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
            options="-D MOG_TYPE_8 ";
        }
        else if ( app_type == BOXM2_MOG3_GREY_16 )
        {
            app_type_ = BOXM2_MOG3_GREY_16;
            foundDataType = true;
            options="-D MOG_TYPE_16 ";
        }
    }
    if (!foundDataType) {
        vcl_cout<<"ERROR: boxm2_vecf_ocl_transform_scene: unsupported gray appearance type" << vcl_endl;
        return false;
    }

  color_app_type_id_ = "";
  for (unsigned int i=0; i<apps.size(); ++i) {
    if ( apps[i] == boxm2_data_traits<BOXM2_GAUSS_RGB>::prefix(color_app_id_) )
    {
      color_app_type_id_ = boxm2_data_traits<BOXM2_GAUSS_RGB>::prefix(color_app_id_);
      vcl_cout<<"found color data type "<<color_app_type_id_<< " in source scene"<<vcl_endl;
      options +="-D HAS_RGB ";
    }

  }

    //set apptype size
    apptypesize_ = (int) boxm2_data_info::datasize(app_type_);
    return true;
}


boxm2_vecf_ocl_transform_scene::boxm2_vecf_ocl_transform_scene(boxm2_scene_sptr source_scene,
                                                               boxm2_scene_sptr target_scene,
                                                               boxm2_opencl_cache_sptr ocl_cache,
                                                               vcl_string gray_app_id,
                                                               vcl_string color_app_id,
                                                               bool do_alpha,
                                                               bool do_interp)

  : source_scene_(source_scene),
    target_scene_(target_scene),
    opencl_cache_(ocl_cache),
    grey_app_id_(gray_app_id), color_app_id_(color_app_id),
    do_alpha_(do_alpha),do_interp_(do_interp)
{
  device_=opencl_cache_->get_device();
  target_initialized_= false;
  this->compile_trans_interp_kernel();
  this->compile_trans_interp_trilin_kernel();
  this->compile_trans_interp_vecf_trilin_kernel();
  this->compile_compute_cell_centers_kernel();
  this->init_ocl_trans();
  if(!this->init_target_scene_buffers(this->target_scene_))
    vcl_cout<<"target scene buffers failed to initialize"<<vcl_endl;
}

boxm2_vecf_ocl_transform_scene::boxm2_vecf_ocl_transform_scene(boxm2_scene_sptr source_scene,
                                                               boxm2_opencl_cache_sptr ocl_cache,
                                                               vcl_string gray_app_id,
                                                               vcl_string color_app_id,
                                                               bool do_alpha,
                                                               bool do_interp)

  :  source_scene_(source_scene),
     target_scene_(VXL_NULLPTR),
     opencl_cache_(ocl_cache),
     grey_app_id_(gray_app_id), color_app_id_(color_app_id),
     do_alpha_(do_alpha),do_interp_(do_interp)
{
  device_=opencl_cache_->get_device();
  target_initialized_= false;

  this->compile_trans_interp_kernel();
  this->compile_trans_interp_trilin_kernel();
  this->compile_trans_interp_vecf_trilin_kernel();
  this->compile_compute_cell_centers_kernel();
  this->init_ocl_trans();
}

boxm2_vecf_ocl_transform_scene::~boxm2_vecf_ocl_transform_scene()
{
  delete trans_interp_kern;
  //delete trans_kern;
  //opencl_cache_->clear_cache();

  // // common stuff
  opencl_cache_->unref_mem( output_.ptr() );
  opencl_cache_->unref_mem( output_f.ptr() );
  opencl_cache_->unref_mem( blk_info_target_.ptr() );
  opencl_cache_->unref_mem( octree_depth_.ptr() );
  opencl_cache_->unref_mem( lookup_.ptr() );

  // transform stuff
  opencl_cache_->unref_mem( centerX_.ptr() );
  opencl_cache_->unref_mem( centerY_.ptr() );
  opencl_cache_->unref_mem( centerZ_.ptr() );
  delete [] long_output;
}


bool boxm2_vecf_ocl_transform_scene::init_target_scene_buffers(boxm2_scene_sptr target_scene)
{
  bool good_buffers = true;
  target_scene_ = target_scene;
  vcl_string options;
  // common stuff
  vcl_vector<boxm2_block_id> blocks_target = target_scene ->get_block_ids();
  vcl_vector<boxm2_block_id> blocks_source = source_scene_->get_block_ids();
  if(blocks_target.size()!=1||blocks_source.size()!=1) {
    std::cerr << "Error: boxm2_vecf_ocl_transform_scene: number of blocks > 1" << std::endl;
    return false;
  }
  vcl_vector<boxm2_block_id>::iterator iter_blk_target = blocks_target.begin();
  vcl_vector<boxm2_block_id>::iterator iter_blk_source = blocks_source.begin();
  blk_target_       = opencl_cache_->get_block(target_scene, *iter_blk_target);
  boxm2_block* tblock = opencl_cache_->get_cpu_cache()->get_block(target_scene, *iter_blk_target);
    octree_depth_buff_ = 3;
    octree_depth_ = new bocl_mem(device_->context(), &(octree_depth_buff_), sizeof(int), "  depth of octree " );
    good_buffers &= octree_depth_->create_buffer(CL_MEM_READ_ONLY | CL_MEM_COPY_HOST_PTR );

    info_buffer_      = target_scene->get_blk_metadata(*iter_blk_target);
    info_buffer_->data_buffer_length = (int) tblock->num_cells();
    blk_info_target_  = new bocl_mem(device_->context(), info_buffer_, sizeof(boxm2_scene_info), " Scene Info Target" );
    good_buffers &= blk_info_target_->create_buffer(CL_MEM_READ_ONLY | CL_MEM_COPY_HOST_PTR);

    blk_info_target_->write_to_buffer(queue_);


  //Gather information about the target and setup target data buffers
  // trans_interp_kernel arguments set the first time the function is called

  //allow these buffers to be read every time in case the target scene changes

  alpha_target_     = opencl_cache_->get_data<BOXM2_ALPHA>(target_scene, *iter_blk_target,0, false);

  if (!good_buffers) {
    std::cerr << "Error allocating one or more opencl buffers" << std::endl;
    return false;
  }

  if(app_type_ == BOXM2_MOG3_GREY) {
    mog_target_       = opencl_cache_->get_data<BOXM2_MOG3_GREY>(target_scene, *iter_blk_target,0,false);
  }
  else if(app_type_ == BOXM2_MOG3_GREY_16) {
    mog_target_       = opencl_cache_->get_data<BOXM2_MOG3_GREY_16>(target_scene, *iter_blk_target,0,false);
  }
  else {
    vcl_cout << "Unknown appearance type for target_scene " << app_type_ << '\n';
    return false;
  }
  if( color_app_type_id_ !="")
    rgb_target_ = opencl_cache_->get_data(target_scene, *iter_blk_source,color_app_type_id_);

  target_initialized_ =true;

  return true;
}

bool boxm2_vecf_ocl_transform_scene::compile_trans_interp_kernel()
{
  vcl_string options;
  // sets apptypesize_ and app_type
  get_scene_appearance(source_scene_, options);
  vcl_cout<<" compiling trans kernel "<<vcl_endl;
  vcl_vector<vcl_string> src_paths;
  vcl_string source_dir = vcl_string(VCL_SOURCE_ROOT_DIR) + "/contrib/brl/bseg/boxm2/ocl/cl/";
  vcl_string vecf_source_dir = vcl_string(VCL_SOURCE_ROOT_DIR)+ "/contrib/brl/bseg/boxm2/vecf/ocl/cl/";
  src_paths.push_back(source_dir     + "scene_info.cl");
  src_paths.push_back(source_dir     + "bit/bit_tree_library_functions.cl");
  src_paths.push_back(vecf_source_dir + "transform_scene_with_interpolation.cl");
  this->trans_interp_kern = new bocl_kernel();
  return trans_interp_kern->create_kernel(&device_->context(),device_->device_id(), src_paths, "transform_scene_interpolate", options, "trans_interp_scene");
}

bool boxm2_vecf_ocl_transform_scene::compile_trans_interp_trilin_kernel()
{
  vcl_string options;
  // sets apptypesize_ and app_type
  get_scene_appearance(source_scene_, options);
  if (do_alpha_)
    options+=" -D DO_ALPHA ";
  if(do_interp_)
    options+=" -D DO_INTERP ";
  vcl_cout<<" compiling trans kernel with options "<<options<< vcl_endl;
  vcl_vector<vcl_string> src_paths;
  vcl_string source_dir = vcl_string(VCL_SOURCE_ROOT_DIR) + "/contrib/brl/bseg/boxm2/ocl/cl/";
  vcl_string vecf_source_dir = vcl_string(VCL_SOURCE_ROOT_DIR)+ "/contrib/brl/bseg/boxm2/vecf/ocl/cl/";
  src_paths.push_back(source_dir     + "scene_info.cl");
  src_paths.push_back(source_dir     + "bit/bit_tree_library_functions.cl");
  src_paths.push_back(source_dir     + "boxm2_ocl_helpers.cl");
  src_paths.push_back(vecf_source_dir + "interp_helpers.cl");
  src_paths.push_back(vecf_source_dir + "warp_and_resample_trilinear.cl");

  vcl_cout<<"compiling trans_interp_similarity_trilin kernel with "<<options<<vcl_endl;
  this->trans_interp_trilin_kern = new bocl_kernel();
  return trans_interp_trilin_kern->create_kernel(&device_->context(),device_->device_id(), src_paths, "warp_and_resample_trilinear_similarity", options, "trans_interp_similarity_trilin_scene");
}

bool boxm2_vecf_ocl_transform_scene::compile_compute_cell_centers_kernel()
{
  vcl_string options;
  vcl_cout<<" compiling compute_cell_centers kernel "<<vcl_endl;
  vcl_vector<vcl_string> src_paths;
  vcl_string source_dir = vcl_string(VCL_SOURCE_ROOT_DIR) + "/contrib/brl/bseg/boxm2/ocl/cl/";
  vcl_string vecf_source_dir = vcl_string(VCL_SOURCE_ROOT_DIR)+ "/contrib/brl/bseg/boxm2/vecf/ocl/cl/";
  src_paths.push_back(source_dir     + "scene_info.cl");
  src_paths.push_back(source_dir     + "bit/bit_tree_library_functions.cl");
  src_paths.push_back(source_dir     + "boxm2_ocl_helpers.cl");
  src_paths.push_back(vecf_source_dir + "compute_cell_centers.cl");
  this->compute_cell_centers_kern = new bocl_kernel();
  return compute_cell_centers_kern->create_kernel(&device_->context(),device_->device_id(), src_paths, "compute_cell_centers", options, "compute_cell_centers");
}


bool boxm2_vecf_ocl_transform_scene::compile_trans_interp_vecf_trilin_kernel()
{
  vcl_string options;
  // sets apptypesize_ and app_type
  get_scene_appearance(source_scene_, options);
  vcl_cout<<" compiling trans kernel "<<vcl_endl;
  vcl_vector<vcl_string> src_paths;
  vcl_string source_dir = vcl_string(VCL_SOURCE_ROOT_DIR) + "/contrib/brl/bseg/boxm2/ocl/cl/";
  vcl_string vecf_source_dir = vcl_string(VCL_SOURCE_ROOT_DIR)+ "/contrib/brl/bseg/boxm2/vecf/ocl/cl/";
  src_paths.push_back(source_dir     + "scene_info.cl");
  src_paths.push_back(source_dir     + "bit/bit_tree_library_functions.cl");
  src_paths.push_back(source_dir     + "boxm2_ocl_helpers.cl");
  src_paths.push_back(vecf_source_dir + "interp_helpers.cl");
  src_paths.push_back(vecf_source_dir + "warp_and_resample_trilinear.cl");
  this->trans_interp_vecf_trilin_kern = new bocl_kernel();
  return trans_interp_vecf_trilin_kern->create_kernel(&device_->context(),device_->device_id(), src_paths, "warp_and_resample_trilinear_vecf", options, "trans_interp_vecf_trilin_scene");
}


bool boxm2_vecf_ocl_transform_scene::init_ocl_trans()
{
    centerX_ = new bocl_mem(device_->context(), boct_bit_tree::centerX, sizeof(cl_float)*585, "centersX lookup buffer");
    centerY_ = new bocl_mem(device_->context(), boct_bit_tree::centerY, sizeof(cl_float)*585, "centersY lookup buffer");
    centerZ_ = new bocl_mem(device_->context(), boct_bit_tree::centerZ, sizeof(cl_float)*585, "centersZ lookup buffer");
    centerX_->create_buffer(CL_MEM_READ_ONLY | CL_MEM_COPY_HOST_PTR);
    centerY_->create_buffer(CL_MEM_READ_ONLY | CL_MEM_COPY_HOST_PTR);
    centerZ_->create_buffer(CL_MEM_READ_ONLY | CL_MEM_COPY_HOST_PTR);

    // output buffer for debugging

     // bit lookup buffer
    boxm2_ocl_util::set_bit_lookup(lookup_arr_);
    lookup_=new bocl_mem(device_->context(), lookup_arr_, sizeof(cl_uchar)*256, "bit lookup buffer");
    lookup_->create_buffer(CL_MEM_READ_ONLY | CL_MEM_COPY_HOST_PTR);

    output_ = new bocl_mem(device_->context(),output_buff_, sizeof(float)*1000, "output" );
    output_->create_buffer(CL_MEM_READ_WRITE | CL_MEM_COPY_HOST_PTR );

    int status = 0;
    queue_ = clCreateCommandQueue(device_->context(),*(device_->device_id()),CL_QUEUE_PROFILING_ENABLE,&status);

    octree_depth_ = VXL_NULLPTR;
    blk_info_target_ = VXL_NULLPTR;
    blk_info_source = VXL_NULLPTR;
    info_buffer_ = VXL_NULLPTR;
    blk_target_ = VXL_NULLPTR;
    alpha_target_ = VXL_NULLPTR;
    mog_target_ = VXL_NULLPTR;
    info_buffer_source = VXL_NULLPTR;
    blk_source = VXL_NULLPTR;
    alpha_source = VXL_NULLPTR;
    mog_source = VXL_NULLPTR;
    nobs_target_ = VXL_NULLPTR;
    rgb_target_ = VXL_NULLPTR;
    long_output=VXL_NULLPTR;
    return true;
}
// the transform is the inverse, i.e. a map from the target to the source
// an arbitrary number of blocks are handled, no interpolation
bool boxm2_vecf_ocl_transform_scene::transform(vgl_rotation_3d<double> rot,
            vgl_vector_3d<double> trans,
            vgl_vector_3d<double> scale)

{
  return true;
}
bool boxm2_vecf_ocl_transform_scene::transform_1_blk(vgl_rotation_3d<double>  rot,
           vgl_vector_3d<double> trans,
           vgl_vector_3d<double> scale,
           bool finish){
   return true;
}
bool boxm2_vecf_ocl_transform_scene::transform_1_blk_interp(vgl_rotation_3d<double>  rot,
                                                            vgl_vector_3d<double> trans,
                                                            vgl_vector_3d<double> scale,
                                                            bool finish){
  if(!target_scene_){
    vcl_cout<< "Error: target scene is not set!"<<vcl_endl;
    return false;
  }
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
   translation_buff[0] = (double) trans.x();
   translation_buff[1] = (double) trans.y();
   translation_buff[2] = (double) trans.z();
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
   // check for invalid parameters
   if( alphaTypeSize == 0 ) //This should never happen, it will result in division by zero later
   {
     vcl_cout << "ERROR: alphaTypeSize == 0 in " << __FILE__ << __LINE__ << vcl_endl;
     return false;
   }

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
   vcl_cout<<"This is the transform scene Kernel"<<vcl_endl;
   //   nobs_source = opencl_cache_->get_data<BOXM2_NUM_OBS>(source_scene_, *iter_blk_source,0,false);
   nbr_exint = opencl_cache_->get_data<BOXM2_CHAR8>(source_scene_, *iter_blk_source,0,true, "nbr_exint");
   nbr_exists = opencl_cache_->get_data<BOXM2_CHAR8>(source_scene_, *iter_blk_source,0,true, "nbr_exist");
   nbr_prob   = opencl_cache_->get_data<BOXM2_FLOAT8>(source_scene_, *iter_blk_source, 32 * info_buffer_source->data_buffer_length,true, "nbr_prob");
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
   output_f->read_to_buffer(queue_);
   status = clFinish(queue_);
   bool good_read = check_val(status, CL_SUCCESS, "READ FROM GPU FAILED: " + error_to_string(status));
   if(!good_read)
     return false;
#endif
   return true;
}

bool boxm2_vecf_ocl_transform_scene::
transform_1_blk_interp_trilin(boxm2_vecf_ocl_vector_field &vec_field,
                              bool finish)
{
  if(!target_scene_){
    vcl_cout<< "Error: target scene is not set!"<<vcl_endl;
    return false;
  }
  vcl_size_t local_threads[1]={64};

  static vcl_size_t global_threads[1]={1};
  // set up all the kernel arguments
  // subsequent calls don't need to do this initialization

  vcl_vector<boxm2_block_id> blocks_target = target_scene_->get_block_ids();
  vcl_vector<boxm2_block_id> blocks_source = source_scene_->get_block_ids();
  if(blocks_target.size()!=1||blocks_source.size()!=1) {
    throw vcl_runtime_error("transform_1_block_interl_trilin() only implemented for single block");
  }
  vcl_vector<boxm2_block_id>::iterator iter_blk_target = blocks_target.begin();
  vcl_vector<boxm2_block_id>::iterator iter_blk_source = blocks_source.begin();

  bocl_mem* target_pts = opencl_cache_->get_data<BOXM2_POINT>(target_scene_, *iter_blk_target, 0, false, "target");

  // Fill in target_pts with 3-d cell center locations
  compute_cell_centers_kern->set_arg( centerX_.ptr() );
  compute_cell_centers_kern->set_arg( centerY_.ptr() );
  compute_cell_centers_kern->set_arg( centerZ_.ptr() );
  compute_cell_centers_kern->set_arg(lookup_.ptr());
  compute_cell_centers_kern->set_arg(blk_info_target_.ptr());
  compute_cell_centers_kern->set_arg(blk_target_);
  compute_cell_centers_kern->set_arg(target_pts);
  compute_cell_centers_kern->set_arg(octree_depth_.ptr());
  compute_cell_centers_kern->set_local_arg(local_threads[0]*10*sizeof(cl_uchar) );    // cumsum buffer,
  compute_cell_centers_kern->set_local_arg(16*local_threads[0]*sizeof(unsigned char)); // local trees target

  global_threads[0] = (unsigned) RoundUp(info_buffer_->scene_dims[0]*info_buffer_->scene_dims[1]*info_buffer_->scene_dims[2],(int)local_threads[0]);

  if(!compute_cell_centers_kern->execute(queue_, 1, local_threads, global_threads))
  {
    vcl_cout<<"Compute Cell Centers Kernel Failed to Execute "<<vcl_endl;
    return false;
  }
  int status = clFinish(queue_);
  bool good_kern = check_val(status, CL_SUCCESS, "COMPUTE CELL CENTERS KERNEL FAILED: " + error_to_string(status));
  if(!good_kern)
    return false;

  compute_cell_centers_kern->clear_args();

  bocl_mem* source_pts = opencl_cache_->get_data<BOXM2_POINT>(target_scene_, *iter_blk_target, 0, false, "source");
  if(!vec_field.compute_inverse_transform(target_scene_, *iter_blk_target, target_pts, source_pts, queue_)) {
    vcl_cout << "ERROR: boxm2_vecf_ocl_transform_scene::transform_1_blk_interp_trilin(): Error computing inverse transform!" << vcl_endl;
    return false;
  }

  //Gather information about the source and setup source data buffers
  info_buffer_source = source_scene_->get_blk_metadata(*iter_blk_source);

  // get more information about the source block since it will actually be used.
  blk_source       = opencl_cache_->get_block(source_scene_, *iter_blk_source);
  alpha_source     = opencl_cache_->get_data<BOXM2_ALPHA>(source_scene_, *iter_blk_source,0,true);
  int alphaTypeSize = (int)boxm2_data_info::datasize(boxm2_data_traits<BOXM2_ALPHA>::prefix());
  // check for invalid parameters
  if( alphaTypeSize == 0 ) //This should never happen, it will result in division by zero later
  {
    vcl_cout << "ERROR: alphaTypeSize == 0 in " << __FILE__ << __LINE__ << vcl_endl;
    return false;
  }

  info_buffer_source->data_buffer_length = (int) (alpha_source->num_bytes()/alphaTypeSize);
  data_size = info_buffer_source->data_buffer_length;

  bocl_mem_sptr blk_info_source_loc = opencl_cache_->alloc_mem(sizeof(boxm2_scene_info), info_buffer_source, "Scene Info");
  blk_info_source_loc->create_buffer(CL_MEM_READ_ONLY | CL_MEM_COPY_HOST_PTR);

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

  vcl_cout<<"This is the transform scene Kernel with arbitrary vector field and trilinear interp"<<vcl_endl;
  //   nobs_source = opencl_cache_->get_data<BOXM2_NUM_OBS>(source_scene_, *iter_blk_source,0,false);

  trans_interp_vecf_trilin_kern->set_arg(lookup_.ptr());
  trans_interp_vecf_trilin_kern->set_arg(blk_info_target_.ptr());
  trans_interp_vecf_trilin_kern->set_arg(blk_info_source_loc.ptr());
  trans_interp_vecf_trilin_kern->set_arg(blk_target_);
  trans_interp_vecf_trilin_kern->set_arg(alpha_target_);
  trans_interp_vecf_trilin_kern->set_arg(mog_target_);
  trans_interp_vecf_trilin_kern->set_arg(blk_source);
  trans_interp_vecf_trilin_kern->set_arg(alpha_source);
  trans_interp_vecf_trilin_kern->set_arg(mog_source);
  trans_interp_vecf_trilin_kern->set_arg(source_pts);
  trans_interp_vecf_trilin_kern->set_arg(octree_depth_.ptr());
  trans_interp_vecf_trilin_kern->set_local_arg(local_threads[0]*10*sizeof(cl_uchar) );    // cumsum buffer,
  trans_interp_vecf_trilin_kern->set_local_arg(16*local_threads[0]*sizeof(unsigned char)); // local trees target
  trans_interp_vecf_trilin_kern->set_local_arg(16*local_threads[0]*sizeof(unsigned char)); // local trees source
  trans_interp_vecf_trilin_kern->set_local_arg(16*local_threads[0]*sizeof(unsigned char)); // neighbor trees

  if(!trans_interp_vecf_trilin_kern->execute(queue_, 1, local_threads, global_threads))
  {
    vcl_cout<<"Kernel Failed to Execute "<<vcl_endl;
    return false;
  }
  status = clFinish(queue_);
  good_kern = check_val(status, CL_SUCCESS, "TRANSFORMATION KERNEL (INTERP) FAILED: " + error_to_string(status));
  if(!good_kern)
    return false;
  mog_target_->read_to_buffer(queue_);
  alpha_target_->read_to_buffer(queue_);

  status = clFinish(queue_);

  trans_interp_vecf_trilin_kern->clear_args();

  opencl_cache_->unref_mem(blk_info_source_loc.ptr());

  bool good_read = check_val(status, CL_SUCCESS, "READ FROM GPU FAILED: " + error_to_string(status));
  vcl_cout<<"good read is "<<good_read<<vcl_endl;
  if(!good_read)
    return false;
  return true;
}

bool boxm2_vecf_ocl_transform_scene::transform_1_blk_interp_trilin(boxm2_scene_sptr target_scene,
                                                                   vgl_rotation_3d<double>  rot,
                                                                   vgl_vector_3d<double> trans,
                                                                   vgl_vector_3d<double> scale,
                                                                   bool finish)
{
  //  if(!target_initialized_)
    if(!  this->init_target_scene_buffers(target_scene)){
      vcl_cout<<"failed to initialize target scene buffers"<<vcl_endl;
      return false;
    }
  // set up the buffers the first time the function is called
  // subsequent calls don't need to recreate the buffers
    static bool first;
    float translation_buff[4];
    float rotation_buff   [9];
    float scale_buff      [4];

   translation_buff[0] = (float) trans.x();
   translation_buff[1] = (float) trans.y();
   translation_buff[2] = (float) trans.z();
   translation_buff[3] = 0.0f;

   vnl_matrix_fixed<double, 3, 3> R = rot.as_matrix();
   rotation_buff[0] =(float) R(0,0);  rotation_buff[3] =(float) R(1,0);  rotation_buff[6] =(float) R(2,0);
   rotation_buff[1] =(float) R(0,1);  rotation_buff[4] =(float) R(1,1);  rotation_buff[7] =(float) R(2,1);
   rotation_buff[2] =(float) R(0,2);  rotation_buff[5] =(float) R(1,2);  rotation_buff[8] =(float) R(2,2);

   scale_buff[0] =(float) scale.x();
   scale_buff[1] =(float) scale.y();
   scale_buff[2] =(float) scale.z();
   scale_buff[3] = 0.0;
   int statusw =0;
   // just copy the transformation parameters to gpu memory

   vcl_size_t local_threads[1]={64};
   static vcl_size_t global_threads[1]={1};
  // set up all the kernel arguments
  // subsequent calls don't need to do this initialization
   bocl_mem_sptr translation_l = new bocl_mem(device_->context(), translation_buff, sizeof(float)*4, " translation " );
   translation_l->create_buffer(CL_MEM_READ_ONLY | CL_MEM_COPY_HOST_PTR );

   bocl_mem_sptr  rotation_l = new bocl_mem(device_->context(), rotation_buff, sizeof(float)*9, " rotation " );
   rotation_l->create_buffer(CL_MEM_READ_ONLY | CL_MEM_COPY_HOST_PTR );

   bocl_mem_sptr scalem_l = new bocl_mem(device_->context(), scale_buff, sizeof(float)*4, " scale " );
   scalem_l->create_buffer(CL_MEM_READ_ONLY | CL_MEM_COPY_HOST_PTR );

   statusw = clFinish(queue_);
   bool good_write = check_val(statusw, CL_SUCCESS, "WRITE TO GPU FAILED " + error_to_string(statusw));
     if(!good_write)
       return false;

     vcl_vector<boxm2_block_id> blocks_target = target_scene_->get_block_ids();
     vcl_vector<boxm2_block_id> blocks_source = source_scene_->get_block_ids();
     vcl_vector<boxm2_block_id>::iterator iter_blk_target = blocks_target.begin();
     vcl_vector<boxm2_block_id>::iterator iter_blk_source = blocks_source.begin();

     if(blocks_target.size()!=1||blocks_source.size()!=1)
       return false;

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
   alpha_source     = opencl_cache_->get_data<BOXM2_ALPHA>  (source_scene_, *iter_blk_source,0,true);

   int alphaTypeSize = (int) boxm2_data_info::datasize(boxm2_data_traits<BOXM2_ALPHA>::prefix());
   // check for invalid parameters
   if( alphaTypeSize == 0 ) //This should never happen, it will result in division by zero later
   {
    vcl_cout << "ERROR: alphaTypeSize == 0 in " << __FILE__ << __LINE__ << vcl_endl;
    return false;
   }

   info_buffer_source->data_buffer_length = (int) (alpha_source->num_bytes()/alphaTypeSize);
   vcl_size_t data_size_target = static_cast<vcl_size_t>(info_buffer_->data_buffer_length);

   bocl_mem_sptr blk_info_source_l  = new bocl_mem(device_->context(), info_buffer_source, sizeof(boxm2_scene_info), " Scene Info Source" );
   blk_info_source_l->create_buffer(CL_MEM_READ_ONLY | CL_MEM_COPY_HOST_PTR);
   blk_info_source_l->write_to_buffer(queue_);

   if(app_type_ == BOXM2_MOG3_GREY) {
     mog_source       = opencl_cache_->get_data<BOXM2_MOG3_GREY>(source_scene_, *iter_blk_source,0,true);
   }
   else if(app_type_ == BOXM2_MOG3_GREY_16) {
     mog_source       = opencl_cache_->get_data<BOXM2_MOG3_GREY_16>(source_scene_, *iter_blk_source,0,true);
   }
   else {
     vcl_cout << "ERROR: boxm2_vecf_ocl_transform_scene: Unsupported appearance type for source_scene " << boxm2_data_info::prefix(app_type_) << '\n';
     return false;
   }
   bocl_mem* rgb_source = VXL_NULLPTR;
   if(color_app_type_id_ !=""){
     rgb_source = opencl_cache_->get_data(source_scene_, *iter_blk_source,color_app_type_id_,0,true);
   }

   trans_interp_trilin_kern->set_arg(centerX_.ptr());
   trans_interp_trilin_kern->set_arg(centerY_.ptr());
   trans_interp_trilin_kern->set_arg(centerZ_.ptr());
   trans_interp_trilin_kern->set_arg(lookup_.ptr());
   trans_interp_trilin_kern->set_arg(blk_info_target_.ptr());
   trans_interp_trilin_kern->set_arg(blk_info_source_l.ptr());
   trans_interp_trilin_kern->set_arg(blk_target_);
   trans_interp_trilin_kern->set_arg(alpha_target_);
   trans_interp_trilin_kern->set_arg(mog_target_);
   if(rgb_target_ && rgb_source){
     trans_interp_trilin_kern->set_arg(rgb_target_);
     trans_interp_trilin_kern->set_arg(rgb_source);
   }
   trans_interp_trilin_kern->set_arg(blk_source);
   trans_interp_trilin_kern->set_arg(alpha_source);
   trans_interp_trilin_kern->set_arg(mog_source);
   trans_interp_trilin_kern->set_arg(translation_l.ptr());
   trans_interp_trilin_kern->set_arg(rotation_l.ptr());
   trans_interp_trilin_kern->set_arg(scalem_l.ptr());
   trans_interp_trilin_kern->set_arg(octree_depth_.ptr());
   trans_interp_trilin_kern->set_arg(output_.ptr());
   trans_interp_trilin_kern->set_local_arg(local_threads[0]*10*sizeof(cl_uchar) );    // cumsum buffer,
   trans_interp_trilin_kern->set_local_arg(16*local_threads[0]*sizeof(unsigned char)); // local trees target
   trans_interp_trilin_kern->set_local_arg(16*local_threads[0]*sizeof(unsigned char)); // local trees source
   trans_interp_trilin_kern->set_local_arg(16*local_threads[0]*sizeof(unsigned char)); // neighbor trees

   if(!trans_interp_trilin_kern->execute(queue_, 1, local_threads, global_threads))
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
   if(rgb_target_ && rgb_source){
     rgb_target_->read_to_buffer(queue_);
   }
   //   output_->read_to_buffer(queue_);
   status = clFinish(queue_);
   trans_interp_trilin_kern->clear_args();
   // float* cpu_buff = (float*) output_f->cpu_buffer();
   // int out_count = 0;
   // for(int i = 0; i<data_size * 8; i += 8)
   //   if (out_count < 100 && cpu_buff[i+0] != 0){
   //     vcl_cout << cpu_buff[i] << ' ' << cpu_buff[i+1] << ' '
   //              << cpu_buff[i+2] << ' ' << cpu_buff[i+3] << ' '
   //              << cpu_buff[i+4] << ' ' << cpu_buff[i+5] << ' '
   //              << cpu_buff[i+6] << ' ' << cpu_buff[i+7] << '\n';
   //     out_count++;
   //   }


   bool good_read = check_val(status, CL_SUCCESS, "READ FROM GPU FAILED: " + error_to_string(status));
   vcl_cout<<"good read is "<<good_read<<vcl_endl;
   if(!good_read)
     return false;
   return true;
}
