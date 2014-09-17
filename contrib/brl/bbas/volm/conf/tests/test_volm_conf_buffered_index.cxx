#include <testlib/testlib_test.h>
#include <volm/conf/volm_conf_buffered_index.h>
#include <volm/conf/volm_conf_object.h>
#include <vnl/vnl_random.h>
#include <vul/vul_file.h>
#include <vcl_algorithm.h>

#define EPISLON 1E-5

static void test_volm_conf_buffered_index()
{
  float buffer_capacity = 200000.0f*2.0f*32.0f/(1024.0f*1024.0f*1024.0f);  // buffer size that can store 200000 objects
  //float buffer_capacity = 2.0f;  // 2.0GB buffer size
  volm_conf_buffered_index_sptr ind_sptr = new volm_conf_buffered_index(buffer_capacity);
  vcl_cout << "Unit size of index: " << ind_sptr->unit_size() << vcl_endl;
  vcl_cout << "Maximum number of volm_conf_object can be stored in index buffer: " << ind_sptr->buffer_size() << vcl_endl;

  // randomly create a vector of volm_conf_object
  vnl_random rnd(9667566);
  vcl_vector<vcl_vector<volm_conf_object> > values;
  unsigned size = rnd.lrand32(100, 300);
  for (unsigned k = 0; k < 40;  k++)
  {
    vcl_vector<volm_conf_object> value;
    unsigned char land_id = k;
    for (unsigned i = 0; i < (k+1)*100; i++)
      value.push_back(volm_conf_object(rnd.drand32(0.0, 360.0), rnd.drand32(0, 1000), k));
    values.push_back(value);
  }
  
  // add values into index database
  vcl_string filename = "./volm_conf_buffered_index.bin";
  TEST("unit size of buffered index should be sizeof(volm_conf_object)", ind_sptr->unit_size(), sizeof(volm_conf_object));
  TEST("initial mode should be NOT_INITIALIZED", ind_sptr->index_mode(), "NOT_INITIALIZED");
  TEST("buffer size in unit of sizeof(volm_conf_object) should be 20", ind_sptr->buffer_size(), 200000);
  TEST("initialize buffered index for WRITE", ind_sptr->initialize_write(filename), true);
  TEST("initial current id in the buffer should be zero", ind_sptr->current_id(), 0);
  TEST("initial global id in the buffer should be zero", ind_sptr->global_current_id(), 0);
  TEST("initial length vector should be empty", ind_sptr->length_vec().empty(), true);
  for (unsigned k = 0; k < values.size(); k++) {
    TEST("add index to buffer", ind_sptr->add_to_index(values[k]), true);
  }
  // finish
  TEST("finished adding index into buffer", ind_sptr->finalize(), true);

  // read values from binary file
  vcl_vector<vcl_vector<volm_conf_object_sptr> > values_in;
  //float buffer_capacity_in = 200.0f*2.0f*32.0f/(1024.0f*1024.0f*1024.0f);  // 2.0GB buffer size
  float buffer_capacity_in = 1.0f; // 2.0GB buffer size
  volm_conf_buffered_index_sptr ind_in_sptr = new volm_conf_buffered_index(buffer_capacity_in);
  TEST("initialize buffered index for READ", ind_in_sptr->initialize_read(filename), true);

  for (unsigned k = 0; k < values.size(); k++)
  {
    vcl_vector<volm_conf_object_sptr> value;
    TEST("get next from buffer: ", ind_in_sptr->get_next(value), true);
    //ind_in_sptr->get_next(value);
    values_in.push_back(value);
  }

  bool is_same = true;
  for (unsigned k = 0; k < values.size(); k++)
    for (unsigned i = 0; i < values[k].size(); i++)
      is_same = is_same && values[k][i].is_same(values_in[k][i]);

  TEST("binary IO test: ", is_same, true);
  return;

}

TESTMAIN( test_volm_conf_buffered_index );
