// This is brl/bmvl/bcal/tests/bcal_test.cxx
#include <bmvl/bcal/bcal_zhang_camera_node.h>
#include <bmvl/bcal/bcal_zhang_linear_calibrate.h>
#include <bmvl/bcal/bcal_calibrate_plane.h>
#include <bmvl/bcal/bcal_camera_graph.h>
#include <bmvl/bcal/bcal_euclidean_transformation.h>
#include <vgl/vgl_homg_point_2d.h>
#include <vcl_string.h>
#include <vcl_cassert.h>
#include <vcl_fstream.h>
#include <testlib/testlib_test.h>

static void testing_graph()
{
  vcl_cout<<"\n--------------testing graph -------------\n";
  bcal_camera_graph<bcal_calibrate_plane, bcal_zhang_camera_node, bcal_euclidean_transformation> cg;

  // add three vertex from source point.
  int source_id = cg.get_source_id();
  cg.add_vertex(source_id);
  cg.add_vertex(source_id);
  cg.add_vertex(source_id);

  cg.print(vcl_cout);
}

static void testing_linear_calibration(vcl_string const& directory)
{
  vcl_cout<<"\n--------------testing calibration -------------\n";

  bcal_camera_graph<bcal_calibrate_plane, bcal_zhang_camera_node, bcal_euclidean_transformation> cg;

  // initialize the template plane
  cg.get_source()->readData((directory+"/data/Model.txt").c_str());

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
  bcal_euclidean_transformation *trans = cg.get_edge(source_id, camID);
  assert(trans);
  trans->set_beat(t_beats);

  cg.print(vcl_cout);

  // read feature point for each view
  cg.get_vertex(camID)->readData((directory+"/data/data1.txt").c_str() , 0);
  cg.get_vertex(camID)->readData((directory+"/data/data2.txt").c_str() , 1);
  cg.get_vertex(camID)->readData((directory+"/data/data3.txt").c_str() , 2);
  cg.get_vertex(camID)->readData((directory+"/data/data4.txt").c_str() , 3);
  cg.get_vertex(camID)->readData((directory+"/data/data5.txt").c_str() , 4);

  // do the calibration
  vcl_cout<<"\n\nlinear calibration..............\n\n ";
  bcal_zhang_linear_calibrate lc;
  lc.setCameraGraph(&cg);
  lc.calibrate();

  cg.print(vcl_cout);
}

static void testing_brown_stereo_grid_camera(vcl_string const& fname)
{
  vcl_cout<<"\n--------------testing brown camera -------------\n";


  //
  // following block is used for reading the model data
  //
  vcl_ifstream  in(fname.c_str());

  if (!in){
    vcl_cerr<<"cannot open the file: "<<fname << vcl_endl;
  }

  int num_points = 0;
  if (in.eof()){
    vcl_cerr<<"wrong file! at least number of points in the calibration board are needed\n";
  }

  in >> num_points;
  vcl_cout << "num_points = " << num_points << '\n';
  vcl_vector<vgl_homg_point_2d<double> > pts(num_points);

  for (int i=0; i<num_points; i++) {
    double u, v; in >> u >> v;
    pts[i] = vgl_homg_point_2d<double>(u, v);
  }

  //
  // set up camera graph and calibrate it
  //
  //

  bcal_camera_graph<bcal_calibrate_plane, bcal_zhang_camera_node, bcal_euclidean_transformation> cg;

  // initialize the template plane
  cg.get_source()->read_data(pts);
  int camID = cg.add_vertex();
  int source_id = cg.get_source_id();

  // add a camera with 7 views into a graph
  int nviews = 0;
  in >> nviews;
  vcl_cout << "nviews = " << nviews << '\n';

  vcl_vector<double> t_beats(nviews);
  for (int i=0; i<nviews; i++)
    t_beats[i] = i;

  // set beats on camera node
  cg.get_vertex(camID)->set_beat(t_beats);

  // set beats on translation
  bcal_euclidean_transformation *trans = cg.get_edge(source_id, camID);
  assert(trans);
  trans->set_beat(t_beats);

  cg.print(vcl_cout);


  //
  // reading the feature point for each view
  //

  for (int i=0; i<nviews; i++){
    vcl_vector<vgl_homg_point_2d<double> > features(num_points);
    for (int j = 0; j<num_points; j++) {
      double u, v; in>>u>>v;
      features[j] = vgl_homg_point_2d<double>(u, v);
    }
    cg.get_vertex(camID)->read_data(features , i);
  }

  // do the calibration
  vcl_cout<<"\n\nlinear calibration..............\n\n";
  bcal_zhang_linear_calibrate lc;
  lc.setCameraGraph(&cg);
  lc.calibrate();

  cg.print(vcl_cout);
}

static void bcal_test(int argc, char* argv[])
{
  vcl_string directory = argc>1 ? argv[1] : ".";
  testing_graph();

  testing_linear_calibration(directory);

  testing_brown_stereo_grid_camera(directory+"/data/point_correspondences.left");
  testing_brown_stereo_grid_camera(directory+"/data/point_correspondences.right");
}

TESTLIB_DEFINE_MAIN_ARGS(bcal_test);
