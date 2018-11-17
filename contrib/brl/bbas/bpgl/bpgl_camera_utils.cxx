#include <cmath>
#include <vector>
#include <iostream>
#include <algorithm>
#include <fstream>
#include "bpgl_camera_utils.h"
//:
// \file
#ifdef _MSC_VER
#  include <vcl_msvc_warnings.h>
#endif
#include <vul/vul_file.h>
#include <vul/vul_file_iterator.h>
#include <vgl/vgl_point_2d.h>
#include <vgl/vgl_line_2d.h>
#include <vgl/vgl_vector_2d.h>
#include <vnl/vnl_math.h>
#include <vnl/vnl_matrix_fixed.h>
#include <vnl/vnl_matrix.h>
#include <vnl/vnl_vector.h>
#include <vnl/vnl_cross.h>
#include <vnl/algo/vnl_svd.h>
#include <vgl/algo/vgl_h_matrix_3d.h>
#include <vgl/algo/vgl_rotation_3d.h>

vpgl_perspective_camera<double> bpgl_camera_utils::
camera_from_horizon(double focal_length, double principal_pt_u,
                    double principal_pt_v, double cam_height,
                    double a, double b, double c)
{
  // assume the x vanishing point is at infinity
  // assume that the y vanishing point is the intersection
  // of a vertical line from the principal point and the
  // horizon line. The distance from the principal point to the
  // vanishing point defines the rotation about the x axis of the camera
  double norm = std::sqrt(a*a + b*b);
  vgl_line_2d<double> line(a/norm, b/norm, c/norm);
  vgl_vector_2d<double> line_dir = line.direction();
  line_dir = normalize(line_dir);
  vgl_vector_2d<double> pp(principal_pt_u, principal_pt_v);
  vpgl_calibration_matrix<double> K(focal_length,
                                    vgl_point_2d<double>(pp.x(), pp.y()));
  // vanishing point 2
  vnl_matrix<double> m(2,2);
  vnl_vector<double> w(2);
  m[0][0]=a; m[0][1]=b;
  m[1][0]=line_dir.x();m[1][1]=line_dir.y();
  w[0]=-c; w[1] = dot_product(pp, line_dir);
  vnl_svd<double> svd(m);
  vnl_vector<double> vp2 = svd.solve(w);
  //lambda_2 can now be computed
  double tempu2 =(vp2[0]-pp.x())/focal_length;
  double tempv2 =(vp2[1]-pp.y())/focal_length;
  double lambda_2 = 1.0 + tempu2*tempu2 + tempv2*tempv2;
  lambda_2 = std::sqrt(1.0/lambda_2);
  // vanishing point 3 can now be computed
  m[0][0]=(vp2[0]-pp.x()); m[0][1]=(vp2[1]-pp.y());
  w[0]=(vp2[0]-pp.x())*pp.x()+
    (vp2[1]-pp.y())*pp.y()-(focal_length*focal_length);
  vnl_svd<double> svd3(m);
  vnl_vector<double> vp3 = svd3.solve(w);
  double tempu3 = (vp3[0]-pp.x())/focal_length;
  double tempv3 =(vp3[1]-pp.y())/focal_length;
  double lambda_3 = 1.0 + tempu3*tempu3 + tempv3*tempv3;
  lambda_3 = std::sqrt(1.0/lambda_3);
  // lambda_3 is negative if the horizon is above the
  // principal point and positive if below
  double sign = (pp.x()*line.a()) + (pp.y()*line.b()) + line.c();
  if (sign>0.0) lambda_3*=-1.0;
  vnl_matrix_fixed<double, 3, 3> R;
  R[0][0]=line_dir.x(); R[0][1]=lambda_2*tempu2; R[0][2]=lambda_3*tempu3;
  R[1][0]=line_dir.y(); R[1][1]=lambda_2*tempv2; R[1][2]=lambda_3*tempv3;
  R[2][0]=0.0;          R[2][1] = lambda_2;      R[2][2]=lambda_3;
  vpgl_perspective_camera<double> cam;
  cam.set_calibration(K);
  cam.set_rotation(vgl_rotation_3d<double>(R));
  //camera height off ground plane
  vgl_point_3d<double> cc(0.0,0.0,cam_height);
  cam.set_camera_center(cc);
  return cam;
}

vpgl_perspective_camera<double> bpgl_camera_utils::
    camera_from_kml(double ni, double nj, double right_fov, double top_fov,
                    double altitude, double heading,
                    double tilt, double roll)
{
  double ppu = ni/2, ppv = nj/2;
  //get focal length
  // right_fov = atan(ppu/f), top_fov = atan(ppv/f)

  double dtor = vnl_math::pi_over_180;
  double tr = std::tan(right_fov*dtor), tt = std::tan(top_fov*dtor);
  double fr = ppu/tr, ft=ppv/tt;
  double f = 0.5*(fr+ft);

  vpgl_calibration_matrix<double> K(f,vgl_point_2d<double>(ppu, ppv));
  vpgl_perspective_camera<double> cam;
  cam.set_calibration(K);
  vnl_vector_fixed<double,3> z_axis(0.0, 0.0, 1.0);//z axis

  // camera principal ray direction considering tilt
  double c_tilt = std::cos(tilt*dtor), s_tilt = std::sin(tilt*dtor);
  vnl_vector_fixed<double,3> principal_ray(0.0, s_tilt, -c_tilt);

  //the rotation that moves z to the principal ray direction
  vgl_rotation_3d<double> R_axis(z_axis, principal_ray);
  // rotation for heading (note minus sign is needed, check definition JLM)
  vgl_rotation_3d<double> R_head(0.0, 0.0, -heading*dtor);
  // composition of tilt and heading
  vgl_rotation_3d<double> R_tot = R_head*R_axis;
  // rotation for roll
  vgl_rotation_3d<double> Rr(0.0, 0.0, roll*dtor);

  // note that the inverse axis rotation is applied to the camera
  //  vgl_rotation_3d<double> R_cam =Rr*(R_axis.inverse());
  vgl_rotation_3d<double> R_cam =Rr*(R_tot.inverse());

  cam.set_rotation(R_cam);

  vgl_point_3d<double> c(0.0,0.0,altitude);
  cam.set_camera_center(c);
#ifdef DEBUG
  std::cout << "axis Rotation\n " << R_axis.as_matrix() << '\n'
           << "roll Rotation\n " << Rr.as_matrix() << '\n'
           << "R_cam\n " << R_cam.as_matrix() << '\n'
           << "cam center " << cam.get_camera_center()<< '\n';
#endif
  return cam;
}

//: returns a list of cameras from specified directory
std::vector<vpgl_perspective_camera<double>* > bpgl_camera_utils::cameras_from_directory(const std::string& dir)
{
    std::vector<vpgl_perspective_camera<double>* > toReturn;
    if (!vul_file::is_directory(dir.c_str()) ) {
        std::cerr<<"Cam dir is not a directory\n";
        return toReturn;
    }

    //get all of the cam and image files, sort them
    std::string camglob=dir+"/*.txt";
    vul_file_iterator file_it(camglob.c_str());
    std::vector<std::string> cam_files;
    while (file_it) {
        std::string camName(file_it());
        cam_files.push_back(camName);
        ++file_it;
    }
    std::sort(cam_files.begin(), cam_files.end());

    //take sorted lists and load from file
    std::vector<std::string>::iterator iter;
    for (iter = cam_files.begin(); iter != cam_files.end(); ++iter)
    {
        //load camera from file
        std::ifstream ifs(iter->c_str());
        auto* pcam =new vpgl_perspective_camera<double>;
        if (!ifs.is_open()) {
            std::cerr << "Failed to open file " << *iter << '\n';
            return toReturn;
        }
        else  {
            ifs >> *pcam;
        }

        toReturn.push_back(pcam);
    }
    return toReturn;
}

// the horizon line is the cross product of the X and Y axis vanishing points
vgl_line_2d<double> bpgl_camera_utils::
horizon(vpgl_perspective_camera<double> const& cam)
{
  vgl_homg_point_3d<double> px(1.0, 0.0, 0.0, 0.0), py(0.0, 1.0, 0.0, 0.0);
  vgl_homg_point_2d<double> ppx = cam.project(px), ppy = cam.project(py);
  vnl_vector_fixed<double, 3> vpx(ppx.x(),ppx.y(), ppx.w());
  vnl_vector_fixed<double, 3> vpy(ppy.x(),ppy.y(), ppy.w());
  vnl_vector_fixed<double,3> hor = vnl_cross_3d(vpx, vpy);
  vgl_line_2d<double> hor_line(hor[0],hor[1],hor[2]);
  return hor_line;
}

std::string bpgl_camera_utils::get_string(double  /*ni*/, double  /*nj*/, double  /*right_f*/, double top_f, double  /*alt*/, double head, double tilt, double roll)
{
  std::stringstream str;
  str << "_h_" << head << "_t_" << tilt << "_r_" << roll << "_top_fov_" << top_f;
  return str.str();
}
void bpgl_camera_utils::
camera_parameters(vpgl_perspective_camera<double> const& cam,
                  double& rodrigues_x, double& rodrigues_y,
                  double& rodrigues_z, double& tx, double& ty,
                  double& tz, double& focal_length_u, double& focal_length_v,
                  double& principal_pt_u, double& principal_pt_v){

    const vgl_rotation_3d<double>& R = cam.get_rotation();
    vnl_vector_fixed<double ,3> rod = R.as_rodrigues();
    rodrigues_x = rod[0];  rodrigues_y = rod[1];   rodrigues_z = rod[2];
    vgl_vector_3d<double> t = cam.get_translation();
    tx = t.x(); ty = t.y(); tz = t.z();
    vnl_matrix_fixed<double, 3, 3> K = cam.get_calibration().get_matrix();
    focal_length_u = K[0][0];  focal_length_v = K[1][1];
    principal_pt_u = K[0][2];  principal_pt_v = K[1][2];
}

bool bpgl_camera_utils::
camera_from_parameters(double rodrigues_x, double rodrigues_y,
                       double rodrigues_z, double tx, double ty,
                       double tz, double focal_length_u, double focal_length_v,
                       double principal_pt_u, double principal_pt_v,
                       vpgl_perspective_camera<double>& cam){
  vnl_vector_fixed<double, 3> rod(rodrigues_x, rodrigues_y, rodrigues_z);
  double ang = rod.magnitude();
  if(ang<0.0 || ang>vnl_math::twopi)
    return false;
  vgl_rotation_3d<double> R(rod);
  vgl_vector_3d<double> t(tx, ty, tz);
  vnl_matrix_fixed<double, 3, 3> K(0.0);
  if(focal_length_u<=0.0||focal_length_v<=0.0)
    return false;
  K[0][0]=focal_length_u;  K[1][1]=focal_length_v;
  K[0][2]=principal_pt_u;  K[1][2]=principal_pt_v;
  K[2][2] = 1.0;
  vpgl_calibration_matrix<double> Km(K);
  cam = vpgl_perspective_camera<double>(Km, R, t);
  return true;
}
