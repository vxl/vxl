// This is mul/mbl/tests/test_read_props.cxx
#include <vcl_iostream.h>
#include <vcl_sstream.h>

#include <mbl/mbl_read_props.h>
#include <testlib/testlib_test.h>

vcl_string strip_ws(vcl_string &s)
{
  vcl_string out;
  vcl_string::size_type i=0;
  while (i < s.length())
  {
    if (s[i] != ' ' && s[i] != '\n')
      out += s[i];
    i++;
  }
  return out;
}

void test_read_props1()
{
  vcl_cout << "\n************************\n"
           <<   " Testing mbl_read_props\n"
           <<   "************************\n";

#if VCL_HAS_WORKING_STRINGSTREAM
  {
    vcl_cout << "\nCase 1\n";
    vcl_istringstream ss("{}");
    mbl_read_props_type props = mbl_read_props( ss );
    mbl_read_props_print(vcl_cout, props);
    TEST("Case 1: props[ \"a\" ] == \"\"",
         props[ "a" ] == "" && !ss.fail(), true);
  }

#if 0 // This one won't work because the { } should be on their own lines
  {
    vcl_cout << "\nCase 2\n";
    vcl_istringstream ss("{ a: a }");
    mbl_read_props_type props = mbl_read_props( ss );
    mbl_read_props_print(vcl_cout, props);
    TEST("Case 2: props[ \"a\" ] == \"a\"",
         props[ "a" ] == "a" && !ss.fail(), true);
  }
#endif // 0

  {
    vcl_cout << "\nCase 3\n";
    vcl_istringstream ss("{\n  a: a\n }");
    mbl_read_props_type props = mbl_read_props( ss );
    mbl_read_props_print(vcl_cout, props);
    TEST("Case 3: props[ \"a\" ] == \"a\"",
         props[ "a" ] == "a" && !ss.fail(), true);
  }

  {
    vcl_cout << "\nCase 4\n";
    vcl_istringstream ss("{\n  // comment\n a: a\n }");
    mbl_read_props_type props = mbl_read_props( ss );
    mbl_read_props_print(vcl_cout, props);
    TEST("Case 4: props[ \"a\" ] == \"a\"",
         props[ "a" ] == "a" && !ss.fail(), true);
  }

  {
    vcl_cout << "\nCase 5\n";
    vcl_istringstream ss("{\n  // comment\n a: a\n b: b\n }");
    mbl_read_props_type props = mbl_read_props( ss );
    mbl_read_props_print(vcl_cout, props);
    TEST("Case 5a: props[ \"a\" ] == \"a\"",
         props[ "a" ] == "a" && !ss.fail(), true);
    TEST("Case 5b: props[ \"b\" ] == \"b\"",
         props[ "b" ] == "b" && !ss.fail(), true);
  }

  {
    vcl_cout << "\nCase 6\n";
    vcl_istringstream ss("{\n  // comment\n a: a\n a: b\n }");
    mbl_read_props_type props = mbl_read_props( ss );
    mbl_read_props_print(vcl_cout, props);
    TEST("Case 6: props[ \"a\" ] == \"b\"",
         props[ "a" ] == "b" && !ss.fail(), true);
  }

  {
    vcl_cout << "\nCase 7\n";
    vcl_istringstream ss("{\n // comment\n a: a\n // comment\n b: b\n }");
    mbl_read_props_type props = mbl_read_props( ss );
    mbl_read_props_print(vcl_cout, props);
    TEST("Case 7a: props[ \"a\" ] == \"a\"",
         props[ "a" ] == "a" && !ss.fail(), true);
    TEST("Case 7b: props[ \"b\" ] == \"b\"",
         props[ "b" ] == "b" && !ss.fail(), true);
  }

  {
    vcl_cout << "\nCase 8\n";
    vcl_istringstream ss("{\n  a: a\n  b: b\n  {\n    ba: ba\n  }\n}");
    vcl_string b_ans( "b\n{\n    ba: ba\n  }" );

//    vcl_cout << ss.str() << vcl_endl;
//    vcl_cout << b_ans << vcl_endl;

    mbl_read_props_type props = mbl_read_props( ss );

//    vcl_cout << props[ "b" ] << vcl_endl;

    mbl_read_props_print(vcl_cout, props);
    TEST("Case 8a: props[ \"a\" ] == \"a\"",
         props[ "a" ] == "a" && !ss.fail(), true);
    vcl_istringstream ssb(props[ "b" ]);
    vcl_string sb;
    ssb >> sb;
    TEST("Case 8b: props[ \"b\" ] == b.*", sb == "b" && !ss.fail(), true);
    mbl_read_props_type propsb = mbl_read_props( ssb );
    mbl_read_props_print(vcl_cout, propsb);
    TEST("Case 8c: propsb[ \"ba\" ] == \"ba\"",
         propsb[ "ba" ] == "ba" && !ssb.fail(), true);
  }

  {
    vcl_cout << "\nCase 8.5\n";
    vcl_istringstream ss("{\n  a: a\n  b: b\n  {\n    ba: ba\n    bb: bb\n  }\n}");
    vcl_string b_ans( "b\n{\n    ba: ba\n    bb: bb\n  }" );

    mbl_read_props_type props = mbl_read_props( ss );

    mbl_read_props_print(vcl_cout, props);
    TEST("Case 8.5a: props[ \"a\" ] == \"a\"", props[ "a" ] == "a" && !ss.fail(), true);
    TEST("Case 8.5b: props[ \"b\" ] == b_ans",
         strip_ws(props[ "b" ]) == strip_ws(b_ans) && !ss.fail(), true);
  }

  {
    vcl_cout << "\nCase 8.6\n";
    vcl_istringstream ss("{\n  a: a\n  b: b { ba: ba bb: bb }\n}");
    vcl_string b_ans( "b { ba: ba bb: bb }" );

    mbl_read_props_type props = mbl_read_props( ss );

    mbl_read_props_print(vcl_cout, props);
    TEST("Case 8.6a: props[ \"a\" ] == \"a\"", props[ "a" ] == "a" && !ss.fail(), true);
    TEST("Case 8.6b: props[ \"b\" ] == b_ans",
         strip_ws(props[ "b" ]) == strip_ws(b_ans) && !ss.fail(), true);
  }

  {
    vcl_cout << "\nCase 9\n";
    vcl_istringstream ss("{\n  a: a\n  b: b\n  {\n    ba: ba\n  }\n  c: c\n}");
    vcl_string b_ans( "b\n{\n    ba: ba\n  }" );

    mbl_read_props_type props = mbl_read_props( ss );

    mbl_read_props_print(vcl_cout, props);
    TEST("Case 9a: props[ \"a\" ] == \"a\"", props[ "a" ] == "a" && !ss.fail(), true);
    TEST("Case 9b: props[ \"b\" ] == b_ans",
         strip_ws(props[ "b" ]) == strip_ws(b_ans) && !ss.fail(), true);
    TEST("Case 9c: props[ \"c\" ] == \"c\"", props[ "c" ] == "c", true);
  }

  {
    vcl_cout << "\nCase 10\n";
    vcl_istringstream ss("{\n  a: a\n  b: b\n  {\n    ba: ba\n  }\n  c: c\n  d: d\n  {\n    da: da\n  }\n}");
    vcl_string b_ans( "b\n{\n    ba: ba\n  }" );
    vcl_string d_ans( "d\n{\n    da: da\n  }" );

    mbl_read_props_type props = mbl_read_props( ss );

    mbl_read_props_print(vcl_cout, props);
    TEST("Case 10a: props[ \"a\" ] == \"a\"", props[ "a" ] == "a" && !ss.fail(), true);
    TEST("Case 10b: props[ \"b\" ] == b_ans",
         strip_ws(props[ "b" ]) == strip_ws(b_ans) && !ss.fail(), true);
    TEST("Case 10c: props[ \"c\" ] == \"c\"", props[ "c" ] == "c", true);
    TEST("Case 10d: props[ \"d\" ] == d_ans",
         strip_ws(props[ "d" ]) == strip_ws(d_ans) && !ss.fail(), true);
  }

  {
    vcl_cout << "\nCase 11\n";
    vcl_istringstream ss("{\n  a: a\n  b: b\n  {\n    ba: ba\n    bb: bb\n    {\n      bba: bba\n    }\n  }\n}");
    vcl_string b_ans( "b\n{\n    ba: ba\n    bb: bb\n    {\n      bba: bba\n    }\n  }" );

//    vcl_cout << ss.str() << vcl_endl;
//    vcl_cout << b_ans << vcl_endl;

    mbl_read_props_type props = mbl_read_props( ss );

//    vcl_cout << props[ "b" ] << vcl_endl;

    mbl_read_props_print(vcl_cout, props);
    TEST("Case 11a: props[ \"a\" ] == \"a\"",
         props[ "a" ] == "a" && !ss.fail(), true);
    TEST("Case 11b: props[ \"b\" ] == b_ans",
         strip_ws(props[ "b" ]) == strip_ws(b_ans) && !ss.fail(), true);
  }
  {
    vcl_cout << "\nCase 12\n";
    vcl_istringstream ss("{\n  a: a\n  b:\n  {\n    ba: ba\n    bb: bb\n    {\n      bba: bba\n    }\n  }\n}");
    vcl_string b_ans( "{\n    ba: ba\n    bb: bb\n    {\n      bba: bba\n    }\n  }" );

//    vcl_cout << ss.str() << vcl_endl;
//    vcl_cout << b_ans << vcl_endl;

    mbl_read_props_type props = mbl_read_props( ss );

//    vcl_cout << props[ "b" ] << vcl_endl;

    mbl_read_props_print(vcl_cout, props);
    TEST("Case 12a: props[ \"a\" ] == \"a\"",
         props[ "a" ] == "a" && !ss.fail(), true);
    TEST("Case 12b: props[ \"b\" ] == b_ans",
         strip_ws(props[ "b" ]) == strip_ws(b_ans) && !ss.fail(), true);
  }

  vcl_cout << "\n\n";
#else // VCL_HAS_WORKING_STRINGSTREAM
  vcl_cout << "\nTests not run since this compiler has no fully functional std:stringstream\n\n";
#endif // VCL_HAS_WORKING_STRINGSTREAM
}

void test_read_props_ws()
{
  vcl_cout << "\n************************\n"
           <<   " Testing mbl_read_props_ws\n"
           <<   "************************\n";

#if VCL_HAS_WORKING_STRINGSTREAM
  {
    vcl_cout << "\nCase 1\n";
    vcl_istringstream ss("{}");
    mbl_read_props_type props = mbl_read_props_ws( ss );
    mbl_read_props_print(vcl_cout, props);
    TEST("Case 1: props[ \"a\" ] == \"\"",
         props[ "a" ] == "" && !ss.fail(), true);
  }

  {
    vcl_cout << "\nCase 2\n";
    vcl_istringstream ss("{ a: a }");
    mbl_read_props_type props = mbl_read_props_ws( ss );
    mbl_read_props_print(vcl_cout, props);
    TEST("Case 2: props[ \"a\" ] == \"a\"",
         props[ "a" ] == "a" && !ss.fail(), true);
  }

  {
    vcl_cout << "\nCase 3\n";
    vcl_istringstream ss("{\n  a: a\n }");
    mbl_read_props_type props = mbl_read_props_ws( ss );
    mbl_read_props_print(vcl_cout, props);
    TEST("Case 3: props[ \"a\" ] == \"a\"",
         props[ "a" ] == "a" && !ss.fail(), true);
  }

  {
    vcl_cout << "\nCase 4\n";
    vcl_istringstream ss("{\n  // comment\n a: a\n }");
    mbl_read_props_type props = mbl_read_props_ws( ss );
    mbl_read_props_print(vcl_cout, props);
    TEST("Case 4: props[ \"a\" ] == \"a\"",
         props[ "a" ] == "a" && !ss.fail(), true);
  }

  {
    vcl_cout << "\nCase 5\n";
    vcl_istringstream ss("{\n  // comment\n a: a\n b: b\n }");
    mbl_read_props_type props = mbl_read_props_ws( ss );
    mbl_read_props_print(vcl_cout, props);
    TEST("Case 5a: props[ \"a\" ] == \"a\"",
         props[ "a" ] == "a" && !ss.fail(), true);
    TEST("Case 5b: props[ \"b\" ] == \"b\"",
         props[ "b" ] == "b" && !ss.fail(), true);
  }

  {
    vcl_cout << "\nCase 5.5\n";
    vcl_string test_string("{\n // comment\n a: a    b: b\n}");
    vcl_istringstream ss(test_string);
    mbl_read_props_type props = mbl_read_props_ws( ss );
    vcl_cout<<test_string<<vcl_endl;
    TEST("Case 5a: props[ \"a\" ] == \"a\"",
         props[ "a" ] == "a" && !ss.fail(), true);
    TEST("Case 5b: props[ \"b\" ] == \"b\"",
         props[ "b" ] == "b" && !ss.fail(), true);
  }

  {
    vcl_cout << "\nCase 6\n";
    vcl_istringstream ss("{\n  // comment\n a: a\n a: b\n }");
    mbl_read_props_type props = mbl_read_props_ws( ss );
    mbl_read_props_print(vcl_cout, props);
    TEST("Case 6: props[ \"a\" ] == \"b\"",
         props[ "a" ] == "b" && !ss.fail(), true);
  }

  {
    vcl_cout << "\nCase 7\n";
    vcl_istringstream ss("{\n // comment\n a: a\n // comment\n b: b\n }");
    mbl_read_props_type props = mbl_read_props_ws( ss );
    mbl_read_props_print(vcl_cout, props);
    TEST("Case 7a: props[ \"a\" ] == \"a\"",
         props[ "a" ] == "a" && !ss.fail(), true);
    TEST("Case 7b: props[ \"b\" ] == \"b\"",
         props[ "b" ] == "b" && !ss.fail(), true);
  }

  {
    vcl_cout << "\nCase 8\n";
    vcl_istringstream ss("{\n  a: a\n  b: b\n  {\n    ba: ba\n  }\n}");
    vcl_string b_ans( "b\n{\n    ba: ba\n  }" );

//    vcl_cout << ss.str() << vcl_endl;
//    vcl_cout << b_ans << vcl_endl;

    mbl_read_props_type props = mbl_read_props_ws( ss );

//    vcl_cout << props[ "b" ] << vcl_endl;

    mbl_read_props_print(vcl_cout, props);
    TEST("Case 8a: props[ \"a\" ] == \"a\"",
         props[ "a" ] == "a" && !ss.fail(), true);
    vcl_istringstream ssb(props[ "b" ]);
    vcl_string sb;
    ssb >> sb;
    TEST("Case 8b: props[ \"b\" ] == b.*", sb == "b" && !ss.fail(), true);
    mbl_read_props_type propsb = mbl_read_props_ws( ssb );
    mbl_read_props_print(vcl_cout, propsb);
    TEST("Case 8c: propsb[ \"ba\" ] == \"ba\"",
         propsb[ "ba" ] == "ba" && !ssb.fail(), true);
  }

  {
    vcl_cout << "\nCase 8.5\n";
    vcl_istringstream ss("{\n  a: a\n  b: b\n  {\n    ba: ba\n    bb: bb\n  }\n}");
    vcl_string b_ans( "b\n{\n    ba: ba\n    bb: bb\n  }" );

    mbl_read_props_type props = mbl_read_props_ws( ss );

    mbl_read_props_print(vcl_cout, props);
    TEST("Case 8.5a: props[ \"a\" ] == \"a\"", props[ "a" ] == "a" && !ss.fail(), true);
    TEST("Case 8.5b: props[ \"b\" ] == b_ans",
         strip_ws(props[ "b" ]) == strip_ws(b_ans) && !ss.fail(), true);
  }

  {
    vcl_cout << "\nCase 8.6\n";
    vcl_istringstream ss("{\n  a: a\n  b: b { ba: ba bb: bb }\n}");
    vcl_string b_ans( "b { ba: ba bb: bb }" );

    mbl_read_props_type props = mbl_read_props_ws( ss );

    mbl_read_props_print(vcl_cout, props);
    TEST("Case 8.6a: props[ \"a\" ] == \"a\"", props[ "a" ] == "a" && !ss.fail(), true);
    TEST("Case 8.6b: props[ \"b\" ] == b_ans",
         strip_ws(props[ "b" ]) == strip_ws(b_ans) && !ss.fail(), true);
  }

  {
    vcl_cout << "\nCase 9\n";
    vcl_istringstream ss("{\n  a: a\n  b: b\n  {\n    ba: ba\n  }\n  c: c\n}");
    vcl_string b_ans( "b\n{\n    ba: ba\n  }" );

    mbl_read_props_type props = mbl_read_props_ws( ss );

    mbl_read_props_print(vcl_cout, props);
    TEST("Case 9a: props[ \"a\" ] == \"a\"", props[ "a" ] == "a" && !ss.fail(), true);
    TEST("Case 9b: props[ \"b\" ] == b_ans",
         strip_ws(props[ "b" ]) == strip_ws(b_ans) && !ss.fail(), true);
    TEST("Case 9c: props[ \"c\" ] == \"c\"", props[ "c" ] == "c", true);
  }

  {
    vcl_cout << "\nCase 10\n";
    vcl_istringstream ss("{\n  a: a\n  b: b\n  {\n    ba: ba\n  }\n  c: c\n  d: d\n  {\n    da: da\n  }\n}");
    vcl_string b_ans( "b\n{\n    ba: ba\n  }" );
    vcl_string d_ans( "d\n{\n    da: da\n  }" );

    mbl_read_props_type props = mbl_read_props_ws( ss );

    mbl_read_props_print(vcl_cout, props);
    TEST("Case 10a: props[ \"a\" ] == \"a\"", props[ "a" ] == "a" && !ss.fail(), true);
    TEST("Case 10b: props[ \"b\" ] == b_ans",
         strip_ws(props[ "b" ]) == strip_ws(b_ans) && !ss.fail(), true);
    TEST("Case 10c: props[ \"c\" ] == \"c\"", props[ "c" ] == "c", true);
    TEST("Case 10d: props[ \"d\" ] == d_ans",
         strip_ws(props[ "d" ]) == strip_ws(d_ans) && !ss.fail(), true);
  }

  {
    vcl_cout << "\nCase 11\n";
    vcl_istringstream ss("{\n  a: a\n  b: b\n  {\n    ba: ba\n    bb: bb\n    {\n      bba: bba\n    }\n  }\n}");
    vcl_string b_ans( "b\n{\n    ba: ba\n    bb: bb\n    {\n      bba: bba\n    }\n  }" );

//    vcl_cout << ss.str() << vcl_endl;
//    vcl_cout << b_ans << vcl_endl;

    mbl_read_props_type props = mbl_read_props_ws( ss );

//    vcl_cout << props[ "b" ] << vcl_endl;

    mbl_read_props_print(vcl_cout, props);
    TEST("Case 11a: props[ \"a\" ] == \"a\"",
         props[ "a" ] == "a" && !ss.fail(), true);
    TEST("Case 11b: props[ \"b\" ] == b_ans",
         strip_ws(props[ "b" ]) == strip_ws(b_ans) && !ss.fail(), true);
  }
#if 0
  {
    vcl_cout << "\nCase 12\n";
    vcl_istringstream ss("{\n  a: a\n  b:\n  {\n    ba: ba\n    bb: bb\n    {\n      bba: bba\n    }\n  }\n}");
    vcl_string b_ans( "{\n    ba: ba\n    bb: bb\n    {\n      bba: bba\n    }\n  }" );

//    vcl_cout << ss.str() << vcl_endl;
//    vcl_cout << b_ans << vcl_endl;

    mbl_read_props_type props = mbl_read_props_ws( ss );

//    vcl_cout << props[ "b" ] << vcl_endl;

    mbl_read_props_print(vcl_cout, props);
    TEST("Case 12a: props[ \"a\" ] == \"a\"",
         props[ "a" ] == "a" && !ss.fail(), true);
    TEST("Case 12b: props[ \"b\" ] == b_ans",
         strip_ws(props[ "b" ]) == strip_ws(b_ans) && !ss.fail(), true);
  }
#endif // 0
  {
    vcl_cout << "\nCase 13\n";
    vcl_istringstream ss("{  \n a1: gd { nx: 8 ny: 8 nz: 8 mr: 25 md: 5 } \n  a2: tn \n a3: al {  } \n a4: 2 \n a5: 2  \n om: sx \n ml: 0.1 \n ns: 1 \n  }");
    //   \n a4: { lo: 2 hi: 3 }
    vcl_string b_ans( "gd { nx: 8 ny: 8 nz: 8 mr: 25 md: 5 }" );

    mbl_read_props_type props = mbl_read_props_ws( ss );

//    vcl_cout << props[ "b" ] << vcl_endl;

    mbl_read_props_type::const_iterator iter=props.begin();
    mbl_read_props_type::const_iterator iter_end=props.end();
    while (iter!=iter_end)
    {
      vcl_cout << iter->first << "," << iter->second << vcl_endl;
      ++iter;
    }

    //mbl_read_props_print(vcl_cout, props);
    TEST("Case 13a: props[ \"a2\" ] == \"tn\"",
         props[ "a2" ] == "tn" && !ss.fail(), true);
    TEST("Case 13b: props[ \"a1\" ] == b_ans",
         strip_ws(props[ "a1" ]) == strip_ws(b_ans) && !ss.fail(), true);
  }

  vcl_cout << "\n\n";
#else // VCL_HAS_WORKING_STRINGSTREAM
  vcl_cout << "\nTests not run since this compiler has no fully functional std:stringstream\n\n";
#endif // VCL_HAS_WORKING_STRINGSTREAM
}

void test_read_props()
{
  test_read_props1();
  test_read_props_ws();
}

TESTMAIN(test_read_props);
