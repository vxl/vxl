#include <vcl_vector.h>
#include <vcl_iostream.h>

#include <vil/vil_image.h>
#include <vil/vil_load.h>

#include <vtol/vtol_vertex_2d_sptr.h>

#include <vgel/vgel_kl.h>
#include <vgel/vgel_multi_view_data.h>

int main( int argc, char **argv)
{
  vgel_kl_params kl_params;
  kl_params.numpoints=500;

  vcl_vector<vil_image> images;

  vil_image im0= vil_load("c:/geoff/images/skull/a.000.jpg");
  vil_image im1= vil_load("c:/geoff/images/skull/a.001.jpg");
  vil_image im2= vil_load("c:/geoff/images/skull/a.002.jpg");

  images.push_back( im0);
  images.push_back( im1);
  images.push_back( im2);

  vgel_multi_view_data_vertex_sptr matched_points;
  matched_points=new vgel_multi_view_data<vtol_vertex_2d_sptr>( 3);

  vgel_kl kl_points(kl_params);
  kl_points.match_sequence(images,matched_points);

  ////////////

  vcl_vector<vtol_vertex_2d_sptr> pts1;
  vcl_vector<vtol_vertex_2d_sptr> pts2;
  vcl_vector<vtol_vertex_2d_sptr> pts3;

  matched_points->get( 0,pts1);
  matched_points->get( 1,pts2);
  matched_points->get( 2,pts3);

  vcl_cerr << pts1.size() << " "
           << pts2.size() << " "
           << pts3.size() << vcl_endl;

  return 0;
}
