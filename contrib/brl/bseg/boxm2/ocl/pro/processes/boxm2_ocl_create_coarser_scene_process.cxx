// This is brl/bseg/boxm2/ocl/pro/processes/boxm2_ocl_create_coarser_scene_process.cxx
#include <iostream>
#include <fstream>
#include <bprb/bprb_func_process.h>
//:
// \file
// \brief  A process for updating parents alpha by the max prob of the children
//
// \author Vishal Jain
// \date Apr 23, 2013

#ifdef _MSC_VER
#  include <vcl_msvc_warnings.h>
#endif
#include <boxm2/ocl/boxm2_opencl_cache.h>
#include <boxm2/boxm2_scene.h>
#include <boxm2/boxm2_block.h>
#include <boxm2/boxm2_data_base.h>
#include <boxm2/ocl/boxm2_ocl_util.h>
//brdb stuff
#include <brdb/brdb_value.h>

//directory utility
#include <vul/vul_timer.h>
#include <vcl_where_root_dir.h>
#include <bocl/bocl_device.h>
#include <bocl/bocl_kernel.h>
#include <boct/boct_bit_tree.h>


namespace boxm2_ocl_create_coarser_scene_process_globals
{
    constexpr unsigned n_inputs_ = 4;
    constexpr unsigned n_outputs_ = 0;
    void compile_kernel(const bocl_device_sptr& device, bocl_kernel* merge_kernel)
    {
        //gather all render sources... seems like a lot for rendering...
        std::vector<std::string> src_paths;
        std::string source_dir = boxm2_ocl_util::ocl_src_root();
        src_paths.push_back(source_dir + "scene_info.cl");
        src_paths.push_back(source_dir + "bit/bit_tree_library_functions.cl");
        src_paths.push_back(source_dir + "bit/update_parents_alpha.cl");

        merge_kernel->create_kernel( &device->context(),
            device->device_id(),
            src_paths,
            "update_parents_alpha",
            "",
            "boxm2 opencl update parents alpha"); //kernel identifier (for error checking)
    }

    //map of compiled kernels, organized by data type
    static std::map<std::string,bocl_kernel* > kernels;
bool copy_fine_to_coarse(boxm2_block & blk,
                         boxm2_block_metadata & mdata,
                         boxm2_data_base * alpha_base ,
                         const boxm2_cache_sptr& cache)
{
    // assuming exactly one scene in cache!
    std::vector<boxm2_scene_sptr> scenes = cache->get_scenes();
    if (scenes.size() == 0) {
      std::cerr << "Error: boxm2_ocl_create_coarser: no scenes in cache! " << std::endl;
      return false;
    }
    if (scenes.size() > 1) {
      std::cerr << "Warning: boxm2_ocl_create_coarser: multiple scenes in cache - using the first." << std::endl;
    }
    boxm2_scene_sptr scene = scenes[0];
    vgl_box_3d<int> bbox = scene->bounding_box_blk_ids();


    boxm2_data<BOXM2_ALPHA> *alpha_data=new boxm2_data<BOXM2_ALPHA>(alpha_base->data_buffer(),
                                                                    alpha_base->buffer_length(),
                                                                    alpha_base->block_id());



    boxm2_block * blksB = cache->get_block(scene,mdata.id_);
    boxm2_data_base * alpha_buffer_B = cache->get_data_base(scene,mdata.id_,boxm2_data_traits<BOXM2_ALPHA>::prefix());
    boxm2_data<BOXM2_ALPHA> *alpha_data_B=new boxm2_data<BOXM2_ALPHA>(alpha_buffer_B->data_buffer(),
                                                                      alpha_buffer_B->buffer_length(),
                                                                      alpha_buffer_B->block_id());

    typedef vnl_vector_fixed<unsigned char, 16> uchar16;
    const boxm2_array_3d<uchar16>&  trees = blk.trees();  //trees to refine
    unsigned ni = trees.get_row1_count();
    unsigned nj = trees.get_row2_count();
    unsigned nk = trees.get_row3_count();

    vgl_point_3d<double> global_center = mdata.local_origin_;
    vgl_vector_3d<double> sub_blk_dims = mdata.sub_block_dim_ ;

    for (unsigned i = 0; i < ni; i++)
    {
        for (unsigned j = 0; j < nj; j++)
        {
            for (unsigned k = 0; k < nk; k++)
            {
                uchar16 tree = trees(i,j,k);
                boct_bit_tree curr_tree( (unsigned char*) tree.data_block(), 4);
                for (int ti=0; ti<585; ti++)
                {
                    //if current bit is 0 and parent bit is 1, you're at a leaf
                    int pi = (ti-1)>>3;                                 //Bit_index of parent bit
                    bool validParent = curr_tree.bit_at(pi) || (ti==0); // special case for root
                    if (validParent && curr_tree.bit_at(ti) == 0 )
                    {
                        int depth = curr_tree.depth_at(ti);
                        float side_len = 1.0f/(float) (1<<depth) *sub_blk_dims.x() ;
                        if(depth == 2)
                        std::cout<<depth<<std::endl;
                        int index = curr_tree.get_data_index(ti);
                        vgl_point_3d<double> cc = curr_tree.cell_center(ti);
                        vgl_point_3d<double> pt = mdata.local_origin_ + vgl_vector_3d<double>(((double)i+cc.x())*sub_blk_dims.x(),
                                                                                              ((double)j+cc.y())*sub_blk_dims.y(),
                                                                                              ((double)k+cc.z())*sub_blk_dims.z());
                        vgl_point_3d<double> local;
                        boxm2_block_id id;
                        if (scene->contains(pt, id, local) && id == mdata.id_)
                        {
                            boxm2_block_metadata mdata = scene->get_block_metadata_const(id);
                            int index_x=(int)std::floor(local.x());
                            int index_y=(int)std::floor(local.y());
                            int index_z=(int)std::floor(local.z());

                            boct_bit_tree tree(blksB->trees_copy()(index_x,index_y,index_z).data_block()) ;
                            int bit_index=tree.traverse_to_level(local,1);
                            int depth=tree.depth_at(bit_index);
                            int data_offset=tree.get_data_index(bit_index,false);

                            float alpha_B = alpha_data_B->data()[data_offset];
                            auto side_len_B=static_cast<float>(mdata.sub_block_dim_.x()/((float)(1<<depth)));
                            alpha_data->data()[index]=alpha_B*side_len_B/side_len;
                        }
                    }
                }
            }
        }
    }
    return true;
}
}

bool boxm2_ocl_create_coarser_scene_process_cons(bprb_func_process& pro)
{
    using namespace boxm2_ocl_create_coarser_scene_process_globals;

    //process takes 1 input
    std::vector<std::string> input_types_(n_inputs_);
    input_types_[0] = "bocl_device_sptr";
    input_types_[1] = "boxm2_scene_sptr";
    input_types_[2] = "boxm2_opencl_cache_sptr";
    input_types_[3] = "vcl_string";             // directory for the coarser model
    // process has 1 output:
    // output[0]: scene sptr
    std::vector<std::string>  output_types_(n_outputs_);
    return pro.set_input_types(input_types_) && pro.set_output_types(output_types_);
}

bool boxm2_ocl_create_coarser_scene_process(bprb_func_process& pro)
{
    using namespace boxm2_ocl_create_coarser_scene_process_globals;
    if ( pro.n_inputs() < n_inputs_ ) {
        std::cout << pro.name() << ": The input number should be " << n_inputs_<< std::endl;
        return false;
    }
    float transfer_time=0.0f;
    float gpu_time=0.0f;
    //get the inputs
    unsigned i = 0;
    bocl_device_sptr device= pro.get_input<bocl_device_sptr>(i++);
    boxm2_scene_sptr scene =pro.get_input<boxm2_scene_sptr>(i++);
    boxm2_opencl_cache_sptr opencl_cache= pro.get_input<boxm2_opencl_cache_sptr>(i++);
    std::string coarse_model_dir = pro.get_input<std::string>(i++);

    if(!vul_file::make_directory_path(coarse_model_dir.c_str()))
    {
        std::cout<<"Invalid Path for the Coarse Model"<<std::endl;
        return false;
    }

    boxm2_scene_sptr coarse_scene = new boxm2_scene(coarse_model_dir,  scene->local_origin() );
    coarse_scene->set_xml_path(coarse_model_dir+"/scene.xml");

    std::vector<boxm2_block_id> scene_blks = scene->get_block_ids();
    auto iter = scene_blks.begin();
    for(; iter!=scene_blks.end(); iter++)
    {
        boxm2_block_metadata mdata = scene->get_block_metadata_const(*iter);
        //: fix the parameteres of the mdata;
        vgl_vector_3d<unsigned> coarse_blk_num (std::ceil(mdata.sub_block_num_.x()/8.0f),
                                                std::ceil(mdata.sub_block_num_.y()/8.0f),
                                                std::ceil(mdata.sub_block_num_.z()/8.0f));
        vgl_vector_3d<double> coarse_blk_dim(mdata.sub_block_dim_.x()*(double) mdata.sub_block_num_.x() / (double) coarse_blk_num.x(),
                                             mdata.sub_block_dim_.y()*(double) mdata.sub_block_num_.y() / (double) coarse_blk_num.y(),
                                             mdata.sub_block_dim_.z()*(double) mdata.sub_block_num_.z() / (double) coarse_blk_num.z() );
        mdata.sub_block_num_ = coarse_blk_num ;
        mdata.sub_block_dim_ = coarse_blk_dim ;
        mdata.init_level_ = 4;
        //: Add a block to the scene.
        coarse_scene->add_block_metadata(mdata);
    }

    coarse_scene->save_scene();
    std::vector<boxm2_block_id> coarse_scene_blks = coarse_scene->get_block_ids();
    auto coarse_iter = coarse_scene_blks.begin();
    for(; coarse_iter!=coarse_scene_blks.end(); coarse_iter++)
    {
        boxm2_block_metadata mdata = coarse_scene->get_block_metadata(*coarse_iter);
        boxm2_block blk(mdata);
        boxm2_sio_mgr::save_block(coarse_scene->data_path()+"/",&blk);

        //: create Alpha Buffer and fill the probabiltiy values;
        boxm2_data_base alpha_buff(mdata,boxm2_data_traits<BOXM2_ALPHA>::prefix(),false);
        copy_fine_to_coarse(blk,mdata,&alpha_buff,opencl_cache->get_cpu_cache());
        boxm2_sio_mgr::save_block_data_base(coarse_scene->data_path()+"/",(*coarse_iter),&alpha_buff,boxm2_data_traits<BOXM2_ALPHA>::prefix());
    }
    std::cout<<"Update Parents Alpha: "<<gpu_time<<std::endl;
    return true;
}
