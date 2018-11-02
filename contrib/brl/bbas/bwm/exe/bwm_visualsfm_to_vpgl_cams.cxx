#include <vector>
#include <set>
#include <iostream>
#include <fstream>
#include <string>
#include <cstdlib>
#include <bwm/bwm_observer_cam.h>
#include <bwm/video/bwm_video_cam_ostream.h>
#include <bwm/video/bwm_video_corr_sptr.h>
#include <bwm/video/bwm_video_corr.h>
#include <bwm/video/bwm_video_site_io.h>

#include <cassert>
#include <vul/vul_arg.h>
#include <vul/vul_file.h>
#include <vgl/algo/vgl_fit_plane_3d.h>
#include <vgl/vgl_distance.h>
#include <vgl/vgl_point_3d.h>
#include <vgl/vgl_vector_3d.h>
#include <vgl/vgl_box_3d.h>
#include <vgl/algo/vgl_orient_box_3d.h>
#include <vgl/algo/vgl_rotation_3d.h>
#include <vnl/vnl_double_3.h>
#include <vnl/vnl_quaternion.h>
#include <vpgl/vpgl_perspective_camera.h>
#include <vpgl/vpgl_calibration_matrix.h>

#include <vidl/vidl_image_list_istream.h>

#ifdef _MSC_VER
#  include <vcl_msvc_warnings.h>
#endif
#include <bwm/bwm_site_mgr.h>

#include <bxml/bxml_write.h>

static void write_vrml_header(std::ofstream& str)
{
  str << "#VRML V2.0 utf8\n"
      << "Background {\n"
      << "  skyColor [ 0 0 0 ]\n"
      << "  groundColor [ 0 0 0 ]\n"
      << "}\n";
}

static void
write_vrml_cameras(std::ofstream& str,std::vector<vpgl_perspective_camera<double> > & cams, double rad, std::set<int> const& bad_cams)
{
  str << "#VRML V2.0 utf8\n"
      << "Background {\n"
      << "  skyColor [ 0 0 0 ]\n"
      << "  groundColor [ 0 0 0 ]\n"
      << "}\n";
  int n = cams.size();
  for (int i =0; i<n; i++) {
    vgl_point_3d<double> cent =  cams[i].get_camera_center();
    str << "Transform {\n"
        << "translation " << cent.x() << ' ' << cent.y() << ' '
        << ' ' << cent.z() << '\n'
        << "children [\n"
        << "Shape {\n"
        << " appearance Appearance{\n"
        << "   material Material\n"
        << "    {\n"
        << "      diffuseColor " << 1.0 << ' ' << ( bad_cams.count(i) ? 0.0 : 1.0 ) << ' ' << 0.0 << '\n'
        << "      transparency " << 0.0 << '\n'
        << "    }\n"
        << "  }\n"
        << " geometry Sphere\n"
        <<   "{\n"
        << "  radius " << rad << '\n'
        <<  "   }\n"
        <<  "  }\n"
        <<  " ]\n"
        << "}\n";
    vgl_vector_3d<double> r = cams[i].principal_axis();
    vnl_double_3 yaxis(0.0, -1.0, 0.0), pvec(r.x(), r.y(), r.z());
    vgl_rotation_3d<double> rot(yaxis, pvec);
    vnl_quaternion<double> q = rot.as_quaternion();

    vnl_double_3 axis = q.axis();
    double ang = q.angle();
    str <<  "Transform {\n"
        << " translation " << cent.x()+6*rad*r.x() << ' ' << cent.y()+6*rad*r.y()
        << ' ' << cent.z()+6*rad*r.z() << '\n'
        << " rotation " << axis[0] << ' ' << axis[1] << ' ' << axis[2] << ' ' <<  ang << '\n'
        << "children [\n"
        << " Shape {\n"
        << " appearance Appearance{\n"
        << "  material Material\n"
        << "   {\n"
        << "     diffuseColor 1 0 0\n"
        << "     transparency 0\n"
        << "    }\n"
        << "  }\n"
        << " geometry Cylinder\n"
        << "{\n"
        << " radius "<<rad/3<<'\n'
        << " height " << 12*rad << '\n'
        << " }\n"
        << " }\n"
        << "]\n"
        << "}\n";
  }
}

static void write_vrml_points(std::ofstream& str,
                              std::vector<vgl_point_3d<double> > const& pts3d, double rad=2.0)
{
  int n = pts3d.size();
  str<<"Shape {\n"
     <<"geometry PointSet {\n"
     <<"coord Coordinate {\n"
     <<"point [\n";
  for (int i =0; i<n; i++)
    str<<pts3d[i].x()<<' '<<pts3d[i].y()<<' '<< pts3d[i].z() <<",\n";

  str<<"]\n"
     <<"}\n"
     <<"color Color {\n"
     <<"color [\n";
  for (int i =0; i<n; i++)
    str<<0<<' '<<1.0<<' '<< 0.0 <<",\n";
  str<<"]\n"
     <<"}\n"
     <<"}\n"
     <<"}\n";
}

#if 0 // unused!
static void write_vrml_box(std::ofstream& os,
                           vgl_box_3d<double> const& bounding_box,
                           vnl_vector_fixed<double,3> const& color, float const& transparency)
{
    os << "Transform {\n"
       << "  translation " << bounding_box.centroid_x() << ' ' << bounding_box.centroid_y() << ' ' << bounding_box.centroid_z() << '\n'
       << "     children[\n"
       << "     Shape {\n"
       << "         appearance Appearance{\n"
       << "                         material Material{\n"
       << "                                      diffuseColor " << color[0] << ' ' << color[1] << ' ' << color[2] << '\n'
       << "                                      transparency " << transparency << '\n'
       << "                                          }\n" //end material
       << "                     }\n" //end appearance
       << "         geometry Box {\n"
       << "         size " << bounding_box.width() << ' ' << bounding_box.height() << ' ' << bounding_box.depth() << '\n'
       << "         }\n" //end box
       << "     }\n" //end Shape
       << "     ]\n" //end children
       << "}\n"; //end Transform
}
#endif

bool fit_plane_ransac(std::vector<vgl_homg_point_3d<double> > & points, vgl_homg_plane_3d<double>  & plane)
{
  unsigned int nchoose=3;
  unsigned int nsize=points.size();
  unsigned int max_its = 500;
  double err=10.0;
  double inlier_dist = 0.01;
  std::vector<int> best_inliers;
  for (unsigned i=0;i<max_its;++i)
  {
    std::cout << '.';
    std::vector<vgl_homg_point_3d<double> > subset;
    std::vector<int> inliers;
    for (unsigned j=0;j<nchoose;++j)
      subset.push_back(points[std::rand()%nsize]);
    std::cout<<subset.size();std::cout.flush();
    vgl_fit_plane_3d<double> fit_plane(subset);
    if (fit_plane.fit(err, &std::cerr))
    {
      vgl_homg_plane_3d<double> plane=fit_plane.get_plane();
      for (unsigned j=0;j<nsize;++j)
      {
        double dist=vgl_distance<double>(points[j],plane);
        if (dist*dist<inlier_dist*inlier_dist)
        {
          inliers.push_back(j);
        }
      }
      if (inliers.size()>best_inliers.size())
        best_inliers=inliers;
    }
  }
  vgl_fit_plane_3d<double> fit_plane_inliers;

  for (unsigned i=0;i<best_inliers.size();++i)
  {
    fit_plane_inliers.add_point(points[best_inliers[i]]);
  }
  std::cout<<"Inliers "<<best_inliers.size()<<std::endl;
  if (fit_plane_inliers.fit(23.0, &std::cerr))
  {
    plane=fit_plane_inliers.get_plane();
    return true;
  }
  else
    return false;
}

bool axis_align_scene(std::vector<vgl_point_3d<double> > & corrs,
                      std::vector<vpgl_perspective_camera<double> > & cams)
{
  std::vector<vgl_homg_point_3d<double> > hpoints;
  for (unsigned i=0;i<corrs.size();++i)
  {
    vgl_homg_point_3d<double> homg_world_pt(corrs[i]);
    hpoints.push_back(homg_world_pt);
  }
  // fit the plane
  vgl_fit_plane_3d<double> fit_plane(hpoints);
  if (!fit_plane.fit(1e6, &std::cerr)) return false;
  vgl_homg_plane_3d<double> plane=fit_plane.get_plane();
  std::cout<<" Original Plane "<<plane<<std::endl;

  vgl_rotation_3d<double> rot_scene(plane.normal(),
                                    vgl_vector_3d<double>(0,0,1));

  std::cout<<"Rotation "<<rot_scene<<std::endl;
  for (unsigned i=0;i<corrs.size();++i)
  {
    vgl_homg_point_3d<double> p(corrs[i]);
    vgl_homg_point_3d<double> prot = rot_scene * p;
    corrs[i]=vgl_point_3d<double>(prot.x()/prot.w(),prot.y()/prot.w(),prot.z()/prot.w());
  }
  // translation
  vgl_point_3d<double> points_center = centre(corrs);

  vnl_vector_fixed<double,3> tr(points_center.x(),
                                points_center.y(),
                                points_center.z());

  std::vector<vgl_homg_point_3d<double> > xformed_points;
  for (unsigned i=0;i<corrs.size();++i)
  {
    vgl_point_3d<double> p(corrs[i]);
    corrs[i]=(vgl_point_3d<double>(p.x()-points_center.x(),
                                   p.y()-points_center.y(),
                                   p.z()-points_center.z()));
    xformed_points.push_back(vgl_homg_point_3d<double>(corrs[i]));
  }
  std::vector<vpgl_perspective_camera<double> > new_cams;
  unsigned int up=0;
  for (unsigned i=0;i<cams.size();++i)
  {
    // new rotation
    vgl_rotation_3d<double> rot_cami=cams[i].get_rotation()*rot_scene.inverse();
    // new translation
    vnl_vector_fixed<double,3> newtranslation=rot_cami.as_matrix()*tr;
    vgl_vector_3d<double> translation_vec(newtranslation[0],newtranslation[1],newtranslation[2]);
    vgl_vector_3d<double> tr_cami=cams[i].get_translation()+translation_vec;

    new_cams.push_back(vpgl_perspective_camera<double>(cams[i].get_calibration(),rot_cami,tr_cami));
    if (new_cams[i].get_camera_center().z()>0)
      ++up;
  }

  for (unsigned i=0;i<cams.size();++i)
    cams[i]=new_cams[i];

  vgl_fit_plane_3d<double> fit_plane1(xformed_points);
  if (!fit_plane1.fit(1e6, &std::cerr))
      return false;

  vgl_homg_plane_3d<double> plane1=fit_plane1.get_plane();

  std::cout<<plane1<<std::endl;
  return true;
}

vnl_vector_fixed<double,3> stddev( std::vector<vgl_point_3d<double> > const& v)
{
  unsigned n = v.size();
  assert(n>0);
  vnl_vector_fixed<double,3> m(0.0f), stddev(0.0f);

  for (unsigned i = 0; i < n; ++i)
  {
    m[0]+=v[i].x();
    m[1]+=v[i].y();
    m[2]+=v[i].z();
  }
  for (unsigned i = 0; i < 3; ++i)
    m[i]/=n;

  for (unsigned i = 0; i < n; ++i)
  {
    stddev[0] += (v[i].x()-m[0])*(v[i].x()-m[0]);
    stddev[1] += (v[i].y()-m[1])*(v[i].y()-m[1]);
    stddev[2] += (v[i].z()-m[2])*(v[i].z()-m[2]);
  }

  for (unsigned i = 0; i < 3; ++i)
    stddev[i] = std::sqrt(stddev[i]/(n-1));

  return stddev;
}

bool LoadNVM(std::ifstream& in,
             vgl_point_2d<double> principal_point,
             std::vector<vpgl_perspective_camera<double> >& camera_data,
             std::vector<vgl_point_3d<double> >& point_data,
             std::vector<vgl_point_2d<double> >& measurements,
             std::vector<int>& ptidx,
             std::vector<int>& camidx,
             std::vector<std::string>& names)
{
  int rotation_parameter_num = 4;
  std::string token;
  // bool format_r9t = false; // unused
  if (in.peek() == 'N')
  {
    in >> token; //file header
    if (std::strstr(token.c_str(), "R9T"))
    {
      rotation_parameter_num = 9;  //rotation as 3x3 matrix
      // format_r9t = true;
    }
  }
  int ncam = 0, npoint = 0, nproj = 0;
  // read # of cameras
  in >> ncam;  if (ncam <= 1) return false;
  //read the camera parameters
  camera_data.resize(ncam); // allocate the camera data
  names.resize(ncam);

  for (int i = 0; i < ncam; ++i)
  {
    double f, q[4], c[3], d[2];
    in >> token >> f ;
    vpgl_calibration_matrix<double> K(f,vgl_point_2d<double>(0,0));//principal_point) ;

    for (int j = 0; j < rotation_parameter_num; ++j) in >> q[j];
    in >> c[0] >> c[1] >> c[2] >> d[0] >> d[1];

    vnl_quaternion<double> quaternion(q[1],q[2],q[3],q[0]);
    vgl_rotation_3d<double> rot(quaternion);
    vgl_vector_3d<double> t(c[0],c[1],c[2]);
    vgl_point_3d<double> cc(c[0],c[1],c[2]);

    vpgl_perspective_camera<double> cam(K,cc,rot);
    camera_data[i] = cam;
    names[i] = token;
  }

  //////////////////////////////////////
  in >> npoint;   if (npoint <= 0) return false;

  //read image projections and 3D points.
  point_data.resize(npoint);
  for (int i = 0; i < npoint; ++i)
  {
    float pt[3]; int cc[3], npj;
    in  >> pt[0] >> pt[1] >> pt[2]
        >> cc[0] >> cc[1] >> cc[2] >> npj;
    for (int j = 0; j < npj; ++j)
    {
      int cidx, fidx; float imx, imy;
      in >> cidx >> fidx >> imx >> imy;

      camidx.push_back(cidx);  //camera index
      ptidx.push_back(i);    //point index

      double u,v ;
      camera_data[cidx].project(pt[0],pt[1],pt[2], u , v);

      //add a measurement to the vector
      measurements.push_back(vgl_point_2d<double>(imx, imy));
      ++nproj;
    }
    point_data[i]=vgl_point_3d<double>(pt[0],pt[1],pt[2]);
  }
  ///////////////////////////////////////////////////////////////////////////////
  std::cout << ncam << " cameras; " << npoint << " 3D points; " << nproj << " projections\n";
  return true;
}

// An executable that read bundler file and convert it into video site.
int main(int argc, char** argv)
{
  //Get Inputs

  vul_arg<std::string> bundlerfile   ("-vsfm", "Output file of bundler",  "");
  vul_arg<std::string> cam_dir       ("-cam_dir",      "directory to store cams", "");
  vul_arg<std::string> img_dir       ("-img_dir",     "list of images filenames", "");
  vul_arg<std::string> vrml_file     ("-vrml_file",      "vrml file", "");
  vul_arg<std::string> xml_file      ("-xml_file",      "xml file", "");
  vul_arg<bool>       draw_box      ("-draw_box", "Draw Bounding Box around points within 2*(standard deviation) from the center of scene",true);
  vul_arg<bool>       filter        ("-filter_cams", "Filter camera based on Reprojection error of 3d correspondences", false);
  vul_arg<float>      filter_thresh ("-filter_thresh", "Threshold for average rms value for a given view. Units are pixels", .75);

  vul_arg_parse(argc, argv);

  // verify image dir
  if (!vul_file::is_directory(img_dir().c_str()))
  {
    std::cout<<"Image directory does not exist"<<std::endl;
    return -1;
  }

  vidl_image_list_istream imgstream(img_dir()+"/*");
  //vidl_image_list_istream imgstream(img_dir()+"/*.jpg");

  if (!imgstream.is_open())
  {
    std::cout<<"Invalid image stream"<<std::endl;
    return -1;
  }

  // get image size
  unsigned ni=imgstream.width();
  unsigned nj=imgstream.height();

  // central point of the image
  vgl_point_2d<double> principal_point((double)ni/2,(double)nj/2);
  // open the bundler file
  std::ifstream bfile( bundlerfile().c_str() );
  if (!bfile)
  {
    std::cout<<"Error Opening Bundler output file"<<std::endl;
    return -1;
  }

  std::vector<vpgl_perspective_camera<double> > cams;
  std::vector<vgl_point_3d<double> > point_data;
  std::vector<vgl_point_2d<double> > measurements;
  std::vector<int> ptidx;
  std::vector<int> camidx;
  std::vector<std::string> names;

  LoadNVM(bfile,principal_point,cams,point_data,measurements, ptidx, camidx,names);
  if (!axis_align_scene(point_data,cams))
    return -1;
  std::set<int>  bad_cams;
  std::ofstream os(vrml_file().c_str());
  if (os)
  {
    write_vrml_header(os);
    write_vrml_points(os,point_data,0.01);
    write_vrml_cameras(os,cams,0.1,bad_cams);
  }
}
