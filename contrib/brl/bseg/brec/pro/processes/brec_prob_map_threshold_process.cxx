// This is brl/bseg/brec/pro/processes/brec_prob_map_threshold_process.cxx

//:
// \file
// \brief A process to generate a thresholded image for a given prob map and threshold value
//
//  CAUTION: Input image is assumed to be a probability map, i.e. have type float with values in [0,1]
//
// \author Ozge Can Ozcanli
// \date November 06, 2008
//
// \verbatim
//  Modifications
//   Ozge C. Ozcanli - Feb 03, 2009 - converted process-class to functions which is the new design for bprb processes.
// \endverbatim

#include <iostream>
#include <bprb/bprb_func_process.h>
#include <bprb/bprb_parameters.h>
#ifdef _MSC_VER
#  include <vcl_msvc_warnings.h>
#endif
#include <brdb/brdb_value.h>
#include <vil/vil_image_view.h>
#include <vil/vil_convert.h>


// Constructor
bool brec_prob_map_threshold_process_cons(bprb_func_process& pro)
{
  //inputs
  bool ok=false;
  std::vector<std::string> input_types;
  input_types.emplace_back("vil_image_view_base_sptr"); //input probability frame
  input_types.emplace_back("vil_image_view_base_sptr"); //input probability frame's mask
  input_types.emplace_back("vil_image_view_base_sptr"); //orig_view
  input_types.emplace_back("float"); //threshold
  ok = pro.set_input_types(input_types);
  if (!ok) return ok;

  //output
  std::vector<std::string> output_types;
  output_types.emplace_back("vil_image_view_base_sptr");   // output a thresholded image where red channel is binarized wrt given threshold
  output_types.emplace_back("vil_image_view_base_sptr");   // output a thresholded image where red channel is mapped wrt prob
  ok = pro.set_output_types(output_types);
  return ok;
}

//: Execute the process
bool brec_prob_map_threshold_process(bprb_func_process& pro)
{
  // Sanity check
  if (pro.n_inputs() < 4) {
    std::cerr << " brec_prob_map_threshold_process - invalid inputs\n";
    return false;
  }

  //: get input
  unsigned i = 0;
  vil_image_view_base_sptr temp = pro.get_input<vil_image_view_base_sptr>(i++);
  vil_image_view<float> frame = *vil_convert_cast(float(), temp);
  if (temp->pixel_format() != VIL_PIXEL_FORMAT_FLOAT)
    return false;
  unsigned ni = frame.ni(), nj = frame.nj();

  temp = pro.get_input<vil_image_view_base_sptr>(i++);
  vil_image_view<bool> input_mask = *vil_convert_cast(bool(), temp);

  temp = pro.get_input<vil_image_view_base_sptr>(i++);
  vil_image_view<vxl_byte> input_img(temp);

  auto thres = pro.get_input<float>(i++);

  vil_image_view<vxl_byte> out(ni, nj, 3);
  out.fill(0);
  vil_image_view<vxl_byte> out2(ni, nj, 3);
  out2.fill(0);


  int count = 0;
  for (unsigned j = 0; j<nj; ++j)
    for (unsigned i = 0; i<ni; ++i)
    {
      if (input_mask(i,j)) {
        out(i,j,0)=input_img(i,j);
        out(i,j,1)=input_img(i,j);
        out(i,j,2)=input_img(i,j);

        out2(i,j,0)=input_img(i,j);
        out2(i,j,1)=input_img(i,j);
        out2(i,j,2)=input_img(i,j);

        if (frame(i,j)<=thres) {
          out(i,j,0) = 255;
          float change = 1-frame(i,j); // invert input map
          // assuming that change is in [0,1] where 0 is no change and 1 is change
          // we set red channel so that: on the threshold value it becomes 255*0.01
          // and it increases exponentially till 1 when it becomes 255
          out2(i,j,0) = (vxl_byte)(255*std::exp((std::log(0.01)/(thres-1))*(change-1)));
          count++;
        }
      }
    }

  std::cout << "----------------------------------\n"
           << "\tthres: " << thres << '\n'
           << "\tnumber of CHANGE PIXELS: " << count << " out of " << ni*nj << " pixels: %" << (float)count/float(ni*nj)*100.0f << std::endl
           << "----------------------------------\n";

  pro.set_output_val<vil_image_view_base_sptr>(0, new vil_image_view<vxl_byte>(out));
  pro.set_output_val<vil_image_view_base_sptr>(1, new vil_image_view<vxl_byte>(out2));

  return true;
}
