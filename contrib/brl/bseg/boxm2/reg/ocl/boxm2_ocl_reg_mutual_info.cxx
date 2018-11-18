#include "boxm2_ocl_reg_mutual_info.h"
#include <algorithm>
#include <iostream>
#include <utility>
//:
// \file
#include <vgl/vgl_box_3d.h>
#include <vgl/vgl_vector_3d.h>
#include <vgl/algo/vgl_rotation_3d.h>
#include <vgl/vgl_intersection.h>

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
#ifdef _MSC_VER
#  include <vcl_msvc_warnings.h>
#endif

typedef vnl_vector_fixed<unsigned char,16> uchar16;



boxm2_ocl_reg_mutual_info::boxm2_ocl_reg_mutual_info(boxm2_opencl_cache_sptr& opencl_cache,
                                                     boxm2_scene_sptr& sceneA,
                                                     boxm2_scene_sptr& sceneB,
                                                     const bocl_device_sptr& device,
                                                     int nbins,
                                                     bool do_vary_scale)
                                                     : nbins_(nbins), opencl_cache_(opencl_cache),device_(device), do_vary_scale_(do_vary_scale)
{
    this->compile_kernel();
    this->init_ocl_minfo();
    sceneA_ =  sceneA;
    sceneB_ =  sceneB ;
}

boxm2_ocl_reg_mutual_info::~boxm2_ocl_reg_mutual_info()
{
    delete kern;
    opencl_cache_->clear_cache();
}

double boxm2_ocl_reg_mutual_info:: mutual_info(vgl_rotation_3d<double> rot,vgl_vector_3d<double> trans, double scale, int depth)
{
    float val = 0.0f;
    this->boxm2_ocl_register_world(std::move(rot),trans,scale,nbins_,depth,val);
    return val;
}
double boxm2_ocl_reg_mutual_info:: mutual_info(vnl_vector<double> const& x,  int depth )
{
    vgl_vector_3d<double>   tx(x[0],x[1],x[2]);
    vgl_rotation_3d<double> r (vnl_vector_fixed<double,3>(x[3],x[4],x[5]));
    double scale = x[6];
    return this->mutual_info(r,tx,scale,depth);

}


bool boxm2_ocl_reg_mutual_info::compile_kernel()
{
    std::vector<std::string> src_paths;
    std::string source_dir = std::string(VCL_SOURCE_ROOT_DIR) + "/contrib/brl/bseg/boxm2/ocl/cl/";
    std::string reg_source_dir = std::string(VCL_SOURCE_ROOT_DIR)+ "/contrib/brl/bseg/boxm2/reg/ocl/cl/";
    src_paths.push_back(source_dir     + "scene_info.cl");
    src_paths.push_back(source_dir     + "bit/bit_tree_library_functions.cl");
    src_paths.push_back(reg_source_dir + "estimate_mi_blockwise_vol.cl");
    this->kern = new bocl_kernel();
    return kern->create_kernel(&device_->context(),device_->device_id(), src_paths, "estimate_mi_blockwise_vol", "", "MI");
}

bool boxm2_ocl_reg_mutual_info::init_ocl_minfo()
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
    return true;
}

bool boxm2_ocl_reg_mutual_info::boxm2_ocl_register_world(const vgl_rotation_3d<double>& rot,
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

    std::vector<boxm2_block_id> blocks_A = sceneA_->get_block_ids();
    std::vector<boxm2_block_id> blocks_B = sceneB_->get_block_ids();
    auto iter_blks_A = blocks_A.begin();
    auto iter_blks_B = blocks_B.begin();
    std::size_t local_threads[1]={64};
    std::size_t global_threads[1]={1};
    int status=0;    float gpu_time = 0.0;
    //std::cout<<"Cache size "<<opencl_cache_->bytes_in_cache()/1024/1024<<" MB"<<std::endl;
    for (;iter_blks_A!=blocks_A.end(); iter_blks_A++)
    {
        //write the image values to the buffer
        bocl_mem* blk_A       = opencl_cache_->get_block(sceneA_, *iter_blks_A);
        bocl_mem* alpha_A     = opencl_cache_->get_data<BOXM2_ALPHA>(sceneA_, *iter_blks_A,0,false);
        boxm2_scene_info* info_buffer = sceneA_->get_blk_metadata(*iter_blks_A);
        int alphaTypeSize = (int)boxm2_data_info::datasize(boxm2_data_traits<BOXM2_ALPHA>::prefix());
        // check for invalid parameters
        if( alphaTypeSize == 0 ) //This should never happen, it will result in division by zero later
        {
            std::cout << "ERROR: alphaTypeSize == 0 in " << __FILE__ << __LINE__ << std::endl;
            return false;
        }

        info_buffer->data_buffer_length = (int) (alpha_A->num_bytes()/alphaTypeSize);
        bocl_mem* blk_info_A  = new bocl_mem(device_->context(), info_buffer, sizeof(boxm2_scene_info), " Scene Info" );
        blk_info_A->create_buffer(CL_MEM_READ_ONLY | CL_MEM_COPY_HOST_PTR);

        vgl_box_3d<float> box_A(info_buffer->scene_origin[0],info_buffer->scene_origin[1],info_buffer->scene_origin[2],
                                info_buffer->scene_origin[0]+info_buffer->scene_dims[0]*info_buffer->block_len,
                                info_buffer->scene_origin[1]+info_buffer->scene_dims[1]*info_buffer->block_len,
                                info_buffer->scene_origin[2]+info_buffer->scene_dims[2]*info_buffer->block_len);

        vgl_box_3d<float> box_A_xformed;
        for(unsigned int k = 0 ; k<box_A.vertices().size(); k++)
        {
            vgl_point_3d<float> p = box_A.vertices()[k];
            vgl_point_3d<float> px(s*(p.x()*R(0,0)+p.y()*R(0,1)+p.z()*R(0,2) + tx.x()),
                                   s*(p.x()*R(1,0)+p.y()*R(1,1)+p.z()*R(1,2) + tx.y()),
                                   s*(p.x()*R(2,0)+p.y()*R(2,1)+p.z()*R(2,2) + tx.z()));
            box_A_xformed.add(px);
        }

        global_threads[0] = (unsigned) RoundUp(info_buffer->scene_dims[0]*info_buffer->scene_dims[1]*info_buffer->scene_dims[2],(int)local_threads[0]);
        for (iter_blks_B = blocks_B.begin();iter_blks_B!=blocks_B.end(); iter_blks_B++)
        {
            boxm2_scene_info* info_buffer_B = sceneB_->get_blk_metadata(*iter_blks_B);
            vgl_box_3d<float> box_B(info_buffer_B->scene_origin[0],info_buffer_B->scene_origin[1],info_buffer_B->scene_origin[2],
                                    info_buffer_B->scene_origin[0]+info_buffer_B->scene_dims[0]*info_buffer_B->block_len,
                                    info_buffer_B->scene_origin[1]+info_buffer_B->scene_dims[1]*info_buffer_B->block_len,
                                    info_buffer_B->scene_origin[2]+info_buffer_B->scene_dims[2]*info_buffer_B->block_len);
            if(!vgl_intersection<float>(box_A_xformed,box_B).is_empty())
            {
               bocl_mem* blk_B       = opencl_cache_->get_block(sceneB_, *iter_blks_B);
               bocl_mem* alpha_B     = opencl_cache_->get_data<BOXM2_ALPHA>(sceneB_, *iter_blks_B,0,false);
               int alphaTypeSize = (int)boxm2_data_info::datasize(boxm2_data_traits<BOXM2_ALPHA>::prefix());
               // check for invalid parameters
               if( alphaTypeSize == 0 ) //This should never happen, it will result in division by zero later
               {
                   std::cout << "ERROR: alphaTypeSize == 0 in " << __FILE__ << __LINE__ << std::endl;
                   return false;
               }

               info_buffer_B->data_buffer_length = (int) (alpha_B->num_bytes()/alphaTypeSize);

               bocl_mem* blk_info_B  = new bocl_mem(device_->context(), info_buffer_B, sizeof(boxm2_scene_info), " Scene Info" );
               blk_info_B->create_buffer(CL_MEM_READ_ONLY | CL_MEM_COPY_HOST_PTR);

               kern->set_arg(centerX.ptr());
               kern->set_arg(centerY.ptr());
               kern->set_arg(centerZ.ptr());
               kern->set_arg(lookup.ptr());
               kern->set_arg(blk_info_A);
               kern->set_arg(blk_info_B);
               kern->set_arg(blk_A);
               kern->set_arg(alpha_A);
               kern->set_arg(blk_B);
               kern->set_arg(alpha_B);
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
               kern->set_local_arg(16*local_threads[0]*sizeof(unsigned char)); // local trees
               if(!kern->execute(queue, 1, local_threads, global_threads))
               {
                   std::cout<<"Kernel Failed to Execute "<<std::endl;
                   return false;
               }
               int status = clFinish(queue);
               check_val(status, MEM_FAILURE, "MIFO EXECUTE FAILED: " + error_to_string(status));
               gpu_time += kern->exec_time();
               //clear render kernel args so it can reset em on next execution
               kern->clear_args();
               clFinish(queue);
               blk_info_B->release_memory();
               delete info_buffer_B;
            }
        }
        blk_info_A->release_memory();
        delete info_buffer;
    }
    joint_histogram->read_to_buffer(queue);
    clFinish(queue);
    auto * joint_histogram_float = reinterpret_cast<float * > (joint_histogram_buff);

    auto * histA = new float[nbins];
    auto * histB = new float[nbins];
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
            entropyA += -(histA[k]>0?histA[k]*std::log(histA[k]):0); // if prob=0 this value is defined as 0

        float entropyB = 0;
        for (int l = 0; l < nbins; l++)
            entropyB += -(histB[l]>0?histB[l]*std::log(histB[l]):0); // if prob=0 this value is defined as 0

        float entropyAB =  0.0; ;
        for (int k = 0; k < nbins; k++)
            for (int l = 0; l < nbins; l++)
                entropyAB += -(joint_histogram_float[k*nbins+l]>0?joint_histogram_float[k*nbins+l]*std::log(joint_histogram_float[k*nbins+l]):0);
        mi = (entropyA +entropyB - entropyAB)/vnl_math::ln2;
    }

    opencl_cache_->unref_mem(joint_histogram);
    opencl_cache_->unref_mem(translation.ptr());
    opencl_cache_->unref_mem(rotation.ptr());
    opencl_cache_->unref_mem(scale.ptr());
    opencl_cache_->unref_mem(ocl_nbins.ptr());
    opencl_cache_->unref_mem(ocl_depth.ptr());
    delete [] joint_histogram_float;
    delete [] histA;
    delete [] histB;
    //delete joint_histogram;
    //clReleaseCommandQueue(queue);
 return true;


}
