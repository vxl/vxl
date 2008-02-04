#include <bwm/video/bwm_video_corr_processor.h>
#include <bwm/video/bwm_video_site_io.h>
#include <bwm/video/bwm_video_corr.h>
#include <vcl_cmath.h>
#include <vnl/vnl_numeric_traits.h>
#include <vnl/vnl_double_3.h>
#include <vnl/algo/vnl_levenberg_marquardt.h>
#include <vnl/algo/vnl_amoeba.h>
#include <vul/vul_file.h>
#include <vpl/vpl.h> // vpl_unlink()
#include <vpgl/algo/vpgl_interpolate.h>
#include <vpgl/algo/vpgl_bundle_adjust.h>
#include <vgl/vgl_homg_point_3d.h>
#include <vgl/vgl_distance.h>
#include <vgl/algo/vgl_rotation_3d.h>

#include <bsta/bsta_histogram.h>
#include <brip/brip_vil_float_ops.h>
#include <bwm/video/bwm_video_cam_istream.h>
#include <bwm/video/bwm_video_cam_ostream.h>
#include <vidl2/vidl2_frame.h>
#include <vidl2/vidl2_convert.h>
#include <vidl2/vidl2_image_list_istream.h>
//Minimum number of correspondences on a frame to compute cameras
static const unsigned min_corrs = 10;
// if an element of pixels is negative it indicates the point was outside the
// image
static void extract_window(float u, float v, int radius,
                           vil_image_view<float> image,
                           vcl_vector<float>& pixels)
{
  pixels.clear();
  float umin = u-radius, umax = u+radius;
  float vmin = v-radius, vmax = v+radius;
  int ni = image.ni(), nj = image.nj();
  for (float vi = vmin; vi<=vmax; ++vi)
    for (float ui = umin; ui<=umax; ++ui)
      if (ui<0||vi<0||ui>=ni||vi>=nj)
        pixels.push_back(-1.0f);
      else{
        double val = brip_vil_float_ops::bilinear_interpolation(image, ui, vi);
        pixels.push_back(static_cast<float>(val));
      }
}

bwm_video_corr_lsqr_cost_func::
bwm_video_corr_lsqr_cost_func(vil_image_view<float> const& base_image,
                              unsigned match_radius,
                              vcl_vector<float> corr_window_ab
                              )
  : vnl_least_squares_function(1,1), base_image_(base_image),
    match_radius_(match_radius), corr_window_ab_(corr_window_ab)
{
  use_gradient_ = false;
  // a mxm image region centered on the projected world pointa
  unsigned m = (2*match_radius_+1);
  vnl_least_squares_function::init(2, m*m);
}

//the least squares cost function for minimizing intensity differences
// x is a 2 element vector for correspondence position
// f is an  M-element vector, where M = (2*match_radius_+1)^2
// i.e. a window around each projected world point.
void bwm_video_corr_lsqr_cost_func::f(vnl_vector<double> const& x,
                                      vnl_vector<double>& fx)
{
  float u = static_cast<float>(x[0]);
  float v = static_cast<float>(x[1]);
  vcl_vector<float> pixels;
  extract_window(u, v, match_radius_, base_image_, pixels);
  unsigned msq = pixels.size();
  for (unsigned k = 0; k<msq; ++k)
    if (corr_window_ab_[k]<0||pixels[k]<0)
      fx[k] = 0.0;
    else
      fx[k] = corr_window_ab_[k] - pixels[k];
#if 0
  vcl_cout << "\nX = (" << x[0] << ' ' << x[1] << '\n'
           << "f\n";
  vcl_cout.precision(3);
  for (unsigned i = 0; i< fx.size(); ++i)
    vcl_cout<< fx[i] << ' ';
  vcl_cout << '\n';
#endif
}

bwm_video_corr_cost_function::
bwm_video_corr_cost_function(vil_image_view<float> const& base_image,
                             unsigned match_radius,
                             vcl_vector<float> corr_window_ab)
  : vnl_cost_function(2), base_image_(base_image),
    match_radius_(match_radius), corr_window_ab_(corr_window_ab)
{}

double bwm_video_corr_cost_function::f(vnl_vector<double> const& x)
{
  float u = static_cast<float>(x[0]);
  float v = static_cast<float>(x[1]);
  double r = 0;
  vcl_vector<float> pixels;
  extract_window(u, v, match_radius_, base_image_, pixels);
  unsigned msq = pixels.size();
  for (unsigned k = 0; k<msq; ++k)
    if (corr_window_ab_[k]<0||pixels[k]<0)
      continue;
    else {
      double res = corr_window_ab_[k] - pixels[k];
      r += res*res;
    }
  return r;
}

bool bwm_video_corr_processor::open_video_stream(vcl_string const& video_path)
{
  if (video_path=="")
    return false;
  //for now assume we are opening an image_list codec
  video_istr_ = new vidl2_image_list_istream(video_path);
  bool open = video_istr_->is_open();
  if (open)
    video_istr_->seek_frame(0);
  return open;
}

bool bwm_video_corr_processor::
open_camera_istream(vcl_string const& camera_path)
{
  if (camera_path=="")
    return false;
  cam_istr_ = new bwm_video_cam_istream(camera_path);
  bool open = cam_istr_->is_open();
  if (open)
    cam_istr_->seek_camera(0);
  return open;
}

bool bwm_video_corr_processor::
open_camera_ostream(vcl_string const& camera_path)
{
  if (camera_path=="")
    return false;
  cam_ostr_ = new bwm_video_cam_ostream(camera_path);
  bool open = cam_ostr_->is_open();
  return open;
}

void remove_dir(vcl_string const& dir)
{
  vcl_string glob = dir + "/*";
  vul_file::delete_file_glob(glob.c_str());
  vpl_unlink(dir.c_str());
}

bool bwm_video_corr_processor::open_video_site(vcl_string const& site_path,
                                               bool cameras_exist)
{
  if (!site_io_.open(site_path))
    return false;
  site_path_ = site_path;
  site_name_ = site_io_.name();
  video_path_ = site_io_.video_path();
  camera_path_ = site_io_.camera_path();
  if (!this->open_video_stream(video_path_))
    return false;
  if (cameras_exist){
    if (!this->open_camera_istream(camera_path_))
      return false;
  }else{
    vcl_string dir = vul_file::dirname(site_path)+"/cameras";
    if (vul_file::exists(dir))
      remove_dir(dir);
    if (!vul_file::make_directory_path(dir))
      return false;
    camera_path_ = dir+"/*";
    if (!this->open_camera_ostream(dir))
      return false;
  }
  vcl_vector<bwm_video_corr_sptr> corrs = site_io_.corrs();
  if (!corrs.size())
    return false;
  this->set_correspondences(corrs);
  return true;
}

bool bwm_video_corr_processor::write_video_site(vcl_string const& site_path)
{
  if (site_name_ == "")
    return false;
  bwm_video_site_io sio;
  sio.set_name(site_name_);
  sio.set_video_path(video_path_);
  sio.set_camera_path(camera_path_);
  sio.set_corrs(corrs_);
  sio.x_write(site_path);
  return true;
}

bool bwm_video_corr_processor::
interpolate_cameras(vcl_vector<vpgl_perspective_camera<double> > known_cams,
                    vcl_vector<bool> unknown,
                    vcl_vector<vpgl_perspective_camera<double> >& interp_cams)
{
  unsigned ncameras = unknown.size();
  interp_cams.resize(ncameras);

  //fill in the known cameras
  unsigned ne = unknown.size();
  unsigned count = 0;
  for (unsigned fi = 0; fi<ne; ++fi)
    if (!unknown[fi])
      interp_cams[fi]=known_cams[count++];
  //then interpolate the missing cameras
  count = 0;
  for (unsigned fi = 0; fi<ne; ++fi)
  {
    if (unknown[fi])//row is empty
    {
      // case 1 - i == 0
      if (fi==0)
      {
        // find the smallest row that is not empty
        bool found = false;
        for (unsigned j = 1; j<ne&&!found; ++j)
          if (!unknown[j])
          {
            found = true;
            for (unsigned k = 0; k<j; ++k)
              interp_cams[k] = known_cams[0];
            fi = j;
          }
        if (!found){
          vcl_cerr << "Shouldn't happen\n";
          return false;}
      }
      // case 2 - i == ne-1 (the last row)
      if (fi == ne-1)//last known camera
        interp_cams[fi] = known_cams[known_cams.size()-1];

      // case 3 - the normal case
      // typically there will be a run of missing cameras
      // also the cameras are filled in up to fi-1;
      // find the next known camera
      bool found = false;
      unsigned ki = fi+1;
      for (; ki<ne&&!found; ++ki)
        if (!unknown[ki])
          found = true;
      if (!found){
        vcl_cerr << "Shouldn't happen\n";
        return false;}
      // get the cameras at fi-1 and next solved camera
      vpgl_perspective_camera<double> c0 = interp_cams[fi-1];
      vpgl_perspective_camera<double> c1 = interp_cams[ki-1];
      vcl_vector<vpgl_perspective_camera<double> > icams;
      unsigned n_interp = ki-fi-1;
      if (verbose_)
        vcl_cout << "Interpolating between frames[" << fi-1
                 << ':' << ki-1 << "]\n";
      if (!vpgl_interpolate::interpolate(c0, c1, n_interp, icams))
      {
        vcl_cerr << "Interpolation failed\n";
        return false;
      }
      for (unsigned c = 0; c<icams.size(); ++c)
        interp_cams[fi+c] = icams[c];
      fi = ki-1;
    }
  }
  if (verbose_){
    vcl_cout << "Solved and Interpolated Cameras\n";
    for (unsigned c = 0; c<ncameras; ++c){
      vgl_point_3d<double> p =interp_cams[c].get_camera_center();
      vgl_rotation_3d<double> R = interp_cams[c].get_rotation();
      vcl_cout << "C[" << c << "]("<< R << '|' << p.x() << ' ' << p.y() << ' '
               << p.z() <<  ")\n"; }
  }
  return true;
}

void min_max_frame(vcl_vector<bwm_video_corr_sptr> const& corrs,
                   unsigned& min_frame, unsigned& max_frame)
{
  min_frame = vnl_numeric_traits<unsigned>::maxval;
  max_frame = 0;
  for (vcl_vector<bwm_video_corr_sptr>::const_iterator cit = corrs.begin();
       cit != corrs.end(); ++cit)
    {
      unsigned minf = (*cit)->min_frame();
      unsigned maxf = (*cit)->max_frame();
      if (minf<min_frame)
        min_frame = minf;
      if (maxf>max_frame)
        max_frame = maxf;
    }
}

//
//compute the mask on existing correspondences
//the mask is m x n , where m = number of cameras and
//n = number of world points (corrs)
//
void bwm_video_corr_processor::mask(unsigned& min_frame, unsigned& max_frame,
                                    vcl_vector<vcl_vector<bool> >& mask)
{
  //get the start and end frame numbers. assume contiguous frames inbetween
  min_frame =0, max_frame = 0;
  min_max_frame(corrs_, min_frame, max_frame);
  if (verbose_)
    vcl_cout << "Start frame = " << min_frame << " End frame = "
             << max_frame << '\n';
  unsigned ncameras = max_frame-min_frame +1;
  unsigned npoints = corrs_.size();
  vcl_vector<bool> init(npoints);
  mask.resize(ncameras, init);

  for (unsigned w = 0; w<npoints; ++w){
    unsigned f_rel = 0;//relative frame number
    for (unsigned f = min_frame; f<=max_frame; ++f, ++f_rel)
    {
      vgl_point_2d<double> pt;
      mask[f_rel][w] = corrs_[w]->match(f, pt);
    }
  }
}

bool bwm_video_corr_processor::
initialize_world_pts_and_cameras(vpgl_calibration_matrix<double> const& K,
                                 double initial_depth)
{
  unsigned min_frame =0, max_frame = 0;
  vcl_vector<vcl_vector<bool> > mask;
  this->mask(min_frame, max_frame, mask);
  //The implementation is not general enough yet to handle
  //a non-zero start frame
  if (min_frame)
    return false;
  unsigned ncameras = max_frame-min_frame +1;
  unsigned npoints = corrs_.size();
  if (verbose_)
    vcl_cout << "Initializing " << ncameras << " cameras on " << npoints
             << " correspondences\n";
  vcl_vector<vgl_point_2d<double> > image_points(ncameras*npoints);
  for (unsigned w = 0; w<npoints; ++w)
  {
    unsigned f_rel = 0;//relative frame num == 0 at min_frame
    for (unsigned f = min_frame; f<=max_frame; ++f, ++f_rel)
    {
      vgl_point_2d<double> pt;
      if (corrs_[w]->match(f, pt))
        image_points[f_rel*npoints + w] = pt;
    }
  }
  //find the number of rows that are completely masked and
  //remove them creating a smaller problem
  vcl_vector<bool> unknown_frames(ncameras, false);
  int filled_rows = 0;
  for (unsigned f_rel = 0; f_rel<ncameras; ++f_rel){
    bool empty = true;
    for (unsigned w = 0; w<npoints; ++w)
      if (mask[f_rel][w]) empty = false;
    if (empty)
      unknown_frames[f_rel] = true;
    else
      ++filled_rows;
  }
  if (verbose_)
    vcl_cout << "Executing bundle adjustment on " << filled_rows
             << " cameras\n";
  //create a new mask array and image point vector for the smaller problem
  vcl_vector<bool> init(npoints);
  vcl_vector<vcl_vector<bool> > cmask(filled_rows, init);
  vcl_vector<vgl_point_2d<double> > cimage_points;
  unsigned ff = 0;
  for (unsigned f = 0; f<ncameras; ++f)
    if (!unknown_frames[f])
    {
      unsigned cnt = 0;
      for (unsigned w = 0; w<npoints; ++w){
        cmask[ff][w] = mask[f][w];
        if (mask[f][w]){ cnt++;
        cimage_points.push_back(image_points[f*npoints + w]);}
      }
      if (cnt<min_corrs){
        vcl_cout << "NCorrs[" << f << "]= " << cnt << '\n';
        for (unsigned w = 0; w<npoints; ++w)
          if (mask[f][w])
          {
            vgl_point_2d<double> pt;
            if (corrs_[w]->match(f, pt))
              vcl_cout << "bad corr " << pt << '\n';
          }
      }
      ++ff;
    }
#if 0
  //sanity check on image points
  unsigned kk = 0;
  for (unsigned ff = 0; ff<filled_rows; ++ff)
    for (unsigned w = 0; w<npoints; ++w)
      if (cmask[ff][w]){
        vcl_cout << "Image Point[" << ff << "][" << w
                 << "]=" << cimage_points[kk] << '\n';
        kk++;
      }
#endif
  vnl_double_3 r(0,0,0.000001);
  vgl_rotation_3d<double> I(r); // small initial rotation
  vgl_homg_point_3d<double> center(0.0, 0.0, -initial_depth);
  vpgl_perspective_camera<double> default_camera(K, center, I);

  vcl_vector<vpgl_perspective_camera<double> > unknown_cameras(filled_rows,
                                                               default_camera);
  // initialize unknown world points
  vgl_point_3d<double> pun(0.0, 0.0, 0.0);
  vcl_vector<vgl_point_3d<double> > unknown_world(npoints,pun);

  // exectute the bundle adjustment
  bool success = vpgl_bundle_adjust::optimize(unknown_cameras,
                                              unknown_world, cimage_points,
                                              cmask);
  //save the solved world points in the correspondences
  for (unsigned w = 0; w<npoints; ++w)
  {
    vgl_point_3d<double> pt = unknown_world[w];
    corrs_[w]->set_world_pt(pt);
  }
  //WARNING!! THIS FUNCTION NOT VALID FOR MIN_FRAME != 0
  if (!this->interpolate_cameras(unknown_cameras, unknown_frames, cameras_))
    return false;
  return true;
}

bool bwm_video_corr_processor::write_cameras_to_stream()
{
  if (cam_ostr_&&cam_ostr_->is_open()){
    for (unsigned i = 0; i<cameras_.size(); ++i)
      cam_ostr_->write_camera(&cameras_[i]);
    return true;
  }
  return false;
}

bool bwm_video_corr_processor::frame_at_index(unsigned frame_index,
                                              vil_image_view<float>& view)
{
  // extract the image from the frame
  if (!video_istr_) return false;
  video_istr_->seek_frame(frame_index);
  vidl2_frame_sptr frame = video_istr_->current_frame();
  if (!frame)
  {
    vcl_cerr << "Failed to seek to frame " << frame_index << '\n';
    return false;
  }
  else if (frame->pixel_format() == VIDL2_PIXEL_FORMAT_MONO_16){
    static vil_image_view<vxl_uint_16> img;
    if (vidl2_convert_to_view(*frame,img))
      view = brip_vil_float_ops::convert_to_float(img);
    else{
      vcl_cerr << "Failed to convert frame to vil_image_view\n";
      return false;
    }
  }
  else{
    static vil_image_view<vxl_byte> img;
    if (vidl2_convert_to_view(*frame,img,VIDL2_PIXEL_COLOR_RGB))
      view = brip_vil_float_ops::convert_to_float(img);
    else{
      vcl_cerr << "Failed to convert frame to vil_image_view\n";
      return false;
    }
  }
  return true;
}

// compute the set of image samples around each correspondence
// at the start frame and end frame of a tracking interval (a, b).
void bwm_video_corr_processor::
compute_ab_corr_windows(unsigned match_radius,
                        vcl_vector<bool> const& mask_a,
                        vcl_vector<bool> const& mask_b)
{
  //n is the number of correspondences to find, i.e. determine (u, v) for each.
  unsigned n = corrs_.size();
  vgl_point_2d<double> ini(-1, -1);
  vcl_vector<vgl_point_2d<double> > corrs_a(n, ini), corrs_b(n, ini);
  for (unsigned i = 0; i<n; ++i){
    if (!(mask_a[i]&&mask_b[i])) continue;
    bwm_video_corr_sptr c = corrs_[i];
    vgl_point_2d<double> ipt;
    if (c->match(frame_index_a_, ipt)) corrs_a[i]=ipt;
    else continue;
    if (c->match(frame_index_b_, ipt)) corrs_b[i]=ipt;
    else continue;
  }
  //extract the correspondence image windows
  //bilinear interpolation is used to sample the input images
  for (unsigned i = 0; i<n; ++i)
  {
    if (!(mask_a[i]&&mask_b[i])) continue;
    vgl_point_2d<double> ipta = corrs_a[i], iptb = corrs_b[i];
    if (ipta.x()<0||ipta.y()<0||iptb.x()<0||iptb.y()<0)
      continue;
    float ua = static_cast<float>(ipta.x());
    float va = static_cast<float>(ipta.y());
    vcl_vector<float> pixels_a;
    extract_window(ua, va, match_radius, image_a_, pixels_a);
    float ub = static_cast<float>(iptb.x());
    float vb = static_cast<float>(iptb.y());
    vcl_vector<float> pixels_b;
    extract_window(ub, vb, match_radius, image_b_, pixels_b);
    corr_windows_a_.push_back(pixels_a);
    corr_windows_b_.push_back(pixels_b);
  }
}

void bwm_video_corr_processor::
set_correspondences(vcl_vector<bwm_video_corr_sptr> const& corrs)
{
  corrs_ = corrs;
}

void bwm_video_corr_processor::
exhaustive_init(vnl_vector<double>& position,
                unsigned win_radius,
                unsigned search_radius,
                vil_image_view<float> const& base,
                vcl_vector<float> corr_win,
                double& start_error,
                double& end_error)
{
  float u0 = static_cast<float>(position[0]);
  float v0 = static_cast<float>(position[1]);
  float uf = 0, vf = 0;
  float umin = u0-search_radius, umax = u0+search_radius;
  float vmin = v0-search_radius, vmax = v0+search_radius;
  if (umin < 0) umin = 0;
  float ni = static_cast<float>(base.ni()), nj = static_cast<float>(base.nj());
  if (umax>=ni) umax = ni-1;
  if (vmin < 0) vmin = 0;
  if (vmax>=nj) vmax = nj-1;
  unsigned pix_cnt = 0;
  double rmin = vnl_numeric_traits<double>::maxval;
  for (float v = vmin; v<=vmax; ++v)
    for (float u = umin; u<=umax; ++u)
    {
      vcl_vector<float> pixels;
      extract_window(u, v, win_radius, base, pixels);
      unsigned msq = pixels.size();
      double r = 0;
      for (unsigned k = 0; k<msq; ++k)
        if (corr_win[k]<0||pixels[k]<0)
          continue;
        else {
          double res = corr_win[k] - pixels[k];
          r += res*res;
          pix_cnt++;
        }
      if (pix_cnt)
        r/=pix_cnt;
      else
        return;
      if (u == u0&&v == v0)
        start_error = vcl_sqrt(r);
      if (r<rmin){
        rmin = r;
        uf = u; vf = v;
      }
    }
  end_error = vcl_sqrt(rmin);
  if (uf == umin || uf == umax || vf == vmin || vf == vmax)
    end_error = -1.0;

  position[0]=uf;   position[1]=vf;
}

// it is assumed that there is a correct correspondence at frame a,
// and at frame b. The approach is to minimize the sum of squared
// differences between the image intensities of the known correspondences
// and the missing correspondence at each frame between the known frames.
// The search starts at the projected world point location in each
// between frame
bool bwm_video_corr_processor::find_missing_corrs(unsigned frame_index_a,
                                                  vcl_vector<bool> mask_a,
                                                  unsigned frame_index_b,
                                                  vcl_vector<bool> mask_b,
                                                  unsigned frame_index_x,
                                                  unsigned win_radius,
                                                  unsigned search_radius,
                                                  bool use_lmq)
{
  unsigned n = corrs_.size();
  if (!n) return false;
  //all correspondences must have a world point for the
  //algorithm to work
  if (!world_pts_valid_){
    world_pts_valid_ = true;
    for (unsigned i = 0; i<n&&world_pts_valid_; ++i)
    {
      bwm_video_corr_sptr c = corrs_[i];
      if (c->world_pt_valid())
        world_pts_.push_back(c->world_pt());
      else
        world_pts_valid_ = false;
    }
  }
  if (!world_pts_valid_){
    vcl_cerr << " world points not valid\n";
      return false;
  }
  //check for index bounds consistency
  if (frame_index_a>=frame_index_b ||
     frame_index_x <= frame_index_a ||
     frame_index_x >= frame_index_b)
    return false;
  //bounding frames may already be cached
  bool compute_ab = false;
  if (frame_index_a != frame_index_a_)
  {
    if (!this->frame_at_index(frame_index_a, image_a_))
      return false;
    frame_index_a_ = frame_index_a;
    compute_ab = true;
  }
  if (frame_index_b != frame_index_b_)
  {
    if (!this->frame_at_index(frame_index_b, image_b_))
      return false;
    frame_index_b_ = frame_index_b;
    compute_ab = true;
  }
  if (verbose_)
    vcl_cout << "Finding correspondences in frame interval ["
             << frame_index_a << ':' << frame_index_b << "]\n";
  if (compute_ab)
    this->compute_ab_corr_windows(win_radius, mask_a, mask_b);
  vil_image_view<float> fvx;
  if (!this->frame_at_index(frame_index_x, fvx))
    return false;
  //get the camera at frame x
  if (!cam_istr_) {
    vcl_cerr << "No camera stream\n";
    return false;
  }
  if (!cam_istr_->seek_camera(frame_index_x)){
    vcl_cerr << "Can't seek to camera frame " << frame_index_x << '\n';
    return false;
  }
  vpgl_perspective_camera<double>* camx = cam_istr_->current_camera();

  double nres = 2*win_radius +1;
  nres *= nres;
  //to find the nearest frame to the search frame
  unsigned da = frame_index_x - frame_index_a;
  unsigned db = frame_index_b - frame_index_x;
  for (unsigned i = 0; i<n; ++i)
  {
    //check if the correspondence is present on frame a and on frame b
    if ( !(mask_a[i] && mask_b[i]) )
      continue;
    vnl_vector<double> unknowns(2);
    vgl_point_2d<double> ip = (*camx).project(world_pts_[i]);
    unknowns[0] = ip.x(); unknowns[1] = ip.y();
    double sr = 0, er = 0;
    vcl_vector<float> cwin = corr_windows_a_[i];
    if (db<da)
      cwin = corr_windows_b_[i];
    this->exhaustive_init(unknowns,win_radius, search_radius,
                          fvx, cwin, sr, er);
    if (verbose_)
      vcl_cout << "c["<<i <<"]:f[" << frame_index_x << "] Bf(" << ip.x()
               << ' ' << ip.y() << ") Af("  << unknowns[0] << ' '
               << unknowns[1] << ")ier "
               << sr << ':' << er << '\n' << vcl_flush;
#if 0
    double xb = unknowns[0], yb = unknowns[1];
    if (use_lmq){
      bwm_video_corr_lsqr_cost_func vlcf(fvx, win_radius,
                                         cwin);
      vnl_levenberg_marquardt lmq(vlcf);
      //a good initial guess for the unknown correspondences is
      //the projection of the world points in frame x
      lmq.minimize(unknowns);
      sr = vcl_sqrt(lmq.get_start_error()/nres);
      er = vcl_sqrt(lmq.get_end_error()/nres);
    }
    else // amoeba
    {
      bwm_video_corr_cost_function vcf(fvx, win_radius,
                                       cwin);
      vnl_amoeba a(vcf);
      sr = vcl_sqrt(vcf.f(unknowns)/nres);
      a.minimize(unknowns);
      er = vcl_sqrt(vcf.f(unknowns)/nres);
    }
    if (verbose_)
      vcl_cout << "c["<<i <<"]:f[" << frame_index_x << "] Bf(" << xb
               << ' ' << yb << ") Af("  << unknowns[0] << ' '
               << unknowns[1] << ")f "
               << sr << ':' << er << '\n' << vcl_flush;
#endif
    bwm_video_corr_sptr c = corrs_[i];
    if (er>0)
      c->add(frame_index_x, vgl_point_2d<double>(unknowns[0], unknowns[1]));
    else
      n_failures_[i]++;
  }
  return true;
}

bool bwm_video_corr_processor::
find_missing_correspondences(unsigned win_radius,
                             unsigned search_radius, bool use_lmq)
{
  unsigned n = corrs_.size();
  if (!n){
    vcl_cerr << "No correspondences\n";
    return false;
  }
  n_failures_.resize(n, 0);
  //get the start and end frame numbers
  unsigned min_frame =0, max_frame = 0;
  vcl_vector<vcl_vector<bool> > mask;
  this->mask(min_frame, max_frame, mask);
  //implementation not yet general enough to handle non_zero start frame
  if (min_frame){
    vcl_cerr << "start frame not zero\n";
    return false;
  }
  unsigned nframes = max_frame - min_frame +1;
  corrs_per_frame_.resize(nframes, 0);
  vcl_vector<unsigned> frame_intervals;
  for (unsigned f = min_frame; f<=max_frame; ++f)
  {
    //number of correspondences on frame
    unsigned count = 0;
    for (unsigned w = 0; w<n; ++w)
      if (corrs_[w]->match(f))
        count++;
    if (count>=min_corrs)
      frame_intervals.push_back(f);
  }
  unsigned nfi = frame_intervals.size();
  if (nfi<2){
    vcl_cerr << " not at least two frame bounds\n";
      return false;
  }
  for (unsigned i = 0; i<nfi-1; ++i)
  {
    unsigned frame_a = frame_intervals[i];
    unsigned frame_b = frame_intervals[i+1];
    vcl_vector<bool> mask_a = mask[frame_a];
    vcl_vector<bool> mask_b = mask[frame_b];
    for (unsigned ix = frame_a +1; ix<frame_b; ++ix){
      if (!this->find_missing_corrs(frame_a, mask_a,
                                    frame_b, mask_b,
                                    ix, win_radius, search_radius, use_lmq))
        return false;
    }
  }
  for (unsigned ic = 0; ic<n; ++ic)
    vcl_cout << "nf[" << ic << "]= " << n_failures_[ic]<< '\n';
  for (unsigned f = min_frame; f<=max_frame; ++f)
  {
    for (unsigned ic=0; ic<n; ++ic)
      if (corrs_[ic]->match(f))
        corrs_per_frame_[f]++;
    vcl_cout << "Nc[" << f << "]= " << corrs_per_frame_[f] << '\n';
  }
  return true;
}

bool bwm_video_corr_processor::refine_world_pts_and_cameras()
{
  //get the start and end frame numbers
  unsigned min_frame =0, max_frame = 0;
  vcl_vector<vcl_vector<bool> > mask;
  this->mask(min_frame, max_frame, mask);
  //The implementation is not general enough yet to handle
  //a non-zero start frame
  if (min_frame){
    vcl_cerr << "Can't handle a non-zero start frame\n";
    return false;
  }
  unsigned ncameras = max_frame-min_frame +1;
  unsigned npoints = corrs_.size();
  if (verbose_)
    vcl_cout << "Refining " << ncameras << " cameras on " << npoints
             << " correspondences\n";

  //next populate the image points and the mask array
  //the mask appears to be m x n ,
  // where m = number of cameras an n = number of world points (corrs)
  //
  vcl_vector<vgl_point_2d<double> > image_points(ncameras*npoints);
  for (unsigned w = 0; w<npoints; ++w)
    for (unsigned f = min_frame; f<=max_frame; ++f)
    {
      vgl_point_2d<double> pt;
      if (corrs_[w]->match(f, pt))
        image_points[f*npoints + w] = pt;
      else if (mask[f][w]){
        vcl_cerr << "fatal[" << f << "][" << w << "]\n";
        return false;
      }
    }
  //find the number of rows that have insufficient correspondences
  //remove them creating a smaller problem
  vcl_vector<bool> unknown_frames(ncameras, false);
  int filled_rows = 0;
  for (unsigned f = 0; f<ncameras; ++f){
    unsigned nc = 0;
    for (unsigned w = 0; w<npoints; ++w)
      if (mask[f][w]) nc++;
    if (nc<min_corrs)
      unknown_frames[f] = true;
    else
      ++filled_rows;
  }
  if (verbose_)
    vcl_cout << "Executing bundle adjustment on " << filled_rows
             << " cameras\n";
  //create a new mask array and image point vector for the smaller problem
  vcl_vector<bool> init(npoints);
  vcl_vector<vcl_vector<bool> > cmask(filled_rows, init);
  vcl_vector<vgl_point_2d<double> > cimage_points;
  vcl_vector<vpgl_perspective_camera<double> > unknown_cameras;
  if (!cam_istr_)
    return false;
  unsigned ff = 0;
  for (unsigned f = 0; f<ncameras; ++f)
    if (!unknown_frames[f])
    {
      for (unsigned w = 0; w<npoints; ++w)
        if (cmask[ff][w] = mask[f][w])
          cimage_points.push_back(image_points[f*npoints + w]);

      if (!cam_istr_->seek_camera(f)){
        vcl_cerr << "Can't seek camera at frame " << f << '\n';
        return false;
      }
      unknown_cameras.push_back(*(cam_istr_->current_camera()));
      ++ff;
    }
  // initialize unknown world points
  vgl_point_3d<double> pun(0.0, 0.0, 0.0);
  vcl_vector<vgl_point_3d<double> > unknown_world(npoints,pun);
  for (unsigned w = 0; w<npoints; ++w)
    if (corrs_[w]->world_pt_valid())
      unknown_world[w]=corrs_[w]->world_pt();

  // exectute the bundle adjustment
  bool success = vpgl_bundle_adjust::optimize(unknown_cameras,
                                              unknown_world, cimage_points,
                                              cmask);
  //save the solved world points in the correspondences
  for (unsigned w = 0; w<npoints; ++w)
  {
    vgl_point_3d<double> pt = unknown_world[w];
    corrs_[w]->set_world_pt(pt);
  }
  vcl_vector<vpgl_perspective_camera<double> > interp_cameras;
  if (!this->interpolate_cameras(unknown_cameras, unknown_frames,
                                 interp_cameras))
    return false;
  // close the input camera stream
  if (cam_ostr_&&cam_ostr_->is_open())
    for (unsigned i = 0; i<ncameras; ++i)
      cam_ostr_->write_camera(&interp_cameras[i]);
  return true;
}

void bwm_video_corr_processor::
print_frame_alignment_quality(unsigned start_frame, unsigned end_frame)
{
  unsigned n = corrs_.size();
  if (!n) return;
  if (!video_istr_ || !cam_istr_)
    return;

  for (unsigned f = start_frame; f<=end_frame; ++f)
  {
    if (!cam_istr_->seek_camera(f))
      continue;
    vpgl_perspective_camera<double>* cam = cam_istr_->current_camera();
    bsta_histogram<double> h(0.0, 5.0, 10);
    double min_error = vnl_numeric_traits<double>::maxval, max_error = 0;
    for (unsigned i = 0; i<n; ++i)
    {
      bwm_video_corr_sptr c = corrs_[i];
      if (!c->world_pt_valid())
        continue;
      vgl_point_2d<double> cpt;
      if (!c->match(f, cpt))
        continue;
      vgl_point_2d<double> ppt = (*cam).project(c->world_pt());
      double d = vgl_distance(cpt, ppt);
      if (d<min_error) min_error = d;
      if (d>max_error) max_error = d;
      if (d>5.0) h.upcount(5.0, 1.0);
      else h.upcount(d, 1.0);
    }
    vcl_cout.precision(2);
    vcl_cout << "frame[" << f << "](" << min_error << '<' << max_error
             << "):c(" << h.p(0.5)+h.p(1.5) << ") p[";
    for (double x = 0.5; x<=5.5; x+=1.0)
      vcl_cout << h.p(x) << ' ';
    vcl_cout << "]\n";
  }
}

void bwm_video_corr_processor::close()
{
  if (video_istr_)
    video_istr_->close();
  video_istr_ = 0;

  if (cam_istr_)
    cam_istr_->close();
  cam_istr_ = 0;

  if (cam_ostr_)
    cam_ostr_->close();
  cam_ostr_ = 0;

  site_name_ = "";
  site_path_ = "";
  video_path_ = "";
  camera_path_ = "";

  corrs_.clear();
  cameras_.clear();
  world_pts_.clear();
  corr_windows_a_.clear();
  corr_windows_b_.clear();
  frame_index_a_ = static_cast<unsigned>(-1);
  frame_index_b_ = static_cast<unsigned>(-1);
}
