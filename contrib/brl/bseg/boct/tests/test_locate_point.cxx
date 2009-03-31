#include <testlib/testlib_test.h>

#include <boct/boct_tree.h>
#include <boct/boct_test_util.h>
#include <vnl/vnl_random.h>

boct_tree_cell<vgl_point_3d<double> >* brute_force_locate_point(vcl_vector<boct_tree_cell<vgl_point_3d<double> >*> leafcells, 
                                             vgl_point_3d<double> p, 
                                             int max_level)
{
 boct_loc_code code(p,max_level);
 
 boct_tree_cell<vgl_point_3d<double> >* point_container;

 for (unsigned i=0;i<leafcells.size();i++)
 {
     if(leafcells[i]->code_.isequal(&code,leafcells[i]->level()))
         point_container=leafcells[i];
 }
 return point_container;
}


MAIN( test_locate_point )
{
  START ("Locate Point");
  short nlevels=10;
  boct_tree<vgl_point_3d<double> > * simpleblock=new boct_tree<vgl_point_3d<double> >(nlevels);
  vgl_point_3d<double> p1(0.1,0.1,0.1);
  boct_tree_cell<vgl_point_3d<double> >* cell=simpleblock->locate_point(p1);
  TEST("Returns the correct level",nlevels-1, cell->level());
  
  boct_tree<vgl_point_3d<double> > * twolevelblock=new boct_tree<vgl_point_3d<double> >(nlevels);
  twolevelblock->split();
  
  vgl_point_3d<double> p2(0.1,0.1,0.1);
  cell=twolevelblock->locate_point(p2);
  TEST("Returns the correct level",nlevels-2, cell->level());
  
  //: Testing on randomly created trees.
  boct_tree<vgl_point_3d<double> > * rtree=new boct_tree<vgl_point_3d<double> >(nlevels);
  create_random_configuration_tree<vgl_point_3d<double> >(rtree);

  vcl_vector<boct_tree_cell<vgl_point_3d<double> >*> leaves=rtree->leaf_cells();
  unsigned int cnt=0;
  for(unsigned i=0;i<100;i++)
  {
      vnl_random rand;
      vgl_point_3d<double> p(rand.drand32(),rand.drand32(),rand.drand32());
      boct_tree_cell<vgl_point_3d<double> >* curr_cell_using_point_locate=rtree->locate_point(p);

      boct_tree_cell<vgl_point_3d<double> >* cell_found_brute_force=brute_force_locate_point(leaves,p,nlevels);
   
      if(curr_cell_using_point_locate->code_.isequal(&cell_found_brute_force->code_,cell_found_brute_force->level()))
          cnt++;
  }
  TEST("Returns the correct Point for 100 points",100, cnt);

  SUMMARY();
}