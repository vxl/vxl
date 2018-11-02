// This is brl/bseg/boxm2/pro/processes/boxm2_mask_sift_features_process.cxx
#include <fstream>
#include <iostream>
#include <cmath>
#include <bprb/bprb_func_process.h>
//:
// \file
// \brief  A process for creating a scene
//
// \author Vishal Jain
// \date Mar 15, 2011

#include <vil/vil_image_view_base.h>
#include <vil/vil_image_view.h>
#ifdef _MSC_VER
#  include <vcl_msvc_warnings.h>
#endif
namespace boxm2_mask_sift_features_process_globals
{
    constexpr unsigned n_inputs_ = 3;
    constexpr unsigned n_outputs_ = 0;
}

bool boxm2_mask_sift_features_process_cons(bprb_func_process& pro)
{
    using namespace boxm2_mask_sift_features_process_globals;

    //process takes 3 inputs
    std::vector<std::string> input_types_(n_inputs_);
    input_types_[0] = "vcl_string";
    input_types_[1] = "vil_image_view_base_sptr";
    input_types_[2] = "vcl_string";

    // process has no output
    std::vector<std::string> output_types_(n_outputs_);

    return pro.set_input_types(input_types_) && pro.set_output_types(output_types_);
}

bool boxm2_mask_sift_features_process(bprb_func_process& pro)
{
    using namespace boxm2_mask_sift_features_process_globals;

    if ( pro.n_inputs() < n_inputs_ ) {
        std::cout << pro.name() << ": The number of inputs should be " << n_inputs_<< std::endl;
        return false;
    }
    //get the inputs
    unsigned i = 0;
    std::string in_filename = pro.get_input<std::string>(i++);
    vil_image_view_base_sptr mask_img = pro.get_input<vil_image_view_base_sptr>(i++);
    std::string out_filename = pro.get_input<std::string>(i++);

    std::ifstream ifile(in_filename.c_str());
    if (auto * mask_image=dynamic_cast<vil_image_view<unsigned char> * > (mask_img.ptr()))
    {
        if (!ifile)
        {
            std::cerr<<"Unable to open the input file\n";
            return false;
        }

        unsigned int num_features=0;
        unsigned int length_features=0;
        ifile>>num_features>>length_features;
        std::cout<<"File Info "<<num_features<<' '<<length_features<<std::endl;

        std::vector<float> r;
        std::vector<float> c;
        std::vector<float> scale;
        std::vector<float> orientation;

        float u,v,s,o;
        float feature[128];
        std::stringstream ss;
        int cnt=0;
        for (unsigned k=0;k<num_features;++k)
        {
            ifile>>v>>u>>s>>o;
            auto pi=(unsigned int)std::floor(u);
            auto pj=(unsigned int)std::floor(v);
            if ((*mask_image)(pi,pj)>0)
            {
                for (unsigned j=0;j<length_features;++j)
                    ifile>>feature[j];
                std::cout<<'('<<pi<<','<<pj<<") ";
            }
            else
            {
                ss<<v<<' '<<u<<' '<<s<<' '<<o;
                for (unsigned j=0;j<length_features;++j)
                {
                    ifile>>feature[j];
                    if (j%20==0)
                        ss<<'\n';
                    ss<<' '<<feature[j];
                }
                ss<<'\n';
                cnt++;
            }
        }
        ifile.close();
        std::ofstream ofile(in_filename.c_str());

        if (!ofile)
        {
            std::cerr<<"Unable to open the output file\n";
            return false;
        }
        ofile<<cnt<<' '<<128<<'\n'
             <<ss.str();
        ofile.close();
    }
    return true;
}
