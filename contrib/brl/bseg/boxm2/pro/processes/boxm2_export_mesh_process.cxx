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
  
  //helper split method
  void split_triangles(imesh_mesh& mesh, vil_image_view<float>* zimg); 
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

  //initialize some sdet_image_mesh parameters
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
  if (!im.compute_mesh()) {
    vcl_cout<<"mesh could not be computed"<<vcl_endl;
    return 0;
  }
  imesh_mesh& mesh = im.get_mesh();
  vcl_cout << "Number of vertices " << mesh.num_verts()
           << "  number of faces "<< mesh.num_faces()<< '\n';

  ////////////////////////////////////////////////////////////////////////////////
  // Take mesh and add points to triangles of high depth variance
  // I.E. if a triangle in mesh has corners both very close and very far from top
  // camera, take it's centroid point and add x,y,z point, creating 3 new faces
  ////////////////////////////////////////////////////////////////////////////////
  mesh.compute_vertex_normals_from_faces();
  split_triangles(mesh, z_img); 

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
  for (unsigned iv = 0; iv<nverts; ++iv)
  {
    //get coordinates so you can index into the height map
    unsigned i = static_cast<unsigned>(verts[iv][0]);
    unsigned j = static_cast<unsigned>(verts[iv][1]);
    if (i<ni && j<nj)
    {
      verts[iv][0] = (*x_img)(i,j);
      verts[iv][1] = (*y_img)(i,j);
      verts[iv][2] = (*z_img)(i,j);
    }
  }

  ////////////////////////////////////////////////////////////////////////////////
  //// Write out in VRML format
  ////////////////////////////////////////////////////////////////////////////////
  vcl_string vrfile = out_dir + "/untextured.wrl";
  vcl_ofstream os(vrfile.c_str());
  imesh_write_vrml(os, mesh);
  os.close();

  // store scene smart pointer
  argIdx = 0;
  imesh_mesh_sptr mesh_sptr = new imesh_mesh(mesh);
  pro.set_output_val<imesh_mesh_sptr>(argIdx++, mesh_sptr);
  return true;
}

void boxm2_export_mesh_process_globals::split_triangles(imesh_mesh& mesh, 
                                                        vil_image_view<float>* z_img)
{
  //find the range of triangles in the Z direction
  double minZ = 10e100, maxZ = -10e100; 
  imesh_regular_face_array<3>& faces = (imesh_regular_face_array<3>&) mesh.faces();
  imesh_vertex_array<3>& verts = mesh.vertices<3>();
  unsigned nfaces = mesh.num_faces();
  for (unsigned iface = 0; iface<nfaces; ++iface)
  {
    unsigned v1 = faces[iface][0]; 
    unsigned v2 = faces[iface][1]; 
    unsigned v3 = faces[iface][2]; 
    double z1 = verts[v1][2]; 
    double z2 = verts[v2][2]; 
    double z3 = verts[v3][2]; 

    //get the min z and max z out of this bunch
    minZ = vcl_min(vcl_min(z1, vcl_min(z2, z3)), minZ);
    maxZ = vcl_max(vcl_max(z1, vcl_min(z2, z3)), maxZ); 
  }
  
  //maximum z diff allowed for a triangle is 1/512 of the total z range
  double max_z_diff = (maxZ-minZ)/512.0; 

  //number of tris that split
  int numSplit = 0;
  bool didSplit = true; 
  while( numSplit < 10000 && didSplit ) {
    
    //nothing has split yet
    didSplit = false;
    
    //grab the mesh's faces and verts
    imesh_regular_face_array<3>& faces = (imesh_regular_face_array<3>&) mesh.faces();
    imesh_vertex_array<3>& verts = mesh.vertices<3>();
    unsigned nfaces = mesh.num_faces();
    for (unsigned iface = 0; iface<nfaces; ++iface)
    {
      unsigned v1 = faces[iface][0]; 
      unsigned v2 = faces[iface][1]; 
      unsigned v3 = faces[iface][2]; 
      vgl_point_3d<double> vert1(verts[v1][0], verts[v1][1], verts[v1][2]); 
      vgl_point_3d<double> vert2(verts[v2][0], verts[v2][1], verts[v2][2]); 
      vgl_point_3d<double> vert3(verts[v3][0], verts[v3][1], verts[v3][2]); 
      
      //get the min z and max z out of this bunch
      double minZ = vcl_min(vert1.z(), vcl_min(vert2.z(), vert3.z()));
      double maxZ = vcl_max(vert1.z(), vcl_min(vert2.z(), vert3.z()));
      
      //if the difference between min and max Z is sufficient, add a point to the mix 
      // right in the middle of the triangle
      if( (maxZ-minZ > max_z_diff) ) {
      
        //split the triangle: 
        didSplit = true;
        numSplit++;
        
        //this center of the triangle gives you X,Y center, but grab Z from the z_img
        vgl_point_3d<double> center = centre<double>(vert1, vert2, vert3); 
        unsigned i = static_cast<unsigned>(center.x());
        unsigned j = static_cast<unsigned>(center.y());      
        double centerZ = (*z_img)(i,j); 
      
        //pop vertex onto the end of the list
        imesh_vertex<3> point(center.x(),center.y(),centerZ);
        verts.push_back(point);
        unsigned vCenter = verts.size()-1; 
        
        //now add the three faces that would result from this
        //v1, v2, center; v1, center, v3; v2, v3, center
        imesh_tri tri1(v1, v2, vCenter);
        imesh_tri tri2(v1, vCenter, v3);
        imesh_tri tri3(v2, v3, vCenter);
        faces[iface] = tri1;  
        faces.push_back(tri2); 
        faces.push_back(tri3); 
      } 
    }
  }
}
