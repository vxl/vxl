#include <testlib/testlib_test.h>
#include <brdb/brdb_value.h>
#include <brdb/brdb_tuple.h>
#include <brdb/brdb_relation.h>
#include <brdb/brdb_relation_sptr.h>
#include <brdb/brdb_database.h>
#include <brdb/brdb_database_sptr.h>
#include <brdb/brdb_database_manager.h>
#include <brdb/brdb_query.h>
#include <vcl_iostream.h>


MAIN( test_database_manager )
{
  START ("DB database manager");

  ////////////////////////////////////////////////////////////////////////////
  //: example: creating values, tuples and relations
  ////////////////////////////////////////////////////////////////////////////

  vcl_vector<vcl_string> r1_names(3);
  vcl_vector<vcl_string> r1_types(3);
  vcl_vector<brdb_tuple_sptr> r1_tuples(20);
  
  vcl_vector<vcl_string> r2_names(2);
  vcl_vector<vcl_string> r2_types(2);
  vcl_vector<brdb_tuple_sptr> r2_tuples(30);
  
  vcl_vector<vcl_string> r3_names(2);
  vcl_vector<vcl_string> r3_types(2);
  vcl_vector<brdb_tuple_sptr> r3_tuples(40);


  r1_names[0] = "ID";
  r1_names[1] = "Name";
  r1_names[2] = "Gender";

  r1_types[0] = brdb_value_t<int>::type();
  r1_types[1] = brdb_value_t<vcl_string>::type();
  r1_types[2] = brdb_value_t<bool>::type();

  for(int i=0; i<20; i++)
  {
    int ID = 100+i;
    vcl_stringstream SS;
    SS << "zhao_yong_" << i;
    vcl_string name;
    SS >> name;
    bool gender;
    
    if(i%2)
      gender = true;
    else
      gender = false;

    r1_tuples[i] = new brdb_tuple(ID, name, gender);
    r1_tuples[i]->print();
  }

  brdb_relation_sptr r1 = new brdb_relation(r1_names, r1_tuples, r1_types);

  r2_names[0] = "ID";
  r2_names[1] = "Age";


  r2_types[0] = brdb_value_t<int>::type();
  r2_types[1] = brdb_value_t<double>::type();

  for(int i=0; i<30; i++)
  {
    int ID = 100+i;
    double age = 20.0+i*0.1;

    r2_tuples[i] = new brdb_tuple(ID, age);
    r2_tuples[i]->print();
  }

  brdb_relation_sptr r2 = new brdb_relation(r2_names, r2_tuples, r2_types);
  

  r3_names[0] = "ID";
  r3_names[1] = "Department";


  r3_types[0] = brdb_value_t<int>::type();
  r3_types[1] = brdb_value_t<vcl_string>::type();

  for(int i=0; i<40; i++)
  {
    int ID = 100+i;
    vcl_stringstream SS;
    SS << "dempartment_" << i;
    vcl_string dept;
    SS >> dept;

    r3_tuples[i] = new brdb_tuple(ID, dept);
    r3_tuples[i]->print();
  }

  brdb_relation_sptr r3 = new brdb_relation(r3_names, r3_tuples, r3_types);


  ////////////////////////////////////////////////////////////////////////////
  //: example: creating database using relations
  ////////////////////////////////////////////////////////////////////////////

  vcl_vector<brdb_relation_sptr> test_relations;
  test_relations.push_back(r1);
  test_relations.push_back(r2);
  test_relations.push_back(r3);

  vcl_vector<vcl_string> relation_names;
  relation_names.push_back("name_gender");
  relation_names.push_back("age");
  relation_names.push_back("department");


  brdb_database_sptr test_db = new brdb_database(test_relations, relation_names);
  test_db->print();

  ////////////////////////////////////////////////////////////////////////////
  //: example: instance database_manager
  ////////////////////////////////////////////////////////////////////////////
  //  brdb_database_manager::instance(test_db);
  brdb_database_manager::instance();
  TEST("constructor", true, true);

  brdb_database_manager::instance()->set_database(test_db);
  TEST("set_database()", true, true);

  brdb_database_manager::instance()->print_database();
  TEST("instance()", true, true);
  TEST("print_database()", true, true);

  //////////////////////////////////////////////////////////////////////////////////
  ////////  test the binary IO funtion of database
  //////////////////////////////////////////////////////////////////////////////////
  TEST("save_database()", brdb_database_manager::instance()->save_database("test_database_manager_bio.vsl"), true);
  brdb_database_manager::instance()->clear_database();
  TEST("load_database()", brdb_database_manager::instance()->load_database("test_database_manager_bio.vsl"), true);
  brdb_database_manager::instance()->print_database();

  TEST("merge_database", brdb_database_manager::instance()->merge_database("test_database_manager_bio.vsl"), true);
  brdb_database_manager::instance()->print_database();

  brdb_database_manager::instance()->clear_database();
  brdb_database_manager::instance()->load_database("test_database_manager_bio.vsl");
  brdb_database_manager::instance()->print_database();



  brdb_database_manager::instance()->clear_relation("age");
  brdb_database_manager::instance()->print_database();
  TEST("clear_relation()", true, true);
  
  vcl_set<vcl_string> names1;
  names1.insert("age");
  names1.insert("name_gender");
  brdb_database_manager::instance()->clear_all_except(names1);
  brdb_database_manager::instance()->print_database();
  TEST("clear_all_except()", true, true);

  vcl_set<vcl_string> names2;
  names2.insert("age");
  brdb_database_manager::instance()->remove_all_except(names2);
  brdb_database_manager::instance()->print_database();
  TEST("remove_all_except()", true, true);

  brdb_database_manager::instance()->remove_relation("age");
  brdb_database_manager::instance()->print_database();
  TEST("remove_relation()", true, true);

  brdb_database_manager::instance()->clear_database();
  brdb_database_manager::instance()->print_database();
  TEST("clear_database()", true, true);

  brdb_database_manager::instance()->add_relation("name_gender", r1);
  // note r2 is empty at this moment;
  r2->clear();
  brdb_database_manager::instance()->add_relation("age", r2);
  brdb_database_manager::instance()->add_relation("department", r3);
  brdb_database_manager::instance()->print_database();
  TEST("add_relation()", true, true);


  for(int i=0; i<30;i++)
  brdb_database_manager::instance()->add_tuple("age", r2_tuples[i]);
  brdb_database_manager::instance()->print_database();
  TEST("add_relation()", true, true);

  brdb_tuple_sptr r2_new_tuple = new brdb_tuple(1000, 32.1234);
  brdb_database_manager::instance()->add_tuple("age", r2_new_tuple);
  brdb_database_manager::instance()->print_database();
  TEST("add_tuple()", true, true);

  brdb_database_manager::instance()->print_database();


  ////////////////////////////////////////////////////////////////////////////
  //: example: join two relations from database
  ////////////////////////////////////////////////////////////////////////////
  brdb_relation_sptr join1;
  brdb_database_manager::instance()->join("name_gender", "age", join1);
  join1->print();

  brdb_relation_sptr join2;
  brdb_database_manager::instance()->join("name_gender", "department", join2);
  join2->print();
  TEST("join two database relations", true, true);

  brdb_database_manager::instance()->print_database();

  ////////////////////////////////////////////////////////////////////////////
  //: example: join two user relations
  ////////////////////////////////////////////////////////////////////////////
  brdb_relation_sptr join3;
  brdb_database_manager::instance()->join(join1, join2, join3);
  join3->print();
  TEST("join two user relations", true, true);

  brdb_database_manager::instance()->print_database();

  ////////////////////////////////////////////////////////////////////////////
  //: example: join one database relation and one user relations
  ////////////////////////////////////////////////////////////////////////////
  brdb_relation_sptr join4;
  brdb_database_manager::instance()->join("age", join2, join4);
  join4->print();
  TEST("join one database relation and one user relation", true, true);


  ////////////////////////////////////////////////////////////////////////////
  //: example: doing selecting on database relations;
  ////////////////////////////////////////////////////////////////////////////

  // define a query
  brdb_query_sptr q1 = new brdb_query("name_gender", "ID", BT, 115);
  brdb_selection_sptr s1;
  brdb_database_manager::instance()->select(q1, s1);
  s1->print();
  s1->delete_tuples();
  brdb_database_manager::instance()->print_database();

  brdb_query_sptr q2 = new brdb_query("name_gender", "Name", BT, vcl_string("zhao_yong_5"));
  brdb_selection_sptr s2;
  brdb_database_manager::instance()->select(q2, s2);
  s2->print();
  s2->delete_tuples();
  brdb_database_manager::instance()->print_database();

  TEST("select in database relation", true, true);

  ////////////////////////////////////////////////////////////////////////////
  //: example: automatic update feature of the selection
  ////////////////////////////////////////////////////////////////////////////
  brdb_query_sptr q1a = new brdb_query("name_gender", "ID", EQ, 102);
  brdb_selection_sptr s1a;
  brdb_database_manager::instance()->select(q1a, s1a);
  brdb_query_sptr q1b = new brdb_query("name_gender", "ID", LT, 105);
  brdb_selection_sptr s1b;
  brdb_database_manager::instance()->select(q1b, s1b);
  brdb_query_sptr q1c = new brdb_query("name_gender", "ID", EQ, 103);
  brdb_selection_sptr s1c;
  brdb_database_manager::instance()->select(q1c, s1c);
  s1a->print();
  s1a->delete_tuples();
  s1a->print();
  s1c->print();
  s1c->delete_tuples();
  s1c->print();
  s1b->print();
  brdb_database_manager::instance()->print_database();



  ////////////////////////////////////////////////////////////////////////////
  //: example: doing selecting on user relations;
  ////////////////////////////////////////////////////////////////////////////
  // define a query
  r2->print();
  brdb_query_sptr q3 = new brdb_query(r2, "ID", LT, 115);
  brdb_selection_sptr s3;
  brdb_database_manager::instance()->select(q3, s3);
  s3->print();
  s3->delete_tuples();
  r2->print();

  TEST("select in user relation", true, true);

  ////////////////////////////////////////////////////////////////////////////
  //: example: doing selecting on user selections;
  ////////////////////////////////////////////////////////////////////////////

  // define a query
  r1->print();
  brdb_query_sptr q4 = new brdb_query(r1, "ID", BT, 105);
  brdb_selection_sptr s4;
  brdb_database_manager::instance()->select(q4, s4);
  s4->print();

  brdb_query_sptr q5 = new brdb_query(s4, vcl_string("ID"), LT, 113);
  brdb_selection_sptr s5;
  brdb_database_manager::instance()->select(q5, s5);

  s5->print();
  s5->delete_tuples();
  brdb_database_manager::instance()->print_database();
  
  TEST("select in user selections", true, true);


  ////////////////////////////////////////////////////////////////////////////
  //: example: getting tuple from database
  ////////////////////////////////////////////////////////////////////////////
  // define a query
  brdb_query_sptr q6 = new brdb_query("name_gender", "ID", EQ, 114);
  brdb_selection_sptr s6;
  brdb_database_manager::instance()->select(q6, s6);
  vcl_string get_name1;
  bool get_gender1;
  brdb_database_manager::instance()->get(s6, "Name", get_name1);
  brdb_database_manager::instance()->get(s6, "Gender", get_gender1);
  vcl_cout << "ID: 104  Name:   " << get_name1 << "   Gender:   " << get_gender1 << vcl_endl;
  TEST("get()", true, true);

  brdb_query_sptr q6_a = new brdb_query("name_gender", "ID", LT, 105);
  brdb_selection_sptr s6_a;
  brdb_database_manager::instance()->select(q6_a, s6_a);
  vcl_vector<vcl_string> get_names;

  for(unsigned int i=0; i<s6_a->size(); i++)
  {
    vcl_string get_name_a;
    brdb_database_manager::instance()->get(s6_a, "Name", i, get_name_a);
    vcl_cout << get_name_a << "  ";
  }

  ////////////////////////////////////////////////////////////////////////////
  //: example: update tuple/values in database
  ////////////////////////////////////////////////////////////////////////////
  // define a query
  brdb_query_sptr q7 = new brdb_query("name_gender", "ID", EQ, 104);
  brdb_selection_sptr s7;
  brdb_database_manager::instance()->select(q7, s7);
  brdb_tuple_sptr update_tuple = new brdb_tuple(104, vcl_string("monster"), true);
  brdb_database_manager::instance()->update_selected_tuple(s7, update_tuple);
  brdb_database_manager::instance()->print_database();

  bool update_gender(false);
  brdb_database_manager::instance()->update_selected_tuple_value(s7, "Name", vcl_string("zhao_yong_4"));
  brdb_database_manager::instance()->update_selected_tuple_value(s7, "Gender", update_gender);
  brdb_database_manager::instance()->print_database();
  TEST("update tuple/values", true, true);

  ////////////////////////////////////////////////////////////////////////////
  //: example: get sql view
  ////////////////////////////////////////////////////////////////////////////
  brdb_query_sptr q8 = new brdb_query("name_gender", "ID", BEQ, 104);
  brdb_selection_sptr s8;
  brdb_database_manager::instance()->select(q8, s8);
  s8->print();
  brdb_relation_sptr sql_view_r = brdb_database_manager::instance()->get_sqlview(s8);
  sql_view_r->print();
  sql_view_r->clear();
  brdb_database_manager::instance()->print_database();
  TEST("get_sqlview()", true, true);

  ////////////////////////////////////////////////////////////////////////////
  //: example: selection and/or/xor
  ////////////////////////////////////////////////////////////////////////////
  brdb_query_sptr q9 = new brdb_query("age", "ID", LEQ, 125);
  brdb_selection_sptr s9;
  brdb_database_manager::instance()->select(q9, s9);
  vcl_cout << "s9: " << vcl_endl;
  s9->print();

  brdb_query_sptr q10 = new brdb_query("age", "ID", BEQ, 120);
  brdb_selection_sptr s10;
  brdb_database_manager::instance()->select(q10, s10);
  vcl_cout << "s10: " << vcl_endl;
  s10->print();

  brdb_selection_sptr s11;
  brdb_selection_sptr s12;
  brdb_selection_sptr s13;
  brdb_selection_sptr s14;
  s11 = brdb_database_manager::selection_and(s9, s10);
  s12 = brdb_database_manager::selection_or(s9, s10);
  s13 = brdb_database_manager::selection_xor(s9, s10);
  s14 = brdb_database_manager::selection_not(s9);
  s11->print();
  s12->print();
  s13->print();
  s14->print();

  s11->clear();
  s12->clear();
  s13->clear();
  s14->clear();

  s11 = s9 & s10;
  vcl_cout << "s11: " << vcl_endl;
  s11->print();
  s12 = s9 | s10;
  vcl_cout << "s12: " << vcl_endl;
  s12->print();
  s13 = s9 ^ s10;
  vcl_cout << "s13: " << vcl_endl;
  s13->print();
  s14 = ~s9;
  vcl_cout << "s14: " << vcl_endl;
  s14->print();

  TEST("selection_and()", true, true);
  TEST("selection_or()", true, true);
  TEST("selection_xor()", true, true);
  TEST("selection_not()", true, true);


  ////////////////////////////////////////////////////////////////////////////
  //: example: database ordering
  ////////////////////////////////////////////////////////////////////////////
  brdb_database_manager::instance()->order_by_name("age", "ID", false);
  brdb_database_manager::instance()->print_database();
  TEST("order_by_name()", true, true);



  ////////////////////////////////////////////////////////////////////////////
  //: example: query by index
  ////////////////////////////////////////////////////////////////////////////
  brdb_database_manager::instance()->print_database();
  brdb_query_sptr q15 = new brdb_query("age", 0);
  brdb_selection_sptr s15;
  brdb_database_manager::instance()->select(q15, s15);
  s15->print();
  s15->delete_tuples();
  brdb_database_manager::instance()->print_database();
  // notice that s15 still has something because the query was make by the 0th tuple in relation "age", 
  // and "age" still have its 0th tuple after the original 0th one was delete
  s15->print();

  s10->print();
  brdb_query_sptr q16 = new brdb_query(s10, 2);
  brdb_selection_sptr s16;
  brdb_database_manager::instance()->select(q16, s16);
  s16->print();
  s16->delete_tuples();
  // notice that s16 dosenot has anything because the query was make by the 0th tuple in selection s10
  // and s10's tuple is deleted.
  s16->print();

  r1->print();
  brdb_query_sptr q17 = new brdb_query(r1, 4);
  brdb_selection_sptr s17;
  brdb_database_manager::instance()->select(q17, s17);
  s17->print();
  
  TEST("selection by index()", true, true);
  


  SUMMARY();
}
