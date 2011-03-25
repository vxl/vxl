// This is brl/bseg/boxm2/pro/processes/boxm2_export_textured_mesh_process.cxx
#include <bprb/bprb_func_process.h>
//:
// \file
// \brief  A process for exporting a texture mapped mesh of a scene
//
// \author Vishal Jain
// \date Mar 15, 2011

#include <vcl_fstream.h>
#include <vul/vul_file.h>
#include <boxm2/boxm2_scene.h>

//vil includes
#include <vil/vil_image_view.h>

//vgl
#include <vgl/vgl_ray_3d.h>
#include <vgl/vgl_point_2d.h>
#include <vgl/vgl_box_2d.h>

//vpgl camera stuff
#include <vpgl/vpgl_perspective_camera.h>
#include <vpgl/algo/vpgl_project.h>

namespace boxm2_export_textured_mesh_process_globals
{
  const unsigned n_inputs_ = 5;
  const unsigned n_outputs_ = 0;
}

bool boxm2_export_textured_mesh_process_cons(bprb_func_process& pro)
{
  using namespace boxm2_export_textured_mesh_process_globals;

  //process takes 2 inputs
  vcl_vector<vcl_string> input_types_(n_inputs_);
  input_types_[0] = "vil_image_view_base_sptr";  //depth image
  input_types_[1] = "vpgl_camera_double_sptr";   //depth image cam
  input_types_[2] = "vcl_string";                //directory of scene images
  input_types_[3] = "vcl_string";                //directory of corresponding cams
  input_types_[4] = "vcl_string";                //output dir of saved mesh

  // process has 1 output
  vcl_vector<vcl_string>  output_types_(n_outputs_);
  //output_types_[0] = "boxm2_scene_sptr";

  return pro.set_input_types(input_types_) && pro.set_output_types(output_types_);
}

bool boxm2_export_textured_mesh_process(bprb_func_process& pro)
{
  using namespace boxm2_export_textured_mesh_process_globals;

  if ( pro.n_inputs() < n_inputs_ ) {
    vcl_cout << pro.name() << ": The input number should be " << n_inputs_<< vcl_endl;
    return false;
  }
  //get the inputs
  unsigned argIdx = 0;
  vil_image_view_base_sptr img = pro.get_input<vil_image_view_base_sptr>(argIdx++);
  vpgl_camera_double_sptr  cam = pro.get_input<vpgl_camera_double_sptr>(argIdx++);
  vcl_string img_dir           = pro.get_input<vcl_string>(argIdx++);
  vcl_string cam_dir           = pro.get_input<vcl_string>(argIdx++);
  vcl_string out_dir           = pro.get_input<vcl_string>(argIdx++);

  //create the mesh directory
  if (!vul_file::make_directory_path(out_dir.c_str())) {
    vcl_cout<<"Couldn't make directory path "<<out_dir<<vcl_endl;
    return false;
  }

  //cast camera and image so they are useful
  vpgl_perspective_camera<double>* pcam = (vpgl_perspective_camera<double>*) cam.ptr();
  vil_image_view<float>* depth = (vil_image_view<float>*) img.ptr();

  //determine size of depth image
  unsigned ni = img->ni();
  unsigned nj = img->nj();

  //calculate XYZ image image that corresponds to depth image
  vil_image_view<vil_rgba<float> >* xyz_img = new vil_image_view<vil_rgba<float> >(ni, nj);
  for (int i=0; i<ni; ++i) {
    for (int j=0; j<nj; ++j) {
      //grab the ray from the camera
      vgl_ray_3d<double> ray = pcam->backproject_ray(i,j);

      //ray o
      float ray_ox=ray.origin().x();
      float ray_oy=ray.origin().y();
      float ray_oz=ray.origin().z();

      //ray dir
      float dray_x = float(ray.direction().x()),
            dray_y = float(ray.direction().y()),
            dray_z = float(ray.direction().z());

      //expected ray depth
      float ray_t = (*depth)(i,j);

      //calculate x,y,z position at expected depth
      (*xyz_img)(i,j) = vil_rgba<float>(ray_ox + ray_t*dray_x,
                                        ray_oy + ray_t*dray_y,
                                        ray_oz + ray_t*dray_z,
                                        0.0f);
    }
  }

  //

  //////////////////////////////////////////////////////////////////////////////
  //Get XY bounding box for image
  //////////////////////////////////////////////////////////////////////////////
  vgl_box_2d<double> image_bb = vpgl_project::project_bounding_box(*pcam, scene->bounding_box());
  vgl_point_2d<double> xy_min(scene->bounding_box().min_x(), scene->bounding_box().min_y());
  vgl_point_2d<double> xy_max(scene->bounding_box().max_x(), scene->bounding_box().max_y());
  vgl_box_2d<double> scene_xy(xy_min, xy_max);
  vcl_cout<<"Image bounding box (scene xy box): "<<image_bb<<'\n'
          <<"Scene 2d bounding box (xy) : "<<scene_xy<<vcl_endl;

#if 0
  //////////////////////////////////////////////////////////////////////////////
  // Generate 3d mesh from depth image
  //////////////////////////////////////////////////////////////////////////////
  vcl_cout<<"Generating image mesh"<<vcl_endl;

  //create new resource sptr
  vil_image_resource_sptr depth_resc = vil_new_image_resource_of_view(*depth_im);

  //initialize some sdet_image_mesh parameters
  sdet_image_mesh_params imp;
  // sigma of the Gaussian for smoothing the image prior to edge detection
  imp.smooth_ = 2.0f;
  // the edge detection threshold
  imp.thresh_ = 2.0f;
  // the shortest edgel chain that is considered for line fitting
  imp.min_fit_length_ = 7;
  // the threshold on rms pixel distance of edgels to the line
  imp.rms_distance_ = 0.1;
  // the width in pixels of the transition of a step edge
  imp.step_half_width_ = 5.0;

  // the mesh processor
  sdet_image_mesh im(imp);
  im.set_image(depth_resc);
  if (!im.compute_mesh()) {
    vcl_cout<<"mesh could not be computed"<<vcl_endl;
    return 0;
  }
  imesh_mesh& mesh = im.get_mesh();
  vcl_cout << "Number of vertices " << mesh.num_verts()
           << "  number of faces "<< mesh.num_faces()<< '\n';

  //////////////////////////////////////////////////////////////////////////////
  // normalize mesh world points to fit in the image_bb from above
  //////////////////////////////////////////////////////////////////////////////


  //////////////////////////////////////////////////////////////////////////////
  // Generate top down image from same camera as above
  //////////////////////////////////////////////////////////////////////////////


  //////////////////////////////////////////////////////////////////////////////
  // for each triangle,
  //   if surface normal is less than X degrees off horizontal
  //      project triangle onto image, store triangle points (see if there are functions for this in imesh)
  //////////////////////////////////////////////////////////////////////////////


  //////////////////////////////////////////////////////////////////////////////
  // Write out in VRML format
  //////////////////////////////////////////////////////////////////////////////
  vcl_string vrfile = dir() + "/vrmesh.wrl";
  vcl_ofstream os(vrfile.c_str());
  imesh_write_vrml(os, mesh);
  os.close();
#endif // 0

  return true;
}
