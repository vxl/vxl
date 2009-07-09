//:
// \file
#include <testlib/testlib_test.h>
#include <bvpl/bvpl_direction_to_color_map.h>
#include <vgl/vgl_point_3d.h>
#include <vcl_iostream.h>

//: Test changes
static void test_direction_to_color_map()
{
  float theta_res = float(vnl_math::pi_over_4);
  float phi_res = float(vnl_math::pi_over_4);

  vcl_vector<vgl_point_3d<double> >  samples;
  float theta = 0.0f, phi = 0.0f;

  samples.push_back(vgl_point_3d<double>(0.0,0.0,1.0));
  samples.push_back(vgl_point_3d<double>(0.0,0.0,-1.0));

  // theta=pi/4,pi/2,3pi/4
  for (phi=vnl_math::pi_over_4;phi <= 3*float(vnl_math::pi_over_4);)
  {
    for (theta=0.0f;theta<float(2.0*vnl_math::pi-theta_res/2.0); )
    {
      samples.push_back(vgl_point_3d<double>(vcl_cos(theta) * vcl_sin(phi),vcl_sin(theta) * vcl_sin(phi),vcl_cos(phi)));
      theta +=theta_res;
    }

    phi+=phi_res;
  }
  vcl_vector<vgl_point_3d<double> >  proj_on_cube;
  project_sphereical_samples_to_cubes(samples,proj_on_cube);
  vcl_vector<vgl_point_3d<double> > p3d=peano_curve_on_cube(2);
  vcl_map<vgl_point_3d<double>,float,point_3d_cmp> indices=find_closest_points_from_cube_to_peano_curve(samples,p3d,proj_on_cube);
#if 0
  vcl_ofstream ofile("d:/vj/test_proj_samples.txt");
  for (unsigned i=0;i<proj_on_cube.size();++i)
  {
    ofile<<proj_on_cube[i].x()<<' '<<proj_on_cube[i].y()<<' '<<proj_on_cube[i].z()<<'\n';
  }
  ofile.close();

  vcl_ofstream ofile1("d:/vj/test_peano_curve.txt");

  for (unsigned i=0;i<p3d.size();++i)
    ofile1<<p3d[i].x()<<' '<<p3d[i].y()<<' '<<p3d[i].z()<<'\n';
  ofile1.close();

  vcl_ofstream ofile2("d:/vj/test_indices.txt");
  for (unsigned i=0;i<indices.size();++i)
    ofile2<<indices[i]<<'\n';
  ofile2.close();
#endif
}

TESTMAIN( test_direction_to_color_map );
