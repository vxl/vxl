#include <vcl_compiler.h>
#include <vcl_iostream.h>
#include <vcl_fstream.h>
#include <vul/vul_arg.h>
#include <vidl_vil1/vidl_vil1_io.h>
#include <vidl_vil1/vidl_vil1_frame.h>
#include <vidl_vil1/vidl_vil1_movie.h>
#include <vidl_vil1/vidl_vil1_image_list_codec.h>

#ifdef HAS_MPEG2
# include <vidl_vil1/vidl_vil1_mpegcodec.h>
#endif

#ifdef VCL_WIN32
#include <vidl_vil1/vidl_vil1_avicodec.h>
#endif
//--------------------------------------------------------------------
//  This executable describes a video as an xml file
//  Sample usage is:
//
//   frame_extractor -video-file c:/videos/PoliceCar/left 
//                   -xml-file c:/videos/PoliceCar/left-description.xml
//
//  The format of the xml file is:
//  <vxl>
//  <video_description length="100" format="AVI" horizontal_resolution = "1024 
//   vertical_resolution="768" bytes_pixel="3" />
//  </vxl>
//---------------------------------------------------------------------

int main(int argc, char** argv)
{
  // Register video codecs
  vidl_vil1_io::register_codec(new vidl_vil1_image_list_codec);

#ifdef VCL_WIN32
  vidl_vil1_io::register_codec(new vidl_vil1_avicodec);
#endif

#ifdef HAS_MPEG2
  vcl_cout << " Has MPEG\n";
  vidl_vil1_io::register_codec(new vidl_vil1_mpegcodec);
#endif
  vul_arg<vcl_string> video_file("-video-file", "input video file");
  vul_arg<vcl_string> xml_file("-xml-file", "video description file");

  vul_arg_parse(argc, argv);
  vidl_vil1_movie_sptr my_movie = vidl_vil1_io::load_movie(video_file().c_str());
  if (!my_movie)
    {
      vcl_cout << "Failed to load movie\n";
      return -1;
    }

  vcl_ofstream* s = new vcl_ofstream(xml_file().c_str());

  if (!s || !(*s)) {
    vcl_cout << "Cannot open " << xml_file() << " for writing\n";
    delete s;
    return -2;
  }

  int length = my_movie->length();
  vcl_string type = my_movie->get_frame(0)->get_codec()->type();
  int width = my_movie->get_frame(0)->get_codec()->width();
  int height = my_movie->get_frame(0)->get_codec()->height();
  int bytes_pixel = my_movie->get_frame(0)->get_codec()->get_bytes_pixel();
  *s << "<vxl>\n"
     << "<video_description length=\"" << length << "\" format=\""
     << type << "\" horizontal_resolution=\"" << width 
     << "\" vertical_resolution=\"" << height 
     << "\" bytes_pixel=\"" << bytes_pixel << "\" />\n"
     << "</vxl>\n";
  s->close();
  delete s;
  return  0;
}
