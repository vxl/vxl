// This is brl/bseg/boxm2/cpp/pro/processes/boxm2_export_stack_images_process.cxx
#include <bprb/bprb_func_process.h>
//:
// \file
// \brief  A process for exporting a scene as a stack of  images
//
// \author Vishal Jain
// \date Mar 28, 2012

#include <vcl_fstream.h>
#include <vcl_iomanip.h>
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
    const unsigned n_inputs_ = 4;
    const unsigned n_outputs_ = 0;
}

bool boxm2_export_stack_images_process_cons(bprb_func_process& pro)
{
    using namespace boxm2_export_stack_images_process_globals;

    //process takes 3 inputs, no outputs
    int i=0;
    vcl_vector<vcl_string> input_types_(n_inputs_);
    input_types_[i++] = "boxm2_scene_sptr";
    input_types_[i++] = "boxm2_cache_sptr";
    input_types_[i++] = "vcl_string";                //output dir of saved DICOM images
    input_types_[i++] = "bool";

    brdb_value_sptr out_app = new brdb_value_t<bool>(true);
    pro.set_input(3, out_app);

    vcl_vector<vcl_string> output_types_(n_outputs_);
    return pro.set_input_types(input_types_) && pro.set_output_types(output_types_);
}

bool boxm2_export_stack_images_process(bprb_func_process& pro)
{
    using namespace boxm2_export_stack_images_process_globals;
    if ( pro.n_inputs() < n_inputs_ ) {
        vcl_cout << pro.name() << ": The input number should be " << n_inputs_<< vcl_endl;
        return false;
    }

    //get the inputs
    unsigned argIdx = 0;
    boxm2_scene_sptr scene = pro.get_input<boxm2_scene_sptr>(argIdx++);
    boxm2_cache_sptr cache = pro.get_input<boxm2_cache_sptr>(argIdx++);
    vcl_string outdir = pro.get_input<vcl_string>(argIdx++);
    bool opacity_only = pro.get_input<bool>(argIdx++);
    //create the mesh directory
    if (outdir != "") {
        if (!vul_file::make_directory_path(outdir.c_str())) {
            vcl_cout<<"Couldn't make directory path "<<outdir<<vcl_endl;
            return false;
        }
    }

    if (opacity_only)
        boxm2_export_stack_images_function::export_opacity_stack_images(scene,cache,outdir);
    else
    {
        vil3d_image_view<unsigned char> img3d;
        if (scene->has_data_type("boxm2_gauss_rgb"))
            boxm2_export_stack_images_function::export_color_stack_images(scene,cache,img3d);
        else
            boxm2_export_stack_images_function::export_greyscale_stack_images(scene,cache,img3d);



        for (unsigned k = 0 ;  k < img3d.nk() ;  k ++)
        {
            vil_image_view<unsigned char> img = vil3d_slice_ji(img3d,k);
            vcl_stringstream ss;
            ss<<outdir<<"/img_"<<vcl_setw(5) << vcl_setfill('0') << k<<".bmp";
            vcl_cout<<"Filename : "<<ss.str()<<vcl_endl;
            vil_save(img,ss.str().c_str());
        }
    }
    return true;
}
