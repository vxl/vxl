// This program demonstrates a simple transcoding from one video file
// to another using the ffmpeg streams.  If ffmpeg is not compiled
// into vidl, then the executable will simply fail without being able
// to open in input file.

#include <cstdlib>
#include <iostream>
#include <vidl/vidl_config.h>

#ifdef _MSC_VER
#  include <vcl_msvc_warnings.h>
#endif
#include <vul/vul_arg.h>
#include <vidl/vidl_ffmpeg_istream.h>
#include <vidl/vidl_ffmpeg_ostream.h>

int main( int argc, char** argv )
{
  vul_arg<std::string> input_filename( "-i", "Input filename" );
  vul_arg<std::string> output_filename( "-o", "Output filename" );
  vul_arg_parse( argc, argv );

  vidl_ffmpeg_istream istr;
  if ( ! istr.open( input_filename() ) )
  {
    std::cerr << "Couldn't open " << input_filename() << std::endl;
    return EXIT_FAILURE;
  }


  vidl_ffmpeg_ostream_params params;
  params.ni_=istr.width();
  params.nj_=istr.height();

  vidl_ffmpeg_ostream ostr;
  ostr.set_params( params );
  ostr.set_filename( output_filename() );
  if ( ! ostr.open() )
  {
    std::cerr << "Couldn't open " << output_filename() << " for writing" << std::endl;
    return EXIT_FAILURE;
  }

  unsigned count = 0;
  while ( istr.advance() )
  {
    if ( ! ostr.write_frame( istr.current_frame() ) )
    {
      std::cerr << "failed to write frame" << std::endl;
      break;
    }
    ++count;
    std::cout << "Processed frame " << istr.frame_number()
             << " (count=" << count << ")" << std::endl;
  }

  return EXIT_SUCCESS;
}
