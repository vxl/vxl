// This is brl/bseg/boxm2/cpp/pro/processes/boxm2_export_stack_images_process.cxx
#include <iostream>
#include <fstream>
#include <iomanip>
#include <bprb/bprb_func_process.h>
//:
// \file
// \brief  A process for exporting a scene as a stack of  images
//
// \author Vishal Jain
// \date Mar 28, 2012

#ifdef _MSC_VER
#  include <vcl_msvc_warnings.h>
#endif
#include <vul/vul_file.h>
#include <boxm2/boxm2_scene.h>
#include <boxm2/io/boxm2_cache.h>

//vil includes
#include <vil/vil_image_view.h>
#include <vil/vil_save.h>
#include <vil3d/vil3d_image_view.h>
#include <vil3d/vil3d_slice.h>
#include <vil3d/vil3d_save.h>

#include <boxm2/cpp/algo/boxm2_export_stack_images_function.h>


namespace boxm2_export_stack_images_process_globals
{
    constexpr unsigned n_inputs_ = 4;
    constexpr unsigned n_outputs_ = 0;
}

bool boxm2_export_stack_images_process_cons(bprb_func_process& pro)
{
    using namespace boxm2_export_stack_images_process_globals;

    //process takes 3 inputs, no outputs
    int i=0;
    std::vector<std::string> input_types_(n_inputs_);
    input_types_[i++] = "boxm2_scene_sptr";
    input_types_[i++] = "boxm2_cache_sptr";
    input_types_[i++] = "vcl_string";
    input_types_[i++] = "vcl_string";

    brdb_value_sptr out_app = new brdb_value_t<bool>(true);
    pro.set_input(3, out_app);

    std::vector<std::string> output_types_(n_outputs_);
    return pro.set_input_types(input_types_) && pro.set_output_types(output_types_);
}

bool boxm2_export_stack_images_process(bprb_func_process& pro)
{
    using namespace boxm2_export_stack_images_process_globals;
    if ( pro.n_inputs() < n_inputs_ ) {
        std::cout << pro.name() << ": The input number should be " << n_inputs_<< std::endl;
        return false;
    }

    //get the inputs
    unsigned argIdx = 0;
    boxm2_scene_sptr scene = pro.get_input<boxm2_scene_sptr>(argIdx++);
    boxm2_cache_sptr cache = pro.get_input<boxm2_cache_sptr>(argIdx++);
    std::string outdir = pro.get_input<std::string>(argIdx++);
    std::string datatype = pro.get_input<std::string>(argIdx++);
    //create the stack directory
    if (outdir != "") {
        if (!vul_file::make_directory_path(outdir.c_str())) {
            std::cout<<"Couldn't make directory path "<<outdir<<std::endl;
            return false;
        }
    }
    //boxm2_export_stack_images_function::export_opacity_stack_images(scene,cache,outdir);


    if (datatype == "boxm2_gauss_rgb" && scene->has_data_type("boxm2_gauss_rgb"))
    {
        vil3d_image_view<unsigned char> img3d;
        boxm2_export_stack_images_function::export_color_stack_images(scene, cache, img3d);
        for (unsigned k = 0; k < img3d.nk(); k++)
        {
            vil_image_view<float> img = vil3d_slice_ji(img3d, k);
            std::stringstream ss;
            ss << outdir << "/" << datatype << "_" << std::setw(5) << std::setfill('0') << k << ".png";
            std::cout << "Filename : " << ss.str() << std::endl;
            vil_save(img, ss.str().c_str());
        }
    }
    else if (datatype == "boxm2_mog3_grey" && scene->has_data_type("boxm2_mog3_grey"))
    {
        vil3d_image_view<unsigned char> img3d;
        boxm2_export_stack_images_function::export_greyscale_stack_images(scene, cache, img3d);
        for (unsigned k = 0; k < img3d.nk(); k++)
        {
            vil_image_view<float> img = vil3d_slice_ji(img3d, k);
            std::stringstream ss;
            ss << outdir << "/" << datatype << "_" << std::setw(5) << std::setfill('0') << k << ".png";
            std::cout << "Filename : " << ss.str() << std::endl;
            vil_save(img, ss.str().c_str());
        }
    }

    else if (datatype.find("aux") != -1)
    {
        vil3d_image_view<float> img3d;
        boxm2_export_stack_images_function::export_float_images(scene, cache, datatype, img3d);
        for (unsigned k = 0; k < img3d.nk(); k++)
        {
            vil_image_view<float> img = vil3d_slice_ji(img3d, k);
            std::stringstream ss;
            ss << outdir << "/" << datatype << "_" << std::setw(5) << std::setfill('0') << k << ".tif";
            std::cout<<"Filename : "<<ss.str()<<std::endl;
            vil_save(img, ss.str().c_str());
        }
    }
    else if (datatype.find("aux1") != -1)
    {
        vil3d_image_view<float> img3d;
        boxm2_export_stack_images_function::export_float_images(scene, cache, datatype, img3d);
        for (unsigned k = 0; k < img3d.nk(); k++)
        {
            vil_image_view<float> img = vil3d_slice_ji(img3d, k);
            std::stringstream ss;
            ss << outdir << "/" << datatype << "_" << std::setw(5) << std::setfill('0') << k << ".tif";
            std::cout << "Filename : " << ss.str() << std::endl;
            vil_save(img, ss.str().c_str());
        }
    }
    else
        return false;

    return true;
}
