// This is brl/bseg/boxm2/pro/processes/boxm2_mask_sift_features_process.cxx
#include <bprb/bprb_func_process.h>
//:
// \file
// \brief  A process for creating a scene
//
// \author Vishal Jain
// \date Mar 15, 2011

#include <vcl_fstream.h>
#include <vil/vil_image_view_base.h>
#include <vil/vil_image_view.h>
#include <vcl_cmath.h>
namespace boxm2_mask_sift_features_process_globals
{
    const unsigned n_inputs_  = 3;
    const unsigned n_outputs_ = 0;
}

bool boxm2_mask_sift_features_process_cons(bprb_func_process& pro)
{
    using namespace boxm2_mask_sift_features_process_globals;

    //process takes 3 inputs
    vcl_vector<vcl_string> input_types_(n_inputs_);
    input_types_[0]  = "vcl_string";
    input_types_[1]  = "vil_image_view_base_sptr";
    input_types_[2]  = "vcl_string";

    // process has no output
    vcl_vector<vcl_string> output_types_(n_outputs_);

    return pro.set_input_types(input_types_) && pro.set_output_types(output_types_);
}

bool boxm2_mask_sift_features_process(bprb_func_process& pro)
{
    using namespace boxm2_mask_sift_features_process_globals;

    if ( pro.n_inputs() < n_inputs_ ) {
        vcl_cout << pro.name() << ": The number of inputs should be " << n_inputs_<< vcl_endl;
        return false;
    }
    //get the inputs
    unsigned i = 0;
    vcl_string in_filename                     = pro.get_input<vcl_string>(i++);
    vil_image_view_base_sptr mask_img          = pro.get_input<vil_image_view_base_sptr>(i++);
    vcl_string out_filename                    = pro.get_input<vcl_string>(i++);

    vcl_ifstream ifile(in_filename.c_str());
    if (vil_image_view<unsigned char> * mask_image=dynamic_cast<vil_image_view<unsigned char> * > (mask_img.ptr()))
    {
        if (!ifile)
        {
            vcl_cerr<<"Unable to open the input file\n";
            return false;
        }

        unsigned int num_features=0;
        unsigned int length_features=0;
        ifile>>num_features>>length_features;
        vcl_cout<<"File Info "<<num_features<<' '<<length_features<<vcl_endl;

        vcl_vector<float> r;
        vcl_vector<float> c;
        vcl_vector<float> scale;
        vcl_vector<float> orientation;

        float u,v,s,o;
        float feature[128];
        vcl_stringstream ss;
        int cnt=0;
        for (unsigned k=0;k<num_features;++k)
        {
            ifile>>v>>u>>s>>o;
            unsigned int pi=(unsigned int)vcl_floor(u);
            unsigned int pj=(unsigned int)vcl_floor(v);
            if ((*mask_image)(pi,pj)>0)
            {
                for (unsigned j=0;j<length_features;++j)
                    ifile>>feature[j];
                vcl_cout<<'('<<pi<<','<<pj<<") ";
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
        vcl_ofstream ofile(in_filename.c_str());

        if (!ofile)
        {
            vcl_cerr<<"Unable to open the output file\n";
            return false;
        }
        ofile<<cnt<<' '<<128<<'\n'
             <<ss.str();
        ofile.close();
    }
    return true;
}
