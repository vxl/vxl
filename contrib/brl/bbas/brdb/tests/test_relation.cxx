#include <testlib/testlib_test.h>
#include <brdb/brdb_value.h>
#include <brdb/brdb_tuple.h>
#include <brdb/brdb_relation.h>
#include <vcl_iostream.h>


MAIN( test_relation )
{
  START ("DB Relation");

  vcl_vector<vcl_string> names(3);
  vcl_vector<vcl_string> types(3);
  vcl_vector<brdb_tuple_sptr> tuples(4);

  names[0] = "frames";
  names[1] = "time";
  names[2] = "nickname";
  types[0] = brdb_value_t<int>::type();
  types[1] = brdb_value_t<double>::type();
  types[2] = brdb_value_t<vcl_string>::type();
  tuples[0] = new brdb_tuple(1, 3.5, vcl_string("string1"));
  tuples[1] = new brdb_tuple(10, 20.378, vcl_string("string4"));
  tuples[2] = new brdb_tuple(5, 0.368, vcl_string("string3"));
  tuples[3] = new brdb_tuple(-100, -120.1, vcl_string("string2"));

  vcl_cout << "construct1" << vcl_endl;
  brdb_relation my_relation1(names,types);
  vcl_cout << "construct2" << vcl_endl;
  brdb_relation my_relation2(names,tuples,types);
  TEST("Constructors", true, true);

  /////////////////////////////////////////////////////////////////////
  /////// Test binary IO
  /////////////////////////////////////////////////////////////////////
  vcl_cout << "my_relation1: " << vcl_endl;
  my_relation1.print();
  vcl_cout << "my_relation2: " << vcl_endl;
  my_relation2.print();
  vcl_cout << vcl_endl;

  vsl_b_ofstream out_stream("test_relation_bio.vsl");
  if (!out_stream){
    vcl_cerr<<"Failed to open " << "test_relation_bio.vsl" << " for output." << vcl_endl;
  }
  vcl_cout << "Opened file successfully " << vcl_endl;

  my_relation2.b_write(out_stream);
  out_stream.close();

  vsl_b_ifstream in_stream("test_relation_bio.vsl");
  if (!in_stream){
    vcl_cerr<<"Failed to open " << "test_relation_bio.vsl" << " for input." << vcl_endl;
  }
  vcl_cout << "Opened file successfully " << vcl_endl;

  my_relation1.b_read(in_stream);
  in_stream.close();

  my_relation1.print();

  TEST("Binary IO read and write", true, true);

  

  TEST("size()", (my_relation2.size()==4), true);
  TEST("arity()", (my_relation2.arity()==3), true);
  TEST("name()", (my_relation2.name(0) == vcl_string("frames") &&
          my_relation2.name(1) == vcl_string("time") &&
          my_relation2.name(2) == vcl_string("nickname")),
          true);
  TEST("index()", (my_relation2.index(vcl_string("frames")) == 0 &&
        my_relation2.index(vcl_string("time")) == 1 &&
        my_relation2.index(vcl_string("nickname")) == 2),
        true);


  vcl_vector<brdb_value_t<int> > my_int(4);
  my_relation2.order_by(vcl_string("frames"), false);
  int i=0;
  for(vcl_vector<brdb_tuple_sptr>::iterator it = my_relation2.begin(); it != my_relation2.end(); ++it, ++i)
    my_relation2.get_value(it, "frames", my_int[i]);

  if(my_int[0].value() >= my_int[1].value() &&
    my_int[1].value() >= my_int[2].value() &&
    my_int[2].value() >= my_int[3].value())
  {
    TEST("sort_by_name", true, true);
    TEST("iterator", true, true);
    TEST("get_value()", true, true);  
  }

  bool set_value_test_pass = true;
  i=0;
  brdb_value_t<int> int_val1, int_val2;
  for(vcl_vector<brdb_tuple_sptr>::iterator it = my_relation2.begin(); it != my_relation2.end(); ++it, ++i)
  {
    my_relation2.get_value(it, "frames", int_val1);
    int_val2 = 0 - int_val1;
    my_relation2.set_value(it, "frames", int_val2);
    my_relation2.get_value(it, "frames", int_val2);

    if(int_val1 + int_val2 != 0)
    {
    set_value_test_pass = false;
    break;
    }
  }

  TEST("set_value", set_value_test_pass, true);


  vcl_vector<brdb_value_t<double> > my_double(4);  
  my_relation2.order_by(1, true);
  i=0;
  for(vcl_vector<brdb_tuple_sptr>::iterator it = my_relation2.begin(); it != my_relation2.end(); ++it, ++i)
    my_relation2.get_value(it, "time", my_double[i]);

  if(my_double[0].value() <= my_double[1].value() &&
    my_double[1].value() <= my_double[2].value() &&
    my_double[2].value() <= my_double[3].value())
  {
    TEST("sort_by_index", true, true);
  }

  
  vcl_vector<brdb_tuple_sptr>::iterator it = my_relation2.begin();
  brdb_tuple_sptr new_tuple1 = new brdb_tuple(300, -20.1, vcl_string("stringX"));
  my_relation2.insert_tuple(new_tuple1, it);
  it = my_relation2.begin();
  brdb_value_t<int> int_val3;
  my_relation2.get_value(it, "frames", int_val3);
  TEST("insert tuple", (int_val3 == 300), true);

  it = my_relation2.begin();
  brdb_tuple_sptr new_tuple2 = new brdb_tuple(-300, 20.1, vcl_string("stringX2"));
  my_relation2.add_tuple(new_tuple2);
  it = my_relation2.begin();
  brdb_value_t<int> int_val5;
  it = my_relation2.end();
  --it;
  my_relation2.get_value(it, "frames", int_val5);
  TEST("add tuple", (int_val5 == -300), true);

  int size1 = my_relation2.size();
  my_relation2.remove_tuple(it);
  it = my_relation2.end();
  --it;
  my_relation2.get_value(it, "frames", int_val5);
  TEST("remove", (int_val5 != -300 && static_cast<int>(my_relation2.size()) == size1-1), true);

  my_relation2.clear();
  TEST("clear()", (my_relation2.arity() == 3 && my_relation2.size() == 0), true);

  SUMMARY();
}
