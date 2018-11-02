// This is brl/bseg/boxm2/pro/processes/boxm2_export_mesh_process.cxx
#include <iostream>
#include <fstream>
#include <bprb/bprb_func_process.h>
//:
// \file
// \brief  A process for exporting a texture mapped mesh of a scene
//
// \author Vishal Jain
// \date Mar 15, 2011

#ifdef _MSC_VER
#  include <vcl_msvc_warnings.h>
#endif
#include <vul/vul_file.h>
#include <boxm2/boxm2_scene.h>
#include <boxm2/boxm2_util.h>

//vil includes
#include <vil/vil_image_view.h>
#include <vil/vil_new.h>
#include <vil/vil_math.h>

//vgl
#include <vgl/vgl_point_3d.h>
#include <bvgl/bvgl_triangle_interpolation_iterator.h>

//det and imesh includes
#include <sdet/sdet_image_mesh.h>
#include <sdet/sdet_image_mesh_params.h>
#include <imesh/imesh_fileio.h>
#include <imesh/imesh_operations.h>

namespace boxm2_export_mesh_process_globals
{
  constexpr unsigned n_inputs_ = 4;
  constexpr unsigned n_outputs_ = 1;
}

bool boxm2_export_mesh_process_cons(bprb_func_process& pro)
{
  using namespace boxm2_export_mesh_process_globals;

  //process takes 2 inputs
  int i=0;
  std::vector<std::string> input_types_(n_inputs_);
  input_types_[i++] = "vil_image_view_base_sptr";  //depth image
  input_types_[i++] = "vil_image_view_base_sptr";  //x image
  input_types_[i++] = "vil_image_view_base_sptr";  //y image
  input_types_[i++] = "vcl_string";                //output dir of saved mesh

  // process has 1 output
  std::vector<std::string>  output_types_(n_outputs_);
  output_types_[0] = "imesh_mesh_sptr";

  return pro.set_input_types(input_types_) && pro.set_output_types(output_types_);
}

bool boxm2_export_mesh_process(bprb_func_process& pro)
{
  using namespace boxm2_export_mesh_process_globals;
  if ( pro.n_inputs() < n_inputs_ ) {
    std::cout << pro.name() << ": The input number should be " << n_inputs_<< std::endl;
    return false;
  }

  //get the inputs
  unsigned argIdx = 0;
  vil_image_view_base_sptr img = pro.get_input<vil_image_view_base_sptr>(argIdx++);
  vil_image_view_base_sptr ximg = pro.get_input<vil_image_view_base_sptr>(argIdx++);
  vil_image_view_base_sptr yimg = pro.get_input<vil_image_view_base_sptr>(argIdx++);
  std::string out_dir = pro.get_input<std::string>(argIdx++);

  //create the mesh directory
  if (out_dir != "") {
    if (!vul_file::make_directory_path(out_dir.c_str())) {
      std::cout<<"Couldn't make directory path "<<out_dir<<std::endl;
      return false;
    }
  }

  //cast camera and image so they are useful
  auto* z_img = (vil_image_view<float>*) img.ptr();
  auto* x_img = (vil_image_view<float>*) ximg.ptr();
  auto* y_img = (vil_image_view<float>*) yimg.ptr();

  //determine size of depth image
  unsigned ni = z_img->ni();
  unsigned nj = z_img->nj();

  //create new resource sptr
  vil_image_resource_sptr z_img_res = vil_new_image_resource_of_view(*z_img);

  //initialize some sdet_image_mesh parameters
  sdet_image_mesh_params imp;
  // sigma of the Gaussian for smoothing the image prior to edge detection
  imp.smooth_ = 1.0f;
  // the edge detection threshold
  imp.thresh_ = 1.68f;
  // the shortest edgel chain that is considered for line fitting
  imp.min_fit_length_ = 5;

  // the threshold on rms pixel distance of edgels to the line
  imp.rms_distance_ = 0.11;
  // the width in pixels of the transition of a step edge
  imp.step_half_width_ = 0.0;

  // the mesh processor
  sdet_image_mesh im(imp);
  im.set_image(z_img_res);
  if (!im.compute_mesh()) {
    std::cout<<"mesh could not be computed"<<std::endl;
    return false;
  }
  imesh_mesh& mesh = im.get_mesh();
  std::cout << "Number of vertices " << mesh.num_verts()
           << "  number of faces "<< mesh.num_faces()<< '\n';

  ////////////////////////////////////////////////////////////////////////////////
  // Take mesh and add points to triangles of high depth variance
  // I.E. if a triangle in mesh has corners both very close and very far from top
  // camera, take it's centroid point and add x,y,z point, creating 3 new faces
  ////////////////////////////////////////////////////////////////////////////////
  mesh.compute_vertex_normals_from_faces();

  ////////////////////////////////////////////////////////////////////////////////
  //// normalize mesh world points to fit in the image_bb from above
  ////////////////////////////////////////////////////////////////////////////////
  // get min and max z values
  float minz=0, maxz=0;
  vil_math_value_range(*z_img, minz, maxz);
  std::cout<<"Min z: "<<minz<<" Max z: "<<maxz<<std::endl;

  //grab vertices in the mesh - convert them to scene coordinates (not image)
  imesh_vertex_array<3>& verts = mesh.vertices<3>();
  unsigned nverts = mesh.num_verts();
  for (unsigned iv = 0; iv<nverts; ++iv)
  {
    //get coordinates so you can index into the height map
    auto i = static_cast<unsigned>(verts[iv][0]);
    auto j = static_cast<unsigned>(verts[iv][1]);
    if (i<ni && j<nj)
    {
      verts[iv][0] = (*x_img)(i,j);
      verts[iv][1] = (*y_img)(i,j);
      verts[iv][2] = (*z_img)(i,j);
    }
  }

  ////////////////////////////////////////////////////////////////////////////////
  //// cut tall triangles into quarters (find midpoints of each
  ////////////////////////////////////////////////////////////////////////////////
//bool did_divide = true;
  //while (did_divide)
  {
    std::set<unsigned int> sel_faces;
    double maxLen = (maxz-minz) / 3.0;
    auto& faces = (imesh_regular_face_array<3>&) mesh.faces();
    imesh_vertex_array<3>&       verts = mesh.vertices<3>();

    unsigned nfaces = mesh.num_faces();
    for (unsigned iface = 0; iface<nfaces; ++iface)
    {
      //store three 3d points
      std::vector<vgl_point_3d<double> > points;
      for (int i=0; i<3; ++i) {
        unsigned vertexId = faces[iface][i];
        double x = verts[vertexId][0];
        double y = verts[vertexId][1];
        double z = verts[vertexId][2];
        points.emplace_back(x,y,z );
      }

      // if any one side of the triangle is too long, cut it
      double lenA = (points[0]-points[1]).sqr_length(); // vgl_vector length == distance
      double lenB = (points[1]-points[2]).sqr_length();
      double lenC = (points[0]-points[2]).sqr_length();
      maxLen *= maxLen; // sqr
      if (lenA > maxLen || lenB > maxLen || lenC > maxLen) {
        sel_faces.insert(iface);
      }
    }

    //if the divide face set is not empty, divide the faces
    if ( !sel_faces.empty() ) {
//    did_divide = true;

      std::cout<<"Subdividing mesh"<<std::endl;
      imesh_quad_subdivide(mesh, sel_faces);

      std::cout<<"Re triangulating mesh"<<std::endl;
      imesh_triangulate(mesh);
    }
//  else {
//    did_divide = false;
//  }
  }

  imesh_mesh_sptr mesh_sptr = new imesh_mesh(mesh);

  ////////////////////////////////////////////////////////////////////////////////
  //// Write out in VRML format
  ////////////////////////////////////////////////////////////////////////////////
  std::string vrfile = out_dir + "/untextured.wrl";
  std::ofstream os(vrfile.c_str());
  imesh_write_vrml(os, mesh);
  os.close();

  // store scene smart pointer
  argIdx = 0;
  pro.set_output_val<imesh_mesh_sptr>(argIdx++, mesh_sptr);
  return true;
}
