#include <vidl_vil1/vidl_vil1_movie.h>
#include <vidl_vil1/vidl_vil1_frame.h>
#include <vcl_iostream.h>

int main ()
{
  vidl_vil1_movie_sptr movie = new vidl_vil1_movie;
  for (vidl_vil1_movie::frame_iterator pframe = movie->begin();
       pframe != movie->end(); ++pframe)
  {
    vidl_vil1_frame_sptr frame = pframe;
    if (!frame) return 1;
  }

  // A better way to do it
  for (vidl_vil1_movie::frame_iterator pframe = movie->first();
       pframe <= movie->last(); ++pframe)
  {
    vil1_image im = pframe->get_image();
    vcl_cout << "Got frame: " << im.width() << 'x' << im.height() << '\n';
  }

  // Running through the frames 2 images at a time
  for (vidl_vil1_movie::frame_iterator pframe = movie->first();
       pframe <= movie->last(); pframe += 2)
  {
    vil1_image im = pframe->get_image();
    vcl_cout << "Got frame: " << im.width() << 'x' << im.height() << '\n';
  }

  // Running backwards throught the image
  for (vidl_vil1_movie::frame_iterator pframe = movie->last();
       pframe >= movie->first(); --pframe)
  {
    vil1_image im = pframe->get_image();
    vcl_cout << "Got frame: " << im.width() << 'x' << im.height() << '\n';
  }

  // Backwards two at a time
  for (vidl_vil1_movie::frame_iterator pframe = movie->last();
       pframe >= movie->first(); pframe -= 2)
  {
    vil1_image im = pframe->get_image();
    vcl_cout << "Got frame: " << im.width() << 'x' << im.height() << '\n';
  }

  // Run over all pairs of images
  for (vidl_vil1_movie::frame_iterator pframe1 = movie->first();
       pframe1 <= movie->last(); ++pframe1)
    for (vidl_vil1_movie::frame_iterator pframe2 = movie->first();
         pframe2 < pframe1; ++pframe2)
    {
      // Run some test on the two images
      vil1_image im1 = pframe1->get_image();
      vil1_image im2 = pframe2->get_image();
      if (im1 == im2)
        vcl_cout << "Frames " << pframe1.current_frame_number()
                 << " and " << pframe2.current_frame_number() << " are equal\n";
    }

  // Running over frames 10 to 20
  vidl_vil1_movie::frame_iterator pframe = movie->begin();
  for (int p=0; p<=20 && pframe!=movie->end(); ++p,++pframe)
    if (p>=10)
      pframe->get_image();

  return 0;
}
