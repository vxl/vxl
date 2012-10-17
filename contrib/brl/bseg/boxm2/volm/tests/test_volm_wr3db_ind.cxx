#include <testlib/testlib_test.h>
#include <boxm2/volm/boxm2_volm_wr3db_index.h>
#include <boxm2/volm/boxm2_volm_wr3db_index_sptr.h>
#include <bbas/volm/volm_spherical_container.h>


#define DROP_FOLDER "C:/Users/ozge/Dropbox/projects/FINDER/simple_index/testscene/"

static void test_volm_wr3db_index()
{
  double vmin = 10;  // min voxel resolution
  double dmax = 60000;
  double solid_angle = 4.0;
  volm_spherical_container_sptr sph = new volm_spherical_container(solid_angle,vmin,dmax);
  vcl_vector<volm_voxel>& voxels = sph->get_voxels();
  bool only_vis = true;
  int data_size = only_vis ? 1 : 2; // 1 byte if only vis values will be indexed, otherwise its 2 bytes
  double ind_size = voxels.size()*data_size/100000.0;
  vcl_cout << "number of voxels in container: " << voxels.size() << " size of voxel array for " << data_size << " bytes: " << ind_size << " MB " << vcl_endl;
  double inc = 10.0;
  double num = (100000/inc)*(100000/inc); 
  vcl_cout << " for 100 km size WR (in each size) for every " << inc << "m there are: " << num << " location hypotheses\n";
  vcl_cout << " then the index size on file is: " << num*ind_size << " MBs = " << num*ind_size/1000 << " GBs = " << num*ind_size/1000000 << " TBs.\n";
  
  unsigned int offset; double d;
  sph->last_vmin(offset, d);
  vcl_cout << " last layer with vmin res is at depth: " << d << " and has offset: " << offset << vcl_endl;
  
  boxm2_volm_wr3db_index_sptr ind = new boxm2_volm_wr3db_index(sph,only_vis);
  
  //for (unsigned i = 0; i < voxels.size(); i++)
  //  ind.values_[i] = 
  //vcl_cout << 
  
  
  //TEST_NEAR("VRML Orientation ",(rot_axis-res_axis).length(),0.0,1e-3);

}

TESTMAIN(test_volm_wr3db_index);


