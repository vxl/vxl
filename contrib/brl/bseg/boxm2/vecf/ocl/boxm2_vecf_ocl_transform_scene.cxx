#include "boxm2_vecf_ocl_transform_scene.h"
//:
// \file
#include <bocl/bocl_cl.h>
#include <vgl/vgl_box_3d.h>
#include <vgl/vgl_vector_3d.h>
#include <vgl/algo/vgl_rotation_3d.h>
#include <vgl/vgl_intersection.h>
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

typedef vnl_vector_fixed<unsigned char,16> uchar16;
bool boxm2_vecf_ocl_transform_scene::get_scene_appearance( boxm2_scene_sptr scene,
							   vcl_string&      options)
{
    vcl_vector<vcl_string> apps = scene->appearances();
    bool foundDataType = false;
    for (unsigned int i=0; i<apps.size(); ++i) {
        if ( apps[i] == boxm2_data_traits<BOXM2_MOG3_GREY>::prefix() )
        {
            app_type_ = apps[i];
            foundDataType = true;
            options=" -D MOG_TYPE_8 ";
        }
        else if ( apps[i] == boxm2_data_traits<BOXM2_MOG3_GREY_16>::prefix() )
        {
            app_type_ = apps[i];
            foundDataType = true;
            options=" -D MOG_TYPE_16 ";
        }
    }
    if (!foundDataType) {
        vcl_cout<<"boxm2_ocl_change_detection_process ERROR: scene doesn't have BOXM2_MOG3_GREY or BOXM2_MOG3_GREY_16 data type"<<vcl_endl;
        return false;
    }
    //set apptype size
    apptypesize_ = (int) boxm2_data_info::datasize(app_type_);
    return true;
}


boxm2_vecf_ocl_transform_scene::boxm2_vecf_ocl_transform_scene(boxm2_scene_sptr& source_scene,
							       boxm2_scene_sptr& target_scene,
							       boxm2_opencl_cache2_sptr ocl_cache)
  : source_scene_(source_scene),
     target_scene_(target_scene),
     opencl_cache_(ocl_cache)
{
  device_=opencl_cache_->get_device();
  this->compile_trans_kernel();
  this->compile_rend_kernel();
  this->compile_norm_kernel();
  this->init_ocl_trans();
}

boxm2_vecf_ocl_transform_scene::~boxm2_vecf_ocl_transform_scene()
{
    delete trans_kern;
	delete rend_kern;
	delete norm_kern;
    opencl_cache_->clear_cache();
}

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
bool boxm2_vecf_ocl_transform_scene::compile_rend_kernel()
{
  vcl_string options;
  // sets apptypesize_ and app_type
  get_scene_appearance(source_scene_, options);
  vcl_vector<vcl_string> src_paths;
  vcl_string source_dir = vcl_string(VCL_SOURCE_ROOT_DIR) + "/contrib/brl/bseg/boxm2/ocl/cl/";
  src_paths.push_back(source_dir + "scene_info.cl");
  src_paths.push_back(source_dir + "pixel_conversion.cl");
  src_paths.push_back(source_dir + "bit/bit_tree_library_functions.cl");
  src_paths.push_back(source_dir + "backproject.cl");
  src_paths.push_back(source_dir + "statistics_library_functions.cl");
  src_paths.push_back(source_dir + "ray_bundle_library_opt.cl");
  src_paths.push_back(source_dir + "bit/render_bit_scene.cl");
  src_paths.push_back(source_dir + "expected_functor.cl");
  src_paths.push_back(source_dir + "bit/cast_ray_bit.cl");
  options += "-D RENDER ";
  options += "-D STEP_CELL=step_cell_render(aux_args.mog,aux_args.alpha,data_ptr,d*linfo->block_len,vis,aux_args.expint)";
  
  this->rend_kern = new bocl_kernel();
  bool good = rend_kern->create_kernel( &device_->context(),
					  device_->device_id(),
					  src_paths,
					  "render_bit_scene",   //kernel name
					  options,              //options
					  "boxm2 opencl render_bit_scene"); //kernel identifier (for error checking)  
  return good;
}
bool boxm2_vecf_ocl_transform_scene::compile_norm_kernel()
{
  vcl_string options;
  // sets apptypesize_ and app_type
  get_scene_appearance(source_scene_, options);
  options += "-D RENDER ";
  vcl_vector<vcl_string> src_paths;
  vcl_string source_dir = vcl_string(VCL_SOURCE_ROOT_DIR) + "/contrib/brl/bseg/boxm2/ocl/cl/";
  src_paths.push_back(source_dir + "pixel_conversion.cl");
  src_paths.push_back(source_dir + "bit/normalize_kernels.cl");
  norm_kern = new bocl_kernel();
  bool good = norm_kern->create_kernel( &device_->context(),
					device_->device_id(),
					src_paths,
					"normalize_render_kernel",   //kernel name
					options,              //options
					"boxm2 opencl norm image"); //kernel identifier (for error checking)
  return good;
}
bool boxm2_vecf_ocl_transform_scene::init_ocl_trans()
{
    centerX = new bocl_mem(device_->context(), boct_bit_tree::centerX, sizeof(cl_float)*585, "centersX lookup buffer");
    centerY = new bocl_mem(device_->context(), boct_bit_tree::centerY, sizeof(cl_float)*585, "centersY lookup buffer");
    centerZ = new bocl_mem(device_->context(), boct_bit_tree::centerZ, sizeof(cl_float)*585, "centersZ lookup buffer");
    centerX->create_buffer(CL_MEM_READ_ONLY | CL_MEM_COPY_HOST_PTR);
    centerY->create_buffer(CL_MEM_READ_ONLY | CL_MEM_COPY_HOST_PTR);
    centerZ->create_buffer(CL_MEM_READ_ONLY | CL_MEM_COPY_HOST_PTR);
    // output buffer for debugging
    output = new bocl_mem(device_->context(), output_buff, sizeof(float)*1000, "output" );
    output->create_buffer(CL_MEM_READ_WRITE | CL_MEM_COPY_HOST_PTR );
    // bit lookup buffer
    cl_uchar lookup_arr[256];
    boxm2_ocl_util::set_bit_lookup(lookup_arr);
    lookup=new bocl_mem(device_->context(), lookup_arr, sizeof(cl_uchar)*256, "bit lookup buffer");
    lookup->create_buffer(CL_MEM_READ_ONLY | CL_MEM_COPY_HOST_PTR);
    int status = 0;
    queue = clCreateCommandQueue(device_->context(),*(device_->device_id()),CL_QUEUE_PROFILING_ENABLE,&status);
   ocl_depth = 0;
    blk_info_target = 0;
    blk_info_source = 0;
    info_buffer = 0;
    blk_target = 0;
    alpha_target = 0;
    mog_target = 0;
    info_buffer_source = 0;
    blk_source = 0;
    alpha_source = 0;
    mog_source = 0;
    return true;
}
// the transform is the inverse, i.e. a map from the target to the source
bool boxm2_vecf_ocl_transform_scene::transform(vgl_rotation_3d<double> rot,
					       vgl_vector_3d<double> trans,
					       vgl_vector_3d<double> scale)

{
   int depth = 0;
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
       bocl_mem* alpha_target     = opencl_cache_->get_data<BOXM2_ALPHA>(target_scene_, *iter_blks_target,0,true);
       boxm2_scene_info* info_buffer = target_scene_->get_blk_metadata(*iter_blks_target);
       int alphaTypeSize = (int)boxm2_data_info::datasize(boxm2_data_traits<BOXM2_ALPHA>::prefix());
       info_buffer->data_buffer_length = (int) (alpha_target->num_bytes()/alphaTypeSize);
       int data_size = info_buffer->data_buffer_length;
       bocl_mem* blk_info_target  = new bocl_mem(device_->context(), info_buffer, sizeof(boxm2_scene_info), " Scene Info" );   
       blk_info_target->create_buffer(CL_MEM_READ_ONLY | CL_MEM_COPY_HOST_PTR);       
       bocl_mem * mog_target = 0;
       if(app_type_ == "boxm2_mog3_grey")
	 mog_target       = opencl_cache_->get_data<BOXM2_MOG3_GREY>(target_scene_, *iter_blks_target,0,true);
       else if(app_type_ == "boxm2_mog3_grey_16")
	 mog_target       = opencl_cache_->get_data<BOXM2_MOG3_GREY_16>(target_scene_, *iter_blks_target,0,true);
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
               bocl_mem* alpha_source     = opencl_cache_->get_data<BOXM2_ALPHA>(source_scene_, *iter_blks_source,0,false);
               int alphaTypeSize = (int)boxm2_data_info::datasize(boxm2_data_traits<BOXM2_ALPHA>::prefix());
               info_buffer_source->data_buffer_length = (int) (alpha_source->num_bytes()/alphaTypeSize);

               bocl_mem* blk_info_source  = new bocl_mem(device_->context(), info_buffer_source, sizeof(boxm2_scene_info), " Scene Info" );   
               blk_info_source->create_buffer(CL_MEM_READ_ONLY | CL_MEM_COPY_HOST_PTR);
	       bocl_mem* mog_source = 0;
	       if(app_type_ == "boxm2_mog3_grey")
		 mog_source       = opencl_cache_->get_data<BOXM2_MOG3_GREY>(source_scene_, *iter_blks_source,0,false);
	       else if(app_type_ == "boxm2_mog3_grey_16")
		 mog_source       = opencl_cache_->get_data<BOXM2_MOG3_GREY_16>(source_scene_, *iter_blks_source,0,false);
	       else {
		 vcl_cout << "Unknown appearance type for source_scene " << app_type_ << '\n';
		 return false;
	       }
               trans_kern->set_arg(centerX.ptr());
               trans_kern->set_arg(centerY.ptr());
               trans_kern->set_arg(centerZ.ptr());
               trans_kern->set_arg(lookup.ptr());
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
               trans_kern->set_arg(ocl_depth.ptr());
               trans_kern->set_arg(output.ptr());
	       trans_kern->set_local_arg(local_threads[0]*10*sizeof(cl_uchar) );    // cumsum buffer,
               trans_kern->set_local_arg(16*local_threads[0]*sizeof(unsigned char)); // local trees target
               trans_kern->set_local_arg(16*local_threads[0]*sizeof(unsigned char)); // local trees source
               if(!trans_kern->execute(queue, 1, local_threads, global_threads))
               {
                   vcl_cout<<"Kernel Failed to Execute "<<vcl_endl;
                   return false;
               }
               int status = clFinish(queue);
               check_val(status, MEM_FAILURE, "MIFO EXECUTE FAILED: " + error_to_string(status));
               gpu_time += trans_kern->exec_time();
               //clear kernel args so it can reset them on next execution
               trans_kern->clear_args();
               clFinish(queue);
               blk_info_source->release_memory();
               delete info_buffer_source;
            }
	   mog_target->read_to_buffer(queue);
	   alpha_target->read_to_buffer(queue);
	 }
       blk_info_target->release_memory();
       delete info_buffer;
     }
   clFinish(queue);
   opencl_cache_->unref_mem(translation.ptr());
   opencl_cache_->unref_mem(rotation.ptr());
   opencl_cache_->unref_mem(scalem.ptr());
   opencl_cache_->unref_mem(ocl_depth.ptr());
   boxm2_lru_cache2::instance()->write_to_disk(target_scene_);
   return true;
}
bool boxm2_vecf_ocl_transform_scene::transform_1_blk(vgl_rotation_3d<double>  rot,
						     vgl_vector_3d<double> trans,
						     vgl_vector_3d<double> scale,
						     bool finish){
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
     translation->write_to_buffer(queue);
     rotation->write_to_buffer(queue);
     scalem->write_to_buffer(queue);
     statusw = clFinish(queue);
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
     trans_kern->set_arg(lookup.ptr());

     ocl_depth = new bocl_mem(device_->context(), &(depth), sizeof(int), "  depth of octree " );
     ocl_depth->create_buffer(CL_MEM_READ_ONLY | CL_MEM_COPY_HOST_PTR );

     //Gather information about the target and setup target data buffers
     // trans_kernel arguments set the first time the function is called
   
     blk_target       = opencl_cache_->get_block(target_scene_, *iter_blk_target);
     alpha_target     = opencl_cache_->get_data<BOXM2_ALPHA>(target_scene_, *iter_blk_target,0,true);
     info_buffer = target_scene_->get_blk_metadata(*iter_blk_target);
     int alphaTypeSize = (int)boxm2_data_info::datasize(boxm2_data_traits<BOXM2_ALPHA>::prefix());
     info_buffer->data_buffer_length = (int) (alpha_target->num_bytes()/alphaTypeSize);
     int data_size = info_buffer->data_buffer_length;
     blk_info_target  = new bocl_mem(device_->context(), info_buffer, sizeof(boxm2_scene_info), " Scene Info" );   
     blk_info_target->create_buffer(CL_MEM_READ_ONLY | CL_MEM_COPY_HOST_PTR);       
   
     if(app_type_ == "boxm2_mog3_grey")
       mog_target       = opencl_cache_->get_data<BOXM2_MOG3_GREY>(target_scene_, *iter_blk_target,0,true);
     else if(app_type_ == "boxm2_mog3_grey_16")
       mog_target       = opencl_cache_->get_data<BOXM2_MOG3_GREY_16>(target_scene_, *iter_blk_target,0,true);
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
   if(!trans_kern->execute(queue, 1, local_threads, global_threads))
     {
       vcl_cout<<"Kernel Failed to Execute "<<vcl_endl;
       return false;
     }
   int status = clFinish(queue);
   bool good_kern = check_val(status, CL_SUCCESS, "TRANSFORMATION KERNEL FAILED: " + error_to_string(status));
   if(!good_kern)
     return false;
   mog_target->read_to_buffer(queue);
   alpha_target->read_to_buffer(queue);
   status = clFinish(queue);
   bool good_read = check_val(status, CL_SUCCESS, "READ FROM GPU FAILED: " + error_to_string(status));
   if(!good_read)
     return false;
   if(finish){
     trans_kern->clear_args();
     boxm2_lru_cache2::instance()->write_to_disk(target_scene_);
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
   return true;
}
bool boxm2_vecf_ocl_transform_scene::
render_scene_appearance(vpgl_camera_double_sptr const& cam, vil_image_view<float>& expected_img,
			vil_image_view<float>& vis_img, int ni, int nj, bool finish){
  // It is assumed that this function will be called multiple times
  // where the scene changes between calls and possibly the camera
  // It is also assumed that the scene is one block
  // and the data is already avaiable in the cache and required buffers
  // have been allocated by a previous function call.
  vpgl_camera_double_sptr & nccam = const_cast<vpgl_camera_double_sptr &>(cam);
  static bool first = true;
  vcl_size_t lthreads[2]={8,8};
  cl_ni=RoundUp(ni,lthreads[0]);
  cl_nj=RoundUp(nj,lthreads[1]);
  if(!first){
    // intialize the render image planes
    vcl_fill(buff, buff + cl_ni*cl_nj, 0.0f);
    vcl_fill(vis_buff, vis_buff + cl_ni*cl_nj, 1.0f);
    vcl_fill(max_omega_buff, max_omega_buff + cl_ni*cl_nj, 0.0f);
    exp_image->write_to_buffer(queue);
    vis_image->write_to_buffer(queue);
    max_omega_image->write_to_buffer(queue);
    // assumes that the camera may be changing between calls
    boxm2_ocl_camera_converter::compute_ray_image( device_, queue, nccam, cl_ni, cl_nj, ray_o_buff, ray_d_buff, 0, 0, false);
    ray_o_buff->write_to_buffer(queue);
    ray_d_buff->write_to_buffer(queue);
    int statusw = clFinish(queue);
    bool good_write = check_val(statusw, CL_SUCCESS, "WRITE TO GPU FAILED " + error_to_string(statusw));
    if(!good_write)
      return false;
  }
  if(first){
  buff = new float[cl_ni*cl_nj];
  vcl_fill(buff, buff + cl_ni*cl_nj, 0.0f);
  exp_image=opencl_cache_->alloc_mem(cl_ni*cl_nj*sizeof(float), buff,"exp image buffer");
  exp_image->create_buffer(CL_MEM_READ_WRITE | CL_MEM_COPY_HOST_PTR);

  vis_buff = new float[cl_ni*cl_nj];
  vcl_fill(vis_buff, vis_buff + cl_ni*cl_nj, 1.0f);
  vis_image = opencl_cache_->alloc_mem(cl_ni*cl_nj*sizeof(float), vis_buff,"vis image buffer");
  vis_image->create_buffer(CL_MEM_READ_WRITE | CL_MEM_COPY_HOST_PTR);

  max_omega_buff = new float[cl_ni*cl_nj];
  vcl_fill(max_omega_buff, max_omega_buff + cl_ni*cl_nj, 0.0f);
  max_omega_image = opencl_cache_->alloc_mem(cl_ni*cl_nj*sizeof(float), max_omega_buff,"vis image buffer");
  max_omega_image->create_buffer(CL_MEM_READ_WRITE | CL_MEM_COPY_HOST_PTR);

  float tnearfar[2] = { 0.0f, 1000000.0f} ;
  
  int img_dim_buff[4];
  img_dim_buff[0] = 0;   img_dim_buff[2] = ni;
  img_dim_buff[1] = 0;   img_dim_buff[3] = nj;
  exp_img_dim=new bocl_mem(device_->context(), img_dim_buff, sizeof(int)*4, "image dims");
  exp_img_dim->create_buffer(CL_MEM_READ_WRITE | CL_MEM_COPY_HOST_PTR);


  tnearfar_mem_ptr = opencl_cache_->alloc_mem(2*sizeof(float), tnearfar, "tnearfar  buffer");
  tnearfar_mem_ptr->create_buffer(CL_MEM_READ_WRITE | CL_MEM_COPY_HOST_PTR);  

    //camera check
  if (cam->type_name()!= "vpgl_perspective_camera" &&
        cam->type_name()!= "vpgl_generic_camera" ) {
      vcl_cout<<"Cannot render with camera of type "<<cam->type_name()<<vcl_endl;
      return false;
    }
  // create all buffers
  cl_float* ray_origins = new cl_float[4*cl_ni*cl_nj];
  cl_float* ray_directions = new cl_float[4*cl_ni*cl_nj];

  ray_o_buff = opencl_cache_->alloc_mem(cl_ni*cl_nj*sizeof(cl_float4), ray_origins, "ray_origins buffer");
  ray_d_buff = opencl_cache_->alloc_mem(cl_ni*cl_nj*sizeof(cl_float4), ray_directions, "ray_directions buffer");
  
  boxm2_ocl_camera_converter::compute_ray_image( device_, queue, nccam, cl_ni, cl_nj, ray_o_buff, ray_d_buff, 0 , 0);

  rend_kern->set_arg( blk_info_target );
  rend_kern->set_arg( blk_target );
  rend_kern->set_arg( alpha_target );
  rend_kern->set_arg( mog_target );
  rend_kern->set_arg( ray_o_buff.ptr() );
  rend_kern->set_arg( ray_d_buff.ptr() );
  rend_kern->set_arg( tnearfar_mem_ptr );
  rend_kern->set_arg( exp_image );
  rend_kern->set_arg( exp_img_dim);
  rend_kern->set_arg( output.ptr() );
  rend_kern->set_arg( lookup.ptr() );
  rend_kern->set_arg( vis_image );
  rend_kern->set_arg( max_omega_image );

        //local tree , cumsum buffer, imindex buffer
  rend_kern->set_local_arg( lthreads[0]*lthreads[1]*sizeof(cl_uchar16) );
  rend_kern->set_local_arg( lthreads[0]*lthreads[1]*10*sizeof(cl_uchar) );
  rend_kern->set_local_arg( lthreads[0]*lthreads[1]*sizeof(cl_int) );
  first = false;
  }  
  //execute kernel
  static vcl_size_t gThreads[] = {cl_ni,cl_nj};
  rend_kern->execute(queue, 2, lthreads, gThreads);
  clFinish(queue);

  norm_kern->set_arg( exp_image );
  norm_kern->set_arg( vis_image );
  norm_kern->set_arg( exp_img_dim);
  norm_kern->execute( queue, 2, lthreads, gThreads);
  clFinish(queue);
  exp_image->read_to_buffer(queue);
  vis_image->read_to_buffer(queue);
  rend_kern->clear_args();
  norm_kern->clear_args();

  expected_img.set_size(ni, nj);
  expected_img.fill(0.0f);
  for (unsigned r=0;r<nj;r++)
    for (unsigned c=0;c<ni;c++)
      expected_img(c,r)=buff[r*cl_ni+c];
  vis_img.set_size(ni, nj);
  vis_img.fill(0.0f);
  for (unsigned r=0;r<nj;r++)
    for (unsigned c=0;c<ni;c++)
      vis_img(c,r)=vis_buff[r*cl_ni+c];
  if(finish){
    delete [] buff;
    delete [] vis_buff;
    delete [] max_omega_buff;
    opencl_cache_->unref_mem(ray_o_buff.ptr());
    opencl_cache_->unref_mem(ray_d_buff.ptr());
    opencl_cache_->unref_mem(tnearfar_mem_ptr);
    opencl_cache_->unref_mem(exp_image);
    opencl_cache_->unref_mem(vis_image);
    opencl_cache_->unref_mem(max_omega_image);
  }    
  return true;
}

