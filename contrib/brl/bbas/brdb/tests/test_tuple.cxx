#include <testlib/testlib_test.h>
#include <brdb/brdb_tuple.h>
#include <brdb/brdb_tuple_sptr.h>
#include <brdb/brdb_value.h>
#include <vcl_iostream.h>


MAIN( test_tuple )
{
  START ("DB Tuple");

  brdb_tuple_sptr tup0 = new brdb_tuple();
  brdb_tuple_sptr tup1 = new brdb_tuple(10);
  brdb_tuple_sptr tup2 = new brdb_tuple(10, 11.4f);
  brdb_tuple_sptr tup3 = new brdb_tuple(-12.43f, vcl_string("this is a string"), -1 );
  brdb_tuple_sptr tup4 = new brdb_tuple(-12.43f, 200, vcl_string("some text"), -1 );

  // pass if it made it this far
  TEST("Constructors", true, true);

  TEST("arity()",tup0->arity()==0 && tup1->arity()==1 && tup2->arity()==2 &&
                 tup3->arity()==3 && tup4->arity()==4, true);

  brdb_tuple tup_cp;
  tup_cp = (*tup3);

  TEST("operator =", tup_cp.arity(), tup3->arity());

  int int_val;
  TEST("get() right type", tup1->get(0,int_val) && int_val==10, true);
  TEST("get() bad bounds", tup2->get(2,int_val), false);

  TEST("set() right type", tup1->set(0, 9) && tup1->get(0,int_val) && int_val==9, true);
  TEST("set() bad bounds", tup2->set(2, 9), false);

  brdb_value_t<vcl_string> new_value_string("hello");
  vcl_string new_string("hello");
  vcl_string get_string1;
  vcl_string get_string2;
  tup1->add_value(new_value_string);
  tup1->get(tup1->arity()-1, get_string1);
  tup2->add(new_string);
  tup2->get(tup2->arity()-1, get_string2);
  TEST("add_value()", (get_string1 == "hello"), true);
  TEST("add()", (get_string2 == "hello"), true);

  tup1->print();
  tup2->print();
  TEST("print()", true, true);


  //////////////////////////////////////////////////////////////////
  //// test prototyping for binary I/O
  //////////////////////////////////////////////////////////////////
  brdb_tuple_sptr out_tup = new brdb_tuple(12, -12.43f, 34.56, static_cast<long>(987654321), vcl_string("this is a string"), false);
  vcl_vector<vcl_string> types;
  vcl_cout << "test tuple types\n";
  for(unsigned int i=0; i<out_tup->arity(); ++i){
    types.push_back((*out_tup)[i].is_a());
    vcl_cout << "  " << types.back() << '\n';
  }
  vcl_cout << vcl_flush;

  brdb_tuple_sptr in_tup = brdb_tuple::make_prototype(types);

  bool type_check = (out_tup->arity() == in_tup->arity());
  for(unsigned int i=0; type_check && i<out_tup->arity(); ++i){
    type_check = (types[i] == (*in_tup)[i].is_a());
  }
  TEST("make_prototype()", type_check, true);
  if(!type_check){
    vcl_cout << "mismatched prototype tuple types\n";
    for(unsigned int i=0; type_check && i<out_tup->arity(); ++i){
      vcl_cout << "  " << (*in_tup)[i].is_a() << '\n';
    }
    vcl_cout << vcl_flush;
  }

  //////////////////////////////////////////////////////////////////
  //// test binary io on regular data types
  //////////////////////////////////////////////////////////////////

  vcl_cout << "out_tup: "; 
  out_tup->print();

  vcl_cout << "in_tup before b_read: ";
  in_tup->print();

  vsl_b_ofstream out_stream("test_tuple_bio.vsl");
  if (!out_stream){
    vcl_cerr<<"Failed to open " << "test_tuple_bio.vsl" << " for output." << vcl_endl;
  }
  vcl_cout << "Opened file successfully " << vcl_endl;
  
  out_tup->b_write_values(out_stream);
  out_stream.close();


  vsl_b_ifstream in_stream("test_tuple_bio.vsl");
  if (!out_stream){
    vcl_cerr<<"Failed to open " << "test_tuple_bio.vsl" << " for input." << vcl_endl;
  }
  vcl_cout << "Opened file successfully " << vcl_endl;
  in_tup->b_read_values(in_stream);
  in_stream.close();

  vcl_cout << "in_tup after b_read: ";
  in_tup->print();

  bool val_check = true;
  for(unsigned int i=0; val_check && i<in_tup->arity(); ++i){
    type_check = ((*out_tup)[i] == (*in_tup)[i]);
  }

  TEST("binary io: b_read, b_write", type_check, true);


  SUMMARY();
}
