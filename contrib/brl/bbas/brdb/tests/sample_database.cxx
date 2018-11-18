// This is brl/bbas/brdb/tests/sample_database.cxx
#include <iostream>
#include <sstream>
#include "sample_database.h"
//:
// \file

#ifdef _MSC_VER
#  include <vcl_msvc_warnings.h>
#endif
#include <brdb/brdb_tuple.h>
#include <brdb/brdb_relation.h>
#include <brdb/brdb_database.h>

//: a function to generate a sample database for testing
brdb_database_sptr generate_sample_database()
{
  std::vector<std::string> r1_names(3);
  std::vector<std::string> r1_types(3);
  std::vector<brdb_tuple_sptr> r1_tuples(20);

  std::vector<std::string> r2_names(2);
  std::vector<std::string> r2_types(2);
  std::vector<brdb_tuple_sptr> r2_tuples(30);

  std::vector<std::string> r3_names(2);
  std::vector<std::string> r3_types(2);
  std::vector<brdb_tuple_sptr> r3_tuples(40);


  r1_names[0] = "ID";
  r1_names[1] = "Name";
  r1_names[2] = "Gender";

  r1_types[0] = brdb_value_t<int>::type();
  r1_types[1] = brdb_value_t<std::string>::type();
  r1_types[2] = brdb_value_t<bool>::type();

  for (int i=0; i<20; i++)
  {
    int ID = 100+i;
    std::stringstream SS;
    SS << "zhao_yong_" << i;
    std::string name;
    SS >> name;
    bool gender;

    if (i%2)
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

  for (int i=0; i<30; i++)
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
  r3_types[1] = brdb_value_t<std::string>::type();

  for (int i=0; i<40; i++)
  {
    int ID = 100+i;
    std::stringstream SS;
    SS << "dempartment_" << i;
    std::string dept;
    SS >> dept;

    r3_tuples[i] = new brdb_tuple(ID, dept);
    r3_tuples[i]->print();
  }

  brdb_relation_sptr r3 = new brdb_relation(r3_names, r3_tuples, r3_types);

  std::vector<brdb_relation_sptr> test_relations;
  test_relations.push_back(r1);
  test_relations.push_back(r2);
  test_relations.push_back(r3);

  std::vector<std::string> relation_names;
  relation_names.emplace_back("name_gender");
  relation_names.emplace_back("age");
  relation_names.emplace_back("department");

  return new brdb_database(test_relations, relation_names);
}
