#include <vcl_iostream.h>
#include <vcl_fstream.h>
#include <vil1/vil1_image.h>
#include <vidl_vil1/vidl_vil1_movie.h>
#include <vidl_vil1/vidl_vil1_clip.h>
#include <vidl_vil1/vidl_vil1_io.h>
#include <vpro/vpro_capture_process.h>

vul_timer vpro_capture_process::time_;

bool vpro_capture_process::capture_started_=false;

vpro_capture_process::vpro_capture_process(vcl_string const& video_file)
{
  video_file_ = video_file;
}

vpro_capture_process::~vpro_capture_process()
{
}

bool vpro_capture_process::execute()
{
  if (this->get_N_input_images()!=1)
    {
      vcl_cout << "In vpro_capture_process::execute() -"
               << " not exactly one input image \n";
    return false;
    }
    //JLM
  //vil1_memory_image_of<unsigned char> img(vpro_video_process::get_input_image(0));
  vil1_image img = vpro_video_process::get_input_image(0);
  frames_.push_back(img);
  
  //reset the timer if this is the first image captured 
  if(!capture_started_){
    capture_started_ = true;
    time_.mark();
  }
  time_stamps_.push_back(time_.real());
  return true;
}

bool vpro_capture_process::finish()
{
  if (!frames_.size())
    return false;
  vidl_vil1_clip_sptr clip = new vidl_vil1_clip(frames_);
  vidl_vil1_movie_sptr mov= new vidl_vil1_movie();
  mov->add_clip(clip);
  if (!vidl_vil1_io::save(mov.ptr(), video_file_.c_str(), "png")){
    vcl_cout << "In vpro_capture_process::finish()"
             << " - failed to save video" << vcl_endl;
    return false;
  }
  frames_.clear();
  capture_started_ = false;
  if ( !save_time_stamps() ){
    vcl_cout << "In vpro_capture_process::finish()"
             << " - failed to save time stamps"<< vcl_endl;
    return false;
  }
  time_stamps_.clear();
  vcl_cout << "Finished Saving Captured Video File\n";
  return true;
}

// Write the time stamps to a text file
bool vpro_capture_process::save_time_stamps()
{
  vcl_string file_name = video_file_+"time_stamps.txt";
  vcl_ofstream time_file(file_name.c_str());
  vcl_cout << file_name.c_str() << vcl_endl;
  for(unsigned i=0; i<time_stamps_.size(); ++i){
    time_file << i << '\t' << time_stamps_[i] << vcl_endl;
  }
  time_file.close();
  return true;
}
