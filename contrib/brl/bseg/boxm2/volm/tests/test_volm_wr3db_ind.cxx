#include <testlib/testlib_test.h>
#include <boxm2/volm/boxm2_volm_wr3db_index.h>
#include <boxm2/volm/boxm2_volm_wr3db_index_sptr.h>
#include <bbas/volm/volm_spherical_container.h>
#include <bbas/volm/volm_spherical_shell_container.h>
#include <bbas/volm/volm_spherical_shell_container_sptr.h>
#include <vnl/vnl_random.h>

static void test_volm_wr3db_ind()
{
  // create a much smaller index for testing purposes
  float vmin = 2.0f;  // min voxel resolution
  float dmax = 3000.0f;
  float solid_angle = 2.0f;
  volm_spherical_container_sptr sph2 = new volm_spherical_container(solid_angle,vmin,dmax);
#if 0
  boxm2_volm_wr3db_index_sptr ind = new boxm2_volm_wr3db_index(sph2);
  std::cout << "number of voxels in container: " << sph2->get_voxels().size() << std::endl;
  sph2->draw_template("./container.vrml",0.0);

   fill the layer after vmin with some depth interval values:
  unsigned int offset, end_offset; double depth;
  sph2->first_res(sph2->min_voxel_res()*2, offset, end_offset, depth);
  std::vector<unsigned char> values(end_offset-offset);
  vnl_random rng;
  for (unsigned i = 0; i+offset < end_offset; ++i)
    values[i] = (unsigned char)rng.drand32(1.0,(double)(sph2->get_depth_offset_map().size()-1));

  std::vector<char> vis_prob;
  vis_prob.resize(sph2->get_voxels().size());
  boxm2_volm_wr3db_index::inflate_index_vis_and_prob(values, sph2, vis_prob);
  sph2->draw_template_vis_prob("./container2.vrml", 0.0, vis_prob);
#endif
  volm_spherical_shell_container_sptr sph_shell = new volm_spherical_shell_container(1.0, 90.0, 30.0, 0.0, 0.0);

  // test io
  int layer_size = (int)sph_shell->get_container_size();
  boxm2_volm_wr3db_index_sptr ind = new boxm2_volm_wr3db_index(layer_size, 1.0f);

  std::vector<unsigned char> vals(layer_size, 0);
  vals[0] = 'a'; vals[3] = 'c';

  vul_file::delete_file_glob("./test_ind.bin");
  TEST("initialize write", ind->initialize_write("./test_ind.bin"), true);
  // now fill up the active cache twice! before the second fill the first batch needs to be written to disc
  for (unsigned i = 0; i < ind->buffer_size(); i++)
    ind->add_to_index(vals);
  // the one below should trigger a write to disc
  vals[0] = 'b';  vals[layer_size-1] = 'd';
  TEST("add to index", ind->add_to_index(vals), true);
  auto *vals_buf = new unsigned char[layer_size];
  vals_buf[0] = 'e';  vals_buf[layer_size-1] = 'f';
  TEST("add to index", ind->add_to_index(vals_buf), true);
  std::cout << "global id: " << ind->current_global_id() << " current active cache id: " << ind->current_id() << std::endl;

  TEST("finalize write", ind->finalize(), true);

  const boxm2_volm_wr3db_index_sptr& ind2 = ind;

  // initialize_read() finalizes write operations in case there is any active write operation
  TEST("initialize read", ind2->initialize_read("./test_ind.bin"), true);
  TEST("global id", ind2->current_global_id(), 0);

  std::vector<unsigned char> vals2(layer_size);
  TEST("getting the first index", ind2->get_next(vals2), true);
  TEST("test index 0", vals2[0] == 'a', true);
  TEST("test index 0", vals2[3] == 'c', true);
  for (unsigned i = 0; i < ind->buffer_size()-1; i++)
    ind2->get_next(vals2);
  ind2->get_next(vals2);

  std::vector<unsigned char> vals_buf2(layer_size);
  ind2->get_next(vals_buf2);
  TEST("test index end", vals_buf2[0] == 'e', true);
  TEST("test index end", vals_buf2[layer_size-1] == 'f', true);
  TEST("finalize read", ind2->finalize(), true);
}

TESTMAIN(test_volm_wr3db_ind);

#if 0
  float vmin = 10.0f;  // min voxel resolution
  float dmax = 60000.0f;
  float solid_angle = 4.0f;

  volm_spherical_container_sptr sph = new volm_spherical_container(solid_angle,vmin,dmax);
  std::vector<volm_voxel>& voxels = sph->get_voxels();
  int data_size = 1; // 1 byte if only vis values will be indexed, otherwise its 2 bytes
  double ind_size = voxels.size()*data_size/1048576.0;  // in MBs
  std::cout << "number of voxels in container: " << voxels.size() << " size of voxel array for " << data_size << " bytes: " << ind_size << " MB" << std::endl;
  double inc = 10.0;
  double num = (100000/inc)*(100000/inc);
  std::cout << " for 100 km size WR (in each size) for every " << inc << "m there are: " << num << " location hypotheses\n"
           << " then the index size on file is: " << num*ind_size << " MBs = " << num*ind_size/1024 << " GBs = " << num*ind_size/1048576 << " TBs.\n";

  unsigned int offset, end_offset; double d;
  //sph->first_res(sph->min_voxel_res()*2, offset, end_offset, d);
  sph->last_res(vmin, offset, end_offset, d);

  std::cout << " last layer with vmin res is at depth: " << d << " and has offset: " << offset << " end_offset: " << end_offset << '\n'
           << "first voxel: " << sph->get_voxels()[offset].center_ << ' ' << sph->get_voxels()[offset].resolution_ << '\n'
           << "last voxel: " << sph->get_voxels()[end_offset-1].center_ << ' ' << sph->get_voxels()[end_offset-1].resolution_ << '\n'
           << "after last voxel: " << sph->get_voxels()[end_offset].center_ << ' ' << sph->get_voxels()[end_offset].resolution_ << std::endl;
  unsigned int size = end_offset-offset;
  std::cout << "size is: " << size << std::endl;
  ind_size = size*sizeof(float)*2*num/1048576.0;  // in MBs
  std::cout << "if we index 2 floats for this layer each location index is of size: "<< size*sizeof(float)*2/1048576.0 << " MBs, for all locs, size is: "<< ind_size << " MB = " << ind_size/1024 << " GB\n"
           << "if we index 1 floats for this layer each location index is of size: "<< size*sizeof(float)/1048576.0 << " MBs, for all locs, size is: "<< ind_size/2 << " MB = " << ind_size/2048 << " GB\n"
           << "if we index 1 unsigned short for this layer, each location index is of size: "<< size*sizeof(unsigned short)/1048576.0 << " MBs, for all locs, size is: "<< size*num*sizeof(unsigned short)/1048576.0 << " MBs = " << size*num*sizeof(unsigned short)/1048576.0/1024 << " GBs\n";
#endif

#if 0
  // use hypotheses to generate index
  boxm2_volm_wr3db_index_sptr ind = new boxm2_volm_wr3db_index(sph);
  ind->index_locations(scene, h);
  std::string out_name = out_file() + "_volm_index_" + tiles[i].get_string() + ".bin";
  if (!ind->write_index(out_name))
    std::cerr << "Problems writing index: " << out_name << std::endl;

  for (unsigned i = 0; i < tiles.size(); i++) {
    std::string name = out_file() + "_volm_index_" + tiles[i].get_string() + ".bin";
    boxm2_volm_wr3db_index_sptr ind = new boxm2_volm_wr3db_index(sph);
    ind->read_index(name);
  }
#endif
