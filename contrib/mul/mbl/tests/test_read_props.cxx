// This is mul/mbl/tests/test_read_props.cxx
#include <vcl_iostream.h>
#include <vcl_sstream.h>

#include <mbl/mbl_read_props.h>
#include <testlib/testlib_test.h>

void test_read_props()
{
  vcl_cout << "\n************************\n"
           <<   " Testing mbl_read_props\n"
           <<   "************************\n";

  {
    vcl_istringstream ss("{}");
    mbl_read_props_type props = mbl_read_props( ss );
    TEST("Case 1", props[ "a" ] == "" && !ss.fail(), true);
  }

#if 0 // This one won't work because the { } should be on their own lines
  {
    vcl_istringstream ss("{ a: a }");
    mbl_read_props_type props = mbl_read_props( ss );
    TEST("Case 2", props[ "a" ] == "a" && !ss.fail(), true);
  }
#endif // 0

  {
    vcl_istringstream ss("{\n  a: a\n }");
    mbl_read_props_type props = mbl_read_props( ss );
    TEST("Case 3", props[ "a" ] == "a" && !ss.fail(), true);
  }

  {
    vcl_istringstream ss("{\n  //comment\n a: a\n }");
    mbl_read_props_type props = mbl_read_props( ss );
    TEST("Case 4", props[ "a" ] == "a" && !ss.fail(), true);
  }

  {
    vcl_istringstream ss("{\n  //comment\n a: a\n b: b\n }");
    mbl_read_props_type props = mbl_read_props( ss );
    TEST("Case 5a", props[ "a" ] == "a" && !ss.fail(), true);
    TEST("Case 5b", props[ "b" ] == "b" && !ss.fail(), true);
  }

  {
    vcl_istringstream ss("{\n  //comment\n a: a\n a: b\n }");
    mbl_read_props_type props = mbl_read_props( ss );
    TEST("Case 6", props[ "a" ] == "b" && !ss.fail(), true);
  }

  {
    vcl_istringstream ss("{\n //comment\n a: a\n //comment\n b: b\n }");
    mbl_read_props_type props = mbl_read_props( ss );
    TEST("Case 7a", props[ "a" ] == "a" && !ss.fail(), true);
    TEST("Case 7b", props[ "b" ] == "b" && !ss.fail(), true);
  }

  {
    vcl_istringstream ss("{\n  a: a\n  b: b\n  {\n    ba: ba\n  }\n}");
    vcl_string b_ans( "b\n{\n    ba: ba\n  }" );

//    vcl_cout << ss.str() << vcl_endl;
//    vcl_cout << b_ans << vcl_endl;

    mbl_read_props_type props = mbl_read_props( ss );

//    vcl_cout << props[ "b" ] << vcl_endl;

    TEST("Case 7a", props[ "a" ] == "a" && !ss.fail(), true);
    TEST("Case 7b", props[ "b" ] == b_ans && !ss.fail(), true);
  }

  {
    vcl_istringstream ss("{\n  a: a\n  b: b\n  {\n    ba: ba\n    bb: bb\n  }\n}");
    vcl_string b_ans( "b\n{\n    ba: ba\n    bb: bb\n  }" );

    mbl_read_props_type props = mbl_read_props( ss );

    TEST("Case 7.5a", props[ "a" ] == "a" && !ss.fail(), true);
    TEST("Case 7.5b", props[ "b" ] == b_ans && !ss.fail(), true);
  }

  {
    vcl_istringstream ss("{\n  a: a\n  b: b\n  {\n    ba: ba\n  }\n  c: c\n}");
    vcl_string b_ans( "b\n{\n    ba: ba\n  }" );

    mbl_read_props_type props = mbl_read_props( ss );

    TEST("Case 8a", props[ "a" ] == "a" && !ss.fail(), true);
    TEST("Case 8b", props[ "b" ] == b_ans && !ss.fail(), true);
    TEST("Case 8c", props[ "c" ] == "c", true);
  }

  {
    vcl_istringstream ss("{\n  a: a\n  b: b\n  {\n    ba: ba\n  }\n  c: c\n  d: d\n  {\n    da: da\n  }\n}");
    vcl_string b_ans( "b\n{\n    ba: ba\n  }" );
    vcl_string d_ans( "d\n{\n    da: da\n  }" );

    mbl_read_props_type props = mbl_read_props( ss );

    TEST("Case 10a", props[ "a" ] == "a" && !ss.fail(), true);
    TEST("Case 10b", props[ "b" ] == b_ans && !ss.fail(), true);
    TEST("Case 10c", props[ "c" ] == "c", true);
    TEST("Case 10d", props[ "d" ] == d_ans && !ss.fail(), true);
  }

  {
    vcl_istringstream ss("{\n  a: a\n  b: b\n  {\n    ba: ba\n    bb: bb\n    {\n      bba: bba\n    }\n  }\n}");
    vcl_string b_ans( "b\n{\n    ba: ba\n    bb: bb\n    {\n      bba: bba\n    }\n  }" );

//    vcl_cout << ss.str() << vcl_endl;
//    vcl_cout << b_ans << vcl_endl;

    mbl_read_props_type props = mbl_read_props( ss );

//    vcl_cout << props[ "b" ] << vcl_endl;

    TEST("Case 11a", props[ "a" ] == "a" && !ss.fail(), true);
    TEST("Case 11b", props[ "b" ] == b_ans && !ss.fail(), true);
  }
  {
    vcl_istringstream ss("{\n  a: a\n  b: \n  {\n    ba: ba\n    bb: bb\n    {\n      bba: bba\n    }\n  }\n}");
    vcl_string b_ans( "{\n    ba: ba\n    bb: bb\n    {\n      bba: bba\n    }\n  }" );

//    vcl_cout << ss.str() << vcl_endl;
//    vcl_cout << b_ans << vcl_endl;

    mbl_read_props_type props = mbl_read_props( ss );

//    vcl_cout << props[ "b" ] << vcl_endl;

    TEST("Case 12a", props[ "a" ] == "a" && !ss.fail(), true);
    TEST("Case 12b", props[ "b" ] == b_ans && !ss.fail(), true);
  }

  vcl_cout << "\n\n";
}

TESTMAIN(test_read_props);
