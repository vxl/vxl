#include <boxm2/ocl/algo/boxm2_ocl_refine_scene_around_geometry.h>
#include <boxm2/ocl/boxm2_ocl_util.h>
#include <boxm2/cpp/algo/boxm2_refine_block_function_with_labels.h>
#include <vcl_where_root_dir.h>

bool
boxm2_ocl_refine_scene_around_geometry
::compile_kernel()
{
  std::vector<std::string> src_paths;
  std::string source_dir = std::string(VCL_SOURCE_ROOT_DIR) + "/contrib/brl/bseg/boxm2/ocl/cl/";
  std::string reg_source_dir = std::string(VCL_SOURCE_ROOT_DIR)+ "/contrib/brl/bseg/boxm2/reg/ocl/cl/";
  src_paths.push_back(source_dir     + "scene_info.cl");
  src_paths.push_back(source_dir     + "bit/bit_tree_library_functions.cl");
  src_paths.push_back(source_dir     + "boxm2_ocl_helpers.cl");
  src_paths.push_back(source_dir     + "label_cells_to_refine.cl");

  std::string opts = boxm2_ocl_util::mog_options(app_type_);
  if(p_thresh_<0){
          opts+= " -D REFINE_ALL ";
          std::cout<<"probability threshold is negative -- refining all cells"<<std::endl;
  }

  auto * label_k = new bocl_kernel();
  label_k->create_kernel(&device_->context(),device_->device_id(), src_paths, "label_cells_to_refine", opts, "refinement kernel");
  kerns_.push_back(label_k);//5

  return true;

}

bool
boxm2_ocl_refine_scene_around_geometry
::refine()
{

        if (!refine_gpu_)
                for (unsigned i=0;i<num_times_;i++){
                        this->label_cells_for_refinement();
                        if (!this->refine_cpp()){
                                std::cout<<"refinement failed in pass "<<i<<std::endl;
                                return false;
                        }
                        std::cout<<"finished pass "<<i<<std::endl;
                }else{
                        return this->refine_gpu();
        }
        return true;
}

bool
boxm2_ocl_refine_scene_around_geometry
::refine_cpp()
{
        std::map<boxm2_block_id, boxm2_block_metadata> blocks = scene_->blocks();
        std::map<boxm2_block_id, boxm2_block_metadata>::iterator blk_iter;

        boxm2_refine_block_function_with_labels<int16> refiner_rgb;
        boxm2_refine_block_function_with_labels<uchar16> refiner_vdp_gray;
        boxm2_refine_block_function_with_labels<uchar8> refiner_mog3_grey;

        for (blk_iter = blocks.begin(); blk_iter != blocks.end(); ++blk_iter)
        {
                boxm2_block_id id = blk_iter->first;
                std::cout<<"Refining Block: "<<id<<std::endl;

                boxm2_block     * blk     = cache_->get_cpu_cache()->get_block(scene_,id);
                boxm2_data_base * alph    = cache_->get_cpu_cache()->get_data_base(scene_,id,boxm2_data_traits<BOXM2_ALPHA>::prefix());
                boxm2_data_base * mog     = cache_->get_cpu_cache()->get_data_base(scene_,id,app_type_);
                boxm2_data_base * labels  = cache_->get_cpu_cache()->get_data_base(scene_,id,boxm2_data_traits<BOXM2_LABEL_SHORT>::prefix("refine"));
                int data_size = alph->buffer_length()/4;
                auto* label_buf = (short *) labels->data_buffer();


                std::vector<boxm2_data_base*> datas;
                datas.push_back(alph);
                datas.push_back(mog);
                datas.push_back(labels);
                //refine block and datas
                boxm2_block_metadata data = blk_iter->second;
                if (app_type_=="boxm2_gauss_rgb_view"){
                        refiner_rgb.init_data(blk,datas,0.3,app_type_);
                        refiner_rgb.refine_deterministic(datas);
                } else if (app_type_=="boxm2_mog6_view_compact"){
                        refiner_vdp_gray.init_data(blk,datas,0.3,app_type_);
                        refiner_vdp_gray.refine_deterministic(datas);
                } else if (app_type_=="boxm2_mog3_grey"){
                        refiner_mog3_grey.init_data(blk,datas,0.3,app_type_);
                        refiner_mog3_grey.refine_deterministic(datas);

                }

                blk->enable_write(); // now cache_ will make sure that it is written to disc
                cache_->deep_remove_data(scene_,id,boxm2_data_traits<BOXM2_LABEL_SHORT>::prefix("refine"),false);
                cache_->clear_cache();
        }
        return true;
}

bool
boxm2_ocl_refine_scene_around_geometry
::refine_gpu()
{
  return false;
}

bool
boxm2_ocl_refine_scene_around_geometry
::label_cells_for_refinement()
{

        int status;
        float gpu_time=0.0f;
        cl_command_queue queue = clCreateCommandQueue( device_->context(),
                        *(device_->device_id()),
                        CL_QUEUE_PROFILING_ENABLE,
                        &status);
        if (status!=0)
                return false;

        cl_uchar lookup_arr[256];
        boxm2_ocl_util::set_bit_lookup(lookup_arr);
        bocl_mem_sptr lookup = cache_->alloc_mem(sizeof(cl_uchar)*256, lookup_arr, "bit lookup buffer");
        lookup->create_buffer(CL_MEM_READ_ONLY | CL_MEM_COPY_HOST_PTR);


        bocl_mem_sptr centerX = cache_->alloc_mem(sizeof(cl_float)*585, boct_bit_tree::centerX, "centersX lookup buffer");
        bocl_mem_sptr centerY = cache_->alloc_mem(sizeof(cl_float)*585, boct_bit_tree::centerY, "centersY lookup buffer");
        bocl_mem_sptr centerZ = cache_->alloc_mem(sizeof(cl_float)*585, boct_bit_tree::centerZ, "centersZ lookup buffer");

        centerX->create_buffer(CL_MEM_READ_ONLY | CL_MEM_COPY_HOST_PTR);
        centerY->create_buffer(CL_MEM_READ_ONLY | CL_MEM_COPY_HOST_PTR);
        centerZ->create_buffer(CL_MEM_READ_ONLY | CL_MEM_COPY_HOST_PTR);
        bocl_kernel* kern = kerns_[0];
        boxm2_scene_sptr sceneA = cache_->get_cpu_cache()->get_scenes()[0];
        std::map<boxm2_block_id, boxm2_block_metadata>& blocks = sceneA->blocks();
        std::map<boxm2_block_id, boxm2_block_metadata>::iterator blk_iter;



        long binCache = cache_.ptr()->bytes_in_cache();
        //cache_->clear_cache();

        std::string identifier = device_->device_identifier();

        //zero GPU grad buffer

        unsigned k = 0;
        {
                bvpl_kernel_sptr filter = filter_vector_->kernels_[k];

                std::stringstream filter_ident; filter_ident << filter->name() << '_' << filter->id();

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
                bocl_mem_sptr filter_buffer= cache_->alloc_mem(sizeof(cl_float4)*filter->float_kernel_.size(), filter_coeff, "filter coefficient buffer");
                filter_buffer->create_buffer(CL_MEM_READ_WRITE | CL_MEM_COPY_HOST_PTR);

                unsigned int filter_size[1];

                filter_size[0]=filter->float_kernel_.size();

                bocl_mem_sptr filter_size_buffer = cache_->alloc_mem(sizeof(unsigned int), filter_size, "filter_size buffer");
                filter_size_buffer->create_buffer(CL_MEM_READ_ONLY | CL_MEM_COPY_HOST_PTR);
                bocl_mem_sptr p_thresh_buffer = cache_->alloc_mem(sizeof(float), &p_thresh_, "probability threshold");
                p_thresh_buffer->create_buffer(CL_MEM_READ_ONLY | CL_MEM_COPY_HOST_PTR);


                for (blk_iter = blocks.begin(); blk_iter != blocks.end(); ++blk_iter)
                {
                        //clear cache

                        boxm2_block_metadata data = blk_iter->second;
                        boxm2_block_id id = blk_iter->first;
                        //cache_->clear_cache();

                        bocl_mem * data_in   = cache_->get_data<BOXM2_ALPHA>(scene_,id, 0, false);


                        int alphaTypeSize = (int)boxm2_data_info::datasize(boxm2_data_traits<BOXM2_ALPHA>::prefix());
                        // check for invalid parameters
                        if( alphaTypeSize == 0 ) //This should never happen, it will result in division by zero later
                        {
                                std::cout << "ERROR: alphaTypeSize == 0 in " << __FILE__ << __LINE__ << std::endl;
                                return false;
                        }

                        std::size_t data_size = data_in->num_bytes()/alphaTypeSize;
                        data_in->write_to_buffer(queue);
                        std::size_t labelTypeSize =  boxm2_data_info::datasize(boxm2_data_traits<BOXM2_LABEL_SHORT>::prefix());;

                        bocl_mem * to_refine = cache_->get_data_new(scene_,id,boxm2_data_traits<BOXM2_LABEL_SHORT>::prefix("refine"), data_size * labelTypeSize,false);

                        auto * output_buff = new float[data_size];
                        bocl_mem_sptr output_f = cache_->alloc_mem(data_size *sizeof(float), output_buff, "output" );
                        output_f->create_buffer(CL_MEM_READ_ONLY | CL_MEM_COPY_HOST_PTR );

                        std::cout<<"Bytes in block "<<id<<" = "<<data_size<<std::endl;

                        //grab the block out of the cache as well
                        bocl_mem* blk = cache_->get_block(scene_,id);
                        blk->write_to_buffer(queue);
                        bocl_mem* blk_info = cache_->loaded_block_info();
                        boxm2_scene_info* info_buffer = scene_->get_blk_metadata(id);
                        std::cout<<"root level is for block id "<<id <<" : "<<info_buffer->root_level<<std::endl;

                        //set workspace
                        std::size_t lThreads[] = {4, 4, 4};
                        std::size_t gThreads[] = { RoundUp(data.sub_block_num_.x(), lThreads[0]),
                                        RoundUp(data.sub_block_num_.y(), lThreads[1]),
                                        RoundUp(data.sub_block_num_.z(), lThreads[2]) };

                        binCache = cache_.ptr()->bytes_in_cache();
                        output_f->zero_gpu_buffer(queue);
                        to_refine->zero_gpu_buffer(queue);

                        kern->set_arg( blk_info );
                        kern->set_arg( blk );
                        kern->set_arg( data_in );
                        kern->set_arg( to_refine);
                        kern->set_arg( p_thresh_buffer.ptr());
                        kern->set_arg( output_f.ptr());
                        kern->set_arg( filter_buffer.ptr() );
                        kern->set_arg( filter_size_buffer.ptr() );
                        kern->set_arg( lookup.ptr() );
                        kern->set_arg( centerX.ptr() );
                        kern->set_arg( centerY.ptr() );
                        kern->set_arg( centerZ.ptr() );
                        kern->set_local_arg( lThreads[0]*lThreads[1]*lThreads[2]*sizeof(cl_uchar16) );  //local trees (uchar16 per local thread)
                        kern->set_local_arg( lThreads[0]*lThreads[1]*lThreads[2]*sizeof(cl_uchar16) );  //neighbor trees (uchar16 per local thread)
                        //execute kernel
                        kern->execute( queue, 3, lThreads, gThreads);
                        int status = clFinish(queue);
                        if (!check_val(status, CL_SUCCESS, "KERNEL FILTER EXECUTE FAILED: " + error_to_string(status)))
                                return false;

                        gpu_time += kern->exec_time();


                        //clear render kernel args so it can reset em on next execution
                        kern->clear_args();

                        //read filter response from gpu to cpu
                        to_refine->read_to_buffer(queue);
                        output_f->read_to_buffer(queue);

                        auto * refine_buf = (short*)to_refine->cpu_buffer();
                        /*for (unsigned i=0;i<data_size;i++)
                                if(refine_buf[i]!=0)
                                std::cout<<refine_buf[i];
                        std::cout<<std::endl;
*/
                        status = clFinish(queue);
                        delete [] output_buff;
                        cache_->unref_mem(output_f.ptr());

                        if (!check_val(status, CL_SUCCESS, "READ REFINEMENT LABELS  FAILED: " + error_to_string(status)))
                                return false;

                        }
                delete [] filter_coeff;
                cache_->unref_mem(filter_buffer.ptr());
                cache_->unref_mem(filter_size_buffer.ptr());
                cache_->unref_mem(p_thresh_buffer.ptr());

        }  //end block iter for

        cache_->unref_mem(lookup.ptr());
        cache_->unref_mem(centerX.ptr());
        cache_->unref_mem(centerY.ptr());
        cache_->unref_mem(centerZ.ptr());

        return true;
}
