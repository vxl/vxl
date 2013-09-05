// This is brl/bpro/core/vil_pro/processes/vil_grey_to_rgb_process.cxx
#include <bprb/bprb_func_process.h>
//:
// \file
// \brief A process for transferring a rendered grey image to RGB image.  The color code is loaded from input text file
//        The text should have format, e.g.
//        id  color_r  color_g  color_b
//         1    255      255      255
//         2    120      120      120
//    for id exceeds the maximum id in text, the color is (0,0,0)
//
// \author Yi Dong
// \date Sep 05, 2013

#include <vul/vul_file.h>
#include <vil/vil_load.h>
#include <vil/vil_image_view.h>
#include <vil/vil_convert.h>

// global variables
namespace vil_grey_to_rgb_process_globals
{
  const unsigned  n_inputs_ = 2;
  const unsigned n_outputs_ = 1;

  vcl_map<float, vil_rgb<vxl_byte> > get_color_id(vcl_string color_text)
  {
    vcl_map<float, vil_rgb<vxl_byte> > color_map;
    unsigned id;
    unsigned r, g, b;
    vcl_ifstream ifs(color_text.c_str());
    vcl_string header;
    vcl_getline(ifs, header);
    while (!ifs.eof()) {
      ifs >> id;  ifs >> r;  ifs >> g;  ifs >> b;
      vil_rgb<vxl_byte> color(r,g,b);
      color_map.insert(vcl_pair<float, vil_rgb<vxl_byte> >((float)id, color));
    }
    ifs.close();
    return color_map;
  }

}

// constructor
bool vil_grey_to_rgb_process_cons(bprb_func_process& pro)
{
  using namespace vil_grey_to_rgb_process_globals;
  
  vcl_vector<vcl_string> input_types_(n_inputs_);
  input_types_[0] = "vil_image_view_base_sptr";   // input grey image
  input_types_[1] = "vcl_string";                 // color code text file

  vcl_vector<vcl_string> output_types_(n_outputs_);
  output_types_[0] = "vil_image_view_base_sptr";  // output rgb image
  return pro.set_input_types(input_types_) && pro.set_output_types(output_types_);
}

bool vil_grey_to_rgb_process(bprb_func_process& pro)
{
  using namespace vil_grey_to_rgb_process_globals;
  
  if ( pro.n_inputs() < n_inputs_ ) {
    vcl_cout << pro.name() << ": The number of inputs should be " << n_inputs_ << vcl_endl;
    return false;
  }

  // get the input
  unsigned i = 0;
  vil_image_view_base_sptr img = pro.get_input<vil_image_view_base_sptr>(i++);
  vcl_string color_text = pro.get_input<vcl_string>(i++);

  // load the color test
  if (!vul_file::exists(color_text)) {
    vcl_cout << pro.name() << ": can not find file: " << color_text << vcl_endl;
    return false;
  }
  vcl_map<float, vil_rgb<vxl_byte> > color_map = get_color_id(color_text);

  for (vcl_map<float, vil_rgb<vxl_byte> >::iterator mit = color_map.begin(); mit != color_map.end(); ++mit)
    vcl_cout << " id = " << mit->first << " --> color = " << mit->second << vcl_endl;

  // load the input image and transfer it to type float
  unsigned ni, nj;
  ni = img->ni();  nj = img->nj();

  vil_image_view<float>* in_img = dynamic_cast<vil_image_view<float>* >(img.ptr());
  if (!in_img) {
    vil_image_view<float> temp(img->ni(), img->nj(), 1);
    vil_image_view<vxl_int_16>* img_view_int = dynamic_cast<vil_image_view<vxl_int_16>*>(img.ptr());
    if (!img_view_int) {
      vil_image_view<vxl_byte>* img_view_byte = dynamic_cast<vil_image_view<vxl_byte>*>(img.ptr());
      if (!img_view_byte) {
        vcl_cerr << pro.name() << ": The image pixel format: " << img->pixel_format() << " is not supported!\n";
        return false;
      }
      else
        vil_convert_cast(*img_view_byte, temp);
    }
    else
      vil_convert_cast(*img_view_int, temp);
    in_img = new vil_image_view<float>(temp);
  }

  // generate a color rgb image
  vil_image_view<vil_rgb<vxl_byte> >* out_img = new vil_image_view<vil_rgb<vxl_byte> >(ni,nj);
  out_img->fill(vil_rgb<vxl_byte>(0,0,0));
  for (int i = 0; i < ni; i++)
    for (int j = 0; j < nj; j++) {
      vil_rgb<vxl_byte> color;
      if (color_map.find((*in_img)(i,j)) != color_map.end())
        color = color_map.find((*in_img)(i,j))->second;
      else
        color = vil_rgb<vxl_byte>(0,0,0);
      (*out_img)(i,j) = color;
    }
  
  pro.set_output_val<vil_image_view_base_sptr>(0, out_img);
  return true;
}