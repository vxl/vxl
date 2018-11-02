#include <iostream>
#ifdef _MSC_VER
#  include <vcl_msvc_warnings.h>
#endif
#include <mvl2/mvl2_video_from_avi.h>
#include <mvl2/mvl2_video_from_sequence.h>

#include <mvl2/mvl2_video_to_avi.h>

#include <mbl/mbl_cloneable_ptr.h>


int main(int argc, char **argv)
{
  if (argc<3)
  {
    std::cout << "usage : " << argv[0] << " file1.(seq|avi) file2.avi\n";
    return -1;
  }

  mvl2_video_from_avi file_vid;
  mvl2_video_from_sequence file_sequ_vid;
  mvl2_video_to_avi vid_output_;
  mbl_cloneable_ptr<mvl2_video_reader> vid_input_;

  if (std::string(argv[1]).rfind(".avi")==std::string(argv[1]).length()-4)
  {
    std::cout << "Trying AVI file format\n";
    vid_input_=file_vid;
  }
  else
  {
    if (std::string(argv[1]).rfind(".seq")==std::string(argv[1]).length()-4)
    {
      std::cout << "Trying SEQUENCE file format\n";
      vid_input_=file_sequ_vid;
    }
    else
    {
      std::cout << "Not an AVI or SEQUENCE format file.\n";
      return -1;
    }
  }

  std::cout << std::endl;

  if (!vid_input_->initialize(320,240,std::string("Color"),std::string(argv[1])))
  {
    std::cerr << "Error while initializing the video sequence\n";
    return -1;
  }

  std::cout << std::endl
           << "The frame rate is : "
           << vid_input_->get_frame_rate() << std::endl
           << "The length of the sequence is : "
           << vid_input_->length() << std::endl
           << "Size of the image : " << vid_input_->get_width()
           << " x " << vid_input_->get_height() << std::endl
           << std::endl;

  vid_output_.set_codec('D','X','5','0');
  vid_output_.set_frame_rate(25);
  vid_output_.set_quality(90000);
  if (!vid_output_.open(320,240,std::string(""),std::string(argv[2])))
  {
    std::cerr << "Error while opening the output video sequence\n";
    vid_input_->uninitialize();
    return -1;
  }

  vil_image_view<vxl_byte> image_;

  for (int i=0;i<vid_input_->length();i++)
    {
    vid_input_->next_frame();
    vid_input_->get_frame(image_);
    vid_output_.write_frame(image_);
    }

  vid_output_.close();
  vid_input_->uninitialize();

  return 0;
}
