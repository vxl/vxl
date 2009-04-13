#include <testlib/testlib_test.h>

#include <boct/boct_tree.h>
#include <boct/boct_test_util.h>
#include <vnl/vnl_random.h>

boct_tree_cell<short,vgl_point_3d<double>,void >* brute_force_locate_point(vcl_vector<boct_tree_cell<short,vgl_point_3d<double>,void >*> leafcells,
                                                                           vgl_point_3d<double> p,
                                                                           int max_level)
{
 boct_loc_code<short> code(p,max_level);

 boct_tree_cell<short,vgl_point_3d<double>,void >* point_container;

 for (unsigned i=0;i<leafcells.size();i++)
 {
     if (leafcells[i]->code_.isequal(&code,leafcells[i]->level()))
         point_container=leafcells[i];
 }
 return point_container;
}


MAIN( test_locate_point )
{
  START ("Locate Point");
  short nlevels=10;
  boct_tree<short,vgl_point_3d<double>,void > * simpleblock=new boct_tree<short,vgl_point_3d<double>,void >(nlevels);
  vgl_point_3d<double> p1(0.1,0.1,0.1);
  boct_tree_cell<short,vgl_point_3d<double>,void >* cell=simpleblock->locate_point(p1);
  TEST("Returns the correct level",nlevels-1, cell->level());

  boct_tree<short,vgl_point_3d<double>,void > * twolevelblock=new boct_tree<short,vgl_point_3d<double>,void >(nlevels);
  twolevelblock->split();

  vgl_point_3d<double> p2(0.1,0.1,0.1);
  cell=twolevelblock->locate_point(p2);
  TEST("Returns the correct level",nlevels-2, cell->level());

  // Testing on randomly created trees.
  boct_tree<short,vgl_point_3d<double>,void > * rtree=new boct_tree<short,vgl_point_3d<double>,void >(nlevels);
  create_random_configuration_tree<short,vgl_point_3d<double>,void >(rtree);

  vcl_vector<boct_tree_cell<short,vgl_point_3d<double>,void >*> leaves=rtree->leaf_cells();
  unsigned int cnt=0;
  for (unsigned i=0;i<100;i++)
  {
      vnl_random rand;
      vgl_point_3d<double> p(rand.drand32(),rand.drand32(),rand.drand32());
      boct_tree_cell<short,vgl_point_3d<double>,void >* curr_cell_using_point_locate=rtree->locate_point(p);

      boct_tree_cell<short,vgl_point_3d<double>,void >* cell_found_brute_force=brute_force_locate_point(leaves,p,nlevels);

      if (curr_cell_using_point_locate->code_.isequal(&cell_found_brute_force->code_,cell_found_brute_force->level()))
          cnt++;
  }
  TEST("Returns the correct Point for 100 points",100, cnt);

  SUMMARY();
}
