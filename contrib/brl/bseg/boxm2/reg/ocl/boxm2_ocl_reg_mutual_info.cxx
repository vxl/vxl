#include "boxm2_ocl_reg_mutual_info.h"
//:
// \file
#include <vgl/vgl_box_3d.h>
#include <vgl/vgl_vector_3d.h>
#include <vgl/algo/vgl_rotation_3d.h>
#include <boxm2/boxm2_block_metadata.h>
#include <boxm2/boxm2_block.h>
#include <boxm2/boxm2_data_base.h>
#include <boxm2/boxm2_util.h>
#include <boxm2/ocl/boxm2_ocl_util.h>
#include <boct/boct_bit_tree.h>
#include <brip/brip_mutual_info.h>
#include <bocl/bocl_mem.h>
#include <bocl/bocl_kernel.h>
#include <vcl_where_root_dir.h>
#include <vcl_algorithm.h>

typedef vnl_vector_fixed<unsigned char,16> uchar16;



boxm2_ocl_reg_mutual_info::boxm2_ocl_reg_mutual_info( boxm2_opencl_cache2_sptr& cacheA,
                                                     boxm2_stream_scene_cache& cacheB,
                                                     bocl_device_sptr device,
                                                     int nbins,
                                                     double scale)
                                                     :  cacheB_(cacheB), cacheA_(cacheA),device_(device),nbins_(nbins), scale_(scale)
{
    this->compile_kernel();
    this->init_ocl_minfo();
}

boxm2_ocl_reg_mutual_info::~boxm2_ocl_reg_mutual_info()
{
    delete [] blk_offsets_array ;
    delete [] alpha_offsets_array;
    delete kern;
    cacheA_->clear_cache();
}

double boxm2_ocl_reg_mutual_info:: mutual_info(vgl_rotation_3d<double> rot,vgl_vector_3d<double> trans, int depth)
{
    float val = 0.0f;
    this->boxm2_ocl_register_world(rot,trans,scale_,nbins_,depth,val);
    return val;
}
double boxm2_ocl_reg_mutual_info:: mutual_info(vnl_vector<double> const& x,  int depth )
{
    vgl_vector_3d<double>   tx(x[0],x[1],x[2]);
    vgl_rotation_3d<double> r (vnl_vector_fixed<double,3>(x[3],x[4],x[5]));
    return this->mutual_info(r,tx,depth);
}


bool boxm2_ocl_reg_mutual_info::compile_kernel()
{
    vcl_vector<vcl_string> src_paths;
    vcl_string source_dir = vcl_string(VCL_SOURCE_ROOT_DIR) + "/contrib/brl/bseg/boxm2/ocl/cl/";
    vcl_string reg_source_dir = vcl_string(VCL_SOURCE_ROOT_DIR)+ "/contrib/brl/bseg/boxm2/reg/ocl/cl/";
    src_paths.push_back(source_dir     + "scene_info.cl");
    src_paths.push_back(source_dir     + "bit/bit_tree_library_functions.cl");
    src_paths.push_back(reg_source_dir + "estimate_mi_vol.cl");
    this->kern = new bocl_kernel();
    return kern->create_kernel(&device_->context(),device_->device_id(), src_paths, "estimate_mi_vol", "", "MI");
}

bool boxm2_ocl_reg_mutual_info::init_ocl_minfo()
{
    boxm2_scene_sptr sceneB = cacheB_.scene();
    // Instantiate OPENCL
    // get scene B on the GPU's host memory
    blk_offsets_array = new unsigned int[cacheB_.blk_offsets_.size()];
    for (unsigned k = 0; k< cacheB_.blk_offsets_.size(); k++)
        blk_offsets_array[k] = cacheB_.blk_offsets_[k];

    alpha_offsets_array = new unsigned int[cacheB_.offsets_["alpha"].size()];
    for (unsigned k = 0; k< cacheB_.offsets_["alpha"].size(); k++)
        alpha_offsets_array[k] =cacheB_.offsets_["alpha"][k]/4;

    vcl_vector<boxm2_block_id> sceneB_ids = sceneB->get_block_ids();
    boxm2_scene_info * sceneB_info = sceneB->get_blk_metadata( sceneB_ids[0] );

    vgl_box_3d<int> bbox = sceneB->bounding_box_blk_ids();

    bbox_buff[0] = 0; 
    bbox_buff[1] = 0; 
    bbox_buff[2] = 0; 
    bbox_buff[3] = bbox.max_x() - bbox.min_x();
    bbox_buff[4] = bbox.max_y() - bbox.min_y();
    bbox_buff[5] = bbox.max_z() - bbox.min_z();
    bbox_buff[6] = sceneB_info->scene_dims[0];
    bbox_buff[7] = sceneB_info->scene_dims[1];
    bbox_buff[8] = sceneB_info->scene_dims[2];

    sceneB_bbox_ids = new bocl_mem(device_->context(), bbox_buff, 9*sizeof(int), " scene B bbox" );
    sceneB_bbox_ids->create_buffer(CL_MEM_READ_ONLY | CL_MEM_COPY_HOST_PTR );

    vgl_box_3d<double> scene_bbox = sceneB->bounding_box();
    sceneB_origin_buff[0] = scene_bbox.min_x();
    sceneB_origin_buff[1] = scene_bbox.min_y();
    sceneB_origin_buff[2] = scene_bbox.min_z();

    sceneB_origin = new bocl_mem(device_->context(), sceneB_origin_buff, 4*sizeof(float), " scene B origin" );
    sceneB_origin->create_buffer(CL_MEM_READ_ONLY | CL_MEM_COPY_HOST_PTR );

    block_dims[0] = sceneB_info->scene_dims[0]*sceneB_info->block_len;
    block_dims[1] = sceneB_info->scene_dims[1]*sceneB_info->block_len;
    block_dims[2] = sceneB_info->scene_dims[2]*sceneB_info->block_len;
    sceneB_block_dims = new bocl_mem(device_->context(), block_dims, 4*sizeof(float), " scene B block dims" );
    sceneB_block_dims->create_buffer(CL_MEM_READ_ONLY | CL_MEM_COPY_HOST_PTR );

    subblk_num_buff[0] = sceneB_info->scene_dims[0];
    subblk_num_buff[1] = sceneB_info->scene_dims[1];
    subblk_num_buff[2] = sceneB_info->scene_dims[2];
    subblk_num_buff[3] = sceneB_info->scene_dims[3];

    sceneB_sub_block_num = new bocl_mem(device_->context(), subblk_num_buff, sizeof(int)* 4, " scene B sub block num" );
    sceneB_sub_block_num->create_buffer(CL_MEM_READ_ONLY | CL_MEM_COPY_HOST_PTR );

    sceneB_sub_block_len = new bocl_mem(device_->context(), &(sceneB_info->block_len), sizeof(float), " scene B sub block len" );
    sceneB_sub_block_len->create_buffer(CL_MEM_READ_ONLY | CL_MEM_COPY_HOST_PTR );

    blks_ocl_B = new bocl_mem(device_->context(), cacheB_.blk_buffer_, cacheB_.total_bytes_per_block_, " block buffer B" );
    blks_ocl_B->create_buffer(CL_MEM_READ_ONLY | CL_MEM_USE_HOST_PTR );

    blks_ocl_B_offsets = new bocl_mem(device_->context(), blk_offsets_array, sizeof(unsigned int)*cacheB_.blk_offsets_.size(), " block buffer B" );
    blks_ocl_B_offsets->create_buffer(CL_MEM_READ_ONLY | CL_MEM_COPY_HOST_PTR );

    alpha_ocl_B = new bocl_mem(device_->context(), cacheB_.data_buffers_["alpha"], cacheB_.total_bytes_per_data_["alpha"], " alpha buffer B " );
    alpha_ocl_B->create_buffer(CL_MEM_READ_ONLY | CL_MEM_USE_HOST_PTR );

    alpha_ocl_B_offsets = new bocl_mem(device_->context(), alpha_offsets_array, sizeof(unsigned int)*cacheB_.offsets_["alpha"].size(), " block buffer B" );
    alpha_ocl_B_offsets->create_buffer(CL_MEM_READ_ONLY | CL_MEM_COPY_HOST_PTR );
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

    
    return true;
}

bool boxm2_ocl_reg_mutual_info::boxm2_ocl_register_world(vgl_rotation_3d<double> rot,
                                                         vgl_vector_3d<double> tx,
                                                         double s,
                                                         int nbins, int depth,
                                                         float & mi)
{
    int * joint_histogram_buff= new int[nbins*nbins];
    for (int k = 0; k<nbins*nbins; k++) joint_histogram_buff[k] = 0;

    bocl_mem * joint_histogram = new bocl_mem(device_->context(), joint_histogram_buff, sizeof(int)*nbins*nbins, "joint histogram" );
    joint_histogram->create_buffer(CL_MEM_READ_WRITE | CL_MEM_COPY_HOST_PTR );

    float translation_buff[4];
    translation_buff[0] = tx.x();        
    translation_buff[1] = tx.y();        
    translation_buff[2] = tx.z();
    translation_buff[3] = 0.0;

    bocl_mem_sptr translation = new bocl_mem(device_->context(), translation_buff, sizeof(float)*4, " translation " );
    translation->create_buffer(CL_MEM_READ_ONLY | CL_MEM_COPY_HOST_PTR );

    float scale_buff[1];
    scale_buff[0] = s;

    bocl_mem_sptr scale = new bocl_mem(device_->context(), scale_buff, sizeof(float), " scale " );
    scale->create_buffer(CL_MEM_READ_ONLY | CL_MEM_COPY_HOST_PTR );

    float rotation_buff[9];
    vnl_matrix_fixed<double, 3, 3> R = rot.as_matrix();

    rotation_buff[0] = R(0,0);  rotation_buff[3] = R(1,0);  rotation_buff[6] = R(2,0);
    rotation_buff[1] = R(0,1);  rotation_buff[4] = R(1,1);  rotation_buff[7] = R(2,1);
    rotation_buff[2] = R(0,2);  rotation_buff[5] = R(1,2);  rotation_buff[8] = R(2,2);

    bocl_mem_sptr rotation = new bocl_mem(device_->context(), rotation_buff, sizeof(float)*9, " rotation " );
    rotation->create_buffer(CL_MEM_READ_ONLY | CL_MEM_COPY_HOST_PTR );
    bocl_mem_sptr ocl_nbins = new bocl_mem(device_->context(), &(nbins), sizeof(int), "  #of bins" );
    ocl_nbins->create_buffer(CL_MEM_READ_ONLY | CL_MEM_COPY_HOST_PTR );
    bocl_mem_sptr ocl_depth = new bocl_mem(device_->context(), &(depth), sizeof(int), "  depth of octree " );
    ocl_depth->create_buffer(CL_MEM_READ_ONLY | CL_MEM_COPY_HOST_PTR );

    // iterate over scene A on the GPU.
    boxm2_scene_sptr sceneA = cacheA_->get_cpu_cache()->get_scenes()[0];
    vcl_map<boxm2_block_id, boxm2_block_metadata>& blocks_A = sceneA->blocks() ;
    vcl_map<boxm2_block_id, boxm2_block_metadata>::iterator blk_iter_A;
    vcl_size_t local_threads[1]={8};
    vcl_size_t global_threads[1]={1};
    int status=0;
      cl_command_queue queue = clCreateCommandQueue(device_->context(),*(device_->device_id()),CL_QUEUE_PROFILING_ENABLE,&status);


    float gpu_time = 0.0;
    for (blk_iter_A = blocks_A.begin(); blk_iter_A!=blocks_A.end(); blk_iter_A++)
    {
        boxm2_block_metadata mdata = sceneA->get_block_metadata(blk_iter_A->first);

        //write the image values to the buffer
        bocl_mem* blk       = cacheA_->get_block(sceneA, blk_iter_A->first);
        bocl_mem* blk_info  = cacheA_->loaded_block_info();
        bocl_mem* alpha     = cacheA_->get_data<BOXM2_ALPHA>(sceneA, blk_iter_A->first,0,false);
        boxm2_scene_info* info_buffer = (boxm2_scene_info*) blk_info->cpu_buffer();
        int alphaTypeSize = (int)boxm2_data_info::datasize(boxm2_data_traits<BOXM2_ALPHA>::prefix());
        info_buffer->data_buffer_length = (int) (alpha->num_bytes()/alphaTypeSize);
        blk_info->write_to_buffer((queue));
        global_threads[0] = (unsigned) RoundUp(mdata.sub_block_num_.x()*mdata.sub_block_num_.y()*mdata.sub_block_num_.z(),(int)local_threads[0]);
        // Kernel ( blk_info , trees, alpha,  transformation, sceneB_bbox,sceneB_bbox_id, blkoffsets, alphaoffsets )
        kern->set_arg(blk_info);
        kern->set_arg(centerX.ptr());
        kern->set_arg(centerY.ptr());
        kern->set_arg(centerZ.ptr());
        kern->set_arg(lookup.ptr());
        kern->set_arg(blk);
        kern->set_arg(alpha);
        kern->set_arg(sceneB_origin.ptr());
        kern->set_arg(sceneB_bbox_ids.ptr());
        kern->set_arg(sceneB_block_dims.ptr());
        kern->set_arg(sceneB_sub_block_len.ptr());
        kern->set_arg(blks_ocl_B.ptr());
        kern->set_arg(alpha_ocl_B.ptr());
        kern->set_arg(blks_ocl_B_offsets.ptr());
        kern->set_arg(alpha_ocl_B_offsets.ptr());
        kern->set_arg(translation.ptr());
        kern->set_arg(rotation.ptr());
        kern->set_arg(scale.ptr());
        kern->set_arg(ocl_nbins.ptr());
        kern->set_arg(ocl_depth.ptr());
        kern->set_arg(joint_histogram);
        kern->set_arg(output.ptr());
        kern->set_local_arg(nbins*nbins*sizeof(int));
        kern->set_local_arg(16*local_threads[0]*sizeof(unsigned char)); // local trees
        kern->set_local_arg(local_threads[0]*10*sizeof(cl_uchar) );    // cumsum buffer,
        kern->set_local_arg(16*local_threads[0]*sizeof(unsigned char)); // local trees
        if(!kern->execute(queue, 1, local_threads, global_threads))
        {
            vcl_cout<<"Kernel Failed to Execute "<<vcl_endl;
            return false;
        }
        int status = clFinish(queue);
        
        check_val(status, MEM_FAILURE, "UPDATE EXECUTE FAILED: " + error_to_string(status));
        gpu_time += kern->exec_time();
        //clear render kernel args so it can reset em on next execution
        kern->clear_args();
        //output->read_to_buffer(queue);
        joint_histogram->read_to_buffer(queue);
        clFinish(queue);
    }



    float * joint_histogram_float = reinterpret_cast<float * > (joint_histogram_buff);
    vcl_cout<<"Zeroth eleemtn "<<joint_histogram_buff[0]<<vcl_endl;
    float * histA = new float[nbins];
    float * histB = new float[nbins];
    for (int k = 0; k<nbins; k++)
    {
        histA[k] = 0.0;
        histB[k] = 0.0;
    }
    float sum  = 0.0;
    // normalize joint histogram
    for (int k = 0; k < nbins; k++) 
        for (int l = 0; l < nbins; l++)
            sum+=joint_histogram_buff[k*nbins+l];

    if(sum <= 0.0 )
        mi = 0.0; 
    else
    {
        for (int k = 0; k < nbins; k++) {
            for (int l = 0; l < nbins; l++) {
                joint_histogram_float[k*nbins+l] = (float)joint_histogram_buff[k*nbins+l] / sum;
            }
        }
        for (int k = 0; k < nbins; k++) {
            for (int l = 0; l < nbins; l++) {
                histA[k]+=joint_histogram_float[k*nbins+l];
            }
        }
        for (int k = 0; k < nbins; k++) {
            for (int l = 0; l < nbins; l++) {
                histB[k]+=joint_histogram_float[l*nbins+k];
            }
        }

        float entropyA = 0;
        for (int k = 0; k < nbins; k++) 
            entropyA += -(histA[k]>0?histA[k]*vcl_log(histA[k]):0); // if prob=0 this value is defined as 0

        float entropyB = 0;
        for (int l = 0; l < nbins; l++) 
            entropyB += -(histB[l]>0?histB[l]*vcl_log(histB[l]):0); // if prob=0 this value is defined as 0

        float entropyAB =  0.0; ;
        for (int k = 0; k < nbins; k++)
            for (int l = 0; l < nbins; l++)
                entropyAB += -(joint_histogram_float[k*nbins+l]>0?joint_histogram_float[k*nbins+l]*vcl_log(joint_histogram_float[k*nbins+l]):0);
        mi = (entropyA +entropyB - entropyAB)/vnl_math::ln2;
    }
    //clReleaseCommandQueue(queue);

    delete [] joint_histogram_float;
    delete [] histA;
    delete [] histB;
    //delete joint_histogram;

 return true;

 
}

