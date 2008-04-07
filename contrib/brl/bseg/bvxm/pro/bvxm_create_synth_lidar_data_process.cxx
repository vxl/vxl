#include "bvxm_create_synth_lidar_data_process.h"
//:
// \file
#include <bvxm/bvxm_util.h>


#include <brdb/brdb_value.h>
#include <bprb/bprb_parameters.h>
#include <vcl_iostream.h>

#include <vgl/vgl_point_3d.h>

#include <vil/vil_image_view.h>

//: Constructor
bvxm_create_synth_lidar_data_process::bvxm_create_synth_lidar_data_process()
{
  //this process takes no input

  //output:
  // 1. lidar image
  // 2. camera related to that image
  output_data_.resize(2,brdb_value_sptr(0));
  output_types_.resize(2);
  output_types_[0]= "vil_image_view_base_sptr";
  output_types_[1]= "vpgl_camera_double_sptr";

  // parameters
  // voxel dimensions, we assume that the corner of the world is (0,0,0)
  if (!parameters()->add("Voxel Dimension in X", "voxel_dim_x", (unsigned int)100))
    vcl_cerr << "ERROR: Adding parameters in " << __FILE__ << vcl_endl;

  if (!parameters()->add("Voxel Dimension in Y", "voxel_dim_y", (unsigned int)100))
    vcl_cerr << "ERROR: Adding parameters in " << __FILE__ << vcl_endl;

  if (!parameters()->add("Voxel Dimension in Z", "voxel_dim_z", (unsigned int)100))
    vcl_cerr << "ERROR: Adding parameters in " << __FILE__ << vcl_endl;

  // information about synthetic data (only boxes)
  if (!parameters()->add("box min_x point", "box_min_x", (unsigned int)20))
    vcl_cerr << "ERROR: Adding parameters in " << __FILE__ << vcl_endl;

  if (!parameters()->add("box min_y point", "box_min_y", (unsigned int)20))
    vcl_cerr << "ERROR: Adding parameters in " << __FILE__ << vcl_endl;

  if (!parameters()->add("box min_z point", "box_min_z", (unsigned int)20))
    vcl_cerr << "ERROR: Adding parameters in " << __FILE__ << vcl_endl;

  if (!parameters()->add("box dim in X", "box_dim_x", (unsigned int)50))
    vcl_cerr << "ERROR: Adding parameters in " << __FILE__ << vcl_endl;

  if (!parameters()->add("box dim in Y", "box_dim_y", (unsigned int)50))
    vcl_cerr << "ERROR: Adding parameters in " << __FILE__ << vcl_endl;

  if (!parameters()->add("box dim in Z", "box_dim_z", (unsigned int)50))
    vcl_cerr << "ERROR: Adding parameters in " << __FILE__ << vcl_endl;

  //lvcs parameters
  if (!parameters()->add("LVCS Path", "lvcs", vcl_string("")))
    vcl_cerr << "ERROR: Adding parameters in " << __FILE__ << vcl_endl;
 
}

//: Destructor
bvxm_create_synth_lidar_data_process::~bvxm_create_synth_lidar_data_process()
{
}


//: Execute the process
bool
bvxm_create_synth_lidar_data_process::execute()
{
  // Sanity check
  if (!this->verify_inputs())
    return false;

  unsigned int v_dimx = parameters()->value<unsigned int>("voxel_dim_x");
  unsigned int v_dimy = parameters()->value<unsigned int>("voxel_dim_y");
  unsigned int v_dimz = parameters()->value<unsigned int>("voxel_dim_z");

  // box dimensions
  unsigned int dimx = parameters()->value<unsigned int>("box_dim_x");
  unsigned int dimy = parameters()->value<unsigned int>("box_dim_y");
  unsigned int dimz = parameters()->value<unsigned int>("box_dim_z");

  // box min point
  unsigned int minx = parameters()->value<unsigned int>("box_min_x");
  unsigned int miny = parameters()->value<unsigned int>("box_min_y");
  unsigned int minz = parameters()->value<unsigned int>("box_min_z");

  //lvcs parameters
  vcl_string lvcs_path;
  if (!parameters()->get_value(vcl_string("lvcs"), lvcs_path)) {
    vcl_cout << "bvxm_create_voxel_world_process::execute() -- problem in retrieving parameter lvcs_path\n";
    return false;
  }

  bgeo_lvcs_sptr lvcs = new bgeo_lvcs();
  if (lvcs_path != "") {
    vcl_ifstream is(lvcs_path.c_str());
    if (!is)
    {
      vcl_cerr << " Error opening file  " << lvcs_path << vcl_endl;
      return false;
    }
    lvcs->read(is);
  }

  // generate boxes, lidar image
  vil_image_view_base_sptr lidar_img;
  vpgl_camera_double_sptr lidar_cam;
  vcl_vector<vgl_box_3d<double> > boxes;
  bvxm_util::generate_test_boxes<double>(minx, miny, minz, dimx, dimy, dimz, v_dimx, v_dimy, v_dimz, boxes);
  gen_lidar_view(v_dimx, v_dimy, v_dimz, boxes, lidar_img, lidar_cam, lvcs);

 
  brdb_value_sptr output0 = new brdb_value_t<vil_image_view_base_sptr>(lidar_img);
  output_data_[0] = output0;

  brdb_value_sptr output1 = new brdb_value_t<vpgl_camera_double_sptr>(lidar_cam);
  output_data_[1] = output1;

  return true;
}


bool bvxm_create_synth_lidar_data_process::gen_lidar_view(int x, int y, int z,
                    vcl_vector<vgl_box_3d<double> > boxes,
                    vil_image_view_base_sptr& lidar,
                    vpgl_camera_double_sptr& cam,
                    bgeo_lvcs_sptr lvcs)
{
  vil_image_view<float> lv(x, y);
  lv.fill(0);

  // Place the heights of boxes on lidar
  for (unsigned int b=0; b<boxes.size(); b++) {
    vgl_box_3d<double> box = boxes[b];
    double z = box.max_z(); // top face of the box
    for (int i=0; i<x; i++)
      for (int j=0; j<y; j++) {
        vgl_point_3d<double> p(i,j,z);
        if (box.contains(p)) {
          // check if there is already a higher d there
          if (lv(i,j) < (float)z)
            lv(i,j) = (float)z;
        }
      }
  }

  lidar = new vil_image_view<float>(lv);

  // generate the camera, which is a one to one mapping between
  // lidar image and voxel slabs
  //bvxm_lidar_camera lidar_cam;
  //lidar_cam.set_lvcs(lvcs);
  cam = new bvxm_lidar_camera();
  return true;
}
