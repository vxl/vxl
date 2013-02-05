#include <testlib/testlib_test.h>
#include <vcl_vector.h>
#include <vsph/vsph_unit_sphere.h>
#include <vsph/vsph_sph_point_3d.h>
#include <depth_map/depth_map_scene.h>
#include <depth_map/depth_map_region.h>
#include <volm/volm_spherical_query_region.h>
#include <volm/volm_spherical_region_query.h>
#include <volm/volm_spherical_container.h>
#include <volm/volm_spherical_container_sptr.h>
#include <volm/volm_spherical_shell_container.h>
#include <volm/volm_spherical_shell_container_sptr.h>
#include <volm/volm_io.h>
#include <volm/volm_camera_space.h>

static void test_region_query()
{
  // create the depth interval using volm_spherical_container

  // parameter for coast
  float vmin = 2.0f;         // min voxel resolution
  float dmax = 3000.0f;      // maximum depth
  float solid_angle = 2.0f;
  volm_spherical_container_sptr sph = new volm_spherical_container(solid_angle,vmin,dmax);

  vcl_string depth_scene_path = "c:/Users/mundy/VisionSystems/Finder/VolumetricQuery/feb_test_queries/tile6_drainage/p1a_test10/p1a_res10_groundtruth_labelme.vsl";

  depth_map_scene_sptr dms = new depth_map_scene;
  vsl_b_ifstream dis(depth_scene_path.c_str());
  if (!dis)
    return;
  dms->b_read(dis);
  dis.close();
  double head_mid=0.0, head_radius=180.0, head_inc=45.0;
  double tilt_mid=90.0, tilt_radius=20.0, tilt_inc=10.0;
  double roll_mid=0.0,  roll_radius=3.0,  roll_inc=3.0;
  double top_fov_vals[] = {3.0,  4.0, 5.0, 12.0, 17.0, 18.0,19.0, 20.0, 24.0};
  vcl_vector<double> fovs(top_fov_vals, top_fov_vals + 9);
  double altitude = 1.6;
  unsigned ni = dms->ni(), nj = dms->nj();
  volm_camera_space_sptr cs_ptr =
    new volm_camera_space(fovs, altitude, ni, nj,
                          head_mid, head_radius, head_inc,
                          tilt_mid, tilt_radius, tilt_inc,
                          roll_mid, roll_radius, roll_inc);
  volm_spherical_region_query srq(dms, cs_ptr, sph);

  srq.print(vcl_cout);


#if 1
  vsl_b_ifstream is("c:/Users/mundy/VisionSystems/Finder/VolumetricQuery/unit_sphere_2.vsl");
  vsph_unit_sphere_sptr usph_ptr;
  vsl_b_read(is, usph_ptr);
  vcl_string reg_display_path = "c:/Users/mundy/VisionSystems/Finder/VolumetricQuery/feb_test_queries/tile6_drainage/p1a_test10/p1a_res10_groundtruth_region_display.wrl";
  srq.display_query_regions(usph_ptr, reg_display_path, 1);
#endif
#if 1
  /// test unit sphere equivalence
  // create spherical shell for query rays
  // parameter for coast
  float cap_angle = 180.0f;
  float point_angle = 2.0f;//for coast (2 really)
  double radius = 1;
  float top_angle = 75.0f;
  float bottom_angle = 75.0f;

  volm_spherical_shell_container_sptr sph_shell = new volm_spherical_shell_container(radius, cap_angle, point_angle, top_angle, bottom_angle);
  vcl_cout << "spherical shell size = " <<sph_shell->get_container_size() << '\n';

  vsph_unit_sphere usph(point_angle, top_angle, (180.0-bottom_angle));
  vcl_cout << "unit sphere size = " << usph.size() << '\n';

  vcl_cout << "\nStart comparison ----\n";
  vcl_vector<vgl_point_3d<double> > shell_cart_pts = sph_shell->cart_points();
  vcl_vector<vgl_vector_3d<double> > usph_cart_pts = usph.cart_vectors();
  vcl_vector<vsph_sph_point_3d> shell_sph_pts = sph_shell->sph_points();
  vcl_vector<vsph_sph_point_2d > usph_sph_pts = usph.sph_points();

  bool good = true;
  double tol = 0.002;
  unsigned nshell = shell_cart_pts.size();
  unsigned nusph = usph_cart_pts.size();
  vcl_cout << "Nsh =" << nshell << " Nusp = " << nusph << vcl_flush;
  if (nshell != nusph) {
    vcl_cout << ">>>>>>> BAD COUNT <<<<<<\n";
  }
  double max_mind = 0.0;
  for (unsigned i = 0; i<nusph; ++i) {
    double min_d = 10000.0;
    vgl_vector_3d<double> usp = usph_cart_pts[i];
    vsph_sph_point_2d uspp = usph_sph_pts[i];
    vgl_vector_3d<double> v_close;
    unsigned int jclose;
    for (unsigned j = 0; j<nshell; ++j) {
      vgl_point_3d<double> ssp = shell_cart_pts[j];
      vgl_vector_3d<double> temp(ssp.x(), ssp.y(), ssp.z());
      double dist = angle(usp, temp);
      if (dist<min_d) {
        min_d = dist;
        v_close = temp;
        jclose = j;
      }
    }
    if (min_d > max_mind)
      max_mind = min_d;
    vsph_sph_point_3d shpp = shell_sph_pts[jclose];
    double dist_sph = vcl_fabs(shpp.theta_-uspp.theta_) +
      vcl_fabs(shpp.phi_-uspp.phi_);
    if (min_d > tol || jclose!=i || dist_sph >tol)
      vcl_cout << '[' << i << "]/[" << jclose << "]-[" << min_d << "]:(" << usp.x() << ' ' << v_close.x() << "),("
               << usp.y() << ' ' << v_close.y() << "),("
               << usp.z() << ' ' << v_close.z() << ")\n";
  }
  vcl_cout << "Max distance between corresponding verts = " << max_mind << '\n';

  vcl_string vert_path = "c:/Users/mundy/VisionSystems/Finder/VolumetricQuery/verts.wrl";
  usph.display_vertices(vert_path);
  vcl_string edge_path = "c:/Users/mundy/VisionSystems/Finder/VolumetricQuery/edges.wrl";
  usph.display_edges(edge_path);

#endif // 1
}

TESTMAIN(test_region_query);
