#include <testlib/testlib_test.h>
#include "sample_database.h"
#include <brdb/brdb_value.h>
#include <brdb/brdb_tuple.h>
#include <brdb/brdb_relation.h>
#include <brdb/brdb_database.h>
#include <vcl_iostream.h>
#include <vcl_string.h>

MAIN( test_database )
{
  START ("Database");

  brdb_database_sptr test_db1 = generate_sample_database();

  //////////////////////////////////////////////////////////////////////////////////
  ////////  test the binary IO funtion of database
  //////////////////////////////////////////////////////////////////////////////////
  vcl_cout << "test_db1: " << vcl_endl;
  test_db1->print();
  vcl_cout << vcl_endl;

  vsl_b_ofstream out_stream("test_database_bio.vsl");
  if (!out_stream){
    vcl_cerr<<"Failed to open test_database_bio.vsl for output.\n";
  }
  vcl_cout << "Opened file successfully " << vcl_endl;

  test_db1->b_write(out_stream);
  out_stream.close();
  test_db1->clear();

  brdb_database_sptr test_db = new brdb_database();

  vsl_b_ifstream in_stream("test_database_bio.vsl");
  if (!in_stream){
    vcl_cerr<<"Failed to open test_relation_bio.vsl for input.\n";
  }
  vcl_cout << "Opened file successfully " << vcl_endl;

  test_db->b_read(in_stream);
  in_stream.close();

  test_db->print();

  TEST("Binary IO read and write", true, true);

  TEST("construct", true, true);

  vcl_set<vcl_string> all_relation_names = test_db->get_all_relation_names();
  vcl_cout << " Relation names:  " << vcl_endl;
  for (vcl_set<vcl_string>::iterator itr = all_relation_names.begin(); itr != all_relation_names.end(); ++itr)
  {
    vcl_cout << "   " << (*itr) << vcl_endl;
  }
  vcl_cout << vcl_endl;
  TEST("get_all_relation_names()", true, true);

  test_db->print();

  TEST("print()", true, true);

  TEST("size()", (test_db->size() == 3), true);

  TEST("exists()", (test_db->exists("department")), true);

  brdb_relation_sptr r3 = test_db->get_relation("department");
  TEST("get_relation()", r3 == NULL, false);

  test_db->remove_relation("department");
  TEST("remove_relation()", test_db->exists("department"), false);

  test_db->add_relation("new_department", r3);
  TEST("add_new_relation()", test_db->exists("new_department"), true);

  brdb_tuple_sptr r3_new_tuple = new brdb_tuple(999, vcl_string("Engineering Department"));
  unsigned int prev_size = r3->size();
  bool added = test_db->add_tuple("new_department", r3_new_tuple);
  TEST("add_tuple()", added && r3->size() == prev_size+1, true);

  test_db->clear();
  TEST("clear()", (test_db->size() == 0), true);

  TEST("empty()", (test_db->empty()), true);

  SUMMARY();
}
