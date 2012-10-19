#include <testlib/testlib_test.h>
#include <boxm2/volm/boxm2_volm_wr3db_index.h>
#include <boxm2/volm/boxm2_volm_wr3db_index_sptr.h>
#include <bbas/volm/volm_spherical_container.h>
#include <vnl/vnl_random.h>

static void test_volm_wr3db_index()
{
  float vmin = 10.0f;  // min voxel resolution
  float dmax = 60000.0f;
  float solid_angle = 4.0f;
#if 0
  volm_spherical_container_sptr sph = new volm_spherical_container(solid_angle,vmin,dmax);
  vcl_vector<volm_voxel>& voxels = sph->get_voxels();
  int data_size = 1; // 1 byte if only vis values will be indexed, otherwise its 2 bytes
  double ind_size = voxels.size()*data_size/1000000.0;  // in MBs
  vcl_cout << "number of voxels in container: " << voxels.size() << " size of voxel array for " << data_size << " bytes: " << ind_size << " MB " << vcl_endl;
  double inc = 10.0;
  double num = (100000/inc)*(100000/inc); 
  vcl_cout << " for 100 km size WR (in each size) for every " << inc << "m there are: " << num << " location hypotheses\n";
  vcl_cout << " then the index size on file is: " << num*ind_size << " MBs = " << num*ind_size/1000 << " GBs = " << num*ind_size/1000000 << " TBs.\n";
  
  unsigned int offset, end_offset; double d;
  //sph->first_res(sph->min_voxel_res()*2, offset, end_offset, d);
  sph->last_res(vmin, offset, end_offset, d);
  
  vcl_cout << " last layer with vmin res is at depth: " << d << " and has offset: " << offset << " end_offset: " << end_offset << vcl_endl;
  vcl_cout << "first voxel: " << sph->get_voxels()[offset].center_ << " " << sph->get_voxels()[offset].resolution_ << vcl_endl;
  vcl_cout << "last voxel: " << sph->get_voxels()[end_offset-1].center_ << " " << sph->get_voxels()[end_offset-1].resolution_ << vcl_endl;
  vcl_cout << "after last voxel: " << sph->get_voxels()[end_offset].center_ << " " << sph->get_voxels()[end_offset].resolution_ << vcl_endl;
  unsigned int size = end_offset-offset;
  vcl_cout << "size is: " << size << vcl_endl;
  ind_size = size*sizeof(float)*2*num/1000000.0;  // in MBs
  vcl_cout << "if we index 2 floats for this layer each location index is of size: "<< size*sizeof(float)*2/1000000.0 << " MBs, for all locs, size is: "<< ind_size << " MB = " << ind_size/1000 << " GB" << vcl_endl; 
  vcl_cout << "if we index 1 floats for this layer each location index is of size: "<< size*sizeof(float)*1/1000000.0 << " MBs, for all locs, size is: "<< ind_size << " MB = " << ind_size/(2*1000) << " GB" << vcl_endl; 
  vcl_cout << "if we index 1 unsigned short for this layer, each location index is of size: "<< size*sizeof(unsigned short)*1/1000000.0 << " MBs, for all locs, size is: "<< size*num*sizeof(unsigned short)*1/1000000.0 << " MBs = " << size*num*sizeof(unsigned short)*1/1000000.0/1000 << " GBs\n";
#endif

  //: create a much smaller index for testing purposes
  vmin = 10.0f;  // min voxel resolution
  dmax = 100.0f;
  solid_angle = 40.0f;
  volm_spherical_container_sptr sph2 = new volm_spherical_container(solid_angle,vmin,dmax);
  boxm2_volm_wr3db_index_sptr ind = new boxm2_volm_wr3db_index(sph2);
  vcl_cout << "number of voxels in container: " << sph2->get_voxels().size() << vcl_endl;
  sph2->draw_template("./container.vrml",0.0);
  
  // fill the layer after vmin with some depth interval values:
  unsigned int offset, end_offset; double depth;
  sph2->first_res(sph2->min_voxel_res()*2, offset, end_offset, depth);
  vcl_vector<unsigned char> values(end_offset-offset);
  vnl_random rng;
  for (unsigned i = offset; i < end_offset; i++)
    values[i-offset] = (unsigned char)rng.drand32(1.0,(double)(sph2->get_depth_offset_map().size()-1));
  
  boxm2_volm_wr3db_index index(sph2);
  index.add_to_index(values);
  
  vcl_vector<unsigned char> vis_prob;
  vis_prob.resize(sph2->get_voxels().size());
  index.inflate_index_vis_and_prob(0, vis_prob);
  
  vcl_vector<unsigned> in;
  sph2->draw_template_vis_prob("./container2.vrml", 0.0, vis_prob);

}

TESTMAIN(test_volm_wr3db_index);


