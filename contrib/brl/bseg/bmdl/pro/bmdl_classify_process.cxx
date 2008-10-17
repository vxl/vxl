#include "bmdl_classify_process.h"
//:
// \file

#include <vcl_cstring.h>

#include <vil/vil_load.h>
#include <vil/vil_image_resource.h>
#include <vil/vil_image_view.h>
#include <vil/vil_pixel_format.h>
#include <vil/vil_convert.h>

#include <bprb/bprb_parameters.h>

#include <bmdl/bmdl_classify.h>

bmdl_classify_process::bmdl_classify_process()
{
  //this process takes 2 input:
  //the file paths of the first return and second return
  input_data_.resize(2, brdb_value_sptr(0));
  input_types_.resize(2);

  int i=0;
  input_types_[i++] = "vcl_string";             // first ret. image path (geotiff)
  input_types_[i++] = "vcl_string";             // last ret. image path (geotiff)

  //output
  output_data_.resize(2,brdb_value_sptr(0));
  output_types_.resize(2);
  int j=0;
  output_types_[j++]= "vil_image_view_base_sptr";  // label image
  output_types_[j++]= "vil_image_view_base_sptr";  // height image

}

bool bmdl_classify_process::execute()
{
  // Sanity check
  if (!this->verify_inputs())
    return false;

  // get the inputs:
  // image
  brdb_value_t<vcl_string>* input0 =
    static_cast<brdb_value_t< vcl_string>* >(input_data_[0].ptr());
  vcl_string first = input0->value();

  brdb_value_t<vcl_string>* input1 =
    static_cast<brdb_value_t< vcl_string>* >(input_data_[1].ptr());
  vcl_string last = input1->value();

  // check first return's validity
  vil_image_resource_sptr first_ret = vil_load_image_resource(first.c_str());
  if (!first_ret) {
    vcl_cout << "bmdl_classify_process -- First return image path is not valid!\n";
    return false;
  }

  // check last return's validity
  vil_image_resource_sptr last_ret = vil_load_image_resource(last.c_str());
  if (!last_ret) {
    vcl_cout << "bmdl_classify_process -- Last return image path is not valid!\n";
    return false;
  }

  vil_image_view_base_sptr label_img=0, height_img=0;
  if (!classify(first_ret, last_ret, label_img, height_img)) {
    vcl_cout << "bmdl_classify_process -- The process has failed!\n";
    return false;
  }

  // store image output (labels)
  brdb_value_sptr output1 =
    new brdb_value_t<vil_image_view_base_sptr>(label_img);
  output_data_[0] = output1;

  // store image output (height)
  brdb_value_sptr output2 =
    new brdb_value_t<vil_image_view_base_sptr>(height_img);
  output_data_[1] = output2;

  return true;
}

bool bmdl_classify_process::classify(vil_image_resource_sptr lidar_first,
                                         vil_image_resource_sptr lidar_last,
                                         vil_image_view_base_sptr& label_img,
                                         vil_image_view_base_sptr& height_img)
{
  // the file should be a at least a tiff (better, geotiff)
  vcl_cout << "File FORMAT=" << lidar_first->file_format();

  if (vcl_strcmp(lidar_first->file_format(), "tiff") != 0 &&
      vcl_strcmp(lidar_last->file_format(),"tiff") != 0) {
    vcl_cout << "bmdl_classify_process::classify -- The lidar images should be a TIFF!\n";
    return false;
  }

  vil_image_view<double> first_return, last_return;
  // convert the images to double pixel type
  first_return = vil_convert_cast(double(),lidar_first->get_view());
  last_return = vil_convert_cast(double(),lidar_last->get_view());
  if(!first_return || !last_return){
    vcl_cout << "bmdl_classify_process::classify -- The Image Pixel Type is not DOUBLE or FLOAT!\n";
    return false;
  }
  

  label_img = new vil_image_view<unsigned int>();
  height_img = new vil_image_view<double>();
  bmdl_classify::label_lidar(first_return, last_return,
                             (vil_image_view<unsigned int>&)(*label_img),
                             (vil_image_view<double>&)(*height_img));
    

  return true;
}

