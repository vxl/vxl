// This is an example of how to use vidl_vil1_mpegcodec,
// written by Ming Li(ming@mpi-sb.mpg.de),
// Max-Planck-Institut fuer Informatik, Germany, 29 Jan 2003.

// may not work for some format mpeg2 files due to the fixed
// load_mpegcodec_callback function !!

#include <vcl_cassert.h>
#include <vcl_cstdlib.h>
#include <vil1/vil1_save.h>
#include <vidl_vil1/vidl_vil1_io.h>
#include <vidl_vil1/vidl_vil1_mpegcodec.h>
#include <vidl_vil1/vidl_vil1_movie.h>
#include <vidl_vil1/vidl_vil1_frame.h>

static void my_load_mpegcodec_callback(vidl_vil1_codec * vc)
{
    bool grey_scale = false;
    bool demux_video = true;
    vcl_string pid = "0x00";
    int numframes = -1;

    vidl_vil1_mpegcodec * mpegcodec = vc->castto_vidl_vil1_mpegcodec();
      if (!mpegcodec) return;

    mpegcodec->set_grey_scale(grey_scale);
    if (demux_video) mpegcodec->set_demux_video();
      mpegcodec->set_pid(pid.c_str());
    mpegcodec->set_number_frames(numframes);
    mpegcodec->init();
}

int main(int argc, char* argv[])
{
  if (argc < 2)
  {
    vcl_cerr << "Please specify an MPEG movie file as first command line argument.\n"
             << "The middle frame will then be saved to a file named test.ppm\n";
    return 1;
  }
  vidl_vil1_io::register_codec(new vidl_vil1_mpegcodec);
  vidl_vil1_mpegcodec *mpegcodec=new vidl_vil1_mpegcodec;
  vidl_vil1_io::register_codec(mpegcodec);
  vidl_vil1_io::load_mpegcodec_callback=&my_load_mpegcodec_callback;

  vidl_vil1_movie_sptr movie = vidl_vil1_io::load_movie(argv[1]);
  assert( movie );
  assert( movie->length()>0 );
  vcl_cout << "Length = " << movie->length() << vcl_endl
           << "Width  = " << movie->width() << vcl_endl
           << "Height = " << movie->width() << vcl_endl;


  //traverse the movie sequence
  int i=0;
  for (vidl_vil1_movie::frame_iterator pframe = movie->first();
       pframe <= movie->last();
       ++pframe,i++)
  {
    vil1_image im = pframe->get_image();
    vcl_cout << "decode frame " << i << vcl_endl;
  }

  //random frame access
  vil1_image im=movie->get_image(movie->length()/2);
  vil1_save(im,"test.ppm");

  mpegcodec->close();

  vcl_exit (0);
  return 0;
}
