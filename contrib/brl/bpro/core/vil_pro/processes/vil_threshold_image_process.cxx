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
    vcl_vector<vcl_string> input_types;
    input_types.push_back("vil_image_view_base_sptr");
    input_types.push_back("float");   // threshold
    input_types.push_back("bool");   // whether to threshold above or below, if true thresholds above, i.e. dest(i,j) = true if src(i,j) >= threshold
    ok = pro.set_input_types(input_types);
    if (!ok) return ok;

    //output
    vcl_vector<vcl_string> output_types;
    output_types.push_back("vil_image_view_base_sptr");
    ok = pro.set_output_types(output_types);
    if (!ok) return ok;
    return true;
}

//: Execute the process
bool vil_threshold_image_process(bprb_func_process& pro)
{
    // Sanity check
    if (pro.n_inputs()< 3) {
        vcl_cout << "vil_threshold_image_process: The input number should be 3" << vcl_endl;
        return false;
    }

    unsigned i=0;
    //Retrieve image from input
    vil_image_view_base_sptr image = pro.get_input<vil_image_view_base_sptr>(i++);

    float thres = pro.get_input<float>(i++);
    bool thres_above = pro.get_input<bool>(i++);

    // retrieve float image
    vil_image_view_base_sptr fimage = vil_convert_cast(float(), image);
    vil_image_view<float> fimg = *fimage;

    vil_image_view<bool>* temp = new vil_image_view<bool>;
    if (thres_above) {
        //: Apply threshold such that dest(i,j,p)=true if src(i,j,p)>=t
        vil_threshold_above(fimg, *temp, thres);
    }
    else {
        //: Apply threshold such that dest(i,j,p)=true if src(i,j,p)<=t
        vil_threshold_below(fimg, *temp, thres);
    }
    vil_image_view<unsigned char>* out = new vil_image_view<unsigned char>(temp->ni(),temp->nj());
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

    pro.set_output_val<vil_image_view_base_sptr>(0, out);
    return true;
}



//: Constructor
bool vil_threshold_max_image_process_cons(bprb_func_process& pro)
{
    //input
    bool ok=false;
    vcl_vector<vcl_string> input_types;
    input_types.push_back("vil_image_view_base_sptr");
    input_types.push_back("float");   // fmax or threshold
    ok = pro.set_input_types(input_types);
    if (!ok) return ok;

    //output
    vcl_vector<vcl_string> output_types;
    output_types.push_back("vil_image_view_base_sptr");
    ok = pro.set_output_types(output_types);
    if (!ok) return ok;
    return true;
}

//: Execute the process
bool vil_threshold_max_image_process(bprb_func_process& pro)
{
    // Sanity check
    if (pro.n_inputs()< 2) {
        vcl_cout << "vil_threshold_image_process: The input number should be 3" << vcl_endl;
        return false;
    }

    unsigned i=0;
    //Retrieve image from input
    vil_image_view_base_sptr image = pro.get_input<vil_image_view_base_sptr>(i++);
    float thres = pro.get_input<float>(i++);

    // retrieve float image
    vil_image_view_base_sptr fimage = vil_convert_cast(float(), image);
    vil_image_view<float> fimg = *fimage;

    float fmin = 0.0f; 
    float fmax = 0.0f;


    vil_math_value_range<float>(fimg, fmin,fmax);

    vil_image_view<unsigned char>* out = new vil_image_view<unsigned char>(fimage->ni(),fimage->nj());
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


