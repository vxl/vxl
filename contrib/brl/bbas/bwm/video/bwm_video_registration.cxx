#include <bwm/video/bwm_video_registration.h>
#include <bwm/video/bwm_video_cam_istream.h>
#include <vcl_cmath.h>
#include <vul/vul_timer.h>
#include <vnl/vnl_matrix_fixed.h>
#include <vgl/algo/vgl_h_matrix_2d.h>
#include <vpgl/algo/vpgl_camera_homographies.h>
#include <brip/brip_vil_float_ops.h>
#include <vidl2/vidl2_frame.h>
#include <vidl2/vidl2_convert.h>
#include <vidl2/vidl2_istream.h>
#include <vidl2/vidl2_ostream.h>
#include <vsol/vsol_box_2d.h>
#include <vsol/vsol_polygon_2d.h>
#include <bsol/bsol_algs.h>

static vsol_polygon_2d_sptr poly_from_image_region(unsigned input_ni,
                                                   unsigned input_nj)
{
  vsol_box_2d_sptr input_roi;
  input_roi = new vsol_box_2d();
  input_roi->add_point(0, 0);
  input_roi->add_point(input_ni, input_nj);
  return bsol_algs::poly_from_box(input_roi);
}

bool bwm_video_registration::
output_frame_bounds_planar(bwm_video_cam_istream_sptr& cam_istream,
                           vgl_plane_3d<double> const& world_plane,
                           unsigned input_ni, unsigned input_nj,
                           vsol_box_2d_sptr& bounds,
                           double& world_sample_distance,
                           unsigned skip_frames
                           )
{
  vsol_polygon_2d_sptr input_poly = poly_from_image_region(input_ni, input_nj);
  bounds = new vsol_box_2d();
  cam_istream->seek_camera(0);
  unsigned current_frame = 0;
  double sd = 0;
  unsigned ncams = 0;
  vpgl_perspective_camera<double>* cam0 = cam_istream->current_camera();
  vgl_h_matrix_2d<double> H0 =
    vpgl_camera_homographies::homography_to_camera(*cam0, world_plane);

  //scan through the camera stream and determine the max min range
  //of projected image bounds. The input polygon is a rectangle and the
  //projected output polygon is an arbitrary quadrilateral
  do
  {
    vpgl_perspective_camera<double>* cam = cam_istream->current_camera();
    if (!cam) break;
    vgl_h_matrix_2d<double> H =
      vpgl_camera_homographies::homography_from_camera(*cam, world_plane);
    vsol_polygon_2d_sptr output_poly;
    if (!bsol_algs::homography(input_poly, H0*H, output_poly))
      return false;
    vsol_box_2d_sptr b = output_poly->get_bounding_box();
    //expand output box by the size of the mapped input box
    double minx = b->get_min_x(), miny = b->get_min_y();
    double maxx = b->get_max_x(), maxy = b->get_max_y();
    bounds->add_point(minx, miny);
    bounds->add_point(maxx, maxy);
    //the largest diagonal of the projected quadrilateral
    double w = b->width(), h = b->height();
    sd += vcl_sqrt(w*w + h*h);//accumulate for the average
    ncams++;
    current_frame +=skip_frames;
  }
  while (cam_istream->seek_camera(current_frame));
  if (!ncams) return false;
  sd/=ncams;//average diagonal of projected image
  double ini = input_ni, inj = input_nj;
  double in_diagonal = vcl_sqrt(ini*ini+inj*inj);
  world_sample_distance = sd/in_diagonal;
  return true;
}

bool convert_from_frame(vidl2_frame_sptr const& frame,
                        vcl_vector<vil_image_view<float> >& views)
{
  if (!frame) return false;

  if (frame->pixel_format() == VIDL2_PIXEL_FORMAT_MONO_16){
    static vil_image_view<vxl_uint_16> img;
    if (vidl2_convert_to_view(*frame,img))
      views.push_back(brip_vil_float_ops::convert_to_float(img));
    else
      return false;
  }
  else{
    static vil_image_view<vxl_byte> img;
    if (vidl2_convert_to_view(*frame,img,VIDL2_PIXEL_COLOR_RGB)) {
      unsigned ni = img.ni(), nj = img.nj();
      for (unsigned p = 0; p<img.nplanes(); ++p)
      {
        vil_image_view<float> view(ni, nj);
        for (unsigned j = 0; j<nj; ++j)
          for (unsigned i = 0; i<ni; ++i)
            view(i,j) = img(i,j,p);
        views.push_back(view);
      }
    }
    else
      return false;
  }
  return true;
}

static vidl2_frame_sptr
convert_to_frame(vcl_vector<vil_image_view<float> >const&  views)
{
  unsigned np = views.size();
  if (!np)
    return 0;
  unsigned ni = views[0].ni(), nj = views[0].nj();
  vcl_vector<vil_image_view<unsigned char> > cviews;
  for (unsigned p = 0; p<np; ++p)
    cviews.push_back(brip_vil_float_ops::convert_to_byte(views[p]));

  vil_image_view<unsigned char> out_view(ni, nj, np);
  for (unsigned j = 0; j<nj; ++j)
    for (unsigned i = 0; i<ni; ++i)
      for (unsigned p = 0; p<np; ++p)
        out_view(i, j, p) = cviews[p](i,j);

  return  new vidl2_memory_chunk_frame(out_view);
}

bool bwm_video_registration::
register_image_stream_planar(vidl2_istream_sptr& in_stream,
                             bwm_video_cam_istream_sptr& cam_istream,
                             vgl_plane_3d<double> const& world_plane,
                             vsol_box_2d_sptr const& bounds,
                             double world_sample_distance,
                             vidl2_ostream_sptr& out_stream,
                             unsigned skip_frames
                             )
{
  if (!in_stream || !cam_istream || !out_stream)
    return false;
  if (!in_stream->seek_frame(0)|| !cam_istream->seek_camera(0))
    return false;
  if (!bounds)
    return false;
  unsigned current_frame = 0;
  vpgl_perspective_camera<double>* cam0 = cam_istream->current_camera();
  vgl_h_matrix_2d<double> H0 =
    vpgl_camera_homographies::homography_to_camera(*cam0, world_plane);
  // the size of the output image is defined by the bounds and the
  // ground sample distance
  double w = bounds->width(), h = bounds->height();
  w/=world_sample_distance;    h/=world_sample_distance;
  unsigned out_ni = static_cast<unsigned>(w);
  unsigned out_nj = static_cast<unsigned>(h);

  vnl_matrix_fixed<double,3, 3> t;
  t[0][0]=1;  t[0][1]=0; t[0][2]=-bounds->get_min_x();
  t[1][0]=0;  t[1][1]=1; t[1][2]=-bounds->get_min_y();
  t[2][0]=0;  t[2][1]=0; t[2][2]=world_sample_distance;
  unsigned f = 0;
  do
  {
    vul_timer tim;
    vidl2_frame_sptr frame = in_stream->current_frame();
    vcl_vector<vil_image_view<float> > float_vs;
    if (!convert_from_frame(frame, float_vs)) return false;
    vpgl_perspective_camera<double>* cam = cam_istream->current_camera();
    if (!cam)
      return false;
    vgl_h_matrix_2d<double> tem =
      vpgl_camera_homographies::homography_from_camera(*cam, world_plane);
    vgl_h_matrix_2d<double> H = H0*tem;
    vnl_matrix_fixed<double,3, 3> Mt = H.get_matrix();
    vnl_matrix_fixed<double,3,3> Msh;
    Msh = t*Mt;
    vgl_h_matrix_2d<double> Hsh(Msh);
    vcl_vector<vil_image_view<float> > out_vs;
    for (unsigned p = 0; p<float_vs.size(); ++p){
      vil_image_view<float> out_view(out_ni, out_nj);
      if (!brip_vil_float_ops::homography(float_vs[p], Hsh, out_view, true))
        return false;
      out_vs.push_back(out_view);
    }
    vidl2_frame_sptr oframe = convert_to_frame(out_vs);
    if (!oframe)
      return false;
    if (!out_stream->write_frame(oframe))
      return false;
    current_frame +=skip_frames;
    vcl_cout << "Registered frame[" << current_frame << "](" << out_ni
             << " x " << out_nj << ") in " << tim.real() << " milliseconds\n"
             << vcl_flush;
  }
  while (in_stream->seek_frame(current_frame) &&
         cam_istream->seek_camera(current_frame));
  return true;
}
