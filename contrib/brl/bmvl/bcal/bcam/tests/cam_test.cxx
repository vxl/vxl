#include <bmvl/bcal/bcam/zhang_camera_node.h>
#include <bmvl/bcal/bcam/zhang_linear_calibrate.h>
#include <bmvl/bcal/bcam/calibrate_plane.h>
#include <bmvl/bcal/bcam/camera_graph.h>
#include <bmvl/bcal/bcam/euclidean_transformation.h>
#include <vcl_string.h>
#include <vcl_cassert.h>

void testing_graph()
{
  vcl_cout<<"\n--------------testing graph -------------\n";
  camera_graph<calibrate_plane, zhang_camera_node, euclidean_transformation> cg;

  // add three vertex from source point.
  int source_id = cg.get_source_id();
  cg.add_vertex(source_id);
  cg.add_vertex(source_id);
  cg.add_vertex(source_id);

  cg.print(vcl_cout);
}

void testing_linear_calibration(vcl_string const& directory)
{
  vcl_cout<<"\n--------------testing calibration -------------\n";

  camera_graph<calibrate_plane, zhang_camera_node, euclidean_transformation> cg;

  // initialize the template plane
  cg.get_source()->readData((directory+"/Model.txt").c_str());

  // add a camera with 5 views into a graph
  int camID = cg.add_vertex();
  int source_id = cg.get_source_id();

  // create time beats.
  vcl_vector<double> t_beats(5);
  t_beats[0] = 0;
  t_beats[1] = 1;
  t_beats[2] = 2;
  t_beats[3] = 3;
  t_beats[4] = 4;

  // set beats on camera node
  cg.get_vertex(camID)->set_beat(t_beats);

  // set beats on translation
  euclidean_transformation *trans = cg.get_edge(source_id, camID);
  assert(trans);
  trans->set_beat(t_beats);

  cg.print(vcl_cout);

  // read feature point for each view
  cg.get_vertex(camID)->readData((directory+"/data1.txt").c_str() , 0);
  cg.get_vertex(camID)->readData((directory+"/data2.txt").c_str() , 1);
  cg.get_vertex(camID)->readData((directory+"/data3.txt").c_str() , 2);
  cg.get_vertex(camID)->readData((directory+"/data4.txt").c_str() , 3);
  cg.get_vertex(camID)->readData((directory+"/data5.txt").c_str() , 4);

  // do the calibration
  vcl_cout<<"\n\nlinear calibration..............\n\n ";
  zhang_linear_calibrate lc;
  lc.setCameraGraph(&cg);
  lc.calibrate();

  cg.print(vcl_cout);
}

int main(int argc, char* argv[])
{
  vcl_string directory = argc>1 ? argv[1] : ".";
  testing_graph();
  testing_linear_calibration(directory);

  return 0;
}

