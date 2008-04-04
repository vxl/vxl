#include "bvxm_create_synth_lidar_data_process.h"
//:
// \file
#include <bvxm/bvxm_voxel_world.h>
#include <bvxm/bvxm_mog_grey_processor.h>
#include <bvxm/bvxm_world_params.h>

#include <brdb/brdb_value.h>
#include <bprb/bprb_parameters.h>
#include <vcl_string.h>
#include <vcl_iostream.h>
#include <vcl_fstream.h>

#include <vgl/vgl_point_3d.h>
#include <vgl/vgl_vector_3d.h>

#include <vul/vul_file.h>

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

  if (!parameters()->add("box dim in X", "box_x", (unsigned int)50))
    vcl_cerr << "ERROR: Adding parameters in " << __FILE__ << vcl_endl;

  if (!parameters()->add("box dim in Y", "box_y", (unsigned int)50))
    vcl_cerr << "ERROR: Adding parameters in " << __FILE__ << vcl_endl;

  if (!parameters()->add("box dim in Z", "box_z", (unsigned int)50))
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

  unsigned int dimx = parameters()->value<unsigned int>("voxel_dim_x");
  unsigned int dimy = parameters()->value<unsigned int>("voxel_dim_y");
  unsigned int dimz = parameters()->value<unsigned int>("voxel_dim_z");
  vgl_vector_3d<unsigned int> voxel_dims(dimx, dimy, dimz);

  vil_image_view_base_sptr lidar_img;
  vpgl_camera_double_sptr lidar_cam;
  gen_lidar(voxel_dim_x, voxel_dim_y, voxel_dim_z, boxes, lidar_img, lidar_cam);

  brdb_value_sptr output0 = new brdb_value_t<vil_image_view_base_sptr>(lidar_img);
  output_data_[0] = output0;

  brdb_value_sptr output1 = new brdb_value_t<vpgl_camera_double_sptr>(lidar_cam);
  output_data_[1] = output1;

  return true;
}

bool gen_lidar_view(int x, int y, int z,
                    //bvxm_voxel_world_sptr world,
                    vcl_vector<vgl_box_3d<double> > boxes,
                    vil_image_view_base_sptr& lidar,
                    vpgl_camera_double_sptr& cam)
{
  
  vil_image_view<unsigned char> lv(x, y);
  lv.fill(0.0);

  // Place the heights of boxes on lidar
  for (unsigned int b=0; b<boxes.size(); b++) {
    vgl_box_3d<double> box = boxes[b];
    int z = box.max_z(); // top face of the box
    for (unsigned i=0; i<grid_size.x(); i++)
      for (unsigned j=0; j<grid_size.y(); j++){
        vgl_point_3d<double> p(i,j,z);
        if (box.contains(p)) {
          // check if there is already a higher d there
          if (lv(i,j) < z)
            lv(i,j) = z;
        }
      }
  }

  lidar = new vil_image_view<unsigned char>(lv);

  // generate the camera, which is a one to one mapping between 
  // lidar image and voxel slabs
  cam = new bvxm_lidar_camera();
  return true;
}

bool generate_test_boxes(double box_min_x, double box_min_y, double box_min_z, 
                    double box_dim_x, double box_dim_y, double box_dim_z,
                    double world_dim_x, double world_dim_y, double world_dim_z,
                    int num_box,
                    vcl_vector<vgl_box_3d<double> >& boxes)
{
  // create the big box at the bottom
  double max_x = box_min_x + box_dim_x;
  double max_y = box_min_y + box_dim_y;
  double max_z = box_min_z + box_dim_z;
  if ((max_x > world_dim_x) || (max_y > world_dim_y) || (max_z > world_dim_z)) {
    vcl_cerr << "generate_boxes() -- the box is out of world boundaries!" << vcl_endl;
    return false;
  }

  vgl_box_3d<double> box(box_min_x, box_min_y, box_min_z, max_x, max_y, max_z);
  boxes.push_back(box);
  // create the top boxes
  for (unsigned i=1; i<num_box; i++) {
    vgl_point_3d<double> centroid = box.centroid();
    // make the top box 2/3 of the size of the previous one 
    double dimx = 2*(box.max_x() - box.min_x())/3;
    double dimy = 2*(box.max_y() - box.min_y())/3;
    double dimz = 2*(box.max_z() - box.min_z())/3;
    centroid.set(centroid.x(), centroid.y(), box.max_z() + dimz/2.0);
    top_box = vgl_box_3d<double> (centroid, dimx, dimy, dimz, vgl_box_3d<double>::centre);
    // translate it a bit
    vgl_point_3d<double> top_centroid = top_box.centroid();
    top_box.set_centroid(vgl_point_3d<double>(top_centroid.x()+dimx/3., top_centroid.y()+dimx/3., top_centroid.z()));
    // check if the box in the world completely
    max_x = top_box.max_x();
    max_y = top_box.max_y();
    max_z = top_box.max_z();
    // stop if the new box is getting out of the boundaries
    if ((max_x > world_dim_x) || (max_y > world_dim_y) || (max_z > world_dim_z)) 
      break;
    boxes.push_back(top_box);
    box = top_box;
  }
  return true;
}
