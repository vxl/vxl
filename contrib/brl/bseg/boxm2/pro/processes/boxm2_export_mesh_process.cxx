// This is brl/bseg/boxm2/pro/processes/boxm2_export_mesh_process.cxx
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
#include <boxm2/boxm2_util.h>

//vil includes
#include <vil/vil_image_view.h>
#include <vil/vil_save.h>
#include <vil/vil_new.h>
#include <vil/vil_math.h>

//vgl
#include <vgl/vgl_ray_3d.h>
#include <vgl/vgl_box_2d.h>
#include <vgl/vgl_box_3d.h>

//vpgl camera stuff
#include <vpgl/vpgl_perspective_camera.h>
#include <vpgl/algo/vpgl_project.h>

//det and imesh includes
#include <sdet/sdet_image_mesh.h>
#include <sdet/sdet_image_mesh_params.h>
#include <imesh/imesh_fileio.h>

namespace boxm2_export_mesh_process_globals
{
  const unsigned n_inputs_ = 4;
  const unsigned n_outputs_ = 1;
}

bool boxm2_export_mesh_process_cons(bprb_func_process& pro)
{
  using namespace boxm2_export_mesh_process_globals;

  //process takes 2 inputs
  int i=0; 
  vcl_vector<vcl_string> input_types_(n_inputs_);
  input_types_[i++] = "vil_image_view_base_sptr";  //depth image
  input_types_[i++] = "vil_image_view_base_sptr";  //x image
  input_types_[i++] = "vil_image_view_base_sptr";  //y image
  input_types_[i++] = "vcl_string";                //output dir of saved mesh

  // process has 1 output
  vcl_vector<vcl_string>  output_types_(n_outputs_);
  output_types_[0] = "imesh_mesh_sptr";

  return pro.set_input_types(input_types_) && pro.set_output_types(output_types_);
}

bool boxm2_export_mesh_process(bprb_func_process& pro)
{
  using namespace boxm2_export_mesh_process_globals;
  if ( pro.n_inputs() < n_inputs_ ){
    vcl_cout << pro.name() << ": The input number should be " << n_inputs_<< vcl_endl;
    return false;
  }
  
  //get the inputs
  unsigned argIdx = 0;
  vil_image_view_base_sptr img = pro.get_input<vil_image_view_base_sptr>(argIdx++);
  vil_image_view_base_sptr ximg = pro.get_input<vil_image_view_base_sptr>(argIdx++);
  vil_image_view_base_sptr yimg = pro.get_input<vil_image_view_base_sptr>(argIdx++);
  vcl_string out_dir           = pro.get_input<vcl_string>(argIdx++);
  
  //create the mesh directory
  if (!vul_file::make_directory_path(out_dir.c_str())) {
    vcl_cout<<"Couldn't make directory path "<<out_dir<<vcl_endl;
    return false;
  }

  //cast camera and image so they are useful
  vil_image_view<float>* z_img = (vil_image_view<float>*) img.ptr(); 
  vil_image_view<float>* x_img = (vil_image_view<float>*) ximg.ptr(); 
  vil_image_view<float>* y_img = (vil_image_view<float>*) yimg.ptr(); 

  //determine size of depth image
  unsigned ni = z_img->ni();
  unsigned nj = z_img->nj();
  
  //create new resource sptr
  vil_image_resource_sptr z_img_res = vil_new_image_resource_of_view(*z_img);

  //intiialize some sdet_image_mesh parameters
  sdet_image_mesh_params imp;
  // sigma of the Gaussian for smoothing the image prior to edge detection
  imp.smooth_ = 0.5f;
  // the edge detection threshold
  imp.thresh_ = 2.0f;
  // the shortest edgel chain that is considered for line fitting
  imp.min_fit_length_ = 7;
  // the threshold on rms pixel distance of edgels to the line
  imp.rms_distance_ = 0.15;
  // the width in pixels of the transition of a step edge
  imp.step_half_width_ = 1.0;
  
  // the mesh processor
  sdet_image_mesh im(imp);
  im.set_image(z_img_res);
  if(!im.compute_mesh()) {
    vcl_cout<<"mesh could not be computed"<<vcl_endl;
    return 0;
  }
  imesh_mesh& mesh = im.get_mesh();
  vcl_cout << "Number of vertices " << mesh.num_verts()
           << "  number of faces "<< mesh.num_faces()<< '\n';

  ////////////////////////////////////////////////////////////////////////////////
  //// normalize mesh world points to fit in the image_bb from above
  ////////////////////////////////////////////////////////////////////////////////
  
  // get min and max z values
  float minz=0, maxz=0;
  vil_math_value_range(*z_img, minz, maxz);
  vcl_cout<<"Min z: "<<minz<<" Max z: "<<maxz<<vcl_endl;
  
  //grab vertices in the mesh - convert them to scene coordinates (not image)
  imesh_vertex_array<3>& verts = mesh.vertices<3>(); 
  unsigned nverts = mesh.num_verts();
  for(unsigned iv = 0; iv<nverts; ++iv)
  {
    //get coordinates so you can index into the height map
    unsigned i = static_cast<unsigned>(verts[iv][0]);
    unsigned j = static_cast<unsigned>(verts[iv][1]);
    double height = minz;
    if(i<ni && j<nj) 
    {
        verts[iv][0] = (*x_img)(i,j); 
        verts[iv][1] = (*y_img)(i,j); 
        verts[iv][2] = (*z_img)(i,j); 
    }
    //for each vertex, convert (X,Y) from [0, ni], [0, nj] (image coordinates)
    //to [-x,x], [-y,y] scene coordinates 
    //double scene_x = (scene_xy.width() * verts[iv][0] / (double) ni) + scene_xy.min_x(); 
    //double scene_y = (scene_xy.height()* verts[iv][1] / (double) nj) + scene_xy.min_y(); 
    //double scene_z = height; 
    
  }
  
  ////////////////////////////////////////////////////////////////////////////////
  //// Write out in VRML format
  ////////////////////////////////////////////////////////////////////////////////
  vcl_string vrfile = out_dir + "/vrmesh.wrl";
  vcl_ofstream os(vrfile.c_str());
  imesh_write_vrml(os, mesh);
  os.close();
  
  // store scene smart pointer
  argIdx = 0; 
  imesh_mesh_sptr mesh_sptr = new imesh_mesh(mesh); 
  pro.set_output_val<imesh_mesh_sptr>(argIdx++, mesh_sptr);
  
  return true;
}
