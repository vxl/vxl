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
  vcl_string image_path = "c:/images/MeshStudies/depth-grey-cropped.tif";
  vil_image_resource_sptr imgr = vil_load_image_resource(image_path.c_str());
  if (!imgr)
    return;
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
  im.set_image(imgr);
  if (!im.compute_mesh())
    return;
  imesh_mesh& mesh = im.get_mesh();
  vcl_cout << "Number of vertices " << mesh.num_verts()
           << "  number of faces "<< mesh.num_faces()<< '\n';
  vcl_string vrfile = "c:/images/MeshStudies/vrmesh.wrl";
  vcl_ofstream os(vrfile.c_str());
  imesh_write_vrml(os, mesh);
  os.close();
}

TESTMAIN(test_image_mesh);
