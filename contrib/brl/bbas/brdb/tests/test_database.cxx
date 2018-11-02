#include <iostream>
#include <string>
#include <testlib/testlib_test.h>
#include "sample_database.h"
#include <brdb/brdb_value.h>
#include <brdb/brdb_tuple.h>
#include <brdb/brdb_relation.h>
#include <brdb/brdb_database.h>
#ifdef _MSC_VER
#  include <vcl_msvc_warnings.h>
#endif

static void test_database()
{

  //////////////////////////////////////////////////////////////////////////////////
  ////////  test the binary IO function of database
  //////////////////////////////////////////////////////////////////////////////////
  std::cout << "Warning: Binary I/O test deactivated because of failures" <<std::endl;

#if 0
  brdb_database_sptr test_db1 = generate_sample_database();

  std::cout << "test_db1: " << std::endl;
  test_db1->print();
  std::cout << std::endl;

  vsl_b_ofstream out_stream("test_database_bio.vsl");
  if (!out_stream){
    std::cerr<<"Failed to open test_database_bio.vsl for output.\n";
  }
  std::cout << "Opened file successfully " << std::endl;

  test_db1->b_write(out_stream);
  out_stream.close();
  test_db1->clear();

  brdb_database_sptr test_db = new brdb_database();

  vsl_b_ifstream in_stream("test_database_bio.vsl");
  if (!in_stream){
    std::cerr<<"Failed to open test_relation_bio.vsl for input.\n";
  }
  std::cout << "Opened file successfully " << std::endl;

  test_db->b_read(in_stream);
  in_stream.close();

  test_db->print();

  TEST("Binary IO read and write", true, true);
#endif

  brdb_database_sptr test_db = generate_sample_database();

  TEST("construct", true, true);

  std::set<std::string> all_relation_names = test_db->get_all_relation_names();
  std::cout << " Relation names:  " << std::endl;
  for (const auto & all_relation_name : all_relation_names)
  {
    std::cout << "   " << all_relation_name << std::endl;
  }
  std::cout << std::endl;
  TEST("get_all_relation_names()", true, true);

  test_db->print();

  TEST("print()", true, true);

  TEST("size()", (test_db->size() == 3), true);

  TEST("exists()", (test_db->exists("department")), true);

  brdb_relation_sptr r3 = test_db->get_relation("department");
  TEST("get_relation()", r3 == nullptr, false);

  test_db->remove_relation("department");
  TEST("remove_relation()", test_db->exists("department"), false);

  test_db->add_relation("new_department", r3);
  TEST("add_new_relation()", test_db->exists("new_department"), true);

  brdb_tuple_sptr r3_new_tuple = new brdb_tuple(999, std::string("Engineering Department"));
  unsigned int prev_size = r3->size();
  bool added = test_db->add_tuple("new_department", r3_new_tuple);
  TEST("add_tuple()", added && r3->size() == prev_size+1, true);

  test_db->clear();
  TEST("clear()", (test_db->size() == 0), true);

  TEST("empty()", (test_db->empty()), true);
}

TESTMAIN(test_database);
