#include <vcl_fstream.h>
#include <vcl_cmath.h> // for exp()
#include <vnl/vnl_math.h>
#include <vnl/vnl_numeric_traits.h>
#include <vnl/vnl_matlab_print2.h>
#include <vgl/algo/vgl_h_matrix_2d_compute_4point.h>
#include <vgl/algo/vgl_h_matrix_2d_compute_linear.h>
#include <brct/brct_plane_calibrator.h>

brct_plane_calibrator::brct_plane_calibrator()
{
  z_back_ = 0;
  z_front_ = 0;
  pts_3d_.resize(Z_FRONT+1);
  corrs_.resize(Z_FRONT+1);
  cam_width_.resize(RIGHT+1);
  cam_height_.resize(RIGHT+1);
  for(int i = 0; i<=RIGHT; i++)
    {
      cam_width_[i]=0;
      cam_height_[i]=0;
    }
  initial_homographies_.resize(Z_FRONT+1);
  current_homographies_.resize(Z_FRONT+1);
  for(int i = 0; i<=Z_FRONT; i++)
    {
      initial_homographies_[i].resize(RIGHT+1);
      current_homographies_[i].resize(RIGHT+1);
    }
  debug_ = false;
  points_3d_valid_ = false;
  initial_homographies_valid_ = false;
}

brct_plane_calibrator::~brct_plane_calibrator()
{
}

void brct_plane_calibrator::init_corrs()
{
  corrs_.clear();
  int n_cams = 2;//for now
  for(int i = 0; i<=Z_FRONT; i++)
    {
      int n_pts = pts_3d_[i].size();
      vcl_vector<brct_corr_sptr> corrs(n_pts);
      for(int j =0; j<n_pts; j++)
        corrs[j] = new brct_corr(n_cams, i, j);
      corrs_[i]=corrs;
    }
}
void brct_plane_calibrator::read_data(vcl_string const& point3d_file)
{
  vcl_ifstream instr(point3d_file.c_str());

  if (!instr)
    {
    vcl_cout<<"cannot open the file - "<< point3d_file <<'\n';
    return;
    }
  vcl_string label;
  instr >> label;
  if(!(label=="Z_BACK:"))
    {
      vcl_cout << "Bad file format\n";
      return;
    }
  instr >> z_back_;
  instr >> label;
  if(!(label=="NPOINTS:"))
    {
      vcl_cout << "Bad file format\n";
      return;
    }
  int npts=0;
  instr >> npts;
  instr >> label;
  if(!(label=="INDEX|X|Y|"))
    {
      vcl_cout << "Bad file format\n";
      return;
    }
  pts_3d_[Z_BACK].resize(npts);
  for(int i = 0; i<npts; i++)
    {
      int index=0;
      instr >> index;
      double x = 0, y=0;
      instr >> x;   instr >> y;
      pts_3d_[Z_BACK][index]= vgl_homg_point_2d<double>(x, y);
    }
  instr >> label;
  if(!(label=="Z_FRONT:"))
    {
      vcl_cout << "Bad file format\n";
      return;
    }
  instr >> z_front_;
  instr >> label;
  if(!(label=="NPOINTS:"))
    {
      vcl_cout << "Bad file format\n";
      return;
    }
  instr >> npts;
  instr >> label;
  if(!(label=="INDEX|X|Y|"))
    {
      vcl_cout << "Bad file format\n";
      return;
    }
  pts_3d_[Z_FRONT].resize(npts);
  for(int i = 0; i<npts; i++)
    {
      int index=0;
      instr >> index;
      double x = 0, y=0;
      instr >> x;   instr >> y;
      pts_3d_[Z_FRONT][index]= vgl_homg_point_2d<double>(x, y);
    }

  this->init_corrs();      
  points_3d_valid_ = true;
  initial_homographies_valid_ = false;
}


bool brct_plane_calibrator::set_image_size(const int cam,
                                           const int width,
                                           const int height)
{
  if(cam>RIGHT)
    return false;
  cam_width_[cam]=width;
  cam_height_[cam]=height;
  return true;
}
static bool four_image_corners(const int width, const int height,
                               vcl_vector<vgl_homg_point_2d<double> >& corners)
{
  if(!width || !height)
    return false;
  corners.clear();
  vgl_homg_point_2d<double> p00(0,0), p01(width,0);
  vgl_homg_point_2d<double> p10(width, height), p11(0, height);
  corners.push_back(p00);   corners.push_back(p01);
  corners.push_back(p10);   corners.push_back(p11);
  return true;
}

static vgl_homg_point_2d<double> 
closest_point(double x0, double y0, 
              vcl_vector<vgl_homg_point_2d<double> >& pts_3d)
{
  double d_min = vnl_numeric_traits<double>::maxval;
  vgl_homg_point_2d<double> closest;
  int i_min;
  for(int i = 0; i<pts_3d.size(); i++)
    {
      double x=pts_3d[i].x()/pts_3d[i].w(), y=pts_3d[i].y()/pts_3d[i].w();
      double d = vcl_sqrt((x-x0)*(x-x0)+(y-y0)*(y-y0));
      if(d<d_min)
        {
          i_min = i;
          d_min = d;
          closest = pts_3d[i];
        }
    }
  //replace the closest point with a remote point
  //so it will not be chosen again
  pts_3d[i_min] = vgl_homg_point_2d<double>(1e8, 1e8);
  return closest;
}
//keep in mind that the image y axis is flipped
static bool four_3d_corners(vcl_vector<vgl_homg_point_2d<double> > const& pts_3d,
                            vcl_vector<vgl_homg_point_2d<double> >& corners)
{
  int n = pts_3d.size();
  if(n<4)
    return false;
  corners.clear();
  // make a copy, since we will be removing found points
  vcl_vector<vgl_homg_point_2d<double> > temp = pts_3d;
 //find the bounding box of 3-d points
  double x_min=vnl_numeric_traits<double>::maxval, x_max=-x_min;
  double y_min=x_min, y_max=x_max;
  for(int i=0; i<n; i++)
    if(pts_3d[i].w()<1e-08)
      return false;
    else
      {
        double x=pts_3d[i].x()/pts_3d[i].w(), y=pts_3d[i].y()/pts_3d[i].w();
        x_min = vnl_math_min(x_min, x);
        x_max = vnl_math_max(x_max, x);
        y_min = vnl_math_min(y_min, y);
        y_max = vnl_math_max(y_max, y);
      }
  //find the point projecting into the upper left image corner
  corners.push_back(closest_point(x_min, y_max, temp));
  //find the point projecting into the upper right image corner
  corners.push_back(closest_point(x_max, y_max, temp));
  //find the point projecting into the lower right image corner
  corners.push_back(closest_point(x_max, y_min, temp));
  //find the point projecting into the lower left image corner
  corners.push_back(closest_point(x_min, y_min, temp));
  
  return true;
}
static bool
four_point_homography(vcl_vector<vgl_homg_point_2d<double> > const& points_3d, 
                      vcl_vector<vgl_homg_point_2d<double> > const& image_pts,
                      vgl_h_matrix_2d<double>& H)
{
  vgl_h_matrix_2d_compute_4point mapper;
  if(!mapper.compute(points_3d, image_pts, H))
    return false;
  return true;
}

bool brct_plane_calibrator::compute_initial_homographies()
{
  vcl_vector<vgl_homg_point_2d<double> > image_corners, corner_3d_points;
  vgl_h_matrix_2d<double> H;

  for(int plane =0; plane<=Z_FRONT; plane++)  
    if(!four_3d_corners(pts_3d_[plane], corner_3d_points))
      return false;
    else
      for(int cam = 0; cam<=RIGHT; cam++)
        if(!four_image_corners(cam_width_[cam], cam_height_[cam],
                               image_corners))
          return false;
        else
          if(four_point_homography(corner_3d_points, image_corners, H))
            initial_homographies_[plane][cam] = H;
 initial_homographies_valid_ = true;
 return true;
}

bool brct_plane_calibrator::write_corrs(vcl_string const& corrs_file)
{
  vcl_ofstream os(corrs_file.c_str());
  if (!os)
  {
    vcl_cout << "In brct_plane_calibrator::write_corrs -"
             << " could not open file " << corrs_file << '\n';
    return false;
  }
  for(int plane = 0; plane <=Z_FRONT; plane++)
    {
      os << "PLANE: " << plane << "\n";
      int n_corrs = corrs_[plane].size();
      os << "NCORRS: " << n_corrs << "\n";
      for(int c = 0; c<n_corrs; c++)
        {
          brct_corr_sptr corr = corrs_[plane][c];
          os << *corr << "\n";
        }
    }
  return true;
}

bool brct_plane_calibrator::read_corrs(vcl_string const& corrs_file)
{
  vcl_ifstream is(corrs_file.c_str());
  if (!is)
  {
    vcl_cout << "In brct_plane_calibrator::read_corrs -"
             << " could not open file " << corrs_file << '\n';
    return false;
  }
  vcl_vector<vcl_vector<brct_corr_sptr> > corrs(Z_FRONT+1);
  for(int p = 0; p<=Z_FRONT; p++)
  {
  vcl_string s;
  is >> s;
  if(!(s=="PLANE:"))
    return false;
  int plane=0;
  is >> plane;
  is >> s;
  if(!(s=="NCORRS:"))
    return false;
  int n_corrs = 0;
  is >> n_corrs;
  
  for(int i = 0; i<n_corrs; i++)
    {
      int n_cams = 0, plane = 0, index = 0;
      is>> s;
      if(!(s=="NCAMS:"))
        return false;
      is >> n_cams;
      is >> s;
      if(!(s=="P:"))
        return false;
      is >> plane;

      is >> s;
      if(!(s=="I:"))
        return false;
      is >> index;
      brct_corr_sptr corr = new brct_corr(n_cams, plane, index);
      for(int cam = 0; cam<n_cams; cam++)
        {
          double x = 0, y = 0;
          is >> s;
          if(!(s=="X:"))
            return false;
          is >> x;
		  is >> s;
          if(!(s=="Y:"))
            return false;
          is >> y;
          corr->set_match(cam, x, y);
        }
      corrs[plane].push_back(corr);
    }
  //file was sucessfully parsed
	corrs_ = corrs;
  }
 return true;
}

bool brct_plane_calibrator::compute_homographies()
{
  for(int plane = 0; plane<=Z_FRONT; plane++)
    for(int cam = 0; cam<=RIGHT; cam++)
      {
        //get the corrs
        vcl_vector<brct_corr_sptr>& corrs = corrs_[plane];
        //collect the corresponding points
        vcl_vector<vgl_homg_point_2d<double> > image_pts, pts_3d;        
        for(vcl_vector<brct_corr_sptr>::iterator cit = corrs.begin();
            cit != corrs.end(); cit++)
          if((*cit)->valid(cam))
            {
              pts_3d.push_back(pts_3d_[plane][(*cit)->index()]);
              image_pts.push_back((*cit)->match(cam));
            }
        int n_3d = pts_3d.size(), n_img = image_pts.size();
        if(n_3d < 4 || n_img < 4)
          {
            vcl_cout << "In brct_plane_calibrator::compute_homographies()-"
                     << " not enough correspondences\n";
            return false;
          }
        vgl_h_matrix_2d_compute_linear hcl;        
        vgl_h_matrix_2d<double> H;
        if(!hcl.compute(pts_3d, image_pts, H))
          {
            vcl_cout << "In brct_plane_calibrator::compute_homographies()-"
                     << " homography computation failed\n";
            return false;
          }
        current_homographies_[plane][cam]=H;
      } 
	return true;
}

bool
brct_plane_calibrator::write_homographies(vcl_string const& homography_file)
{
  vcl_ofstream os(homography_file.c_str());
  if (!os)
  {
    vcl_cout << "In brct_plane_calibrator::write_homographies -"
             << " could not open file " << homography_file << '\n';
    return false;
  }
  os << "N_PLANES: " <<  Z_FRONT+1 << "\n";
  os << "N_CAMS: " <<  RIGHT+1 << "\n";
  for(int plane = 0; plane<=Z_FRONT; plane++)
    for(int cam = 0; cam<=RIGHT; cam++)
      {
        os << "PLANE: " <<  plane << "\n";
        if(!plane)
          os << "Z: " << z_back_  << "\n";
        else
          os << "Z: " << z_front_ << "\n";
        os << "CAM: " <<   cam << "\n";
        //        vnl_matlab_print(os, current_homographies_[plane][cam].get_matrix());
        os << current_homographies_[plane][cam].get_matrix();
      }
	return true;
}

vcl_vector<vgl_point_2d<double> > 
brct_plane_calibrator::projected_3d_points_initial(const int plane,
                                                   const int cam)
{
  vcl_vector<vgl_point_2d<double> > temp;
  if(plane>Z_FRONT)
    return temp;
  if(cam>RIGHT)
    return temp;
  for(int i = 0; i<pts_3d_[plane].size(); i++)
    {
      vgl_homg_point_2d<double> hp = 
        initial_homographies_[plane][cam](pts_3d_[plane][i]);
      vgl_point_2d<double> p(hp);
      temp.push_back(p);
    }
  return temp;
}
vcl_vector<vgl_point_2d<double> > 
brct_plane_calibrator::projected_3d_points(const int plane, const int cam)
{
  vcl_vector<vgl_point_2d<double> > temp;
  if(plane>Z_FRONT)
    return temp;
  if(cam>RIGHT)
    return temp;
  for(int i = 0; i<pts_3d_[plane].size(); i++)
    {
      vgl_homg_point_2d<double> hp = 
        current_homographies_[plane][cam](pts_3d_[plane][i]);
      vgl_point_2d<double> p(hp);
      temp.push_back(p);
    }
  return temp;
}
