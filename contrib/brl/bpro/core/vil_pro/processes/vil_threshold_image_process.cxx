// This is brl/bpro/core/vil_pro/processes/vil_threshold_image_process.cxx
#include <bprb/bprb_func_process.h>
//:
// \file

#include <bprb/bprb_parameters.h>
#include <vil/vil_image_view.h>
#include <vil/vil_convert.h>
#include <vil/algo/vil_threshold.h>

//: Constructor
bool vil_threshold_image_process_cons(bprb_func_process& pro)
{
    //input
    bool ok=false;
    std::vector<std::string> input_types;
    input_types.emplace_back("vil_image_view_base_sptr");
    input_types.emplace_back("float");     // threshold
    input_types.emplace_back("bool");      // whether to threshold above or below, if true thresholds above, i.e. dest(i,j) = true if src(i,j) >= threshold
    input_types.emplace_back("unsigned");  // the desired value of positive pixels in the output image
    ok = pro.set_input_types(input_types);
    if (!ok) return ok;

    //output
    std::vector<std::string> output_types;
    output_types.emplace_back("vil_image_view_base_sptr");
    ok = pro.set_output_types(output_types);
    if (!ok) return ok;
    return true;
}

//: Execute the process
bool vil_threshold_image_process(bprb_func_process& pro)
{
    // Sanity check
    if (pro.n_inputs()< 3) {
        std::cout << "vil_threshold_image_process: The input number should be 3" << std::endl;
        return false;
    }

    unsigned i=0;
    //Retrieve image from input
    vil_image_view_base_sptr image = pro.get_input<vil_image_view_base_sptr>(i++);

    auto thres = pro.get_input<float>(i++);
    bool thres_above = pro.get_input<bool>(i++);
    auto positive_id = pro.get_input<unsigned>(i++);  // if passed as zero, return the bool image!

    // retrieve float image
    vil_image_view_base_sptr fimage = vil_convert_cast(float(), image);
    vil_image_view<float> fimg = *fimage;

    auto* temp = new vil_image_view<bool>;
    if (thres_above) {
        //: Apply threshold such that dest(i,j,p)=true if src(i,j,p)>=t
        vil_threshold_above(fimg, *temp, thres);
    }
    else {
        //: Apply threshold such that dest(i,j,p)=true if src(i,j,p)<=t
        vil_threshold_below(fimg, *temp, thres);
    }
    if (!positive_id) {
        pro.set_output_val<vil_image_view_base_sptr>(0, temp);
        return true;
    }
    auto* out = new vil_image_view<unsigned char>(temp->ni(),temp->nj());
    for (unsigned k = 0 ; k < out->ni(); k++)
    {
        for (unsigned l = 0 ; l < out->nj(); l++)
        {
            if ((*temp)(k,l) )
                (*out)(k,l) =(unsigned char)(positive_id);
            else
                (*out)(k,l) =0;
        }
    }

    pro.set_output_val<vil_image_view_base_sptr>(0, out);
    return true;
}



//: Constructor
bool vil_threshold_max_image_process_cons(bprb_func_process& pro)
{
    //input
    bool ok=false;
    std::vector<std::string> input_types;
    input_types.emplace_back("vil_image_view_base_sptr");
    input_types.emplace_back("float");   // fmax or threshold
    ok = pro.set_input_types(input_types);
    if (!ok) return ok;

    //output
    std::vector<std::string> output_types;
    output_types.emplace_back("vil_image_view_base_sptr");
    ok = pro.set_output_types(output_types);
    if (!ok) return ok;
    return true;
}

//: Execute the process
bool vil_threshold_max_image_process(bprb_func_process& pro)
{
    // Sanity check
    if (pro.n_inputs()< 2) {
        std::cout << "vil_threshold_image_process: The input number should be 3" << std::endl;
        return false;
    }

    unsigned i=0;
    //Retrieve image from input
    vil_image_view_base_sptr image = pro.get_input<vil_image_view_base_sptr>(i++);
    auto thres = pro.get_input<float>(i++);

    // retrieve float image
    vil_image_view_base_sptr fimage = vil_convert_cast(float(), image);
    vil_image_view<float> fimg = *fimage;

    float fmin = 0.0f;
    float fmax = 0.0f;


    vil_math_value_range<float>(fimg, fmin,fmax);

    auto* out = new vil_image_view<unsigned char>(fimage->ni(),fimage->nj());
    out->fill(255);

    if(fmax > fmin)
    {

        for (unsigned k = 0 ; k < out->ni(); k++)
            for (unsigned l = 0 ; l < out->nj(); l++)
                if (fimg(k,l) > 0.9*fmax || fimg(k,l) > thres    )
                    (*out)(k,l) = 0;
    }

    pro.set_output_val<vil_image_view_base_sptr>(0, out);
    return true;
}



//: threshold an image such that dest(i,j,p)=true if min_thres<=src(i,j,p)<=max_thres if inside is true or dest(i,j,p)=true if src(i,j,p)<=min_thres or src(i,j,p)>=t1
bool vil_threshold_image_region_process_cons(bprb_func_process& pro)
{
    // input
    bool ok=false;
    std::vector<std::string> input_types(4);
    input_types[0] = "vil_image_view_base_sptr";  // input image
    input_types[1] = "float";                     // min threshold
    input_types[2] = "float";                     // max threshold
    input_types[3] = "bool";                      // whether to threshold inside or outside, if true threshold inside, i.e. dest(i,j,p)=true if t0<=src(i,j,p)<=t1
    ok = pro.set_input_types(input_types);
    if (!ok) return ok;

    // output
    std::vector<std::string> output_types(1);
    output_types[0] = "vil_image_view_base_sptr";
    ok = pro.set_output_types(output_types);
    if (!ok) return ok;
    return true;
}

//: execute the process
bool vil_threshold_image_region_process(bprb_func_process& pro)
{
    // sanity check
    if (!pro.verify_inputs())
      return false;

    // get input
    unsigned i = 0;
    vil_image_view_base_sptr image = pro.get_input<vil_image_view_base_sptr>(i++);
    auto min_thres = pro.get_input<float>(i++);
    auto max_thres = pro.get_input<float>(i++);
    bool thres_inside = pro.get_input<bool>(i++);

    // retrieve float image
    vil_image_view_base_sptr fimage = vil_convert_cast(float(), image);
    vil_image_view<float> fimg = *fimage;

    auto* temp = new vil_image_view<bool>;
    if (thres_inside) {
      // apply thresholds such that dest(i,j,p)=true if t0<=src(i,j,p)<=t1
      vil_threshold_inside(fimg, *temp, min_thres, max_thres);
    }
    else {
      // apply thresholds such that dest(i,j,p)=true if src(i,j,p)<=t0 or src(i,j,p)>=t1
      vil_threshold_outside(fimg, *temp, min_thres, max_thres);
    }

    auto* out = new vil_image_view<unsigned char>(temp->ni(), temp->nj());
    for (unsigned k = 0 ; k < out->ni(); k++)
    {
        for (unsigned l = 0 ; l < out->nj(); l++)
        {
            if ((*temp)(k,l) )
                (*out)(k,l) =255;
            else
                (*out)(k,l) =0;
        }
    }

    // output
    pro.set_output_val<vil_image_view_base_sptr>(0, out);
    return true;
}
