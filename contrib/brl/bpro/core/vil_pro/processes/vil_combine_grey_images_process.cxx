// This is brl/bpro/core/vil_pro/processes/vil_combine_grey_images_process.cxx
#include <bprb/bprb_func_process.h>
//:
// \file

#include <bprb/bprb_parameters.h>
#include <vil/vil_load.h>
#include <vil/vil_image_view.h>

//: Constructor
bool vil_combine_grey_images_process_cons(bprb_func_process& pro)
{
  //this process takes 1 input:
  // input(0): Filename containing the list of images to combine
  bool ok=false;
  std::vector<std::string> input_types;
  input_types.emplace_back("vcl_string");
  ok = pro.set_input_types(input_types);
  if (!ok) return ok;

  std::vector<std::string> output_types;
  output_types.emplace_back("vil_image_view_base_sptr");  // label image
  ok = pro.set_output_types(output_types);
  if (!ok) return ok;

  return true;
}

// Get images from a file.
bool get_images(const std::string& file,
                std::vector<vil_image_view_base_sptr>  &grey_imgs,
                unsigned& width, unsigned& height)
{
  grey_imgs.clear();
  std::ifstream ifs( file.c_str() );
  //vector to store image sizes
  std::vector< std::vector<unsigned> > sizes;

  //Caution: Your file should not have an empty line at the end
  while (!ifs.eof())
  {
     std::string image_filename;
     ifs >> image_filename;
     if (image_filename.empty())
       continue;
     vil_image_view_base_sptr loaded_image = vil_load(image_filename.c_str() );
     if ( !loaded_image ) {
       std::cerr << "Failed to load image file: " << image_filename << '\n';
       return false;
     }
     if ( loaded_image->nplanes() != 1)
     {
       std::cerr << "Image" << image_filename << "is not gray scale\n";
       return false;
     }
     grey_imgs.push_back( loaded_image );
     std::vector<unsigned> this_size;
     this_size.push_back(loaded_image->ni());
     this_size.push_back(loaded_image->nj());
     sizes.push_back(this_size);
   }

  //check that all images have the same size
  for (unsigned i = 1; i < sizes.size(); i++)
  {
    if ( sizes[i] != sizes[i-1]) {
      std::cerr << "Grey-scale images have different sizes\n";
      return false;
    }
  }
  width = sizes[0][0];
  height = sizes[0][1];

  return true;
}


// Combine the grey-scale images.
bool combine(std::vector<vil_image_view_base_sptr>const  &grey_imgs,
             vil_image_view_base_sptr &mul_img,
             unsigned width, unsigned height)
{
  vil_image_view<float> byte_img(width,height,grey_imgs.size());

  for (unsigned int p = 0; p < grey_imgs.size(); p++)
  {
    vil_image_view<float> grey_img = static_cast<vil_image_view<float> >(*grey_imgs[p]);

    for (unsigned i = 0; i < width; i++)
      for (unsigned j = 0; j < height; j++)
        byte_img(i,j,p) = grey_img(i,j);
  }

  mul_img = new vil_image_view<float>(byte_img);

  return true;
}

//: Execute the process
bool vil_combine_grey_images_process(bprb_func_process& pro)
{
  // Sanity check
  if (pro.n_inputs()< 1) {
    std::cout << "vil_combine_grey_images_process: The input number should be 1" << std::endl;
    return false;
  }

  // get the inputs
  unsigned i=0;
  std::string imgs_file = pro.get_input<std::string>(i++);

  //Retrieve image from input
  std::vector<vil_image_view_base_sptr> grey_imgs;
  unsigned width, height;

  if (!get_images(imgs_file, grey_imgs, width, height))
    return false;

  vil_image_view_base_sptr out_img_ptr;

  if (!combine(grey_imgs,out_img_ptr,width,height))
    return false;

  pro.set_output_val<vil_image_view_base_sptr>(0, out_img_ptr);

  return true;
}
