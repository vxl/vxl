// This program demonstrates a simple transcoding from one video file
// to another using the ffmpeg streams.  If ffmpeg is not compiled
// into vidl2, then the executable will simply fail without being able
// to open in input file.

#include <vidl2/vidl2_config.h>

#include <vcl_iostream.h>
#include <vcl_cstdlib.h>
#include <vul/vul_arg.h>
#include <vidl2/vidl2_ffmpeg_istream.h>
#include <vidl2/vidl2_ffmpeg_ostream.h>

int
main( int argc, char** argv )
{
  vul_arg<vcl_string> input_filename( "-i", "Input filename" );
  vul_arg<vcl_string> output_filename( "-o", "Output filename" );
  vul_arg_parse( argc, argv );

  vidl2_ffmpeg_istream istr;
  if( ! istr.open( input_filename() ) )
  {
    vcl_cerr << "Couldn't open " << input_filename() << "\n";
    return EXIT_FAILURE;
  }

  vidl2_ffmpeg_ostream ostr;
  ostr.set_filename( output_filename() );
  if( ! ostr.open() )
  {
    vcl_cerr << "Couldn't open " << input_filename() << " for writing\n";
    return EXIT_FAILURE;
  }

  unsigned count = 0;
  do
  {
    if( ! ostr.write_frame( istr.current_frame() ) )
    {
      break;
    }
    ++count;
    vcl_cout << "Processed frame " << istr.frame_number()
             << " (count=" << count << "\n";
  }
  while( istr.advance() );

  return EXIT_SUCCESS;
}
