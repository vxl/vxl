#include <vcl_iostream.h>
#include <vul/vul_arg.h>
#include <vil1/vil1_save.h>
#include <vidl/vidl_io.h>
#include <vidl/vidl_movie_sptr.h>
#include <vidl/vidl_movie.h>
#include <vidl/vidl_image_list_codec.h>

#ifdef HAS_MPEG2
# include <vidl/vidl_mpegcodec.h>
#endif

#ifdef VCL_WIN32
#include <vidl/vidl_avicodec.h>
#endif
//--------------------------------------------------------------------
//  This executable extracts a specified frame from a video and writes it
//  out as a jpeg image.  Sample usage is:
//
//   frame_extractor -video-path c:/videos/PoliceCar/left -frame-index 32 
//                   -image-file c:/videos/PoliceCar/left-frame-32.jpeg
//
//  Note that the user is responsible for including the jpeg extension,
//  if desired. Most readers will sucessfully read the image without an
//  extension, using probes.
//---------------------------------------------------------------------
int main(int argc, char** argv)
{
  // Register video codecs
  vidl_io::register_codec(new vidl_image_list_codec);

#ifdef VCL_WIN32
  vidl_io::register_codec(new vidl_avicodec);
#endif

#ifdef HAS_MPEG2
  vcl_cout << " Has MPEG\n";
  vidl_io::register_codec(new vidl_mpegcodec);
#endif
  vul_arg<vcl_string> video_file("-video-path", "input video file");
  vul_arg<double> frame_index("-frame-index", "for N frames, range [1, N-1], fractional [0.0, 0.999]");
  vul_arg<vcl_string> image_file("-image-file", "output image file, ext=.jpeg");
  vul_arg_parse(argc, argv);
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
  int pos = 0;
  if(frame_index()<1.0)
    pos = (int)(frame_index()*(my_movie->length()-1));
  else
    pos = (int)frame_index();

  vil1_image image = my_movie->get_image(pos);
  vcl_string type = "jpeg";
  vil1_save(image, image_file().c_str(), type.c_str());
  return  my_movie->length();
}
