#include <vcl_iostream.h>
#include <vul/vul_arg.h>
#include <vil/vil_save.h>
#include <vidl/vidl_io.h>
#include <vidl/vidl_movie.h>
#include <vidl/vidl_frame.h>

//--------------------------------------------------------------------
//  This executable extracts a specified frame from a video and writes it
//  out as an image.  Sample usage is:
//
//   frame_extractor -video-path c:/videos/PoliceCar/left -frame-index 32 
//                   -image-file c:/videos/PoliceCar/left-frame-32.jpeg
//
//  Note that the user is responsible for including the file extension,
//  if desired.  The file type for saving is determined from the file name.
//---------------------------------------------------------------------
int main(int argc, char** argv)
{
  // --- Program Arguments ---
  vul_arg<vcl_string> video_file("-video-path", "input video file");
  vul_arg<double> frame_index("-frame-index", "for N frames, range [1, N-1], fractional [0.0, 0.999]");
  vul_arg<vcl_string> image_file("-image-file", "output image file, ext=.jpeg");
  vul_arg_parse(argc, argv);

  // --- Load the Movie ---
  vidl_movie_sptr my_movie = vidl_io::load_movie(video_file().c_str());
  if (!my_movie)
    {
      vcl_cout << "Failed to load movie \n";
      return -1;
    }
  vcl_cout << "Movie has " << my_movie->length() << " frames\n";
  if(frame_index()<0||frame_index()>=my_movie->length())
    {
      vcl_cout << "Frame index out of range \n";
      return -2;
    }
  int pos;
  if(frame_index()<1.0)
    pos = (int)(frame_index()*(my_movie->length()-1));
  else
    pos = (int)frame_index();
  
  // --- Save the Frame ---
  vil_save(*my_movie->get_view(pos), image_file().c_str());

  return 0;
}
