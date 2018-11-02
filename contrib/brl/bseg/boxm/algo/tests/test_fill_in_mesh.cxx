#include <iostream>
#include <limits>
#include <testlib/testlib_test.h>

#include <boxm/sample/boxm_sample.h>
#include <boxm/boxm_scene.h>
#include <boxm/algo/boxm_fill_in_mesh.h>
#include <boxm/algo/boxm_save_block_raw.h>
#include <boct/boct_tree_cell.h>

#include <vgl/vgl_point_3d.h>
#include <vgl/vgl_vector_3d.h>
#include <vpl/vpl.h>
#include <vul/vul_file.h>
#include <vpgl/vpgl_lvcs.h>
#include <imesh/imesh_mesh.h>
#include <imesh/imesh_fileio.h>
#ifdef _MSC_VER
#  include <vcl_msvc_warnings.h>
#endif

static void test_fill_in_mesh()
{
  vpgl_lvcs lvcs(33.33,44.44,10.0, vpgl_lvcs::wgs84, vpgl_lvcs::DEG, vpgl_lvcs::METERS);
  vgl_point_3d<double> origin(0,0,0);
  vgl_vector_3d<double> block_dim(10,10,10);
  vgl_vector_3d<unsigned> world_dim(1,1,1);
  boxm_scene<boct_tree<short,boxm_sample<BOXM_APM_MOG_GREY>  > > scene(lvcs, origin, block_dim, world_dim);
  scene.set_appearance_model(BOXM_APM_MOG_GREY);
  scene.set_paths("./boxm_fill_in", "block");
  vul_file::make_directory("./boxm_fill_in");
  std::ofstream os("scene.xml");
  x_write(os, scene, "scene");
  os.close();

  //// create a mesh
  unsigned int num_faces=6;
  std::unique_ptr<imesh_vertex_array<3> > verts(new imesh_vertex_array<3>(8));
  std::unique_ptr<imesh_face_array > faces(new imesh_face_array(num_faces));
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

  std::vector<unsigned int> face(4,0);
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
#if __cplusplus >= 201103L || (defined(_CPPLIB_VER) && _CPPLIB_VER >= 520)
  std::unique_ptr<imesh_vertex_array_base> tempv(std::move(verts));
  mesh.set_vertices(std::move(tempv));
  std::unique_ptr<imesh_face_array_base> tempf(std::move(faces));
  mesh.set_faces(std::move(tempf));
#else
  mesh.set_vertices(std::unique_ptr<imesh_vertex_array_base>(verts));
  mesh.set_faces(std::unique_ptr<imesh_face_array_base>(faces));
#endif

  typedef boct_tree<short,boxm_sample<BOXM_APM_MOG_GREY> > tree_type;
  boxm_block_iterator<boct_tree<short,boxm_sample<BOXM_APM_MOG_GREY> > > iter(&scene);
  while (!iter.end())
  {
    scene.load_block(iter.index().x(),iter.index().y(),iter.index().z());
    boxm_block<boct_tree<short,boxm_sample<BOXM_APM_MOG_GREY> > > * block=scene.get_active_block();
    auto * tree=new boct_tree<short,boxm_sample<BOXM_APM_MOG_GREY> >(7,6);
    boxm_sample<BOXM_APM_MOG_GREY> s;
    s.alpha=std::numeric_limits<float>::max();
    tree->init_cells(s);
    block->init_tree(tree);
    scene.write_active_block();
    iter++;
  }

  boxm_sample<BOXM_APM_MOG_GREY> val;
  val.alpha=0;
  std::vector<imesh_mesh> meshes; meshes.push_back(mesh);
  boxm_fill_in_mesh_into_scene<short, boxm_sample<BOXM_APM_MOG_GREY> >(scene, meshes, false, val);

  std::vector<std::string> fnames;
  boxm_block_iterator<tree_type> it(&scene);

  // check the first block
  scene.load_block(it.index().x(),it.index().y(),it.index().z());
  boxm_block<boct_tree<short,boxm_sample<BOXM_APM_MOG_GREY> > > * block=scene.get_active_block();
  boct_tree<short,boxm_sample<BOXM_APM_MOG_GREY> > * tree=block->get_tree();


  boct_tree_cell<short,boxm_sample<BOXM_APM_MOG_GREY> >* cell0 = tree->locate_point_global(vgl_point_3d<double>(3,3,6));
  TEST("Centroid of the mesh is in the octree", cell0->data().alpha, 0.0f);


  vpl_rmdir("boxm_fill_in");
  vpl_unlink("scene.xml");
}

TESTMAIN(test_fill_in_mesh);
