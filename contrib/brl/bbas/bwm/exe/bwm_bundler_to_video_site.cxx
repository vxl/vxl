//:
// \file

#include <bwm/bwm_observer_cam.h>
#include <bwm/video/bwm_video_cam_ostream.h>
#include <bwm/video/bwm_video_corr_sptr.h>
#include <bwm/video/bwm_video_corr.h>
#include <bwm/video/bwm_video_site_io.h>

#include <vcl_vector.h>
#include <vcl_set.h>

#include <vcl_iostream.h>
#include <vcl_fstream.h>
#include <vcl_string.h>
#include <vul/vul_arg.h>
#include <vul/vul_file.h>
#include <vgl/vgl_polygon.h>
#include <vgl/vgl_polygon_scan_iterator.h>
#include <vgl/algo/vgl_fit_plane_3d.h>
#include <vgl/vgl_distance.h>
#include <vgl/vgl_point_3d.h>
#include <vgl/vgl_vector_3d.h>
#include <vgl/vgl_box_3d.h>
#include <vgl/algo/vgl_rotation_3d.h>
#include <vnl/vnl_double_3.h>
#include <vnl/vnl_quaternion.h>
#include <vpgl/vpgl_camera.h>
#include <vpgl/vpgl_rational_camera.h>
#include <vpgl/vpgl_local_rational_camera.h>
#include <vpgl/vpgl_perspective_camera.h>
#include <vpgl/algo/vpgl_camera_bounds.h>

#include <vidl/vidl_image_list_istream.h>
#include <vnl/vnl_transpose.h>

#include <vil/vil_load.h>
#include <vil/vil_image_view_base.h>
#include <vil/vil_image_view.h>
#include <vil/vil_save.h>
#include <vcl_cstdlib.h> // for rand()
#include <bwm/bwm_site_mgr.h>
static void write_vrml_header(vcl_ofstream& str)
{
  str << "#VRML V2.0 utf8\n"
      << "Background {\n"
      << "  skyColor [ 0 0 0 ]\n"
      << "  groundColor [ 0 0 0 ]\n"
      << "}\n";
}
static void
wrtie_vrml_cameras(vcl_ofstream& str,vcl_vector<vpgl_perspective_camera<double> > & cams, double rad)
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
        << "      diffuseColor " << 1 << ' ' << 1.0 << ' ' << 0.0 << '\n'
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
    vnl_double_3 yaxis(0.0, 1.0, 0.0), pvec(r.x(), r.y(), r.z());
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
static void write_vrml_points(vcl_ofstream& str,
                              vcl_vector<vgl_point_3d<double> > const& pts3d, double rad=2.0)
{
  int n = pts3d.size();
  str<<"Shape {\n";
  str<<"geometry PointSet {\n";
  str<<"coord Coordinate {\n";
  str<<"point [\n";
  for (int i =0; i<n; i++)
    str<<pts3d[i].x()<<' '<<pts3d[i].y()<<' '<< pts3d[i].z() <<','<<"\n";

  str<<"]\n";
  str<<"}\n";
  str<<"color Color {\n";
  str<<"color [\n";
  for (int i =0; i<n; i++)
    str<<0<<' '<<1.0<<' '<< 0.0 <<','<<"\n";
  str<<"]\n";
  str<<"}\n";
  str<<"}\n";
  str<<"}\n";
}
bool fit_plane_ransac(vcl_vector<vgl_homg_point_3d<double> > & points, vgl_homg_plane_3d<double>  & plane)
{
  unsigned int nchoose=3;
  unsigned int nsize=points.size();
  unsigned int max_its = 500;
  double err=10.0;
  double inlier_dist = 0.01;
  vcl_vector<int> best_inliers;
  for (unsigned i=0;i<max_its;++i)
  {
    vcl_cout << '.';
    vcl_vector<vgl_homg_point_3d<double> > subset;
    vcl_vector<int> inliers;
    for (unsigned j=0;j<nchoose;++j)
      subset.push_back(points[vcl_rand()%nsize]);
    vcl_cout<<subset.size();vcl_cout.flush();
    vgl_fit_plane_3d<double> fit_plane(subset);
    if (fit_plane.fit(err))
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
  vcl_cout<<"Inliers "<<best_inliers.size()<<vcl_endl;
  if (fit_plane_inliers.fit(23.0))
  {
    plane=fit_plane_inliers.get_plane();
    return true;
  }
  else
    return false;
}

bool axis_align_scene(vcl_vector<bwm_video_corr_sptr> & corrs,
                      vcl_vector<vpgl_perspective_camera<double> > & cams)
{
  vcl_vector<vgl_homg_point_3d<double> > points;
  for (unsigned i=0;i<corrs.size();++i)
  {
    vgl_homg_point_3d<double> homg_world_pt(corrs[i]->world_pt());
    points.push_back(homg_world_pt);
  }

  // fit the plane
  vgl_fit_plane_3d<double> fit_plane(points);
  if (!fit_plane.fit(1e6))
    return false;

  vgl_homg_plane_3d<double> plane=fit_plane.get_plane();
  vgl_rotation_3d<double> rot_scene(plane.normal(),vgl_vector_3d<double>(0,0,1));

  double sumx=0,sumy=0,sumz=0;
  for (unsigned i=0;i<corrs.size();++i)
  {
    vgl_homg_point_3d<double> p(corrs[i]->world_pt());
    vgl_homg_point_3d<double> pnew=rot_scene*p;
    if (!pnew.ideal())
    {
      vgl_point_3d<double> pnew_nonhomg(pnew.x()/pnew.w(),pnew.y()/pnew.w(),pnew.z()/pnew.w());
      sumx+=pnew_nonhomg.x();sumy+=pnew_nonhomg.y();sumz+=pnew_nonhomg.z();
      corrs[i]->set_world_pt(pnew_nonhomg);
    }
  }
  vgl_point_3d<double> center(sumx/corrs.size(),sumy/corrs.size(),sumz/corrs.size());
  vnl_vector_fixed<double,3> tr(center.x(),center.y(),center.z());
  vcl_vector<vgl_homg_point_3d<double> > xformed_points;
  for (unsigned i=0;i<corrs.size();++i)
  {
    vgl_point_3d<double> p(corrs[i]->world_pt());
    corrs[i]->set_world_pt(vgl_point_3d<double>(p.x()-center.x(),p.y()-center.y(),p.z()-center.z()));
    xformed_points.push_back(vgl_homg_point_3d<double>(p.x()-center.x(),p.y()-center.y(),p.z()-center.z()));
  }
  vcl_vector<vpgl_perspective_camera<double> > new_cams;
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

  if (2*up<cams.size())// flip the world
  {
    vnl_quaternion<double> q(vnl_math::pi,0,0);
    vgl_rotation_3d<double> rotx_pi(q);
    for (unsigned i=0;i<corrs.size();++i)
    {
      vgl_homg_point_3d<double> p(corrs[i]->world_pt());
      vgl_homg_point_3d<double> pnew=rotx_pi*p;
      if (!pnew.ideal())
      {
        vgl_point_3d<double> pnew_nonhomg(pnew.x()/pnew.w(),pnew.y()/pnew.w(),pnew.z()/pnew.w());
        corrs[i]->set_world_pt(pnew_nonhomg);
      }
    }
    for (unsigned i=0;i<cams.size();++i)
    {
      // new rotation
      vgl_rotation_3d<double> rot_cami=new_cams[i].get_rotation()*rotx_pi.inverse();
      new_cams[i]=vpgl_perspective_camera<double>(new_cams[i].get_calibration(),rot_cami,new_cams[i].get_translation());
    }
  }
  for (unsigned i=0;i<cams.size();++i)
    cams[i]=new_cams[i];

  vgl_fit_plane_3d<double> fit_plane1(xformed_points);
  if (!fit_plane1.fit(1e6))
      return false;

  vgl_homg_plane_3d<double> plane1=fit_plane1.get_plane();

  return true;
}

// An executable that read bundler file and convert it into video site.
int main(int argc, char** argv)
{
  //Get Inputs

  vul_arg<vcl_string> bundlerfile   ("-bundler", "Output file of bundler",  "");
  vul_arg<vcl_string> cam_dir       ("-cam_dir",      "directory to store cams", "");
  vul_arg<vcl_string> img_dir       ("-img_dir",     "list of images filenames", "");
  vul_arg<vcl_string> site_name     ("-site_name",    "Name of the site", "");
  vul_arg<vcl_string> site_directory("-site_dir", "Directory for the site", "");
  vul_arg<vcl_string> cam_txt_dir   ("-cam_txt_dir",      "directory to store txt cams", "");
  vul_arg<vcl_string> vrml_file     ("-vrml_file",      "vrml file", "");

  vul_arg_parse(argc, argv);

  // open the bundler file
  vcl_ifstream bfile( bundlerfile().c_str() );
  if (!bfile)
  {
    vcl_cout<<"Error Opening Bundler output file"<<vcl_endl;
    return -1;
  }

  // verify image dir
  if (!vul_file::is_directory(img_dir().c_str()))
  {
    vcl_cout<<"Image directory does not exist"<<vcl_endl;
    return -1;
  }

  vidl_image_list_istream imgstream(img_dir()+"/*");

  if (!imgstream.is_open())
  {
    vcl_cout<<"Invalid image stream"<<vcl_endl;
    return -1;
  }

  // get image size
  unsigned ni=imgstream.width();
  unsigned nj=imgstream.height();

  // central point of the image
  vgl_point_2d<double> principal_point((double)ni/2,(double)nj/2);

  char buffer[1024];
  bfile.getline(buffer,1024); // read the header line

  if (bfile.eof())
  {
    vcl_cout<<"File Missing data"<<vcl_endl;
    return -1;
  }

  unsigned num_cams=0, num_pts=0;
  bfile>>num_cams>>num_pts; // reading number of cameras and number of 3-d pts

  // read the cams from bundler and write it to a directory
  vcl_vector<vpgl_perspective_camera<double> > cams;
  vcl_set<int> bad_cams;

  // reading the cameras from bundler
  for (unsigned i=0;i<num_cams;++i)
  {
    double f,k1,k2;
    vnl_matrix_fixed<double,3,3> R;
    vnl_vector_fixed<double,3> T;

    bfile>>f>>k1>>k2;
    bfile>>R>>T;

    // negating to convert bundlers camera facing towards -ve z to positive
    R(2,0) = -R(2,0);R(2,1) = -R(2,1);R(2,2) = -R(2,2);
    R(1,0) = -R(1,0);R(1,1) = -R(1,1);R(1,2) = -R(1,2);
    vgl_rotation_3d<double> rot(R);

    // negating to convert bundlers camera facing towards -ve z to positive
    vgl_vector_3d<double> t(T(0),-T(1),-T(2));
    if (f>0.0)
    {
      vpgl_calibration_matrix<double> K(f,principal_point) ;
      vpgl_perspective_camera<double> cam(K,rot,t);
      cams.push_back(cam);
    }
    else
    {
      vpgl_perspective_camera<double> cam;
      cams.push_back(cam);
      bad_cams.insert(i);
    }
  }


  vcl_vector<bwm_video_corr_sptr> corrs;
  //  Projection error
  double err=0;double cnt=0;
  double maxerr=0;
  int maxerrcam=-1;
  // read the correspondence and 3-d points
  for (unsigned i=0;i<num_pts;++i)
  {
    bwm_video_corr_sptr  corr=new bwm_video_corr();
    double x,y,z;

    // read the 3-d point
    bfile>>x>>y>>z;

    corr->set_world_pt(vgl_point_3d<double>(x,y,z));

    vgl_homg_point_3d<double> homg_world_pt(corr->world_pt());

    int r,g,b;
    // read the intenstiy but don't do anything with it right now.
    bfile>>r>>g>>b;

    unsigned num_views;
    bfile>>num_views;

    for (unsigned j=0;j<num_views;++j)
    {
      unsigned view_number, key_number;
      double img_x,img_y;

      
      bfile>>view_number>>key_number>>img_x>>img_y;
      
      img_x=img_x+principal_point.x();
      img_y=nj-(img_y+principal_point.y());
      double maxerrframe=0;
      double u=0,v=0;
      if (cams[view_number].is_behind_camera(homg_world_pt))
        bad_cams.insert(view_number);
      else
      {
        cams[view_number].project(x,y,z,u,v);
        double rms=vcl_sqrt((u-img_x)*(u-img_x)+(v-img_y)*(v-img_y));
        err+=rms;
        ++cnt;
      }
      corr->add(view_number,vgl_point_2d<double>(img_x,img_y));
    }
    corrs.push_back(corr);
  }
  vcl_cout<<"Avg Error per projection "<<err/cnt<<vcl_endl;
  vcl_cout<<"Max Error"<<maxerr<<" is in camera # "<<maxerrcam<<vcl_endl;
  // save it to a site
  if (!axis_align_scene(corrs,cams))
    return -1;
  bwm_video_cam_ostream camstream(cam_dir());
  for (unsigned i=0;i<num_cams;++i)
    camstream.write_camera(&cams[i]);
  camstream.close();

  char filename[1024];
  if (vul_file::is_directory(cam_txt_dir().c_str()))
  {
    for (unsigned i=0;i<num_cams;++i)
    {
      vcl_sprintf(filename,"%s/camera%05d.txt",cam_txt_dir().c_str(),i);
      vcl_ofstream ofile(filename);
      double u1,v1;
      cams[i].project(0,0,0,u1,v1);
      if (ofile)
      {
        ofile<<cams[i].get_calibration().get_matrix()<<'\n'
             <<cams[i].get_rotation().as_matrix()<<'\n'
             <<cams[i].get_translation().x()<<' '<<cams[i].get_translation().y()<<' '<<cams[i].get_translation().z()<<'\n';
      }
    }
  }

  vcl_vector<vgl_point_3d<double> > pts_3d;
  vgl_box_3d<double> bounding_box;
  for (unsigned i=0;i<corrs.size();++i)
  {
    bounding_box.add(corrs[i]->world_pt());
    pts_3d.push_back(corrs[i]->world_pt());
  }
  //: Dimesnions of the World
  vcl_cout<<"Bounding Box "<<bounding_box<<vcl_endl;
  vcl_cout<<"Center       "<<centre(pts_3d)<<vcl_endl;
  vcl_cout<<"Stddev       "<<stddev(pts_3d)<<vcl_endl;


  //: Determing the resolution of the cells
  vgl_ray_3d<double> cone_axis;
  double cone_half_angle;
  double solid_angle;

  vpgl_camera_bounds::pixel_solid_angle(cams[0],ni/4,nj/4,cone_axis,cone_half_angle,solid_angle);

  vgl_point_3d<double> cc=cams[0].camera_center();
  double res=2*(cc-centre(pts_3d)).length()*cone_half_angle;

  vcl_cout<<"Resolution     "<<res<<vcl_endl;
  vcl_ofstream os(vrml_file().c_str());
  if (os)
  {
      write_vrml_header(os);
      write_vrml_points(os,pts_3d,res);
      wrtie_vrml_cameras(os,cams,res*5);
  }
  else
      vcl_cout<<"ERROR OPENING  "<< vrml_file() <<vcl_endl;

  bwm_video_site_io site;
  site.set_name(site_name());
  site.set_corrs(corrs);
  site.set_site_directory(site_directory());
  site.set_video_path(img_dir()+"/*");
  site.set_camera_path(cam_dir()+"/*");

  vcl_string xml_filename=site_directory()+"/"+site_name()+".xml";
  site.x_write(xml_filename.c_str());

  vcl_cout<<"Bad cameras "<<bad_cams.size()<<" :";
  vcl_set<int>::iterator iter=bad_cams.begin();
  for (;iter!=bad_cams.end();++iter)
    vcl_cout<<*iter<<' ';


}
