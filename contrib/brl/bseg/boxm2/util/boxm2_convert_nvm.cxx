#include "boxm2_convert_nvm.h"
#include "boxm2_point_util.h"
//:
// \file
#include <vpgl/algo/vpgl_camera_bounds.h>
#include <vgl/algo/vgl_orient_box_3d.h>
#include <vgl/algo/vgl_rotation_3d.h>
#include <vnl/vnl_double_3.h>
#include <vnl/vnl_matrix_fixed.h>
#include <vnl/vnl_quaternion.h>
#include <vnl/algo/vnl_cholesky.h>
#include <vnl/algo/vnl_svd_fixed.h>
#include <vcl_cassert.h>
#include <vul/vul_file.h>

//: Main boxm2_convert_nvm function
//  Takes in bundle.out file and image directory that created img_dir
void boxm2_util_convert_nvm(vcl_string nvm_file,
                                vcl_string img_dir,
                                vcl_map<vcl_string, vpgl_perspective_camera<double>* >& cams,
                                vgl_box_3d<double>& bbox,
                                double& resolution)
{
  boxm2_convert_nvm b2s(nvm_file, img_dir);
  cams        = b2s.get_cams();
  bbox        = b2s.get_bbox();
  resolution  = b2s.get_resolution();
}

// reads bundler file and populates list of cameras, and a scene bounding box
boxm2_convert_nvm::boxm2_convert_nvm(vcl_string nvm_file, vcl_string img_dir)
{
  img_dir_ = img_dir;
  nvm_file_ = nvm_file;
  
  // verify image dir
  if (!vul_file::is_directory(img_dir.c_str()))
  {
    vcl_cout<<"boxm2_convert_nvm::Image directory does not exist"<<vcl_endl;
    return;
  }
  vidl_image_list_istream imgstream(img_dir+"/*");
  if (!imgstream.is_open())
  {
    vcl_cout<<"boxm2_convert_nvm::Invalid image stream"<<vcl_endl;
    return;
  }

  // get image size
  unsigned ni=imgstream.width();
  unsigned nj=imgstream.height();

  // central point of the image
  vgl_point_2d<double> ppoint((double)ni/2,(double)nj/2);  
  
  // open the bundler file
  vcl_ifstream bfile( nvm_file.c_str() );
  if (!bfile)
  {
    vcl_cout<<"boxm2_convert_nvm::Error Opening Bundler output file"<<vcl_endl;
    return;
  }
  this->read_cameras(bfile, ppoint);
  this->read_points(bfile, ppoint);
  vcl_cout<<"NVM file out projection error: "<<vcl_endl;
  boxm2_point_util::calc_projection_error(cams_, bad_cams_, corrs_, view_error_map_, view_count_map_);

  //--------------------------------------------------------------------------
  // make sure the scene is axis aligned
  ////--------------------------------------------------------------------------
  if (!boxm2_point_util::axis_align_scene(corrs_,cams_))
    return;

  //------------------------------------------------------------------------
  // Filter out the cams with very high error
  //------------------------------------------------------------------------
  boxm2_point_util::report_error(view_error_map_, view_count_map_, bad_cams_, 1.5f);

  //------------------------------------------------------------------------
  // Save camera and corresponding image file
  //------------------------------------------------------------------------
  for (unsigned i = 0; i < cams_.size(); ++i) {
    if ( !bad_cams_.count(i) ) {
      //imgstream.seek_frame(i);
      //vcl_string path = imgstream.current_path();
      vcl_string stripped_name = vul_file::strip_extension(names_[i]); 
      vcl_string path = img_dir + "/" + stripped_name + ".png";
      CamType* cam = new CamType(cams_[i]);
      final_cams_[path] = cam;
      //vcl_cout<<"Final cam: "<<path<<vcl_endl;
    }
  }//end camera write

  //------------------------------------------------------------------------
  // Save calc bounding box
  //------------------------------------------------------------------------
  vcl_vector<vgl_point_3d<double> > pts_3d;
  vgl_box_3d<double> bounding_box;
  for (unsigned i=0; i<corrs_.size(); ++i)
  {
    bounding_box.add(corrs_[i]->world_pt());
    pts_3d.push_back(corrs_[i]->world_pt());
  }

  // Dimensions of the World
  vcl_cout<<"Full Point Bounding Box "<<bounding_box<<vcl_endl;
  vgl_point_3d<double> c = centre(pts_3d);
  vcl_cout<<"Center of Gravity "<< c <<vcl_endl;
  vnl_vector_fixed<double,3> sigma = boxm2_point_util::stddev(pts_3d);
  vcl_cout<<"Point stddev "<< sigma <<vcl_endl;

  //--------------------------------------------------------------------------
  // Define dimensions to be used for a boxm scene
  // Note: x-y dimensions are kind of a good approximation
  // the z-dimension however suffers because most points tend to be on the ground and the average miss represents points off the gound
  //--------------------------------------------------------------------------
  double minx=-3.0f*sigma[0], miny=-3.0f*sigma[1], minz=-1.0f*sigma[2];
  double maxx= 3.0f*sigma[0], maxy= 3.0f*sigma[1], maxz= 4.0f*sigma[2];
  bbox_ = vgl_box_3d<double>(minx, miny, minz, maxx, maxy,maxz);

  //--------------------------------------------------------------------------
  // Determining the resolution of the cells
  //--------------------------------------------------------------------------
  int good_cam = 0;
  while ( bad_cams_.count(good_cam) > 0 ) good_cam++;
  //vcl_cout<<"Determining resolution of cells with cam: "<< good_cam << vcl_endl;

  vgl_ray_3d<double> cone_axis;
  double cone_half_angle, solid_angle;
  vpgl_camera_bounds::pixel_solid_angle(cams_[good_cam], ni/4, nj/4,cone_axis,cone_half_angle,solid_angle);
  vgl_point_3d<double> cc = cams_[good_cam].camera_center();
  resolution_ = 2*(cc-centre(pts_3d)).length()*cone_half_angle;
  //vcl_cout<<"Resolution     "<<resolution_<<vcl_endl;
}

//------------------------------------------------------------------------
// reading the cameras from nvm file
//------------------------------------------------------------------------
bool boxm2_convert_nvm::read_cameras(vcl_ifstream& in, vgl_point_2d<double> ppoint)
{
  int rotation_parameter_num = 4;
  vcl_string token;
  bool format_r9t = false;
  if (in.peek() == 'N')
  {
    in >> token; //file header
    if (vcl_strstr(token.c_str(), "R9T"))
    {
      rotation_parameter_num = 9;  //rotation as 3x3 matrix
      format_r9t = true;
    }
  }
  int ncam = 0, npoint = 0, nproj = 0;
  
  // read # of cameras
  in >> ncam;  if (ncam <= 1) return false;
  
  //read the camera parameters
  cams_.resize(ncam); // allocate the camera data
  names_.resize(ncam); // allocate token data

  //read cams one by one from file... 
  for (int i = 0; i < ncam; ++i)
  {
    double f, q[4], c[3], d[2];
    in >> token >> f ;
    vpgl_calibration_matrix<double> K(f,ppoint);//principal_point) ;

    for (int j = 0; j < rotation_parameter_num; ++j) in >> q[j];
    in >> c[0] >> c[1] >> c[2] >> d[0] >> d[1];

    vnl_quaternion<double> quaternion(q[1],q[2],q[3],q[0]);
    vgl_rotation_3d<double> rot(quaternion);
    vgl_vector_3d<double> t(c[0],c[1],c[2]);
    vgl_point_3d<double> cc(c[0],c[1],c[2]);

    vpgl_perspective_camera<double> cam(K,cc,rot);
    cams_[i] = cam;
    names_[i] = token;
  }
  return true; 
}

//------------------------------------------------------------------------
// Read points into vector of bwm_video_corr_sptrs
//------------------------------------------------------------------------
bool boxm2_convert_nvm::read_points(vcl_ifstream& in, vgl_point_2d<double> ppoint)
{
  int npoint;
  in >> npoint;   
  if (npoint <= 0) 
    return false;

  //read image projections and 3D points.
  for (int i = 0; i < npoint; ++i)
  {
    float pt[3]; int cc[3], npj;
    in  >> pt[0] >> pt[1] >> pt[2]
        >> cc[0] >> cc[1] >> cc[2] >> npj;
    
    //create new bwm video corr
    bwm_video_corr_sptr corr = new bwm_video_corr();
    corr->set_world_pt(vgl_point_3d<double>(pt[0],pt[1],pt[2]));
    vgl_homg_point_3d<double> homg_world_pt(corr->world_pt());
    
    for (int j = 0; j < npj; ++j)
    {
      int cidx, fidx;   //camera index (view number), fidx?
      double imx, imy;  //image x and y
      in >> cidx >> fidx >> imx >> imy;
      imx += ppoint.x(); 
      imy += ppoint.y(); 

      //track correlations
      corr->add(cidx, vgl_point_2d<double>(imx,imy));
    }
    corrs_.push_back(corr); 
  }
  return true;
}
