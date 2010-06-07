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
#include <vpgl/vpgl_camera.h>
#include <vpgl/vpgl_rational_camera.h>
#include <vpgl/vpgl_local_rational_camera.h>
#include <vpgl/vpgl_perspective_camera.h>
#include <vidl/vidl_image_list_istream.h>
#include <vnl/vnl_transpose.h>

#include <vil/vil_load.h>
#include <vil/vil_image_view_base.h>
#include <vil/vil_image_view.h>
#include <vil/vil_save.h>
#include <vcl_cstdlib.h> // for rand()

bool fit_plane_ransac(vcl_vector<vgl_homg_point_3d<double> > & points, vgl_homg_plane_3d<double>  & plane)
{
  unsigned int nchoose=3;
  unsigned int nsize=points.size();
  unsigned int max_its = 500;
  double err=10.0;
  double inlier_dist = 0.01;
  vcl_vector<int> best_inliers;
  for (unsigned i=0;i<max_its;i++)
  {
    vcl_cout << '.';
    vcl_vector<vgl_homg_point_3d<double> > subset;
    vcl_vector<int> inliers;
    for (unsigned j=0;j<nchoose;j++)
      subset.push_back(points[vcl_rand()%nsize]);
    vcl_cout<<subset.size();vcl_cout.flush();
    vgl_fit_plane_3d<double> fit_plane(subset);
    if (fit_plane.fit(err))
    {
      vgl_homg_plane_3d<double> plane=fit_plane.get_plane();
      for (unsigned j=0;j<nsize;j++)
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

  for (unsigned i=0;i<best_inliers.size();i++)
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
  for (unsigned i=0;i<corrs.size();i++)
  {
    vgl_homg_point_3d<double> homg_world_pt(corrs[i]->world_pt());
    points.push_back(homg_world_pt);
  }

  // fit the plane
  vgl_fit_plane_3d<double> fit_plane(points);
  if (!fit_plane.fit(1e6))
    return false;

  vgl_homg_plane_3d<double> plane=fit_plane.get_plane();
  vcl_cout<<"Normal "<<plane.normal();
  vgl_rotation_3d<double> rot_scene(plane.normal(),vgl_vector_3d<double>(0,0,1));

  double sumx=0,sumy=0,sumz=0;
  for (unsigned i=0;i<corrs.size();i++)
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
  for (unsigned i=0;i<corrs.size();i++)
  {
    vgl_point_3d<double> p(corrs[i]->world_pt());
    corrs[i]->set_world_pt(vgl_point_3d<double>(p.x()-center.x(),p.y()-center.y(),p.z()-center.z()));
  }
  vcl_vector<vpgl_perspective_camera<double> > new_cams;
  int up=0;
  for (unsigned i=0;i<cams.size();i++)
  {
    // new rotation
    vgl_rotation_3d<double> rot_cami=cams[i].get_rotation()*rot_scene.inverse();

    // new translation
    vnl_vector_fixed<double,3> newtranslation=rot_cami.as_matrix()*tr;
    vgl_vector_3d<double> translation_vec(newtranslation[0],newtranslation[1],newtranslation[2]);
    vgl_vector_3d<double> tr_cami=cams[i].get_translation()+translation_vec;
    new_cams.push_back(vpgl_perspective_camera<double>(cams[i].get_calibration(),rot_cami,tr_cami));
    if (new_cams[i].get_camera_center().z()>0)
      up++;
  }

  if (up<cams.size()-up)// flip the world
  {
    vcl_cout<<"Flipping the world "<<vcl_endl;
    vnl_quaternion<double> q(vnl_math::pi,0,0);
    vgl_rotation_3d<double> rotx_pi(q);
    for (unsigned i=0;i<corrs.size();i++)
    {
      vgl_homg_point_3d<double> p(corrs[i]->world_pt());
      vgl_homg_point_3d<double> pnew=rotx_pi*p;
      if (!pnew.ideal())
      {
        vgl_point_3d<double> pnew_nonhomg(pnew.x()/pnew.w(),pnew.y()/pnew.w(),pnew.z()/pnew.w());
        corrs[i]->set_world_pt(pnew_nonhomg);
      }
    }
    for (unsigned i=0;i<cams.size();i++)
    {
      // new rotation
      vgl_rotation_3d<double> rot_cami=new_cams[i].get_rotation()*rotx_pi.inverse();
      new_cams[i]=vpgl_perspective_camera<double>(new_cams[i].get_calibration(),rot_cami,new_cams[i].get_translation());
    }
  }
  for (unsigned i=0;i<cams.size();i++)
	cams[i]=new_cams[i];
  return true;
}

// An executable that read bundler file and convert it into video site.
int main(int argc, char** argv)
{
  //Get Inputs

  vul_arg<vcl_string> bundlerfile("-bundler", "Output file of bundler",  "");
  vul_arg<vcl_string> cam_dir    ("-cam_dir",      "directory to store cams", "");
  vul_arg<vcl_string> img_dir   ("-img_dir",     "list of images filenames", "");
  vul_arg<vcl_string> site_name  ("-site_name",    "Name of the site", "");
  vul_arg<vcl_string> site_directory("-site_dir", "Directory for the site", "");
  vul_arg<vcl_string> cam_txt_dir    ("-cam_txt_dir",      "directory to store txt cams", "");

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
    vcl_cout<<"File Missing data "<<vcl_endl;
    return -1;
  }

  unsigned num_cams=0, num_pts=0;
  bfile>>num_cams>>num_pts; // reading number of cameras and number of 3-d pts

  // read the cams from bundler and write it to a directory
  vcl_vector<vpgl_perspective_camera<double> > cams;
  vcl_set<int> bad_cams;

  // reading the cameras from bundler
  for (unsigned i=0;i<num_cams;i++)
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
  double err=0;
  double cnt=0;
  // read the correspondence and 3-d points
  for (unsigned i=0;i<num_pts;i++)
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

    for (unsigned j=0;j<num_views;j++)
    {
      unsigned view_number, key_number;
      double img_x,img_y;
      bfile>>view_number>>key_number>>img_x>>img_y;
      img_x=img_x+principal_point.x();
      img_y=nj-(img_y+principal_point.y());
      double u=0,v=0;
      if (cams[view_number].is_behind_camera(homg_world_pt))
      {
        bad_cams.insert(view_number);
      }
      else
      {
        cams[view_number].project(x,y,z,u,v);
        err+=vcl_sqrt((u-img_x)*(u-img_x)+(v-img_y)*(v-img_y));
        cnt++;
      }
      corr->add(view_number,vgl_point_2d<double>(img_x,img_y));
    }
    corrs.push_back(corr);
  }
  // save it to a site
  vcl_cout<<"Bad cameras "<<bad_cams.size()<<" :";
  vcl_set<int>::iterator iter=bad_cams.begin();
  for (;iter!=bad_cams.end();iter++)
    vcl_cout<<*iter<<' ';
  vcl_cout<<vcl_endl<<"Error per projection"<<err/cnt<<vcl_endl;
  if (!axis_align_scene(corrs,cams))
    return -1;
  bwm_video_cam_ostream camstream(cam_dir());
  for (unsigned i=0;i<num_cams;i++)
    camstream.write_camera(&cams[i]);
  camstream.close();

  char filename[1024];
  if (vul_file::is_directory(cam_txt_dir().c_str()))
  {
    for (unsigned i=0;i<num_cams;i++)
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
  vgl_box_3d<double> bounding_box;
  for (unsigned i=0;i<corrs.size();i++)
    bounding_box.add(corrs[i]->world_pt());
  vcl_cout<<"Bounding Box "<<bounding_box<<vcl_endl;
  bwm_video_site_io site;
  site.set_name(site_name());
  site.set_corrs(corrs);
  site.set_site_directory(site_directory());
  site.set_video_path(img_dir()+"/*");
  site.set_camera_path(cam_dir()+"/*");

  vcl_string xml_filename=site_directory()+"/"+site_name()+".xml";
  site.x_write(xml_filename.c_str());
}
