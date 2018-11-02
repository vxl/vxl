#include <iostream>
#include <testlib/testlib_test.h>
#include <brdb/brdb_value.h>
#include <brdb/brdb_tuple.h>
#include <brdb/brdb_relation.h>
#ifdef _MSC_VER
#  include <vcl_msvc_warnings.h>
#endif

static void test_relation()
{
  std::vector<std::string> names(3);
  std::vector<std::string> types(3);
  std::vector<brdb_tuple_sptr> tuples(4);

  names[0] = "frames";
  names[1] = "time";
  names[2] = "nickname";
  types[0] = brdb_value_t<int>::type();
  types[1] = brdb_value_t<double>::type();
  types[2] = brdb_value_t<std::string>::type();
  tuples[0] = new brdb_tuple(1, 3.5, std::string("string1"));
  tuples[1] = new brdb_tuple(10, 20.378, std::string("string4"));
  tuples[2] = new brdb_tuple(5, 0.368, std::string("string3"));
  tuples[3] = new brdb_tuple(-100, -120.1, std::string("string2"));

  std::cout << "construct1" << std::endl;
  brdb_relation my_relation1(names,types);
  std::cout << "construct2" << std::endl;
  brdb_relation my_relation2(names,tuples,types);
  TEST("Constructors", true, true);

  /////////////////////////////////////////////////////////////////////
  /////// Test binary IO
  /////////////////////////////////////////////////////////////////////
  std::cout << "my_relation1: " << std::endl;
  my_relation1.print();
  std::cout << "my_relation2: " << std::endl;
  my_relation2.print();
  std::cout << std::endl;

  vsl_b_ofstream out_stream("test_relation_bio.vsl");
  if (!out_stream)
    std::cerr<<"Failed to open test_relation_bio.vsl for output.\n";
  else
    std::cout << "Opened file test_relation_bio.vsl successfully" << std::endl;

  my_relation2.b_write(out_stream);
  out_stream.close();

  vsl_b_ifstream in_stream("test_relation_bio.vsl");
  if (!in_stream)
    std::cerr<<"Failed to open test_relation_bio.vsl for input.\n";
  else
    std::cout << "Opened file test_relation_bio.vsl successfully" << std::endl;

  my_relation1.b_read(in_stream);
  in_stream.close();

  my_relation1.print();

  TEST("Binary IO read and write", true, true);

  TEST("size()", my_relation2.size(), 4);
  TEST("arity()", my_relation2.arity(), 3);
  TEST("name(0)", my_relation2.name(0), std::string("frames"));
  TEST("name(1)", my_relation2.name(1), std::string("time"));
  TEST("name(2)", my_relation2.name(2), std::string("nickname"));
  TEST("index=0", my_relation2.index(std::string("frames")), 0);
  TEST("index=1", my_relation2.index(std::string("time")), 1);
  TEST("index=2", my_relation2.index(std::string("nickname")), 2);

  std::vector<brdb_value_t<int> > my_int(4);
  my_relation2.order_by(std::string("frames"), false);

  auto it = my_relation2.begin();

  for (int i=0; it != my_relation2.end(); ++it, ++i)
    my_relation2.get_value(it, "frames", my_int[i]);

  TEST("sort_by_name", my_int[0].value() >= my_int[1].value() &&
                       my_int[1].value() >= my_int[2].value() &&
                       my_int[2].value() >= my_int[3].value(), true);

  bool set_value_test_pass = true;
  brdb_value_t<int> int_val1, int_val2;

  for (it = my_relation2.begin(); it != my_relation2.end(); ++it)
  {
    my_relation2.get_value(it, "frames", int_val1);
    int_val2 = 0 - int_val1;
    my_relation2.set_value(it, "frames", int_val2);
    my_relation2.get_value(it, "frames", int_val2);

    if (int_val1 + int_val2 != 0)
    {
      set_value_test_pass = false;
      break;
    }
  }
  TEST("set_value", set_value_test_pass, true);

  std::vector<brdb_value_t<double> > my_double(4);
  my_relation2.order_by(1, true);
  it = my_relation2.begin();
  for (int i=0; it != my_relation2.end(); ++it, ++i)
    my_relation2.get_value(it, "time", my_double[i]);

  TEST("sort_by_index", my_double[0].value() <= my_double[1].value() &&
                        my_double[1].value() <= my_double[2].value() &&
                        my_double[2].value() <= my_double[3].value(), true);

  it = my_relation2.begin();
  brdb_tuple_sptr new_tuple1 = new brdb_tuple(300, -20.1, std::string("stringX"));
  my_relation2.insert_tuple(new_tuple1, it);
  it = my_relation2.begin();
  brdb_value_t<int> int_val3;
  my_relation2.get_value(it, "frames", int_val3);
  TEST("insert tuple", int_val3, 300);

  it = my_relation2.begin();
  brdb_tuple_sptr new_tuple2 = new brdb_tuple(-300, 20.1, std::string("stringX2"));
  my_relation2.add_tuple(new_tuple2);
  it = my_relation2.begin();
  brdb_value_t<int> int_val5;
  it = my_relation2.end();
  --it;
  my_relation2.get_value(it, "frames", int_val5);
  TEST("add tuple", int_val5, -300);

  unsigned int size1 = my_relation2.size();
  my_relation2.remove_tuple(it);
  it = my_relation2.end();
  --it;
  my_relation2.get_value(it, "frames", int_val5);
  TEST("remove", int_val5 != -300 && my_relation2.size()+1 == size1, true);

  my_relation2.clear();
  TEST("clear(): arity", my_relation2.arity(), 3);
  TEST("clear(): size",  my_relation2.size(),  0);
}

TESTMAIN(test_relation);
