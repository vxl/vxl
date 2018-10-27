// This is brl/bseg/boxm2/ocl/pro/processes/boxm2_filter_scene_data_process.cxx
#include <bprb/bprb_func_process.h>
//:
// \file
// \brief  A process for updating parents alpha by the max prob of the children
//
// \author Vishal Jain
// \date Apr 23, 2013


#include <boxm2/ocl/algo/boxm2_ocl_filter_scene_data.h>

void boxm2_ocl_filter_scene_data::compile_kernels()
    {
        //gather all render sources... seems like a lot for rendering...
        std::vector<std::string> src_paths;
        std::string source_dir = boxm2_ocl_util::ocl_src_root();
        src_paths.push_back(source_dir + "scene_info.cl");
        src_paths.push_back(source_dir + "bit/bit_tree_library_functions.cl");
        src_paths.push_back(source_dir + "boxm2_ocl_helpers.cl");
        if(appType_=="boxm2_gauss_rgb_view"){
                src_paths.push_back(source_dir + "bit/filter_scene_data_rgb.cl");

        }else{
                src_paths.push_back(source_dir + "bit/filter_scene_data.cl");
        }
        std::string opts = boxm2_ocl_util::mog_options(appType_);

        std::cout<<"compiling downsampling and filler kernel with options "<<opts<<std::endl;

        auto * smooth_kernel= new bocl_kernel();
        smooth_kernel->create_kernel( &device_->context(),
                                     device_->device_id(),
                                     src_paths,
                                     "kernel_filter_scene",
                                     opts,
                                     "boxm2 opencl smooth scene data"); //kernel identifier (for error checking)
        kernels.push_back(smooth_kernel);
    }

bool boxm2_ocl_filter_scene_data::apply_filter(int index)
{
  if(index>=filter_vector_->size())
    return false;

    // bit lookup buffer
    cl_uchar lookup_arr[256];
    boxm2_ocl_util::set_bit_lookup(lookup_arr);
    bocl_mem_sptr lookup = opencl_cache_->alloc_mem(sizeof(cl_uchar)*256, lookup_arr,  "bit lookup buffer");
    lookup->create_buffer(CL_MEM_READ_ONLY | CL_MEM_COPY_HOST_PTR);

     bvpl_kernel_sptr filter = filter_vector_->kernels_[index]; //get last kernel, the averaging one
     bocl_kernel* kern= kernels[0];

    std::stringstream filter_ident; filter_ident << filter->name() << '_' << filter->id();
    std::cout<<"Smoothing scene with filter: " << filter_ident.str() << " of size: " << filter->float_kernel_.size() <<std::endl;
    //filter->print();

    //set up the filter, filter buffer and other related filter variables
    auto kit = filter->float_kernel_.begin();
    unsigned ci=0;
    auto* filter_coeff = new cl_float4 [filter->float_kernel_.size()];
    for (; kit!= filter->float_kernel_.end(); kit++, ci++)
    {
      vgl_point_3d<float> loc = kit->first;
      float w = kit->second.c_;

      filter_coeff[ci].s[0] = loc.x();
      filter_coeff[ci].s[1] = loc.y();
      filter_coeff[ci].s[2] = loc.z();
      filter_coeff[ci].s[3] = w;
    }

    bocl_mem_sptr filter_buffer = opencl_cache_->alloc_mem(sizeof(cl_float4)*filter->float_kernel_.size(), filter_coeff, "filter coefficient buffer");
  filter_buffer->create_buffer(CL_MEM_READ_WRITE | CL_MEM_COPY_HOST_PTR);

  unsigned int filter_size[1];
  filter_size[0]=filter->float_kernel_.size();
  bocl_mem_sptr filter_size_buffer = opencl_cache_->alloc_mem(sizeof(unsigned int), filter_size, "filter_size buffer");
  filter_size_buffer->create_buffer(CL_MEM_READ_ONLY | CL_MEM_COPY_HOST_PTR);
    //2. set workgroup size

    std::map<boxm2_block_id, boxm2_block_metadata> blocks = scene_->blocks();
    std::map<boxm2_block_id, boxm2_block_metadata>::iterator blk_iter;
    bocl_mem_sptr centerX = opencl_cache_->alloc_mem(sizeof(cl_float)*585, boct_bit_tree::centerX, "centersX lookup buffer");
    bocl_mem_sptr centerY = opencl_cache_->alloc_mem(sizeof(cl_float)*585, boct_bit_tree::centerY, "centersY lookup buffer");
    bocl_mem_sptr centerZ = opencl_cache_->alloc_mem(sizeof(cl_float)*585, boct_bit_tree::centerZ, "centersZ lookup buffer");
    centerX->create_buffer(CL_MEM_READ_ONLY | CL_MEM_COPY_HOST_PTR);
    centerY->create_buffer(CL_MEM_READ_ONLY | CL_MEM_COPY_HOST_PTR);
    centerZ->create_buffer(CL_MEM_READ_ONLY | CL_MEM_COPY_HOST_PTR);

    for (blk_iter = blocks.begin(); blk_iter != blocks.end(); ++blk_iter)
    {
        boxm2_block_id id = blk_iter->first;
        std::cout<<"Smoothing scene_"<<id<<std::endl;
        //clear cache
        boxm2_block_metadata data = blk_iter->second;
        int numTrees = data.sub_block_num_.x() * data.sub_block_num_.y() * data.sub_block_num_.z();

        //write the image values to the buffer
        vul_timer transfer;
        bocl_mem* blk       = opencl_cache_->get_block(scene_,id);
        bocl_mem* alpha     = opencl_cache_->get_data<BOXM2_ALPHA>(scene_,id,0,false);

        int alphaTypeSize = (int)boxm2_data_info::datasize(boxm2_data_traits<BOXM2_ALPHA>::prefix());
        // check for invalid parameters
        if( alphaTypeSize == 0 ) //This should never happen, it will result in division by zero later
        {
            std::cout << "ERROR: alphaTypeSize == 0 in " << __FILE__ << __LINE__ << std::endl;
            return false;
        }

                std::size_t data_size = alpha->num_bytes()/alphaTypeSize;
                bocl_mem * blk_info  = opencl_cache_->loaded_block_info();

                bocl_mem * mog       = opencl_cache_->get_data(scene_,id,appType_,data_size * appTypeSize_,false);
        bocl_mem * alpha_new =opencl_cache_->get_data_new(scene_,id,boxm2_data_traits<BOXM2_ALPHA>::prefix("new"),data_size * alphaTypeSize,false);
        bocl_mem * mog_new  = opencl_cache_->get_data_new(scene_,id, appType_+"_new", data_size * appTypeSize_, false);
        auto* output_arr = new float[data_size];
        bocl_mem_sptr cl_output = opencl_cache_->alloc_mem(sizeof(float)*data_size, output_arr, "output buffer");
        cl_output->create_buffer(CL_MEM_READ_WRITE | CL_MEM_COPY_HOST_PTR);

        alpha_new->zero_gpu_buffer(*queue_);
        mog_new->zero_gpu_buffer(*queue_);
        cl_output->zero_gpu_buffer(*queue_);
        std::size_t lThreads[] = {4, 4, 4};
                        std::size_t gThreads[] = { RoundUp(data.sub_block_num_.x(), lThreads[0]),
                                        RoundUp(data.sub_block_num_.y(), lThreads[1]),
                                        RoundUp(data.sub_block_num_.z(), lThreads[2]) };


        //set first kernel args
        kern->set_arg( blk_info );
        kern->set_arg( blk );
        kern->set_arg( alpha );
        kern->set_arg( alpha_new);
        kern->set_arg( mog);
        kern->set_arg( mog_new);
        kern->set_arg( cl_output.ptr());
        kern->set_arg( filter_buffer.ptr() );
        kern->set_arg( filter_size_buffer.ptr() );
        kern->set_arg( lookup.ptr() );
        kern->set_arg( centerX.ptr() );
        kern->set_arg( centerY.ptr() );
        kern->set_arg( centerZ.ptr() );

        kern->set_local_arg( lThreads[0]*lThreads[1]*lThreads[2]*sizeof(cl_uchar16) );  //local trees (uchar16 per local thread)
                kern->set_local_arg( lThreads[0]*lThreads[1]*lThreads[2]*sizeof(cl_uchar16) );  //neighbor trees (uchar16 per local thread)
                        //execute kernel
                kern->execute( *queue_, 3, lThreads, gThreads);

        //clear render kernel args so it can reset em on next execution
        kern->clear_args();
        alpha_new->read_to_buffer(*queue_);
        mog_new->read_to_buffer(*queue_);
        cl_output->read_to_buffer(*queue_);
        clFinish(*queue_);

      /*  for(unsigned i=0;i<data_size;i++)
                if (output_arr[i]!=0)
                        std::cout<<output_arr[i]<<" ";*/
        std::memcpy((char*)alpha->cpu_buffer(),(char*)alpha_new->cpu_buffer(),data_size * alphaTypeSize);
        std::memcpy((char*)mog->cpu_buffer(),  (char*)mog_new->cpu_buffer(),data_size * appTypeSize_);

        alpha->write_to_buffer(*queue_);
        mog->write_to_buffer(*queue_);

        delete [] output_arr;
        opencl_cache_->unref_mem(cl_output.ptr());
        opencl_cache_->deep_remove_data(scene_,id,boxm2_data_traits<BOXM2_ALPHA>::prefix("new"),false);
        opencl_cache_->deep_remove_data(scene_,id,appType_+"_new",false);
    }
    delete [] filter_coeff;

    opencl_cache_->unref_mem(lookup.ptr());
    opencl_cache_->unref_mem(filter_buffer.ptr());
    opencl_cache_->unref_mem(filter_size_buffer.ptr());
    opencl_cache_->unref_mem(centerX.ptr());
    opencl_cache_->unref_mem(centerY.ptr());
    opencl_cache_->unref_mem(centerZ.ptr());


    return true;
}
