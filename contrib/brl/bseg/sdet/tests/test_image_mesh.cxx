// This is brl/bseg/sdet/tests/test_image_mesh.cxx
#include <vcl_vector.h>
#include <vcl_string.h>
#include <vcl_iostream.h>
#include <vcl_fstream.h>
#include <vil/vil_image_resource.h>
#include <vil/vil_load.h>
#include <sdet/sdet_image_mesh.h>
#include <sdet/sdet_image_mesh_params.h>
#include <imesh/imesh_fileio.h>
#include <testlib/testlib_test.h>

static void test_image_mesh()
{
  //  vcl_string image_path = "C:/images/MeshStudies/depth-grey-cropped.tif";
  //  vcl_string image_path = "C:/images/MeshStudies/depth-grey-smooth.tif";
  vcl_string image_path = "C:/images/MeshStudies/mrf_med_grey.tif";

  vcl_cout<<"TESTING image mesh"<<vcl_endl;

  vil_image_resource_sptr imgr = vil_load_image_resource(image_path.c_str());
  if (!imgr) {
    vcl_cout<<"Cannot load "<<image_path<<vcl_endl;
    return;
  }

  //initialize some sdet_image_mesh parameters
  sdet_image_mesh_params imp;
  // sigma of the Gaussian for smoothing the image prior to edge detection
  imp.smooth_ = 1.0f;
  // the edge detection threshold
  imp.thresh_ = 0.8f;
  // the shortest edgel chain that is considered for line fitting
  imp.min_fit_length_ = 7;
  // the threshold on rms pixel distance of edgels to the line
  imp.rms_distance_ = 0.1;
  // the width in pixels of the transition of a step edge
  imp.step_half_width_ = 4.0;

  // the mesh processor
  sdet_image_mesh im(imp);
  im.set_image(imgr);
  if (!im.compute_mesh()) {
    vcl_cout<<"mesh could not be computed"<<vcl_endl;
    return;
  }
  imesh_mesh& mesh = im.get_mesh();
  vcl_cout << "Number of vertices " << mesh.num_verts();
  vcl_string vrfile = "C:/images/MeshStudies/vmidrmesh.wrl";
  vcl_ofstream os(vrfile.c_str());
  imesh_write_vrml(os, mesh);
  os.close();
}

TESTMAIN(test_image_mesh);
