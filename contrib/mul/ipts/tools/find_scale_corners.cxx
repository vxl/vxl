// Find all local peaks in DoG scale space of an image

#include <vimt/vimt_scale_pyramid_builder_2d.h>
#include <vimt/vimt_image_pyramid.h>
#include <vil/vil_convert.h>
#include <vil/algo/vil_corners.h>
#include <vul/vul_arg.h>
#include <vil/vil_load.h>
#include <vil/vil_save.h>
#include <ipts/ipts_scale_space_peaks.h>

void print_usage()
{
  vcl_cout<<"find_scale_corners -i input_image -s scale_step -o out_image -c corner_pyramid\n"
          <<"Generates scale pyramid, computes corners and looks for local peaks\n";
}

template<class T>
void draw_cross(vil_image_view<T>& image, unsigned x, unsigned y, 
                unsigned s, T value)
{
  for (unsigned i=0;i<s;++i)
  {
    image(x,y+i)=value;
    image(x,y-i)=value;
    image(x-i,y)=value;
    image(x+i,y)=value;
  }
}

int main( int argc, char* argv[] )
{
  vul_arg<vcl_string> in_path("-i","Input image path");
  vul_arg<vcl_string> out_path("-o","Output image file (peaks)");
  vul_arg<vcl_string> corner_path("-c","Output image file (corners)");
  vul_arg<double> scale("-s","Scale step",1.41);

//  vul_arg<vcl_string> smooth_path("-s","Output image file (Smooth )","smooth.pnm");
  vul_arg_parse(argc, argv);

  if (in_path() == "")
  {
    print_usage();
    vul_arg_display_usage_and_exit();
  }

  vil_image_view<vxl_byte> image = vil_load(in_path().c_str());
  if (image.ni()==0)
  {
    vcl_cout<<"Failed to load image.\n";
    return 1;
  }

  vimt_image_2d_of<float> image_f,flat_corners,flat_smooth;
  vil_convert_cast(image,image_f.image());

  vimt_image_pyramid smooth_pyramid,corner_pyramid;
  vimt_scale_pyramid_builder_2d<float> pyr_builder;
  pyr_builder.set_scale_step(scale());
  pyr_builder.build(smooth_pyramid,image_f);

  // Compute corners for all levels of an image pyramid
  corner_pyramid.resize(smooth_pyramid.n_levels(),vimt_image_2d_of<float>());
  for (int i=0;i<smooth_pyramid.n_levels();++i)
  {
    vimt_image_2d_of<float>& smooth_im
            = static_cast<vimt_image_2d_of<float>&>(smooth_pyramid(i));
    vimt_image_2d_of<float>& corner_im
            = static_cast<vimt_image_2d_of<float>&>(corner_pyramid(i));
    corner_im.set_world2im(smooth_im.world2im());
    vil_corners(smooth_im.image(),corner_im.image());
  }

  vcl_vector<vgl_point_3d<double> > peak_pts;
  ipts_scale_space_peaks_2d(peak_pts,corner_pyramid,float());
  vcl_cout<<"Found "<<peak_pts.size()<<" peaks.\n";

  for (unsigned i=0;i<peak_pts.size();++i)
  {
    if (peak_pts[i].z()>1.1)
    draw_cross(image,unsigned(peak_pts[i].x()+0.5),
                     unsigned(peak_pts[i].y()+0.5),
                     unsigned(peak_pts[i].z()+0.5), vxl_byte(255) );
  }

  vimt_image_pyramid_flatten(flat_corners,corner_pyramid);
//  vimt_image_pyramid_flatten(flat_smooth,smooth_pyramid);

  vil_save(image,out_path().c_str());
  vcl_cout<<"Image + pts saved to "<<out_path()<<vcl_endl;


  if (corner_path()!="")
  {
    vil_image_view<vxl_byte> out_corners;
    vil_convert_stretch_range(flat_corners.image(),out_corners);
    vil_save(out_corners,corner_path().c_str());
    vcl_cout<<"Corner pyramid saved to "<<corner_path()<<vcl_endl;
  }
#if 0
  vil_image_view<vxl_byte> out_smooth;
  vil_convert_stretch_range(flat_smooth.image(),out_smooth);
  vil_save(out_smooth,smooth_path().c_str());
  vcl_cout<<"Smooth pyramid saved to "<<smooth_path()<<vcl_endl;
#endif // 0
  return 0;
}
