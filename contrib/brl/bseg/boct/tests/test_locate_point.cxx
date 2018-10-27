#include <testlib/testlib_test.h>

#include <boct/boct_tree.h>
#include <boct/boct_test_util.h>
#include <vnl/vnl_random.h>

boct_tree_cell<short,vgl_point_3d<double> >* brute_force_locate_point(std::vector<boct_tree_cell<short,vgl_point_3d<double> >*> leafcells,
                                                                      vgl_point_3d<double> p,
                                                                      int max_level)
{
  short root_level = max_level-1;
  auto max_val = (double)(1<<root_level);
 boct_loc_code<short> code(p,root_level, max_val);

 boct_tree_cell<short,vgl_point_3d<double> >* point_container=nullptr;

 for (auto & leafcell : leafcells)
 {
   if (leafcell->code_.isequal(&code))
     point_container=leafcell;
 }
 return point_container;
}


static void test_locate_point()
{
  short nlevels=10;
  auto * simpleblock=new boct_tree<short,vgl_point_3d<double> >(nlevels);
  vgl_point_3d<double> p1(0.1,0.1,0.1);
  boct_tree_cell<short,vgl_point_3d<double> >* cell=simpleblock->locate_point(p1);
  TEST("Returns the correct level", nlevels, cell->level()+1);

  auto * twolevelblock=new boct_tree<short,vgl_point_3d<double> >(nlevels);
  twolevelblock->split();

  vgl_point_3d<double> p2(0.1,0.1,0.1);
  cell=twolevelblock->locate_point(p2);
  TEST("Returns the correct level", nlevels, cell->level()+2);

  // Testing on randomly created trees.
  auto * rtree=new boct_tree<short,vgl_point_3d<double> >(nlevels);
  create_random_configuration_tree<short,vgl_point_3d<double> >(rtree);

  std::vector<boct_tree_cell<short,vgl_point_3d<double> >*> leaves=rtree->leaf_cells();
  unsigned int cnt=0;
  for (unsigned i=0; i<100; ++i)
  {
    vnl_random rand;
    vgl_point_3d<double> p(rand.drand32(),rand.drand32(),rand.drand32());
    boct_tree_cell<short,vgl_point_3d<double> >* curr_cell_using_point_locate=rtree->locate_point(p);

    boct_tree_cell<short,vgl_point_3d<double> >* cell_found_brute_force=brute_force_locate_point(leaves,p,nlevels);

    if (curr_cell_using_point_locate->code_.isequal(&cell_found_brute_force->code_))
      ++cnt;
  }
  TEST("Returns the correct Point for 100 points", cnt, 100);
}

TESTMAIN(test_locate_point);
