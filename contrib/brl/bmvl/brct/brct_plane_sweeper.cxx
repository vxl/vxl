#include "brct_plane_sweeper.h"
//:
// \file
#include <vcl_fstream.h>
#include <vcl_cmath.h>
#include <vbl/vbl_array_2d.h>
#include <vnl/vnl_numeric_traits.h>
#include <vnl/vnl_matrix_fixed.h>
#include <vgl/vgl_homg_point_2d.h>
#include <vgl/vgl_point_2d.h>
#include <vsol/vsol_box_2d.h>
#include <vsol/vsol_point_2d.h>
#include <vsol/vsol_point_3d.h>
#include <bsol/bsol_algs.h>
#include <sdet/sdet_harris_detector.h>
#include <brip/brip_vil1_float_ops.h>
#include <brct/brct_algos.h>

brct_plane_sweeper::brct_plane_sweeper(brct_plane_sweeper_params const& sp) // FIXME: sp unused
 : n_planes_(0), n_cams_(0), del_(1.f), to_cam_(0), pindx_(192,256,vsol_box_2d_sptr(0))
{
}

brct_plane_sweeper::~brct_plane_sweeper()
{
}

static vgl_h_matrix_2d<double> translation_h(const double tx, const double ty)
{
  vnl_matrix_fixed<double,3, 3> M;
  M.fill(0.0);
  M[0][0]=1.0;   M[0][2]=tx;
  M[1][1]=1.0;   M[1][2]=ty;
  M[2][2]=1.0;
  vgl_h_matrix_2d<double> H(M);
  return H;
}

//: normalize to make H[2][2]=1
static vgl_h_matrix_2d<double> normalize_h(vgl_h_matrix_2d<double> const& H)
{
  vnl_matrix_fixed<double,3, 3> M = H.get_matrix();
  double s = M[2][2];
  if (vcl_fabs(s)<1e-06)
  {
    vcl_cout << "In brct_plane_sweeper - singular homography\n";
    return H;
  }
  for (int r = 0; r<3; r++)
    for (int c = 0; c<3; c++)
      M[r][c]/=s;
  vgl_h_matrix_2d<double> Hn(M);
  return Hn;
}

static vgl_h_matrix_2d<double> translate_h(vgl_h_matrix_2d<double> const& H,
                                           const double tx, const double ty)
{
  vnl_matrix_fixed<double,3, 3> M = H.get_matrix(), Mt;
  Mt.fill(0.0);
  Mt[0][0]=1;  Mt[1][1]=1; Mt[2][2]=1;
  Mt[0][2] = tx;
  Mt[1][2] = ty;
  vgl_h_matrix_2d<double> Ht(Mt*M);
  return Ht;
}

vcl_vector<vsol_point_2d_sptr> brct_plane_sweeper::
project_corners(vgl_h_matrix_2d<double> const & H,
                vcl_vector<vsol_point_2d_sptr> const& corners)
{
  vcl_vector<vsol_point_2d_sptr> out;
  for (vcl_vector<vsol_point_2d_sptr>::const_iterator pit = corners.begin();
       pit != corners.end(); pit++)
  {
    vgl_homg_point_2d<double> p((*pit)->x(), (*pit)->y()), hp;
    hp = H(p);
    vgl_point_2d<double> np(hp);//to normalize
    vsol_point_2d_sptr temp = new vsol_point_2d(np);
    out.push_back(temp);
  }
  return out;
}

bool brct_plane_sweeper::read_homographies(vcl_string const& homography_file)
{
  vcl_ifstream is(homography_file.c_str());
  if (!is)
  {
    vcl_cout << "In brct_plane_calibrator::write_homographies -"
             << " could not open file " << homography_file << '\n';
    return false;
  }
  vcl_string s;
  is >> s;
  if (!(s=="N_PLANES:"))
  {
    vcl_cout << "Parse error\n";
    return false;
  }
  is >> n_planes_;
  is >> s;
  if (!(s=="N_CAMS:"))
  {
    vcl_cout << "Parse error\n";
    return false;
  }
  is >> n_cams_;
  //resize the arrays
  z_.resize(n_planes_);
  homographies_.resize(n_planes_);
  for (int i=0; i<n_planes_; i++)
    homographies_[i].resize(n_cams_);
  images_.resize(n_cams_);
  smooth_images_.resize(n_cams_);
  harris_corners_.resize(n_cams_);
  //read the homography data
  int p = 0, c =0;
  double z=0;
  vnl_matrix_fixed<double, 3, 3> M;
  for (int plane = 0; plane < n_planes_; plane++)
    for (int cam = 0; cam < n_cams_; cam++)
    {
      is >> s;
      if (!(s=="PLANE:"))
      {
        vcl_cout << "Parse error\n";
        return false;
      }
      is >> p;
      is >> s;
      if (!(s=="Z:"))
      {
        vcl_cout << "Parse error\n";
        return false;
      }
      is >> z;
      z_[plane]=z;
      is >> s;
      if (!(s=="CAM:"))
      {
        vcl_cout << "Parse error\n";
        return false;
      }
      is >> c;
      is >> M;
      vgl_h_matrix_2d<double> H(M);
      vgl_h_matrix_2d<double> Hn = normalize_h(H);
      homographies_[plane][cam]=Hn;
    }
  homographies_valid_=true;
  return true;
}

bool brct_plane_sweeper::set_image(const int cam, vil1_image const& image)
{
  if (cam>=n_cams_)
  {
    vcl_cout << "In brct_plane_sweeper::set_image - "
             << " cam index out of bounds\n";
    return false;
  }
  if (!image)
  {
    vcl_cout << "In brct_plane_sweeper::set_image - "
             << " null image\n";
    return false;
  }
  images_[cam]=image;
  vil1_memory_image_of<float> flt_image =
    brip_vil1_float_ops::convert_to_float(image);
  vil1_memory_image_of<float> smooth =
    brip_vil1_float_ops::gaussian(flt_image, corr_sigma_);
  smooth_images_[cam]=smooth;
  return true;
}

//: compute homographies for each camera that project to the specified z plane.
//  For now assume two cameras.
void brct_plane_sweeper::
homographies_at_z(double z, vcl_vector<vgl_h_matrix_2d<double> >& homgs)
{
  if (!homographies_valid_)
    return;
  //we assume z_[0] is defined as zero
  double zc = z_[1];
  if (!zc)
    return;
  homgs.clear();
  double f = z/zc;
  for (int cam = 0; cam<n_cams_; cam++)
  {
    vgl_h_matrix_2d<double> H0 = homographies_[0][cam];
    vgl_h_matrix_2d<double> H1 = homographies_[1][cam];
    vnl_matrix_fixed<double,3, 3> M0 = H0.get_matrix();
    vnl_matrix_fixed<double,3, 3> M1 = H1.get_matrix();
    vnl_matrix_fixed<double,3, 3> M;

    //set the homgraphy columns 0 and 1 to the average of M0 and M1
    for (int r = 0; r<3; r++)
      for (int c =0; c<2; c++)
        M[r][c] = 0.5*(M0[r][c] + M1[r][c]);
    //interpolate the third column with respect to depth
    for (int r = 0; r<3; r++)
      M[r][2] = f*(M1[r][2]-M0[r][2]) + M0[r][2];
    vgl_h_matrix_2d<double> H(M);
    homgs.push_back(H);
  }
}


vil1_memory_image_of<unsigned char>
brct_plane_sweeper::project_image_to_plane(const int plane,
                                           const int cam)
{
  vil1_memory_image_of<unsigned char> out;
  if (!homographies_valid_)
  {
    vcl_cout << "In brct_plane_sweeper::project_image_to_plane -"
             << " homographies not loaded yet\n";
    return out;
  }
  vgl_h_matrix_2d<double> H = homographies_[plane][cam];
  vgl_h_matrix_2d<double> Hinv = H.get_inverse();
  vil1_memory_image_of<float> image =smooth_images_[cam], temp;
  brip_vil1_float_ops::homography(image, Hinv, temp);
  out = brip_vil1_float_ops::convert_to_byte(temp, 0, 255);
  return out;
}

static vsol_box_2d_sptr box_from_image(vil1_memory_image_of<float> const& img)
{
  int w = img.width();
  int h = img.height();
  vsol_box_2d_sptr box = new vsol_box_2d();
  box->add_point(0,0);
  box->add_point(w,h);
  return box;
}

bool brct_plane_sweeper::
overlapping_box(vcl_vector<vgl_h_matrix_2d<double> > const& homgs,
                vsol_box_2d_sptr & box)
{
  vsol_box_2d_sptr intersection = new vsol_box_2d();
  double maxd = vnl_numeric_traits<double>::maxval;
  intersection->add_point(-maxd, -maxd);
  intersection->add_point(+maxd, +maxd);
  //intersect with each image projection
  for (int cam =0; cam<n_cams_; cam++)
  {
    vgl_h_matrix_2d<double> H = homgs[cam];
    vgl_h_matrix_2d<double> Hinv = H.get_inverse();
    vil1_memory_image_of<float> image =smooth_images_[cam];
    vsol_box_2d_sptr b = box_from_image(image);
    vsol_box_2d_sptr Hinv_b;
    if (!bsol_algs::homography(b, Hinv, Hinv_b))
      return false;
    if (!bsol_algs::intersection(Hinv_b, intersection, intersection))
      return false;
  }
  box = intersection;
  return true;
}

bool brct_plane_sweeper::
overlapping_projections(vcl_vector<vgl_h_matrix_2d<double> > const& homgs,
                        vcl_vector<vil1_memory_image_of<float> >& imgs,
                        double& tx, double& ty)
{
  if (!n_cams_)
  {
    vcl_cout << "In brct_plane_sweeper::overlapping_projections(.._)-"
             << " no cameras\n";
  return false;
  }
  imgs.clear();
  //get the bounding box of the overlapping region
  vsol_box_2d_sptr intersection;
  if (!this->overlapping_box(homgs, intersection))
  {
    vcl_cout << "In brct_plane_sweeper::overlapping_projections(.._)-"
             << " no overlap\n";
  return false;
  }
  //The min corner must be mapped to (0,0) to form an image
  tx  = -(intersection->get_min_x()), ty = -(intersection->get_min_y());
  int w = (int)intersection->width(), h = (int)intersection->height();
  for (int cam =0; cam<n_cams_; cam++)
  {
    vgl_h_matrix_2d<double> H = homgs[cam];
    //JLM Debug
    //      vcl_cout << "H(" << cam << ")\n" << H << "\n\n";
    //end debug
    vgl_h_matrix_2d<double> Hinv = H.get_inverse();
    //modify the homography to map the corner to (0, 0)
    vgl_h_matrix_2d<double> Hinvt = translate_h(Hinv, tx, ty);
    //project the image
    vil1_memory_image_of<float> image =smooth_images_[cam];
    vil1_memory_image_of<float> projection(w, h);
    if (!brip_vil1_float_ops::homography(image, Hinvt, projection, true))
      return false;
    imgs.push_back(projection);
  }
  return true;
}

//: compute overlapping projections of images and Harris corners
bool brct_plane_sweeper::
overlapping_projections(vcl_vector<vgl_h_matrix_2d<double> > const& homgs,
                        vcl_vector<vil1_memory_image_of<float> >& imgs,
                        vcl_vector<vcl_vector<vsol_point_2d_sptr> >& corners,
                        double& tx, double& ty)
{
  if (!overlapping_projections(homgs, imgs, tx, ty))
     return false;
  corners.clear();
  //project the harris corners
  for (int cam =0; cam<n_cams_; cam++)
  {
    vgl_h_matrix_2d<double> H = homgs[cam];
    vgl_h_matrix_2d<double> Hinv = H.get_inverse();
    vgl_h_matrix_2d<double> Hinvt = translate_h(Hinv, tx, ty);
    vcl_vector<vsol_point_2d_sptr> hc = harris_corners_[cam];
    vcl_vector<vsol_point_2d_sptr> pcs =
      this->project_corners(Hinvt, hc);
    corners.push_back(pcs);
  }
 return true;
}


//:compute the projections of left and right images that overlap on the specified plane
bool brct_plane_sweeper::
overlapping_projections(const int plane,
                        vcl_vector<vil1_memory_image_of<float> >& imgs)
{
  vcl_vector<vgl_h_matrix_2d<double> > homgs = homographies_[plane];
  double tx=0, ty=0;
  if (!overlapping_projections(homgs, imgs, tx, ty))
    return false;
  return true;
}

//:compute the projections of left and right images that overlap on the plane at depth z
bool brct_plane_sweeper::
overlapping_projections(const double z,
                        vcl_vector<vil1_memory_image_of<float> >& imgs)
{
  vcl_vector<vgl_h_matrix_2d<double> > homgs;
  this->homographies_at_z(z, homgs);
  double tx=0, ty=0;
  if (!overlapping_projections(homgs, imgs, tx, ty))
    return false;
  return true;
}

bool brct_plane_sweeper::
overlapping_projections(const double z,
                        vcl_vector<vil1_memory_image_of<float> >& imgs,
                        vcl_vector<vcl_vector<vsol_point_2d_sptr> >& corners)
{
  vcl_vector<vgl_h_matrix_2d<double> > homgs;
  this->homographies_at_z(z, homgs);
  double tx=0, ty=0;
  if (!overlapping_projections(homgs, imgs, corners, tx, ty))
    return false;
  return true;
}

//:
// perform the cross-correlation of each pixel of the overlapping projection
// of the left and right images onto the specified plane.
vil1_memory_image_of<unsigned char>
brct_plane_sweeper::cross_correlate_projections(const int plane)
{
  vil1_memory_image_of<unsigned char> out;
  if (n_cams_<2)
  {
    vcl_cout << "In brct_plane_sweeper::cross_correlate_projections(..)-"
             << "less than two images\n";
    return out;
  }
  vcl_vector<vil1_memory_image_of<float> > imgs;
  this->overlapping_projections(plane, imgs);
  vil1_memory_image_of<float> cc;
  if (!brip_vil1_float_ops::cross_correlate(imgs[0], imgs[1], cc,
                                           corr_radius_, intensity_thresh_))
    return out;
  out = brip_vil1_float_ops::convert_to_byte(cc, corr_min_, corr_max_);
  return out;
}

//:
// perform the cross-correlation of each pixel of the overlapping projection
// of the left and right images onto the specified world Z value.
vil1_memory_image_of<unsigned char>
brct_plane_sweeper::cross_correlate_projections(const double z)
{
  vil1_memory_image_of<unsigned char> out;
  if (n_cams_<2)
  {
    vcl_cout << "In brct_plane_sweeper::cross_correlate_projections(..)-"
             << "less than two images\n";
    return out;
  }
  vcl_vector<vil1_memory_image_of<float> > imgs;
  vcl_vector<vgl_h_matrix_2d<double> > homgs;
  this->homographies_at_z(z, homgs);
  double tx=0, ty=0;
  this->overlapping_projections(homgs, imgs, tx, ty);
  vil1_memory_image_of<float> cc;
  if (!brip_vil1_float_ops::cross_correlate(imgs[0], imgs[1], cc,
                                           corr_radius_, intensity_thresh_))
    return out;
  out = brip_vil1_float_ops::convert_to_byte(cc, corr_min_, corr_max_);
  return out;
}

bool
brct_plane_sweeper::
cross_correlate_proj_corners(const double z,
                             vil1_image& back,
                             vcl_vector<vsol_point_2d_sptr>& matched_corners,
                             vcl_vector<vsol_point_2d_sptr>& back_proj_cnrs,
                             vcl_vector<vsol_point_2d_sptr>& orig_cnrs0,
                             bool reset_flags)
{
  if (n_cams_<2)
  {
    vcl_cout << "In brct_plane_sweeper::cross_correlate_projections(..)-"
             << "less than two images\n";
    return false;
  }
  //reset match flags
  vcl_vector<vil1_memory_image_of<float> > imgs;
  vcl_vector<vsol_point_2d_sptr> temp;
  vcl_vector<vcl_vector<vsol_point_2d_sptr> > proj_corners;
  if (!overlapping_projections(z, imgs, proj_corners))
    return false;
  vcl_cout << "Correlating corners at z"  << z << vcl_endl;
  //matched corners are in original image1 coordinates
  if (!this->correlate_corners(imgs, proj_corners, matched_corners))
    return false;
  //JLM DEBUG
  vcl_vector<vgl_h_matrix_2d<double> > homgs;
  this->homographies_at_z(z, homgs);
  vgl_h_matrix_2d<double> H0=homgs[0], H1 = homgs[1];
  vgl_h_matrix_2d<double> H1inv = H1.get_inverse();
  vcl_vector<vsol_point_2d_sptr> world_pts =
    this->project_corners(H1inv, matched_corners);
  back_proj_cnrs = this->project_corners(H0, world_pts);
  orig_cnrs0 = harris_corners_[0];
  vcl_cout << "World Points/BackProj\n";
  int n = world_pts.size();
  for (int i = 0; i<n; i++)
  {
    bsol_algs::print(world_pts[i]);
    bsol_algs::print(back_proj_cnrs[i]);
    vcl_cout << vcl_endl;
  }

  //END JLM DEBUG
  back = images_[1];
  return true;
}

vsol_box_2d_sptr brct_plane_sweeper::
depth_image_box(const double zmin, const double zmax)
{
  vsol_box_2d_sptr un = new vsol_box_2d();
  un->add_point(0, 0);//initialize the empty box
  vcl_vector<vgl_h_matrix_2d<double> > homgs;
  for (double z = zmin; z<=zmax; z+=del_)
  {
    this->homographies_at_z(zmin, homgs);
    vsol_box_2d_sptr bb;
    this->overlapping_box(homgs, bb);
   if (!bsol_algs::box_union(un, bb, un))
     continue;
  }
  return un;
}

#if 0 // unused static function
static void debug_print(const int c0, const int r0, const int dr,
                        const int c, const int r,
                        const float cc_val)
{
  int cmin = c0-dr, rmin = r0-dr;
  int cmax = c0+dr, rmax = r0+dr;
  if (c<cmin||c>cmax||r<rmin||r>rmax)
    return;
  vcl_cout << "C[" << r << "][" << c << "]= " << cc_val << vcl_endl;
}
#endif // 0

//: sweep the z plane and find positions of max cross-correlation
bool brct_plane_sweeper::
depth_image(vil1_memory_image_of<unsigned char>& depth_out,
            vil1_memory_image_of<unsigned char>& corr_out)
{
  if (n_cams_<2)
  {
    vcl_cout << "In brct_plane_sweeper::cross_correlate_projections(..)-"
             << "less than two images\n";
    return false;
  }
  z_corr_images_.clear();
  int n_zvals = nz_;
  if (n_zvals>256)
    n_zvals = 256;
  if (n_zvals<=0)
    n_zvals = 1;
  del_ = (zmax_-zmin_)/n_zvals;
  //get the size of the depth image
  //scan over the required z planes and get the union of all overlapping boxes
  vsol_box_2d_sptr un = this->depth_image_box(zmin_, zmax_);
  double Tx = -(un->get_min_x()), Ty = -(un->get_min_y());
  int w = (int)(un->width()), h = (int)(un->height());

  //data processing images
  //an image of maximum correlation value over the depth range
  vil1_memory_image_of<float> max_corr(w, h);
  float maxv = vnl_numeric_traits<float>::maxval;
  max_corr.fill(-maxv);
  //the depth image
  vil1_memory_image_of<float> depth(w, h);
  float null_depth = -10;
  depth.fill(null_depth);
  float null_corr = -1.0;

  //iterate through the depth planes
  vcl_vector<vil1_memory_image_of<float> > imgs;
  vcl_vector<vgl_h_matrix_2d<double> > homgs, trans_homgs;
  for (double z = zmin_; z<=zmax_; z+=del_)
  {
    //get the overlap of the left and right images at the specified depth
    this->homographies_at_z(z, homgs);
    double tx=0, ty=0;
    this->overlapping_projections(homgs, imgs, tx, ty);
    //correlate the two images
//     vil1_memory_image_of<float> cc =
//       brip_vil1_float_ops::cross_correlate(imgs[0], imgs[1], radius_, intensity_thresh_);

    vil1_memory_image_of<float> cc;
    if (!brip_vil1_float_ops::cross_correlate(imgs[0], imgs[1],
                                             cc, corr_radius_,
                                             intensity_thresh_))
      return false;
    //translate the correlation image to the base image
    double dtx = 2*Tx-tx, dty = 2*Ty-ty;
    vgl_h_matrix_2d<double> Htrans = translation_h(dtx, dty);
    // the base image space for registering each cc image
    vil1_memory_image_of<float> base(w, h);
    if (!brip_vil1_float_ops::homography(cc, Htrans, base, true, null_corr))
      continue;
    z_corr_images_.push_back(base);
    vcl_cout << "corr for depth " << z << vcl_endl;
    for (int r = 0; r<h; r++)
      for (int c=0; c<w; c++)
      {
        float cc_val = base(c, r);
        if (cc_val<corr_thresh_)
          continue;
        if (cc_val > max_corr(c, r))
        {
          max_corr(c, r) = cc_val;
          depth(c, r) = float(z);
        }
      }
  }
  depth_out = brip_vil1_float_ops::convert_to_byte(depth, null_depth, zmax_);
  corr_out = brip_vil1_float_ops::convert_to_byte(max_corr, corr_min_,
                                                  corr_max_);
  return true;
}

//: sweep the z plane and find positions of max cross-correlation
bool brct_plane_sweeper::
harris_depth_match(vcl_vector<vsol_point_3d_sptr>& points_3d,
                   vcl_vector<vsol_point_2d_sptr>& proj_points)
{
  if (n_cams_<2)
  {
    vcl_cout << "In brct_plane_sweeper::cross_correlate_projections(..)-"
             << "less than two images\n";
    return false;
  }
  points_3d.clear();
  proj_points.clear();
  //reset flags
  int n_zvals = nz_;
  if (n_zvals>256)
    n_zvals = 256;
  if (n_zvals<=0)
    n_zvals = 1;
  del_ = (zmax_-zmin_)/n_zvals;
  //get the size of the depth image
  //scan over the required z planes and get the union of all overlapping boxes
  vsol_box_2d_sptr un = this->depth_image_box(zmin_, zmax_);
  int w = (int)(un->width()), h = (int)(un->height());

  //data processing images
  //an image of maximum correlation value over the depth range
  vil1_memory_image_of<float> max_corr(w, h);
  float maxv = vnl_numeric_traits<float>::maxval;
  max_corr.fill(-maxv);
  //the depth image
  vil1_memory_image_of<float> depth(w, h);
  float null_depth = -10;
  depth.fill(null_depth);
  //iterate through the depth planes
  vil1_memory_image_of<float> back;
  vcl_vector<vil1_memory_image_of<float> > imgs;
  vcl_vector<vgl_h_matrix_2d<double> > homgs, trans_homgs;
  vcl_vector<vcl_vector<vsol_point_2d_sptr> > proj_corners;

  for (double z = zmin_; z<=zmax_; z+=del_)
  {
    //get the overlap of the left and right images at the specified depth
    this->homographies_at_z(z, homgs);
    double tx=0, ty=0;
    this->overlapping_projections(homgs, imgs, proj_corners, tx, ty);
    vcl_vector<vsol_point_2d_sptr> matched_corners;
    if (!this->correlate_corners(imgs, proj_corners, matched_corners))
      return false;
    vcl_cout << "Matched " << matched_corners.size() << " corners at z = "
             << z << vcl_endl;
    for (vcl_vector<vsol_point_2d_sptr>::iterator pit = matched_corners.begin();
         pit != matched_corners.end(); pit++)
    {
      proj_points.push_back(*pit);
      //          bsol_algs::print(*pit);
    }

    //back project the matched corners (cam 1) onto the world x-y-z plane
    vcl_vector<vsol_point_2d_sptr> trans_pts =
      this->project_corners(homgs[1], matched_corners);
    //convert to 3-d points
    for (vcl_vector<vsol_point_2d_sptr>::iterator pit = trans_pts.begin();
         pit != trans_pts.end(); pit++)
    {
      double x = (*pit)->x(), y = (*pit)->y();
      vsol_point_3d_sptr p3d = new vsol_point_3d(x, y, z);
      points_3d.push_back(p3d);
      bsol_algs::print(p3d);
    }
    vcl_cout << vcl_endl;
  }
  return true;
}

bool brct_plane_sweeper::compute_harris()
{
  int n_images = images_.size();
  if (n_images!=n_cams_)
  {
    vcl_cout << "In brct_plane_sweeper::compute_harris() - "
             << "images not matched to cameras\n";
    return false;
  }
  vcl_cout << hdp_ << vcl_endl;
  sdet_harris_detector hd(hdp_);
  for (int cam = 0; cam<n_cams_; cam++)
  {
    hd.clear();
    hd.set_image(images_[cam]);
    hd.extract_corners();
    vcl_vector<vsol_point_2d_sptr> points = hd.get_points();
    harris_corners_[cam]=points;
  }
  harris_valid_ = true;
  return true;
}

//return true if (r, c) has a neighbor within the radius
static bool has_neighbor(const int r, const int c , const int radius,
                         const int h, const int w,
                         vbl_array_2d<bool> const& cnr_array)
{
  //check bounds
  int rmin = radius, rmax = h-radius-1, cmin = radius, cmax = w-radius-1;
  if (r<rmin||r>rmax||c<cmin||c>cmax)
    return false;
  for (int r0 = -radius; r0<=radius; r0++)
    for (int c0 = -radius; c0<=radius; c0++)
      if (cnr_array[r+r0][c+c0])
        return true;
  return false;
}

//: match harris corners from image 1 to image 0 and return matches
bool brct_plane_sweeper::
correlate_corners(vcl_vector<vil1_memory_image_of<float> > const& imgs,
                  vcl_vector<vcl_vector<vsol_point_2d_sptr> > const& corners,
                  vcl_vector<vsol_point_2d_sptr>& matched_corners)

{
  int n_imgs=imgs.size();
  int n_corn=corners.size();
  if (n_imgs!=2||n_imgs!=n_corn)
    return false;
  matched_corners.clear();
  int w = imgs[0].width(), h = imgs[0].height();
  vbl_array_2d<bool> pt_index(h, w);
  pt_index.fill(false);
  vcl_vector<vsol_point_2d_sptr> const& pts0 = corners[0],
    pts1 = corners[1];
  //for marking
  vcl_vector<vsol_point_2d_sptr> h1 = harris_corners_[1];
  //set the image 0 point_index array at Harris corner locations
  for (vcl_vector<vsol_point_2d_sptr>::const_iterator pit = pts0.begin();
       pit != pts0.end(); pit++)
  {
    int r = (int)((*pit)->y()), c = (int)((*pit)->x());
    if (r<0||r>=h||c<0||c>=w)
      continue;
    pt_index[r][c] = true;
  }
  //test each Harris corner in image 1 for a neighbor
  int n1 = pts1.size();
  for (int i = 0; i< n1; i++)
  {
    //sub-pixel locations
    double x0 = pts1[i]->x(), y0 = pts1[i]->y();
    //integer locations
    int r = (int)y0, c = (int)x0;
    if (r<0||r>=h||c<0||c>=w)
      continue;
    int pr = (int)point_radius_;
    if (has_neighbor(r, c, pr, h, w, pt_index))
    {
      float val =
        brip_vil1_float_ops::cross_correlate(imgs[0], imgs[1],
                                             float(x0), float(y0),
                                             corr_radius_,
                                             intensity_thresh_);

      if (val > corr_thresh_)
      {
        vsol_point_2d_sptr p = h1[i];
        matched_corners.push_back(p);//cache original Harris point
        vcl_cout << "C(" << p->x() << ' ' << p->y()
                 << ")=" << val << vcl_endl;
      }
    }
  }
  return true;
}

vcl_vector<vsol_point_2d_sptr>
brct_plane_sweeper::harris_corners(const int cam)
{
  vcl_vector<vsol_point_2d_sptr> points;
  if (cam>=n_cams_||!harris_valid_)
    return points;
  return harris_corners_[cam];
}


vil1_memory_image_of<unsigned char>
brct_plane_sweeper::z_corr_image(const int i)
{
  vil1_memory_image_of<unsigned char> temp;
  int li = i;
  int n = z_corr_images_.size();
  if (!n)
    return temp;
  if (i>=n)
    li = n-1;
  temp = brip_vil1_float_ops::convert_to_byte(z_corr_images_[li],
                                              corr_min_, corr_max_);
  return temp;
}

void brct_plane_sweeper::corr_vals(const int col, const int row,
                                   vcl_vector<float>& z,
                                   vcl_vector<float>& corr)
{
  z.clear();
  corr.clear();
  int i = 0, n = z_corr_images_.size();
  for (float zi = zmin_; zi<=zmax_; zi+=del_, i++)
  {
    z.push_back(zi);
    if (i<n)
      corr.push_back(z_corr_images_[i](col, row));
  }
}

vsol_point_2d_sptr brct_plane_sweeper::
map_point(vsol_point_2d_sptr const& p, vgl_h_matrix_2d<double> const& Hcomp)

{
  vgl_homg_point_2d<double> hp(p->x(), p->y()), hp_to;
  //map the point
  hp_to = Hcomp(hp);
  vgl_point_2d<double> p_to(hp_to);
  return new vsol_point_2d(p_to);
}

vsol_point_2d_sptr brct_plane_sweeper::
map_point(vsol_point_2d_sptr const& p, const int cam, const double z)
{
  vsol_point_2d_sptr q;
  if (!p)
    return q;
  vcl_vector<vgl_h_matrix_2d<double> > homgs;
  this->homographies_at_z(z, homgs);
  //assume two cameras for now
  int nc = homgs.size();
  if (nc!=2||cam>=nc)
    return q;
  vgl_h_matrix_2d<double> H_from = homgs[cam], H_to =homgs[1-cam];
  //form the composite homography
  vgl_h_matrix_2d<double> H_from_inv = H_from.get_inverse();
  vgl_h_matrix_2d<double> Hcomp = H_to*H_from_inv;
  return this->map_point(p, Hcomp);
}

//Map points from one image to the other assuming that they lie on plane z
bool brct_plane_sweeper::
map_points(const int from_cam, const double z,
           vcl_vector<vsol_point_2d_sptr> const& from_points,
           vcl_vector<vsol_point_2d_sptr>& to_points)
{
  //get the homographies interpolated at plane z
  vcl_vector<vgl_h_matrix_2d<double> > homgs;
  this->homographies_at_z(z, homgs);
  int nc = homgs.size();
  if (nc!=2||from_cam>=nc)
    return false;
  to_points.clear();
  int to_cam = 1-from_cam;
  vgl_h_matrix_2d<double> H_from = homgs[from_cam], H_to =homgs[to_cam];
  //map the point to the plane at z
  vgl_h_matrix_2d<double> H_from_inv = H_from.get_inverse();
  vgl_h_matrix_2d<double> Hcomp = H_to*H_from_inv;

  for (vcl_vector<vsol_point_2d_sptr>::const_iterator pit = from_points.begin();
       pit != from_points.end(); pit++)
    to_points.push_back(this->map_point(*pit, Hcomp));

  return true;
}

//: Map an image from one camera to the other, assuming all the pixels lie on the plane at z.
//  Put the result into the coordinate frame of the target camera (to camera).
bool brct_plane_sweeper::map_image(const int from_cam, const double z,
                                   vil1_memory_image_of<float>& mapped_image)
{
  //get the homographies interpolated at plane z
  vcl_vector<vgl_h_matrix_2d<double> > homgs;
  this->homographies_at_z(z, homgs);
  int nc = homgs.size();
  if (nc!=2||from_cam>=nc)
    return false;
  int to_cam = 1-from_cam;
  //Form the composed map
  vgl_h_matrix_2d<double> Ha = homgs[from_cam], Hb =homgs[to_cam];
  vgl_h_matrix_2d<double> Hainv = Ha.get_inverse();
  vgl_h_matrix_2d<double> Hcomp = Hb*Hainv;
  vil1_memory_image_of<float> temp = smooth_images_[from_cam],
    to = smooth_images_[to_cam];
  vsol_box_2d_sptr from_box =box_from_image(temp), to_box = box_from_image(to),
    inter_box;
  vcl_cout << "Intersecting Mapped Box\n";
  if (this->intersecting_bounding_box(Hcomp, from_box, to_box, inter_box))
    bsol_algs::print(inter_box);

  mapped_image.resize(temp.width(), temp.height());
  if (!brip_vil1_float_ops::homography(temp, Hcomp, mapped_image, true, 0))
    return false;
  return true;
}

//: Map the image of the from_cam to the image space of the to_cam.
//  Assumes that the world is a plane at z.
bool brct_plane_sweeper::map_image_to_image(const int from_cam, const double z,
                                            vil1_memory_image_of<unsigned char>& mapped_to_image,
                                            vil1_memory_image_of<unsigned char>& orig_to_image)
{
  vil1_memory_image_of<float> temp;
  if (!this->map_image(from_cam, z, temp))
    return false;
  int to_cam = 1-from_cam;
  mapped_to_image =  brip_vil1_float_ops::convert_to_byte(temp, 0, 255);
  orig_to_image =
    brip_vil1_float_ops::convert_to_byte(smooth_images_[to_cam], 0, 255);
  return true;
}

bool brct_plane_sweeper::
intersecting_bounding_box(vgl_h_matrix_2d<double> const& Hcomp,
                          vsol_box_2d_sptr const& from_box,
                          vsol_box_2d_sptr const& to_box,
                          vsol_box_2d_sptr & box)
{
  vsol_box_2d_sptr intersection = new vsol_box_2d();
  vsol_box_2d_sptr mapped_box;
  if (!bsol_algs::homography(from_box, Hcomp, mapped_box))
    return false;
  if (!bsol_algs::intersection(mapped_box, to_box, box))
    return false;
  return true;
}

bool brct_plane_sweeper::
map_harris_corners(const int from_cam, const double z,
                   vcl_vector<vsol_point_2d_sptr>& mapped_to_points,
                   vcl_vector<vsol_point_2d_sptr>& orig_to_points)
{
  if (from_cam<0||from_cam>=2)
    return false;
  if (!harris_valid_)
    return false;
  int to_cam = 1-from_cam;
  vcl_vector<vsol_point_2d_sptr> harris_from = harris_corners_[from_cam];
  orig_to_points = harris_corners_[to_cam];
  this->map_points(from_cam, z, harris_from, mapped_to_points);
  return true;
}

//:initialize the harris corner matching index
void brct_plane_sweeper::init_harris_match(const int from_cam)
{
  //make the grid 1/4 the size of the image
  int nrows = 192, ncols = 256;
  int to_cam = 1-from_cam;
  vcl_vector<vsol_point_2d_sptr> to_points = harris_corners_[to_cam];
  pindx_ = bsol_point_index_2d(nrows, ncols, to_points);
  vcl_vector<vsol_point_2d_sptr> temp = pindx_.points();
  vcl_cout << "\nTotal points in point index = " << temp.size() << vcl_endl;
  pindx_.clear_marks();
  matched_corners_.clear();
  del_ = (zmax_-zmin_)/nz_;
}

//:for debugging testing
bool brct_plane_sweeper::
match_harris_corners(const int from_cam, const double z,
                     vcl_vector<vsol_point_2d_sptr>& matched_points,
                     vcl_vector<vsol_point_2d_sptr>& orig_to_points)
{
  if (from_cam<0||from_cam>=2)
    return false;
  if (!harris_valid_)
    return false;
  matched_points.clear();
  orig_to_points.clear();
  float rad = point_radius_;
  int to_cam = 1-from_cam;
  vcl_vector<vsol_point_2d_sptr> harris_from = harris_corners_[from_cam];
  vcl_vector<vsol_point_2d_sptr> to_points = harris_corners_[to_cam];
  vcl_vector<vsol_point_2d_sptr> mapped_to_points;
  if (!this->map_points(from_cam, z, harris_from, mapped_to_points))
    return false;
  vsol_point_2d_sptr mp;
  for (vcl_vector<vsol_point_2d_sptr>::iterator pit = mapped_to_points.begin();
       pit != mapped_to_points.end(); pit++)
    if (pindx_.closest_in_radius(rad, *pit, mp))
    {
      matched_points.push_back(*pit);
      pindx_.mark_point(mp);
      orig_to_points.push_back(mp);
    }
  return true;
}

bool brct_plane_sweeper::harris_sweep(const int from_cam)
{
  if (from_cam<0||from_cam>=2)
    return false;
  if (!harris_valid_)
    return false;
  this->init_harris_match(from_cam);
  vcl_vector<vsol_point_2d_sptr> harris_from = harris_corners_[from_cam];
  to_cam_ = 1-from_cam;
  vil1_memory_image_of<float> orig_to_image = smooth_images_[to_cam_];
  int n_matched = 0;
  int nh = harris_from.size();
  //mark the harris corners in the from_cam that are matched
  vcl_vector<bool> match_index(nh, false);
  for (double z = zmin_; z<=zmax_; z+=del_)
  {
    vcl_vector<vsol_point_2d_sptr> matched_points, mapped_points;
    if (!this->map_points(from_cam, z, harris_from, mapped_points))
      continue;

    vsol_point_2d_sptr mp;

    for (int i = 0; i<nh; i++)
    {
      if (!match_index[i])
        if (pindx_.closest_in_radius(point_radius_, mapped_points[i], mp))
        {
          matched_points.push_back(mapped_points[i]);
          match_index[i]=true;//removed even if correlation fails
        }
    }
    int n = matched_points.size();

    //now confirm with correlation
    vcl_vector<vsol_point_2d_sptr> correlated_points;
    if (n)
    {
      vil1_memory_image_of<float> mapped_to_image;
      this->map_image(from_cam, z, mapped_to_image);
      for (vcl_vector<vsol_point_2d_sptr>::iterator pit=matched_points.begin();
           pit != matched_points.end(); pit++)
      {
        float x = (float)(*pit)->x(), y = (float)(*pit)->y();
        float cc =
          brip_vil1_float_ops::cross_correlate(mapped_to_image,
                                               orig_to_image,
                                               x, y, corr_radius_,
                                               intensity_thresh_);
        if (cc>corr_thresh_)
        {
          pindx_.mark_point(mp);
          correlated_points.push_back(*pit);
        }
      }
      int nc = correlated_points.size();
      n_matched += nc;
      vcl_cout << "N(" << z << ")=" << nc << " total matched = "
               << n_matched << vcl_endl;
      matched_corners_.push_back(correlated_points);
    }
  }
  return true;
}

vcl_vector<vsol_point_2d_sptr>
brct_plane_sweeper::matched_points_at_z_index(const int z_index)
{
  int n = matched_corners_.size();
  int zi = z_index;
  if (z_index>=n)
    zi = n-1;
  return matched_corners_[zi];
}

//:get the matched corners as 3-d points with x-y in the matched image
vcl_vector<vsol_point_3d_sptr>
brct_plane_sweeper::proj_points_3d()
{
  vcl_vector<vsol_point_3d_sptr> out;
  double z = zmin_;
  int n_z = matched_corners_.size();
  if (!n_z)
    return out;
  for (int i = 0; i<n_z; i++, z+=del_)
  {
    vcl_vector<vsol_point_2d_sptr> pts = matched_corners_[i];
    for (vcl_vector<vsol_point_2d_sptr>::iterator pit = pts.begin();
         pit != pts.end(); pit++)
    {
      double x = (*pit)->x(), y = (*pit)->y();
      vsol_point_3d_sptr p3d = new vsol_point_3d(x, y, z);
      out.push_back(p3d);
    }
  }
  return out;
}

//:get the matched corners as 3-d points with world x-y coordinates
vcl_vector<vsol_point_3d_sptr>
brct_plane_sweeper::world_points_3d()
{
  vcl_vector<vsol_point_3d_sptr> out, temp;
  temp = this->proj_points_3d();
  int n = temp.size();
  if (!n)
    return out;
  //need h in order to flip y for a right-handed world coordinate system
  int h = 0;
  if (smooth_images_[0])
    h = smooth_images_[0].height();
  if (!h)
    return out;
  for (int i=0; i<n; i++)
  {
    vsol_point_3d_sptr p = temp[i];
    double x = p->x(), y = p->y(), z = p->z();
    vsol_point_3d_sptr pw = new vsol_point_3d(x, h-y, z);//flipped y
    out.push_back(pw);
  }
  return out;
}

bool brct_plane_sweeper::save_world_points(vcl_string const& out_file)
{
  vcl_ofstream out(out_file.c_str());
  if (!out)
    return false;
  vcl_vector<vsol_point_3d_sptr> pts3d =
    this->world_points_3d();
  int n = pts3d.size();
  if (!n)
    return false;
  brct_algos::write_vrml_header(out);
  brct_algos::write_vrml_points(out, pts3d);
  brct_algos::write_vrml_trailer(out);
  out.close();
  return true;
}
