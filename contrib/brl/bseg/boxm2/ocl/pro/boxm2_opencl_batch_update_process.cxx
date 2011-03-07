#include "boxm2_opencl_batch_update_process.h"

//boxm2 data structures
#include <boxm2/boxm2_scene.h>
#include <boxm2/boxm2_block.h>
#include <boxm2/boxm2_data_base.h>
#include <vil/vil_save.h>
#include <vil/vil_math.h>
#include <vil/vil_load.h>
#include <boxm2/ocl/boxm2_ocl_util.h>

//brdb stuff
#include <brdb/brdb_value.h>

//directory utility
#include <vul/vul_timer.h>
#include <vul/vul_file_iterator.h>
#include <vul/vul_file.h>
#include <vcl_where_root_dir.h>

//TODO IN THIS INIT METHOD: Need to pass in a ref to the OPENCL_CACHE so this
//class can easily access BOCL_MEMs
bool boxm2_opencl_batch_update_process::init_kernel(cl_context* context,
                                              cl_device_id* device,
                                              vcl_string opts)
{
  context_ = context;
  vcl_vector<vcl_string> src_paths;

  vcl_string source_dir = vcl_string(VCL_SOURCE_ROOT_DIR) + "/contrib/brl/bseg/boxm2/ocl/cl/";
  src_paths.push_back(source_dir + "scene_info.cl");
  src_paths.push_back(source_dir + "cell_utils.cl");
  src_paths.push_back(source_dir + "bit/bit_tree_library_functions.cl");
  src_paths.push_back(source_dir + "backproject.cl");
  src_paths.push_back(source_dir + "ray_bundle_library_opt.cl");
  vcl_vector<vcl_string> non_ray_src = vcl_vector<vcl_string>(src_paths);
  src_paths.push_back(source_dir + "bit/batchkernels.cl");
  src_paths.push_back(source_dir + "update_functors.cl");
  src_paths.push_back(source_dir + "bit/cast_ray_bit.cl");

  //compilation options
  vcl_string options = " -D INTENSITY ";
  options += " -D MOG_TYPE_8 "; 
  options += opts;

  //create all passes
  bocl_kernel* seg_len = new bocl_kernel();
  vcl_string seg_opts = options + " -D CUMLEN -D STEP_CELL=step_cell_cumlen(aux_args,data_ptr,llid,d) ";
  seg_len->create_kernel(context_, device, src_paths, "cum_len_main", seg_opts, "update::seg_len");
  update_kernels_.push_back(seg_len);


  bocl_kernel* update_hist = new bocl_kernel();
  vcl_string hist_opts = options + " -D UPDATE_HIST -D STEP_CELL=step_cell_update_hist(aux_args,data_ptr,llid,d) ";
  update_hist->create_kernel(context_, device, src_paths, "update_hist_main", hist_opts, "update::hist");
  update_kernels_.push_back(update_hist);
  
  vcl_vector<vcl_string> clean_seg_kernels_src;
  clean_seg_kernels_src.push_back(source_dir + "scene_info.cl");
  clean_seg_kernels_src.push_back(source_dir + "bit/batchkernels.cl");

  bocl_kernel* clean_seg_len = new bocl_kernel();
  vcl_string clean_seg_len_opts = options + " -D CLEAN_SEG_LEN ";
  clean_seg_len->create_kernel(context_, device, clean_seg_kernels_src, "clean_seg_len_main", clean_seg_len_opts, "update::hist");
  update_kernels_.push_back(clean_seg_len);

  //may need DIFF LIST OF SOURCES FOR THSI GUY TOO
  non_ray_src.push_back(source_dir+"bit/batchkernels.cl");
  bocl_kernel* update_prob = new bocl_kernel();
  vcl_string prob_opts = options + " -D UPDATE_PROB ";

  update_prob->create_kernel(context_, device, non_ray_src, "update_prob_main", prob_opts, "update::update_prob");
  update_kernels_.push_back(update_prob);

  return true;
}


// Opencl Update Process
// arguments will be (should be)
// * scene pointer
// * camera (for input image)
// * input image
// * visibility image...
bool boxm2_opencl_batch_update_process::execute(vcl_vector<brdb_value_sptr>& input, vcl_vector<brdb_value_sptr>& output)
{
    transfer_time_ = 0.0f; gpu_time_ = 0.0f; total_time_ = 0.0f;
    vul_timer total;
    int i = 0;

    //scene argument
    brdb_value_t<boxm2_scene_sptr>* scene_brdb = static_cast<brdb_value_t<boxm2_scene_sptr>* >( input[i++].ptr() );
    boxm2_scene_sptr scene = scene_brdb->value();

    //camera
    brdb_value_t<vcl_string>* brdb_cam_dir = static_cast<brdb_value_t<vcl_string>* >( input[i++].ptr() );
    vcl_string camdir = brdb_cam_dir->value();

    brdb_value_t<vcl_string>* brdb_img_dir = static_cast<brdb_value_t<vcl_string>* >( input[i++].ptr() );
    vcl_string imgdir = brdb_img_dir->value();

    if (!vul_file::is_directory(camdir.c_str()) || !vul_file::is_directory(imgdir.c_str())) {
        vcl_cout<<"Cam dir is not a directory"<<vcl_endl;
        return -1;
    }

    //get all of the cam and image files, sort them
    vcl_string camglob=camdir+"/*.txt";
    vcl_string imgglob=imgdir+"/*.???";
    vul_file_iterator file_it(camglob.c_str());
    vul_file_iterator img_file_it(imgglob.c_str());
    vcl_vector<vcl_string> cam_files;
    vcl_vector<vcl_string> img_files;
    while (file_it && img_file_it) {
        vcl_string camName(file_it());
        vcl_string imgName(img_file_it());
        cam_files.push_back(camName);
        img_files.push_back(imgName);
        ++file_it; ++img_file_it;
    }
    vcl_sort(cam_files.begin(), cam_files.end());
    vcl_sort(img_files.begin(), img_files.end());
    if (cam_files.size() != img_files.size()) {
        vcl_cerr<<"Image files and cam files not one to one\n";
        return -1;
    }
    //output buffer
    float* output_arr = new float[500];
    for (int i=0; i<500; ++i) output_arr[i] = 0.0f;
    cl_output_ = new bocl_mem((*context_), output_arr, sizeof(float)*500, "output buffer");
    cl_output_->create_buffer(CL_MEM_READ_WRITE | CL_MEM_COPY_HOST_PTR);

    //bit lookup buffer
    cl_uchar* lookup_arr = new cl_uchar[256];
    boxm2_ocl_util::set_bit_lookup(lookup_arr);
    lookup_ = new bocl_mem((*context_), lookup_arr, sizeof(cl_uchar)*256, "bit lookup buffer");
    lookup_->create_buffer(CL_MEM_READ_ONLY | CL_MEM_COPY_HOST_PTR);

    int* img_dim_buff = new int[4];
    cl_float* cam_buffer = new cl_float[16*3];
    vcl_vector<boxm2_block_id>::iterator id;

    for(unsigned img_num=0;img_num<img_files.size();img_num++)
    {
        vcl_cout<<"Image # "<<img_num<<vcl_endl;
        //Go through each kernel, execute on each block
        vpgl_perspective_camera<double> cam;
        boxm2_ocl_util::load_perspective_camera(cam_files[img_num],cam);
        boxm2_ocl_util::set_persp_camera(&cam, cam_buffer);
        persp_cam_ = new bocl_mem((*context_), cam_buffer, 3*sizeof(cl_float16), "persp cam buffer");
        persp_cam_->create_buffer(CL_MEM_READ_WRITE | CL_MEM_COPY_HOST_PTR);
        //input image buffer
        vil_image_view_base_sptr img_view=boxm2_ocl_util::prepare_input_image(img_files[img_num]);

        /////////////////////// NEW IMAGE BUFFERS ////////////////////////////////////
        //exp image dimensions
        img_size_[0] = img_view->ni();
        img_size_[1] = img_view->nj();
        img_dim_buff[0] = 0;
        img_dim_buff[1] = 0;
        img_dim_buff[2] = img_view->ni();
        img_dim_buff[3] = img_view->nj();
        img_dim_ = new bocl_mem((*context_), img_dim_buff, sizeof(cl_int4), "image dims");
        img_dim_->create_buffer(CL_MEM_READ_WRITE | CL_MEM_COPY_HOST_PTR);
        //For each ID in the visibility order, grab that block

        for (unsigned int i=0; i<3; ++i)
        {
            if(vil_image_view<float>* in_img_view = static_cast<vil_image_view<float>* >(img_view.ptr()))
                this->write_input_image(in_img_view);

            vcl_vector<boxm2_block_id> vis_order = scene->get_vis_blocks(&cam);
            //vcl_cout<<"UPDATE KERNEL : "<<i<<vcl_endl;
            //zip through visible blocks, and execute this pass's kernel
            for (id = vis_order.begin(); id != vis_order.end(); ++id)
            {
                //write the image values to the buffer
                vul_timer transfer;
                blk_       = cache_->get_block(*id);
                alpha_     = cache_->get_data<BOXM2_ALPHA>(*id);
                blk_info_  = cache_->loaded_block_info();
                hist_      = cache_->get_data<BOXM2_BATCH_HISTOGRAM>(*id);
                //get aux data
                aux_       = cache_->get_data<BOXM2_AUX>(*id);
                transfer_time_ += (float) transfer.all();
                //set workspace and args for this pass
                this->set_workspace(i);
                this->set_args(i);

                //execute kernel
                update_kernels_[i]->execute( (*command_queue_), 2, lThreads_, gThreads_);
                int status = clFinish(*command_queue_);
                check_val(status, MEM_FAILURE, "UPDATE EXECUTE FAILED: " + error_to_string(status));

                //clear render kernel args so it can reset em on next execution
                update_kernels_[i]->clear_args();

                //write info to disk
                blk_->read_to_buffer(*command_queue_);
                alpha_->read_to_buffer(*command_queue_);
                aux_->read_to_buffer(*command_queue_);
                hist_->read_to_buffer(*command_queue_);
                //read image out to buffer (from gpu)
                image_->read_to_buffer(*command_queue_);
                vis_image_->read_to_buffer(*command_queue_);
                cl_output_->read_to_buffer(*command_queue_);
                clFinish(*command_queue_);
                //for (int i=0; i<500; ++i) vcl_cout<<output_arr[i]<<" ";
                //vcl_cout<<vcl_endl;
         }
        }
        delete persp_cam_;
    }
     delete[] cam_buffer;

    vcl_cout<<"UPDATE KERNEL : UPDATE_PROB"<<vcl_endl;
    vcl_vector<boxm2_block_id> block_ids=scene->get_block_ids();

    int kernel_index=3;
    for (id = block_ids.begin(); id != block_ids.end(); ++id)
    {
        //write the image values to the buffer
        vul_timer transfer;
        blk_       = cache_->get_block(*id);
        alpha_     = cache_->get_data<BOXM2_ALPHA>(*id);
        blk_info_  = cache_->loaded_block_info();
        hist_           = cache_->get_data<BOXM2_BATCH_HISTOGRAM>(*id);
        intensity_      = cache_->get_data<BOXM2_MOG3_GREY>(*id);
        //get aux data
        //aux_       = cache_->get_data<BOXM2_AUX>(*id);
        transfer_time_ += (float) transfer.all();

        //set workspace and args for this pass
        this->set_workspace(kernel_index);
        this->set_args(kernel_index);

        //execute kernel
        update_kernels_[kernel_index]->execute( (*command_queue_), 2, lThreads_, gThreads_);
        int status = clFinish(*command_queue_);
        check_val(status, MEM_FAILURE, "UPDATE EXECUTE FAILED: " + error_to_string(status));

        //clear render kernel args so it can reset em on next execution
        update_kernels_[kernel_index]->clear_args();

        //write info to disk
        alpha_->read_to_buffer(*command_queue_);
        //aux_->read_to_buffer(*command_queue_);
        hist_->read_to_buffer(*command_queue_);
        cl_output_->read_to_buffer(*command_queue_);
        intensity_->read_to_buffer(*command_queue_);
        clFinish(*command_queue_);

        //for (int i=0; i<500; ++i) vcl_cout<<output_arr[i]<<" ";
        //vcl_cout<<vcl_endl;
    }
    //clean up camera, lookup_arr, img_dim_buff
    delete[] output_arr;
    delete[] img_dim_buff;
    delete[] lookup_arr;

    delete cl_output_;
    delete img_dim_;
    delete lookup_;


    //record total time
    total_time_ = (float) total.all();
    return true;
}

bool boxm2_opencl_batch_update_process::clean()
{
  return true;
}

bool boxm2_opencl_batch_update_process::set_workspace(unsigned pass)
{
    switch (pass) {
    case UPDATE_SEGLEN:
    case UPDATE_HIST:
        lThreads_[0]  = 8;
        lThreads_[1]  = 8;
        gThreads_[0] = RoundUp(img_size_[0],lThreads_[0]);
        gThreads_[1] = RoundUp(img_size_[1],lThreads_[1]);
        break;
    case CLEAN_SEGLEN:
        {
            boxm2_scene_info* info_buffer = (boxm2_scene_info*) blk_info_->cpu_buffer();
            int numbuf = info_buffer->num_buffer;
            int datlen = info_buffer->data_buffer_length;
            gThreads_[0] = RoundUp(numbuf*datlen,64);
            gThreads_[1] = 1;
            lThreads_[0]  = 1;
            lThreads_[1]  = 1;
            break;
        }
    case UPDATE_PROB:
        {
            boxm2_scene_info* info_buffer = (boxm2_scene_info*) blk_info_->cpu_buffer();
            int numtrees = info_buffer->scene_dims[0]*info_buffer->scene_dims[1]*info_buffer->scene_dims[2];
            gThreads_[0] = RoundUp(numtrees,64);
            gThreads_[1] = 1;
            lThreads_[0]  = 1;
            lThreads_[1]  = 1;
            break;
        }
    }
  return true;
}


bool boxm2_opencl_batch_update_process::set_args(unsigned pass)
{
  switch (pass)
  {
    case UPDATE_SEGLEN :
      update_kernels_[pass]->set_arg( blk_info_ );
      update_kernels_[pass]->set_arg( blk_ );
      update_kernels_[pass]->set_arg( aux_ );
      update_kernels_[pass]->set_arg( lookup_ );
      update_kernels_[pass]->set_arg( persp_cam_ );
      update_kernels_[pass]->set_arg( img_dim_ );
      update_kernels_[pass]->set_arg( cl_output_ );
      update_kernels_[pass]->set_local_arg( lThreads_[0]*lThreads_[1]*sizeof(cl_uchar16) );//local tree,
      update_kernels_[pass]->set_local_arg( lThreads_[0]*lThreads_[1]*10*sizeof(cl_uchar) ); //cumsum buffer, imindex buffer
      break;
    case UPDATE_HIST :
        update_kernels_[pass]->set_arg( blk_info_ );
        update_kernels_[pass]->set_arg( blk_ );
        update_kernels_[pass]->set_arg( alpha_ );
        update_kernels_[pass]->set_arg( aux_ );
        update_kernels_[pass]->set_arg( hist_ );
        update_kernels_[pass]->set_arg( lookup_ );
        update_kernels_[pass]->set_arg( persp_cam_ );
        update_kernels_[pass]->set_arg( img_dim_ );
        update_kernels_[pass]->set_arg( image_ );
        update_kernels_[pass]->set_arg( vis_image_ );
        update_kernels_[pass]->set_arg( cl_output_ );
        update_kernels_[pass]->set_local_arg( lThreads_[0]*lThreads_[1]*sizeof(cl_uchar16) );//local tree,
        update_kernels_[pass]->set_local_arg( lThreads_[0]*lThreads_[1]*10*sizeof(cl_uchar) ); //cumsum buffer, imindex buffer
        break;
    case CLEAN_SEGLEN :
        update_kernels_[pass]->set_arg( blk_info_ );
        update_kernels_[pass]->set_arg( aux_ );
        update_kernels_[pass]->set_arg( cl_output_ );
        break;
    case UPDATE_PROB :
        update_kernels_[pass]->set_arg( blk_info_ );
        update_kernels_[pass]->set_arg( lookup_ );
        update_kernels_[pass]->set_arg( blk_ );
        update_kernels_[pass]->set_arg( alpha_ );
        //update_kernels_[pass]->set_arg( aux_ );
        update_kernels_[pass]->set_arg( intensity_ );
        update_kernels_[pass]->set_arg( hist_ );
        update_kernels_[pass]->set_arg( cl_output_ );
        update_kernels_[pass]->set_local_arg( lThreads_[0]*lThreads_[1]*16* sizeof(cl_uchar) );//local tree,
     break;
  }
  return true;
}


bool boxm2_opencl_batch_update_process::write_input_image(vil_image_view<float>* input_image)
{

    //write to buffer (or create it)
    unsigned ni=RoundUp(input_image->ni(),8);
    unsigned nj=RoundUp(input_image->nj(),8);
    float* buff = (image_) ? (float*) image_->cpu_buffer() : new float[4*ni*nj];
    float* vis_buff = (vis_image_) ? (float*) vis_image_->cpu_buffer() : new float[ni*nj];

    int count=0;
    for (unsigned j=0;j<nj;j++)
    {
        for (unsigned i=0;i<ni;i++)
        {
      buff[4*count] = 0.0f;
      buff[4*count + 1] = 0.0f;
      buff[4*count + 2] = 1.0f;
      buff[4*count + 3] = 0.0f;
            if (i<input_image->ni() && j< input_image->nj())
                buff[4*count]=(*input_image)(i,j);

            vis_buff[count]=1.0f;
            ++count;
        }
    }
    //now write to bocl_mem
    if (!image_) {
        //create mem
        image_ = new bocl_mem((*context_), buff, ni*nj * sizeof(cl_float4), "input image buffer");
        image_->create_buffer(CL_MEM_READ_WRITE | CL_MEM_COPY_HOST_PTR);
    }
    else {
        image_->write_to_buffer(*command_queue_);
    }
    if (!vis_image_) {
        //create mem
        vis_image_ = new bocl_mem((*context_), vis_buff, ni*nj * sizeof(cl_float), "input image buffer");
        vis_image_->create_buffer(CL_MEM_READ_WRITE | CL_MEM_COPY_HOST_PTR);
    }
    else {
        vis_image_->write_to_buffer(*command_queue_);
    }
  return true;
}
