// Find all local peaks in entropy of scale space of an image

#include <vimt/vimt_scale_pyramid_builder_2d.h>
#include <vimt/vimt_image_pyramid.h>
#include <vil/vil_convert.h>
#include <vul/vul_arg.h>
#include <vil/vil_load.h>
#include <vil/vil_save.h>
#include <ipts/ipts_scale_space_peaks.h>
#include <ipts/ipts_local_entropy.h>
#include <ipts/ipts_draw.h>

void print_usage()
{
  vcl_cout<<"find_scale_entropy_peaks -i input_image -s scale_step -h half_width -o out_image -e entropy_pyramid\n"
          <<"Generates scale pyramid"<<vcl_endl
          <<"computes local entropy in squares with given half width"<<vcl_endl
          <<"looks for local peaks in the entropy pyramid"<<vcl_endl;
}

int main( int argc, char* argv[] )
{
  vul_arg<vcl_string> in_path("-i","Input image path");
  vul_arg<vcl_string> out_path("-o","Output image file (peaks)","output.pnm");
  vul_arg<int> half_width("-h","Half-width of square for entropy measurements",5);
  vul_arg<vcl_string> entropy_path("-e","Output image file for entropy pyramid)");
  vul_arg<double> scale("-s","Scale step",1.41);

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

  vimt_image_2d_of<vxl_byte> image0,flat_smooth;
  vimt_image_2d_of<float> flat_entropy;

  image0.image() = image;  // Wrap vil_image_view in a vimt_image

  vimt_image_pyramid smooth_pyramid,entropy_pyramid;
  vimt_scale_pyramid_builder_2d<vxl_byte> pyr_builder;
  pyr_builder.set_scale_step(scale());
  pyr_builder.build(smooth_pyramid,image0);

  // Entropy calculation translates results - allow for this
  vimt_transform_2d translate;
  translate.set_translation(-half_width(),-half_width());

  // Work out how many levels and be used
  int n_levels = 0;
  for (int i=0;i<smooth_pyramid.n_levels();++i)
  {
    vimt_image_2d_of<vxl_byte>& smooth_im
            = static_cast<vimt_image_2d_of<vxl_byte>&>(smooth_pyramid(i));
     if (smooth_im.image().ni()>2*half_width()+1 &&
         smooth_im.image().nj()>2*half_width()+1)  n_levels++;
  }

  // Compute entropys for all levels of an image pyramid
  entropy_pyramid.resize(n_levels,vimt_image_2d_of<float>());
  for (int i=0;i<n_levels;++i)
  {
    vimt_image_2d_of<vxl_byte>& smooth_im
            = static_cast<vimt_image_2d_of<vxl_byte>&>(smooth_pyramid(i));
    vimt_image_2d_of<float>& entropy_im
            = static_cast<vimt_image_2d_of<float>&>(entropy_pyramid(i));
    ipts_local_entropy(smooth_im.image(),entropy_im.image(),half_width());
    entropy_im.set_world2im(translate*smooth_im.world2im());
 }

  vcl_vector<vgl_point_3d<double> > peak_pts;
  ipts_scale_space_peaks_2d(peak_pts,entropy_pyramid,float());
  vcl_cout<<"Found "<<peak_pts.size()<<" peaks.\n";

  for (unsigned i=0;i<peak_pts.size();++i)
  {
    if (peak_pts[i].z()>1.1)
    ipts_draw_cross(image,int(peak_pts[i].x()+0.5),
                     int(peak_pts[i].y()+0.5),
                     int(peak_pts[i].z()+0.5), vxl_byte(255) );
  }

  vimt_image_pyramid_flatten(flat_entropy,entropy_pyramid);
//  vimt_image_pyramid_flatten(flat_smooth,smooth_pyramid);

  vil_save(image,out_path().c_str());
  vcl_cout<<"Image + pts saved to "<<out_path()<<vcl_endl;


  if (entropy_path()!="")
  {
    vil_image_view<vxl_byte> out_entropy;
    vil_convert_stretch_range(flat_entropy.image(),out_entropy);
    vil_save(out_entropy,entropy_path().c_str());
    vcl_cout<<"entropy pyramid saved to "<<entropy_path()<<vcl_endl;
  }
#if 0
  vil_image_view<vxl_byte> out_smooth;
  vil_convert_stretch_range(flat_smooth.image(),out_smooth);
  vil_save(out_smooth,smooth_path().c_str());
  vcl_cout<<"Smooth pyramid saved to "<<smooth_path()<<vcl_endl;
#endif // 0
  return 0;
}
