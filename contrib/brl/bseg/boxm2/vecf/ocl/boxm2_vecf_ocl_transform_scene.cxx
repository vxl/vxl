
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
#include <vnl/vnl_vector_fixed.h>
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
              boxm2_opencl_cache_sptr ocl_cache)
  : source_scene_(source_scene),
     target_scene_(target_scene),
     opencl_cache_(ocl_cache),
     renderer_(target_scene, ocl_cache),
     depth_renderer_(target_scene, ocl_cache)
{
  device_=opencl_cache_->get_device();
  this->compile_trans_interp_kernel();
  this->compile_trans_interp_trilin_kernel();
  this->init_ocl_trans();


  if (!init_render_args()) {
    throw vcl_runtime_error("init_render_args returned false");
  }
}

boxm2_vecf_ocl_transform_scene::~boxm2_vecf_ocl_transform_scene()
{
  delete trans_interp_kern;
  //delete trans_kern;
  //opencl_cache_->clear_cache();

  // common stuff
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


bool boxm2_vecf_ocl_transform_scene::init_render_args()
{

  bool good_buffers = true;

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

  octree_depth_buff_ = 3;
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
  vcl_cout<<" compiling trans kernel "<<vcl_endl;
  vcl_vector<vcl_string> src_paths;
  vcl_string source_dir = vcl_string(VCL_SOURCE_ROOT_DIR) + "/contrib/brl/bseg/boxm2/ocl/cl/";
  vcl_string vecf_source_dir = vcl_string(VCL_SOURCE_ROOT_DIR)+ "/contrib/brl/bseg/boxm2/vecf/ocl/cl/";
  src_paths.push_back(source_dir     + "scene_info.cl");
  src_paths.push_back(source_dir     + "bit/bit_tree_library_functions.cl");
  src_paths.push_back(source_dir     + "boxm2_ocl_helpers.cl");
  src_paths.push_back(vecf_source_dir + "interp_helpers.cl");
  src_paths.push_back(vecf_source_dir + "warp_and_resample_trilinear.cl");
  this->trans_interp_trilin_kern = new bocl_kernel();
  return trans_interp_trilin_kern->create_kernel(&device_->context(),device_->device_id(), src_paths, "warp_and_resample_trilinear", options, "trans_interp_trilin_scene");
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


     // bit lookup buffer
    boxm2_ocl_util::set_bit_lookup(lookup_arr_);
    lookup_=new bocl_mem(device_->context(), lookup_arr_, sizeof(cl_uchar)*256, "bit lookup buffer");
    lookup_->create_buffer(CL_MEM_READ_ONLY | CL_MEM_COPY_HOST_PTR);

    bocl_mem_sptr output_ = new bocl_mem(device_->context(),output_buff_, sizeof(float)*1000, "output" );
    output_->create_buffer(CL_MEM_READ_WRITE | CL_MEM_COPY_HOST_PTR );

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
    long_output=0;
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
   vcl_cout<<"This is the transform scene Kernel"<<vcl_endl;
   //   nobs_source = opencl_cache_->get_data<BOXM2_NUM_OBS>(source_scene_, *iter_blk_source,0,false);
   nbr_exint = opencl_cache_->get_data<BOXM2_CHAR8>(source_scene_, *iter_blk_source,0,true, "nbr_exint");
   nbr_exists = opencl_cache_->get_data<BOXM2_CHAR8>(source_scene_, *iter_blk_source,0,true, "nbr_exist");
   nbr_prob   = opencl_cache_->get_data<BOXM2_FLOAT8>(source_scene_, *iter_blk_source, 32 * info_buffer_source->data_buffer_length,true, "nbr_prob");
   int sz =nbr_prob->num_bytes()/info_buffer_source->data_buffer_length;
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
bool boxm2_vecf_ocl_transform_scene::transform_1_blk_interp_trilin(vgl_rotation_3d<double>  rot,
                                                            vgl_vector_3d<double> trans,
                                                            vgl_vector_3d<double> scale,
                                                            bool finish){
 static bool first = true;
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

     trans_interp_trilin_kern->set_arg(centerX_.ptr());
     trans_interp_trilin_kern->set_arg(centerY_.ptr());
     trans_interp_trilin_kern->set_arg(centerZ_.ptr());
     trans_interp_trilin_kern->set_arg(lookup_.ptr());

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
   info_buffer_source->data_buffer_length = (int) (alpha_source->num_bytes()/alphaTypeSize);
   data_size = info_buffer_source->data_buffer_length;
   long_output = new float[8*data_size];
   output_f = new bocl_mem(device_->context(), long_output, 8 * sizeof(float) * data_size, "output" );
   output_f->create_buffer(CL_MEM_READ_WRITE | CL_MEM_COPY_HOST_PTR );

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
   vcl_cout<<"This is the transform scene Kernel with trilinear interp"<<vcl_endl;
   //   nobs_source = opencl_cache_->get_data<BOXM2_NUM_OBS>(source_scene_, *iter_blk_source,0,false);
   trans_interp_trilin_kern->set_arg(blk_info_target_.ptr());
   trans_interp_trilin_kern->set_arg(blk_info_source);
   trans_interp_trilin_kern->set_arg(blk_target_);
   trans_interp_trilin_kern->set_arg(alpha_target_);
   trans_interp_trilin_kern->set_arg(mog_target_);
   trans_interp_trilin_kern->set_arg(blk_source);
   trans_interp_trilin_kern->set_arg(alpha_source);
   trans_interp_trilin_kern->set_arg(mog_source);
   trans_interp_trilin_kern->set_arg(translation);
   trans_interp_trilin_kern->set_arg(rotation);
   trans_interp_trilin_kern->set_arg(scalem);
   trans_interp_trilin_kern->set_arg(octree_depth_.ptr());
   trans_interp_trilin_kern->set_arg(output_f.ptr());
   trans_interp_trilin_kern->set_local_arg(local_threads[0]*10*sizeof(cl_uchar) );    // cumsum buffer,
   trans_interp_trilin_kern->set_local_arg(16*local_threads[0]*sizeof(unsigned char)); // local trees target
   trans_interp_trilin_kern->set_local_arg(16*local_threads[0]*sizeof(unsigned char)); // local trees source
   trans_interp_trilin_kern->set_local_arg(16*local_threads[0]*sizeof(unsigned char)); // neighbor trees
   first = false;
   }

   output_f->zero_gpu_buffer(queue_);
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

   output_f->read_to_buffer(queue_);
   status = clFinish(queue_);

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

bool boxm2_vecf_ocl_transform_scene::
render_scene_appearance(vpgl_camera_double_sptr const& cam,
                        vil_image_view<float>& expected_img, vil_image_view<float>& vis_img,
                        unsigned ni, unsigned nj)
{

  bool status = renderer_.render(cam, ni, nj);
  renderer_.get_last_vis(vis_img);
  renderer_.get_last_rendered(expected_img);

  return status;
}

bool boxm2_vecf_ocl_transform_scene::render_scene_depth(vpgl_camera_double_sptr const & cam,
                                                        vil_image_view<float>& expected_depth,
                                                        vil_image_view<float>& vis_img,
                                                        unsigned ni, unsigned nj)
{
  bool status = depth_renderer_.render(cam, ni, nj);
  depth_renderer_.get_last_vis(vis_img);
  depth_renderer_.get_last_rendered(expected_depth);
  return status;
}
