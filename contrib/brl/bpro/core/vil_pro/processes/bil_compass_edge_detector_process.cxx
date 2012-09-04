// This is brl/bpro/core/vil_pro/processes/bil_compass_edge_detector_process.cxx
#include <bprb/bprb_func_process.h>
//:
// \file

#include <bprb/bprb_parameters.h>
#include <vil/vil_convert.h>
#include <vil/vil_image_view_base.h>
#include <bil/algo/bil_compass_edge_detector.h>

//: Constructor
bool bil_compass_edge_detector_process_cons(bprb_func_process& pro)
{
  //this process takes four inputs:

  vcl_vector<vcl_string> input_types;
  input_types.push_back("vil_image_view_base_sptr");
  input_types.push_back("unsigned"); // no of orientations
  input_types.push_back("double");   // sigma
  input_types.push_back("double");   // threshold

  //this process has 1 output
  vcl_vector<vcl_string> output_types;
  output_types.push_back("vil_image_view_base_sptr"); // output image

  return pro.set_input_types(input_types)
      && pro.set_output_types(output_types);
}

//: Execute the process
bool bil_compass_edge_detector_process(bprb_func_process& pro)
{
  // Sanity check
  if (pro.n_inputs()< 4) {
    vcl_cout << "bil_compass_edge_detector_process: The number of inputs should be 4" << vcl_endl;
    return false;
  }


  unsigned i=0;
  //Retrieve image from input
  vil_image_view_base_sptr img = pro.get_input<vil_image_view_base_sptr>(i++);
  unsigned n_orient = pro.get_input<unsigned>(i++);
  double sigma = pro.get_input<double>(i++);
  double threshold = pro.get_input<double>(i++);

  if (img->pixel_format()==VIL_PIXEL_FORMAT_BYTE)
  {
      vcl_cout<<img->ni()<<' '<<img->nj()<<vcl_endl;
      vil_image_view<vxl_byte> inimage = vil_convert_cast(vxl_byte(), img);
      vil_image_view<float> hist_grad(inimage.ni(),inimage.nj());
      vil_image_view<float> edgeimage=bil_detect_compass_edges(inimage, n_orient/2, sigma ,threshold, hist_grad);
      vil_image_view<vxl_byte> * outimage=new vil_image_view<vxl_byte>(edgeimage.ni(),edgeimage.nj());
      outimage->fill(0);
      for (unsigned i=0;i<edgeimage.ni();i++){
          for (unsigned j=0;j<edgeimage.nj();j++){
              if (edgeimage(i,j)>0.0)
              {
                  (*outimage)(i,j)=255;
              }
          }
      }
      vil_image_view_base_sptr outimg_sptr=outimage;
      pro.set_output_val<vil_image_view_base_sptr>(0, outimg_sptr);
  }
  else
  {
      vcl_cout<<"The type is not supported:  please add the support"<<vcl_endl;
      return false;
  }
  return true;
}


