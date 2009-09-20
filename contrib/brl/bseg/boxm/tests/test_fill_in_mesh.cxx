#include <testlib/testlib_test.h>
#include "test_utils.h"

#include <boxm/boxm_utils.h>
#include <boxm/boxm_sample.h>
#include <boxm/boxm_scene.h>
#include <boxm/boxm_fill_in_mesh.h>
#include <boct/boct_tree_cell.h>
#include <boxm/boxm_save_block_raw.h>
#include <vgl/vgl_point_3d.h>
#include <vgl/vgl_vector_3d.h>
#include <vpl/vpl.h>
#include <vul/vul_file.h>
#include <vpgl/vpgl_camera.h>
#include <vpgl/vpgl_perspective_camera.h>
#include <vpgl/vpgl_calibration_matrix.h>
#include <imesh/imesh_mesh.h>
#include <imesh/imesh_fileio.h>
#include <vcl_limits.h>

MAIN( test_fill_in_mesh )
{
  START ("TEST FILL IN MESH");
  bgeo_lvcs lvcs(33.33,44.44,10.0, bgeo_lvcs::wgs84, bgeo_lvcs::DEG, bgeo_lvcs::METERS);
  vgl_point_3d<double> origin(0,0,0);
  vgl_vector_3d<double> block_dim(10,10,10);
  vgl_vector_3d<unsigned> world_dim(1,1,1);
  boxm_scene<boct_tree<short,boxm_sample<BOXM_APM_MOG_GREY>  > > scene(lvcs, origin, block_dim, world_dim);
  scene.set_appearence_model(BOXM_APM_MOG_GREY);
  scene.set_paths("./boxm_fill_in", "block");
  vul_file::make_directory("./boxm_fill_in");
  vcl_ofstream os("scene.xml");
  x_write(os, scene, "scene");
  os.close();

  //// create a mesh
  unsigned int num_faces=6;
  vcl_auto_ptr<imesh_vertex_array<3> > verts(new imesh_vertex_array<3>(8));
  vcl_auto_ptr<imesh_face_array > faces(new imesh_face_array(num_faces));
  imesh_vertex<3>& vert0 = (*verts)[0]; imesh_vertex<3>& vert4 = (*verts)[4];
  vert0[0]=2;                           vert4[0]=2;
  vert0[1]=2;                           vert4[1]=2;
  vert0[2]=4;                           vert4[2]=8;
  imesh_vertex<3>& vert1 = (*verts)[1]; imesh_vertex<3>& vert5 = (*verts)[5];
  vert1[0]=4;                           vert5[0]=4;
  vert1[1]=2;                           vert5[1]=2;
  vert1[2]=4;                           vert5[2]=8;
  imesh_vertex<3>& vert2 = (*verts)[2]; imesh_vertex<3>& vert6 = (*verts)[6];
  vert2[0]=4;                           vert6[0]=4;
  vert2[1]=4;                           vert6[1]=4;
  vert2[2]=4;                           vert6[2]=8;

  imesh_vertex<3>& vert3 = (*verts)[3]; imesh_vertex<3>& vert7 = (*verts)[7];
  vert3[0]=2;                           vert7[0]=2;
  vert3[1]=4;                           vert7[1]=4;
  vert3[2]=4;                           vert7[2]=8;

  vcl_vector<unsigned int> face(4,0);
  face[0]=4;face[1]=5;face[2]=6;face[3]=7;
  (*faces)[0]=face;
  face[0]=3;face[1]=2;face[2]=1;face[3]=0;
  (*faces)[1]=face;
  face[0]=0;face[1]=4;face[2]=7;face[3]=3;
  (*faces)[2]=face;
  face[0]=1;face[1]=2;face[2]=6;face[3]=5;
  (*faces)[3]=face;
  face[0]=1;face[1]=5;face[2]=4;face[3]=0;
  (*faces)[4]=face;
  face[0]=3;face[1]=7;face[2]=6;face[3]=2;
  (*faces)[5]=face;


  imesh_mesh mesh;
  mesh.set_vertices(vcl_auto_ptr<imesh_vertex_array_base>(verts));
  mesh.set_faces(vcl_auto_ptr<imesh_face_array_base>(faces));

  typedef boct_tree<short,boxm_sample<BOXM_APM_MOG_GREY> > tree_type;
  boxm_block_iterator<boct_tree<short,boxm_sample<BOXM_APM_MOG_GREY> > > iter(&scene);
  while (!iter.end())
  {
    scene.load_block(iter.index().x(),iter.index().y(),iter.index().z());
    boxm_block<boct_tree<short,boxm_sample<BOXM_APM_MOG_GREY> > > * block=scene.get_active_block();
    boct_tree<short,boxm_sample<BOXM_APM_MOG_GREY> > * tree=new boct_tree<short,boxm_sample<BOXM_APM_MOG_GREY> >(7,6);
    boxm_sample<BOXM_APM_MOG_GREY> s;
    s.alpha=vcl_numeric_limits<float>::max();
    tree->init_cells(s);
    block->init_tree(tree);
    scene.write_active_block();
    iter++;
  }

  boxm_sample<BOXM_APM_MOG_GREY> val;
  val.alpha=0;
  vcl_vector<imesh_mesh> meshes; meshes.push_back(mesh);
  boxm_fill_in_mesh_into_scene<short, boxm_sample<BOXM_APM_MOG_GREY> >(scene, meshes, false, val);

  vcl_vector<vcl_string> fnames;
  boxm_block_iterator<tree_type> it(&scene);

  // check the first block
  scene.load_block(it.index().x(),it.index().y(),it.index().z());
  boxm_block<boct_tree<short,boxm_sample<BOXM_APM_MOG_GREY> > > * block=scene.get_active_block();
  boct_tree<short,boxm_sample<BOXM_APM_MOG_GREY> > * tree=block->get_tree();


  boct_tree_cell<short,boxm_sample<BOXM_APM_MOG_GREY> >* cell0 = tree->locate_point_global(vgl_point_3d<double>(3,3,6));
  TEST("Centroid of the mesh is in the octree", cell0->data().alpha, 0.0f);


  vpl_rmdir("./boxm_fill_in");
  vpl_unlink("./scene.xml");
  SUMMARY();
}
