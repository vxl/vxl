#include <vidl/vidl_movie.h>
#include <vidl/vidl_frame.h>
#include <vcl_iostream.h>

int main ()
{
  vidl_movie_sptr movie = new vidl_movie;
  for (vidl_movie::frame_iterator pframe = movie->begin();
       pframe != movie->end();
       ++pframe)
  {
    vidl_frame_sptr frame = pframe;
    if (!frame) return 1;
  }

  // A better way to do it
  for (vidl_movie::frame_iterator pframe = movie->first();
       pframe <= movie->last();
       ++pframe)
  {
    vil_image_view_base_sptr im = pframe->get_view();
    vcl_cout << "Got frame: " << im->ni() << 'x' << im->nj() << '\n';
  }

  // Running through the frames 2 images at a time
  for (vidl_movie::frame_iterator pframe = movie->first();
       pframe <= movie->last();
       pframe += 2)
  {
    vil_image_view_base_sptr im = pframe->get_view();
    vcl_cout << "Got frame: " << im->ni() << 'x' << im->nj() << '\n';
  }

  // Running backwards throught the image
  for (vidl_movie::frame_iterator pframe = movie->last();
       pframe >= movie->first();
       --pframe)
  {
    vil_image_view_base_sptr im = pframe->get_view();
    vcl_cout << "Got frame: " << im->ni() << 'x' << im->nj() << '\n';
  }

  // Backwards two at a time
  for (vidl_movie::frame_iterator pframe = movie->last();
       pframe >= movie->first();
       pframe -= 2)
  {
    vil_image_view_base_sptr im = pframe->get_view();
    vcl_cout << "Got frame: " << im->ni() << 'x' << im->nj() << '\n';
  }

  // Run over all pairs of images
  for (vidl_movie::frame_iterator pframe1 = movie->first();
       pframe1 <= movie->last();
       ++pframe1)
  {
    for (vidl_movie::frame_iterator pframe2 = movie->first();
         pframe2 < pframe1;
         ++pframe2)
    {
      // Run some test on the two images
    }
  }

  // Running over frames 10 to 20
  vidl_movie::frame_iterator pframe = movie->begin();
  for (int p=0; p<10 && pframe!=movie->end(); ++p) ++pframe;
  for (int p=10; p<=20 && pframe!=movie->end(); ++p,++pframe)
  {
    pframe->get_view();
  }

  return 0;
}
