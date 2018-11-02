#include <iostream>
#include <limits>
#include <testlib/testlib_test.h>

#include <boxm/sample/boxm_sample.h>
#include <boxm/boxm_scene.h>
#include <boxm/algo/boxm_upload_mesh.h>
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

static void test_upload_mesh()
{
  vpgl_lvcs lvcs(33.33,44.44,10.0, vpgl_lvcs::wgs84, vpgl_lvcs::DEG, vpgl_lvcs::METERS);
  vgl_point_3d<double> origin(0,0,0);
  vgl_vector_3d<double> block_dim(10,10,10);
  vgl_vector_3d<unsigned> world_dim(2,2,1);
  boxm_scene<boct_tree<short,boxm_sample<BOXM_APM_MOG_GREY>  > > scene(lvcs, origin, block_dim, world_dim);
  scene.set_appearance_model(BOXM_APM_MOG_GREY);

#if 0
  if (vul_file_is_directory("./boxm_upload"))
  {
    vul_file_iterator fiter("./boxm_upload/*");
    while (!fiter)
    {
      vpl_unlink(fiter());
      fiter++;
    }
    vpl_rmdir("./boxm_upload");
  }
#endif // 0

  scene.set_paths("./boxm_upload", "block");
  vul_file::make_directory("./boxm_upload");
  std::ofstream os("scene.xml");
  x_write(os, scene, "scene");
  os.close();

  // create a mesh
  unsigned int num_faces=1;
  std::unique_ptr<imesh_vertex_array<3> > verts(new imesh_vertex_array<3>(3));
  std::unique_ptr<imesh_face_array > faces(new imesh_face_array(num_faces));
  imesh_vertex<3>& vert0 = (*verts)[0];
  vert0[0]=2;
  vert0[1]=2;
  vert0[2]=4;
  imesh_vertex<3>& vert1 = (*verts)[1];
  vert1[0]=8;
  vert1[1]=2;
  vert1[2]=4;
  imesh_vertex<3>& vert2 = (*verts)[2];
  vert2[0]=8;
  vert2[1]=8;
  vert2[2]=4;

  for (unsigned int f=0; f<num_faces; ++f) {
    std::vector<unsigned int>& face = (*faces)[f];
    unsigned int cnt=3;
    face.resize(cnt,0);
    for (unsigned int v=0; v<cnt; ++v)
      face[v]=v;
  }
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
    auto * tree=new boct_tree<short,boxm_sample<BOXM_APM_MOG_GREY> >(6,5);
    boxm_sample<BOXM_APM_MOG_GREY> s;
    s.alpha=std::numeric_limits<float>::max();
    tree->init_cells(s);
    block->init_tree(tree);
    scene.write_active_block();
    iter++;
  }

  boxm_sample<BOXM_APM_MOG_GREY> val;
  val.alpha=0;
  boxm_upload_mesh_into_scene<short, boxm_sample<BOXM_APM_MOG_GREY> >(scene, mesh, false, val);

  std::vector<std::string> fnames;
  boxm_block_iterator<tree_type> it(&scene);
  it.begin();
  // check the first block
  scene.load_block(it.index().x(),it.index().y(),it.index().z());
  boxm_block<boct_tree<short,boxm_sample<BOXM_APM_MOG_GREY> > > * block=scene.get_active_block();
  boct_tree<short,boxm_sample<BOXM_APM_MOG_GREY> > * tree=block->get_tree();

  boct_tree_cell<short,boxm_sample<BOXM_APM_MOG_GREY> >* cell0 = tree->locate_point_global(vert0.operator vgl_point_3d<double>());
  TEST("Vertex 0 is in the octree", cell0->data().alpha, 0.0f);

  boct_tree_cell<short,boxm_sample<BOXM_APM_MOG_GREY> >* cell1 = tree->locate_point_global(vert1.operator vgl_point_3d<double>());
  TEST("Vertex 1 is in the octree", cell1->data().alpha, 0.0f);

  boct_tree_cell<short,boxm_sample<BOXM_APM_MOG_GREY> >* cell2 = tree->locate_point_global(vert2.operator vgl_point_3d<double>());
  TEST("Vertex 2 is in the octree", cell2->data().alpha, 0.0f);

  boct_tree_cell<short,boxm_sample<BOXM_APM_MOG_GREY> >* cell = tree->locate_point_global(vgl_point_3d<double>(1,1,1));
  TEST_NEAR("A non-polygon point should be non-zero", cell->data().alpha, std::numeric_limits<float>::max(), 1e-6);

  vpl_rmdir("./boxm_upload");
  vpl_unlink("./scene.xml");
}

TESTMAIN(test_upload_mesh);
