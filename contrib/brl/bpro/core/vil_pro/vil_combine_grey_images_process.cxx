// This is brl/bpro/core/vil_pro/vil_combine_grey_images_process.cxx
#include "vil_combine_grey_images_process.h"
//:
// \file

#include <bprb/bprb_parameters.h>
#include <vil/vil_load.h>
#include <vil/vil_image_view.h>


//: Constructor
vil_combine_grey_images_process::vil_combine_grey_images_process()
{
  //this process takes 1 input:
  // input(0): Filename containing the list of images to combine
  input_data_.resize(1,brdb_value_sptr(0));
  input_types_.resize(1);
  input_types_[0]="vcl_string";

  //this process has 1 output
  // output(0): the multi-schannel image
  output_data_.resize(1,brdb_value_sptr(0));
  output_types_.resize(1);
  output_types_[0]="vil_image_view_base_sptr";
}

//: Destructor
vil_combine_grey_images_process::~vil_combine_grey_images_process()
{
}

// Get images from a file.
bool
vil_combine_grey_images_process::get_images(vcl_string file,
                 vcl_vector<vil_image_view_base_sptr>  &grey_imgs)
{
  grey_imgs.clear();
  vcl_ifstream ifs( file.c_str() );
  //vector to store image sizes
  vcl_vector< vcl_vector<unsigned> > sizes;

  //Caution: Your file should not have an empty line at the end
  while(!ifs.eof())
  {
      vcl_string image_filename;
      ifs >> image_filename;
      if(image_filename.empty())
        continue;
      vil_image_view_base_sptr loaded_image = vil_load(image_filename.c_str() );
      if ( !loaded_image ) {
        vcl_cerr << "Failed to load image file" << image_filename << vcl_endl;
        return false;
      }
      if ( loaded_image->nplanes() != 1)
      {
        vcl_cerr << "Image" << image_filename << "is not gray scale "<< vcl_endl;
        return false;
      }
      grey_imgs.push_back( loaded_image );
      vcl_vector<unsigned> this_size;
      this_size.push_back(loaded_image->ni());
      this_size.push_back(loaded_image->nj());
      sizes.push_back(this_size);
   }

  //check that all images have the same size
  for (unsigned i = 1; i < sizes.size(); i++)
  {
    if ( sizes[i] != sizes[i-1]){
      vcl_cerr << "Grey-scale images have different sizes" << vcl_endl;
      return false;
    }

  }
  width_ = sizes[0][0];
  height_ = sizes[0][1];
  
  
  return true;
}


// Combine the grey-scale images.
bool
vil_combine_grey_images_process::combine(vcl_vector<vil_image_view_base_sptr>const  &grey_imgs,
                                         vil_image_view_base_sptr &mul_img)
{
  
  vil_image_view<float> byte_img(width_,height_,grey_imgs.size());
    
  for (unsigned int p = 0; p < grey_imgs.size(); p++)
  {
    vil_image_view<float> grey_img = static_cast<vil_image_view<float> >(*grey_imgs[p]);

    for (unsigned i = 0; i < width_; i++)
      for (unsigned j = 0; j < height_; j++)
        byte_img(i,j,p) = grey_img(i,j);

  }

  mul_img = new vil_image_view<float>(byte_img);
 
  return true;
}

//: Execute the process
bool
vil_combine_grey_images_process::execute()
{
  // Sanity check
  if (!this->verify_inputs())
    return false;

  //Retrieve image from input
  brdb_value_t<vcl_string>* input0 =
    static_cast<brdb_value_t<vcl_string>* >(input_data_[0].ptr());

  vcl_string imgs_file = input0->value();

  vcl_vector<vil_image_view_base_sptr> grey_imgs;

  if (!this->get_images(imgs_file, grey_imgs))
    return false;

  vil_image_view_base_sptr out_img_ptr;

  if (!this->combine(grey_imgs,out_img_ptr))
    return false;

  brdb_value_sptr output0 = new brdb_value_t<vil_image_view_base_sptr>(out_img_ptr);
  output_data_[0] = output0;

  return true;
}

