// \file
//
// \version 1.0
// \author  Gaston Araguas UTN Cordoba, Argentina
// \date    3 March, 2008

#include <vgui/vgui.h>
#include <vgui/vgui_easy2D_tableau.h>
#include <vgui/vgui_viewer2D_tableau.h>
#include <vgui/vgui_shell_tableau.h>
#include <vgl/vgl_homg_point_2d.h>
#include <vgl/algo/vgl_h_matrix_2d.h>
#include <vgl/algo/vgl_h_matrix_2d_compute_linear.h>
#include <vnl/vnl_math.h>

int main(int argc, char **argv)
{
  vgui::init(argc, argv);
  std::vector<vgl_homg_point_2d<double> > pointsA;
  std::vector<vgl_homg_point_2d<double> > pointsB;

  // Use an easy2D tableau which allows us to draw 2D objects:
  vgui_easy2D_tableau_new easy2D("Homography between two point sets");
  easy2D->set_foreground(0,1,0);
  easy2D->set_point_radius(5);

  // set A
  vgl_homg_point_2d<double> const p1(100,200,1);
  vgl_homg_point_2d<double> const p2(200,200,1);
  vgl_homg_point_2d<double> const p3(300,300,1);
  vgl_homg_point_2d<double> const p4(300,100,1);
  pointsA.push_back(p1);
  pointsA.push_back(p2);
  pointsA.push_back(p3);
  pointsA.push_back(p4);
  easy2D->add_point(float(p1.x()),float(p1.y()));
  easy2D->add_point(float(p2.x()),float(p2.y()));
  easy2D->add_point(float(p3.x()),float(p3.y()));
  easy2D->add_point(float(p4.x()),float(p4.y()));

  // connect all points with lines
  easy2D->set_foreground(1,0,0); easy2D->set_line_width(1);
  easy2D->add_line(float(p1.x()),float(p1.y()),float(p2.x()),float(p2.y()));
  easy2D->add_line(float(p2.x()),float(p2.y()),float(p3.x()),float(p3.y()));
  easy2D->add_line(float(p3.x()),float(p3.y()),float(p4.x()),float(p4.y()));
  easy2D->add_line(float(p4.x()),float(p4.y()),float(p1.x()),float(p1.y()));

  // draw a line from (0,0) to the center of gravity of set A
  vgl_homg_point_2d<double> centre_pointA( centre(pointsA).x()/centre(pointsA).w(),
                                           centre(pointsA).y()/centre(pointsA).w(),1.0 );
  easy2D->set_foreground(1,1,0); easy2D->set_point_radius(5);
  easy2D->add_point(float(centre_pointA.x()),float(centre_pointA.y()));
  easy2D->set_foreground(0,0,1); easy2D->set_line_width(3);
  easy2D->add_line(0,0,float(centre_pointA.x()),float(centre_pointA.y()));

  // rotation and translation
  double angle = -vnl_math::pi_over_180*20;
  vgl_h_matrix_2d<double> H; H.set_identity().set_translation(100.0,20.0).set_rotation(angle);

  // apply the rotation and translation to set A, obtain set B
  easy2D->set_foreground(0,1,0); easy2D->set_point_radius(5);
  vgl_homg_point_2d<double> tr_p1 = H*p1;
  vgl_homg_point_2d<double> tr_p2 = H*p2;
  vgl_homg_point_2d<double> tr_p3 = H*p3;
  vgl_homg_point_2d<double> tr_p4 = H*p4;
  pointsB.push_back(tr_p1);
  pointsB.push_back(tr_p2);
  pointsB.push_back(tr_p3);
  pointsB.push_back(tr_p4);
  easy2D->add_point(float(tr_p1.x()),float(tr_p1.y()));
  easy2D->add_point(float(tr_p2.x()),float(tr_p2.y()));
  easy2D->add_point(float(tr_p3.x()),float(tr_p3.y()));
  easy2D->add_point(float(tr_p4.x()),float(tr_p4.y()));

  // connect all points with lines
  easy2D->set_foreground(1,0,0); easy2D->set_line_width(1);
  easy2D->add_line(float(tr_p1.x()),float(tr_p1.y()),float(tr_p2.x()),float(tr_p2.y()));
  easy2D->add_line(float(tr_p2.x()),float(tr_p2.y()),float(tr_p3.x()),float(tr_p3.y()));
  easy2D->add_line(float(tr_p3.x()),float(tr_p3.y()),float(tr_p4.x()),float(tr_p4.y()));
  easy2D->add_line(float(tr_p4.x()),float(tr_p4.y()),float(tr_p1.x()),float(tr_p1.y()));

  // compute the homography between set A and set B
  vgl_h_matrix_2d_compute_linear hcl;
  vgl_h_matrix_2d<double> H_comp = hcl.compute(pointsA,pointsB);

  // translate and rotate the centre of gravity of set A using the computed homography H_comp
  vgl_homg_point_2d<double> centre_pointB = H_comp*centre_pointA;
  centre_pointB.set(centre_pointB.x()/centre_pointB.w(),centre_pointB.y()/centre_pointB.w(),1.0);

  // draw a line from (0,0) to transformed centre of gravity of set A, i.e. centre of gravity of set B
  easy2D->set_foreground(1,1,0); easy2D->set_point_radius(5);
  easy2D->add_point(float(centre_pointB.x()/centre_pointB.w()),float(centre_pointB.y()/centre_pointB.w()));
  easy2D->set_foreground(0,1,0); easy2D->set_line_width(3);
  easy2D->add_line(0,0,float(centre_pointB.x()),float(centre_pointB.y()));

  vgui_viewer2D_tableau_new viewer(easy2D);
  vgui_shell_tableau_new shell(viewer);
  return vgui::run(shell, 512, 512);
}
