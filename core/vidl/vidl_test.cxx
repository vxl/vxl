#include <vidl/vidl_movie.h>
#include <vidl/vidl_frame.h>
#include <vcl_iostream.h>

void tryit ()
  {
  vidl_movie movie;
  for (vidl_movie::frame_iterator pframe = movie.begin();
       pframe != movie.end();
       ++pframe)
    {
    vidl_frame_sptr frame = pframe;
    if (!frame) return;
    }

  // A better way to do it
  for (vidl_movie::frame_iterator pframe = movie.first();
        pframe <= movie.last();
        ++pframe)
    {
    vil_image im = pframe->get_image();
    vcl_cout << "Got frame: " << im.width() << 'x' << im.height() << '\n';
    }

  // Running through the frames 2 images at a time
  for (vidl_movie::frame_iterator pframe = movie.first();
        pframe <= movie.last();
        pframe = pframe + 2)
    {
    vil_image im = pframe->get_image();
    vcl_cout << "Got frame: " << im.width() << 'x' << im.height() << '\n';
    }

  // Running backwards throught the image
  for (vidl_movie::frame_iterator pframe = movie.last();
        pframe >= movie.first();
        --pframe)
    {
    vil_image im = pframe->get_image();
    vcl_cout << "Got frame: " << im.width() << 'x' << im.height() << '\n';
    }

  // Backwards two at a time
  for (vidl_movie::frame_iterator pframe = movie.last();
        pframe >= movie.first();
        pframe = pframe - 2)
    {
    vil_image im = pframe->get_image();
    vcl_cout << "Got frame: " << im.width() << 'x' << im.height() << '\n';
    }

  // Run over all pairs of images
  for (vidl_movie::frame_iterator pframe1 = movie.first();
      pframe1 <= movie.last() - 1;
      ++pframe1)
     {
     for (vidl_movie::frame_iterator pframe2 = pframe1+1;
        pframe2 <= movie.last();
        ++pframe2)
        {
        // Run some test on the two images
        }
     }

  // Running over frames 10 to 20
  vidl_movie::frame_iterator pframe = movie.begin();
  for (pframe = 10; pframe <=20; ++pframe)
     {
     pframe->get_image();
     }
  }
