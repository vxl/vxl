#include <testlib/testlib_test.h>
#include <volm/conf/volm_conf_object.h>
#include <volm/conf/volm_conf_buffer.h>
#include <vnl/vnl_random.h>
#include <vcl_algorithm.h>

static void test_volm_conf_buffer()
{
  float buffer_capacity = 200000.0f*2.0f*sizeof(volm_conf_object)/(1024.0f*1024.0f*1024.0f);  // buffer size that can store 200000 objects
  volm_conf_buffer<volm_conf_object> index(buffer_capacity);
  vcl_cout << "Unit size of index of volm_conf_object: " << index.unit_size() << vcl_endl;
  vcl_cout << "Maximum number of volm_conf_object can be stored in index buffer: " << index.buffer_size() << vcl_endl;

  // randomly create a vector of volm_conf_object
  vnl_random rnd(9667566);
  vcl_vector<vcl_vector<volm_conf_object> > values;
  unsigned data_size = rnd.lrand32(0, 5);
  for (unsigned k = 0; k < data_size;  k++)
  {
    vcl_vector<volm_conf_object> value;
    for (unsigned i = 0; i < (k+1)*100; i++)
      value.push_back(volm_conf_object(rnd.drand32(0.0, 360.0), rnd.drand32(0, 1000), rnd.drand32(0, 100.0), k));
    values.push_back(value);
  }

  // add values into index buffer
  vcl_string filename = "./volm_conf_buffered_index.bin";
  TEST("unit size of buffered index", index.unit_size(), sizeof(volm_conf_object));
  TEST("initial mode should be NOT_INITIALIZED", index.index_mode(), "NOT_INITIALIZED");
  TEST("buffer size", index.buffer_size(), 200000);
  TEST("initialize buffer for WRITE operation", index.initialize_write(filename), true);
  TEST("initial current id in the buffer should be zero", index.current_id(), 0);
  TEST("initial global id in the buffer should be zero", index.global_current_id(), 0);
  TEST("initial length vector should be empty", index.length_vec().empty(), true);
  for (unsigned k = 0; k < values.size(); k++)
    TEST("add index to buffer", index.add_to_index(values[k]), true);
  TEST("finish adding index into buffer", index.finalize(), true);

  // read values from binary file
  vcl_vector<vcl_vector<volm_conf_object> > values_in;
  float buffer_capacity_in = 1.0f;  // 1.0 GB buffer size
  volm_conf_buffer<volm_conf_object> index_in(buffer_capacity_in);
  TEST("initialize buffer for READ operation", index_in.initialize_read(filename), true);
  for (unsigned k = 0; k < data_size; k++)
  {
    vcl_vector<volm_conf_object> value;
    TEST("get next from buffer", index_in.get_next(value), true);
    values_in.push_back(value);
  }

  bool is_same = true;
  for (unsigned k = 0; k < data_size; k++)
    for (unsigned i = 0; i < values[k].size(); i++) {
      if (k < 10 && i < 5) {
        vcl_cout << "------------------ value " << k << " -------------------- " << vcl_endl;
        vcl_cout << "    input value " << i << ": ";  values[k][i].print(vcl_cout);
        vcl_cout << "   loaded value " << i << ": ";  values_in[k][i].print(vcl_cout);
      }
      is_same = is_same && values[k][i].is_same(values_in[k][i]);
    }

  TEST("buffer binary IO", is_same, true);

  return;
}

TESTMAIN( test_volm_conf_buffer );
