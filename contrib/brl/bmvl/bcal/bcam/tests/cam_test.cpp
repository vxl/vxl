#include <bmvl/bcal/bcam/zhang_camera_node.h>
#include <bmvl/bcal/bcam/zhang_linear_calibrate.h>
#include <bmvl/bcal/bcam/calibrate_plane.h>
#include <bmvl/bcal/bcam/camera_graph.h>
#include <bmvl/bcal/bcam/euclidean_transformation.h> 

int main()
{

  camera_graph<calibrate_plane, zhang_camera_node, euclidean_transformation> cg;
  
  // initialize the template plane
  cg.getSource()->readData("Model.txt");

  // add a camera with 5 views into a graph
  int camID = cg.addVertex();

  // create time beats. 
  vcl_vector<double> t_beats(5);
  t_beats[0] = 0;
  t_beats[1] = 1;
  t_beats[2] = 2;
  t_beats[3] = 3;
  t_beats[4] = 4;
  cg.get_vertex(camID)->set_beat(t_beats);

  cg.get_vertex(camID)->readData("data1.txt" , 0);
  cg.get_vertex(camID)->readData("data2.txt" , 1);
  cg.get_vertex(camID)->readData("data3.txt" , 2);
  cg.get_vertex(camID)->readData("data4.txt" , 3);
  cg.get_vertex(camID)->readData("data5.txt" , 4);

  // do the calibration
  zhang_linear_calibrate lc;
  lc.setCameraGraph(&cg);
  lc.calibrate();

  return 0;
}

