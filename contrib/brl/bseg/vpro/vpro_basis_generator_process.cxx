#include <vcl_list.h>
#include <vcl_iostream.h>
#include <vil1/vil1_image.h>
#include <vidl_vil1/vidl_vil1_movie.h>
#include <vidl_vil1/vidl_vil1_clip.h>
#include <vidl_vil1/vidl_vil1_io.h>
#include <brip/brip_float_ops.h>
#include <vpro/vpro_basis_generator_process.h>

vpro_basis_generator_process::vpro_basis_generator_process(vcl_string const& video_file)
{
  video_file_ = video_file;
}

vpro_basis_generator_process::~vpro_basis_generator_process()
{
}

bool vpro_basis_generator_process::execute()
{
  if (this->get_N_input_images()!=1)
    {
      vcl_cout << "In vpro_basis_generator_process::execute() -"
               << " not exactly one input image \n";
    return false;
    }

  vil1_image img = vpro_video_process::get_input_image(0);
  input_images_.clear();
  vil1_memory_image_of<float> flt = brip_float_ops::convert_to_float(img);
  frames_.push_back(flt);
  return true;
}

bool vpro_basis_generator_process::finish()
{
  
  if (!frames_.size())
    return false;
  vcl_vector<vil1_memory_image_of<float> > basis;
  brip_float_ops::basis_images(frames_, basis);
  vcl_list<vil1_image> basis_frames;
  for(vcl_vector<vil1_memory_image_of<float> >::iterator iit = basis.begin();
      iit != basis.end(); iit++)
    {
      vil1_image img = brip_float_ops::convert_to_byte(*iit);
      basis_frames.push_back(img);
    }
  vidl_vil1_clip_sptr clip = new vidl_vil1_clip(basis_frames);
  vidl_vil1_movie_sptr mov= new vidl_vil1_movie();
  mov->add_clip(clip);
  if (!vidl_vil1_io::save(mov.ptr(), video_file_.c_str(), "tiff")){
    vcl_cout << "In vpro_basis_generator_process::finish()"
             << " - failed to save video" << vcl_endl;
    return false;
  }
  frames_.clear();
  return true;
}

