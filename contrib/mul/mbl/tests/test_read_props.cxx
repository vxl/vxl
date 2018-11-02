// This is mul/mbl/tests/test_read_props.cxx
#include <iostream>
#include <sstream>
#ifdef _MSC_VER
#  include <vcl_msvc_warnings.h>
#endif

#include <mbl/mbl_read_props.h>

#include <testlib/testlib_test.h>


static std::string strip_ws(std::string &s)
{
  std::string out;
  std::string::size_type i=0;
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
  std::cout << "\n************************\n"
           <<   " Testing mbl_read_props\n"
           <<   "************************\n";
  {
    std::cout << "\nCase 1\n";
    std::istringstream ss("{}");
    mbl_read_props_type props = mbl_read_props( ss );
    mbl_read_props_print(std::cout, props);
    TEST("Case 1: props[ \"a\" ] == \"\"", props["a"]=="" && !ss.fail(), true);
  }

#if 0 // This one won't work because the { } should be on their own lines
  {
    std::cout << "\nCase 2\n";
    std::istringstream ss("{ a: a }");
    mbl_read_props_type props = mbl_read_props( ss );
    mbl_read_props_print(std::cout, props);
    TEST("Case 2: props[ \"a\" ] == \"a\"", props["a"]=="a" && !ss.fail(), true);
  }
#endif // 0

  {
    std::cout << "\nCase 3\n";
    std::istringstream ss("{\n  a: a\n }");
    mbl_read_props_type props = mbl_read_props( ss );
    mbl_read_props_print(std::cout, props);
    TEST("Case 3: props[ \"a\" ] == \"a\"", props["a"]=="a" && !ss.fail(), true);
    TEST("Case 3: props[ \"b\" ] == \"\"", props["b"]=="" && !ss.fail(), true);
  }

  {
    std::cout << "\nCase 4\n";
    std::istringstream ss("{\n  // comment\n a: a\n }");
    mbl_read_props_type props = mbl_read_props( ss );
    mbl_read_props_print(std::cout, props);
    TEST("Case 4: props[ \"a\" ] == \"a\"", props["a"]=="a" && !ss.fail(), true);
  }

  {
    std::cout << "\nCase 5\n";
    std::istringstream ss("{\n  // comment\n a: a\n b: b\n }");
    mbl_read_props_type props = mbl_read_props( ss );
    mbl_read_props_print(std::cout, props);
    TEST("Case 5a: props[ \"a\" ] == \"a\"", props["a"]=="a" && !ss.fail(), true);
    TEST("Case 5b: props[ \"b\" ] == \"b\"", props["b"]=="b" && !ss.fail(), true);
  }

  {
    std::cout << "\nCase 6\n";
    std::istringstream ss("{\n  // comment\n a: a\n a: b\n }");
    bool caught_exception = false;
    try
    {
      mbl_read_props_type props = mbl_read_props( ss );
      mbl_read_props_print(std::cout, props);
    }
    catch (...)
    {
      caught_exception = true;
    }
    TEST("Case 6: Caught double props exception", caught_exception, true);
  }

  {
    std::cout << "\nCase 7\n";
    std::istringstream ss("{\n // comment\n a: a\n // comment\n b: b\n }");
    mbl_read_props_type props = mbl_read_props( ss );
    mbl_read_props_print(std::cout, props);
    TEST("Case 7a: props[ \"a\" ] == \"a\"", props["a"]=="a" && !ss.fail(), true);
    TEST("Case 7b: props[ \"b\" ] == \"b\"", props["b"]=="b" && !ss.fail(), true);
  }

  {
    std::cout << "\nCase 8\n";
    std::istringstream ss("{\n  a: a\n  b: b\n  {\n    ba: ba\n  }\n}");
    std::string b_ans( "b\n{\n    ba: ba\n  }" );

//    std::cout << ss.str() << std::endl;
//    std::cout << b_ans << std::endl;

    mbl_read_props_type props = mbl_read_props( ss );

//    std::cout << props[ "b" ] << std::endl;

    mbl_read_props_print(std::cout, props);
    TEST("Case 8a: props[ \"a\" ] == \"a\"", props["a"]=="a" && !ss.fail(), true);
    std::istringstream ssb(props[ "b" ]);
    std::string sb;
    ssb >> sb;
    TEST("Case 8b: props[ \"b\" ] == b.*", sb == "b" && !ss.fail(), true);
    mbl_read_props_type propsb = mbl_read_props( ssb );
    mbl_read_props_print(std::cout, propsb);
    TEST("Case 8c: propsb[ \"ba\" ] == \"ba\"",
         propsb[ "ba" ] == "ba" && !ssb.fail(), true);
  }

  {
    std::cout << "\nCase 8.5\n";
    std::istringstream ss("{\n  a: a\n  b: b\n  {\n    ba: ba\n    bb: bb\n  }\n}");
    std::string b_ans( "b\n{\n    ba: ba\n    bb: bb\n  }" );

    mbl_read_props_type props = mbl_read_props( ss );

    mbl_read_props_print(std::cout, props);
    TEST("Case 8.5a: props[ \"a\" ] == \"a\"", props[ "a" ] == "a" && !ss.fail(), true);
    TEST("Case 8.5b: props[ \"b\" ] == b_ans",
         strip_ws(props[ "b" ]) == strip_ws(b_ans) && !ss.fail(), true);
  }

  {
    std::cout << "\nCase 8.6\n";
    std::istringstream ss("{\n  a: a\n  b: b { ba: ba bb: bb }\n}");
    std::string b_ans( "b { ba: ba bb: bb }" );

    mbl_read_props_type props = mbl_read_props( ss );

    mbl_read_props_print(std::cout, props);
    TEST("Case 8.6a: props[ \"a\" ] == \"a\"", props["a"]=="a" && !ss.fail(), true);
    TEST("Case 8.6b: props[ \"b\" ] == b_ans",
         strip_ws(props[ "b" ]) == strip_ws(b_ans) && !ss.fail(), true);
  }

  {
    std::cout << "\nCase 9\n";
    std::istringstream ss("{\n  a: a\n  b: b\n  {\n    ba: ba\n  }\n  c: c\n}");
    std::string b_ans( "b\n{\n    ba: ba\n  }" );

    mbl_read_props_type props = mbl_read_props( ss );

    mbl_read_props_print(std::cout, props);
    TEST("Case 9a: props[ \"a\" ] == \"a\"", props["a"]=="a" && !ss.fail(), true);
    TEST("Case 9b: props[ \"b\" ] == b_ans",
         strip_ws(props[ "b" ]) == strip_ws(b_ans) && !ss.fail(), true);
    TEST("Case 9c: props[ \"c\" ] == \"c\"", props["c"]=="c" && !ss.fail(), true);
  }

  {
    std::cout << "\nCase 10\n";
    std::istringstream ss("{\n  a: a\n  b: b\n  {\n    ba: ba\n  }\n  c: c\n  d: d\n  {\n    da: da\n  }\n}");
    std::string b_ans( "b\n{\n    ba: ba\n  }" );
    std::string d_ans( "d\n{\n    da: da\n  }" );

    mbl_read_props_type props = mbl_read_props( ss );

    mbl_read_props_print(std::cout, props);
    TEST("Case 10a: props[ \"a\" ] == \"a\"", props["a"]=="a" && !ss.fail(), true);
    TEST("Case 10b: props[ \"b\" ] == b_ans",
         strip_ws(props[ "b" ]) == strip_ws(b_ans) && !ss.fail(), true);
    TEST("Case 10c: props[ \"c\" ] == \"c\"", props["c"]=="c" && !ss.fail(), true);
    TEST("Case 10d: props[ \"d\" ] == d_ans",
         strip_ws(props[ "d" ]) == strip_ws(d_ans) && !ss.fail(), true);
  }

  {
    std::cout << "\nCase 11\n";
    std::istringstream ss("{\n  a: a\n  b: b\n  {\n    ba: ba\n    bb: bb\n    {\n      bba: bba\n    }\n  }\n}");
    std::string b_ans( "b\n{\n    ba: ba\n    bb: bb\n    {\n      bba: bba\n    }\n  }" );

//    std::cout << ss.str() << std::endl;
//    std::cout << b_ans << std::endl;

    mbl_read_props_type props = mbl_read_props( ss );

//    std::cout << props[ "b" ] << std::endl;

    mbl_read_props_print(std::cout, props);
    TEST("Case 11a: props[ \"a\" ] == \"a\"", props["a"]=="a" && !ss.fail(), true);
    TEST("Case 11b: props[ \"b\" ] == b_ans",
         strip_ws(props[ "b" ]) == strip_ws(b_ans) && !ss.fail(), true);
  }
  {
    std::cout << "\nCase 12\n";
    std::istringstream ss("{\n  a: a\n  b:\n  {\n    ba: ba\n    bb: bb\n    {\n      bba: bba\n    }\n  }\n}");
    std::string b_ans( "{\n    ba: ba\n    bb: bb\n    {\n      bba: bba\n    }\n  }" );

//    std::cout << ss.str() << std::endl;
//    std::cout << b_ans << std::endl;

    mbl_read_props_type props = mbl_read_props( ss );

//    std::cout << props[ "b" ] << std::endl;

    mbl_read_props_print(std::cout, props);
    TEST("Case 12a: props[ \"a\" ] == \"a\"", props["a"]=="a" && !ss.fail(), true);
    TEST("Case 12b: props[ \"b\" ] == b_ans",
         strip_ws(props[ "b" ]) == strip_ws(b_ans) && !ss.fail(), true);
  }
  {
    std::cout << "\nCase 13: get_required_property()\n";
    std::istringstream ss("{\n  a: a\n  b: b\n\n}");
    mbl_read_props_type props = mbl_read_props( ss );
    mbl_read_props_print(std::cout, props);
    std::string val1 = props.get_required_property("a");
    TEST("Case 13a: present, correct", val1=="a", true);

    bool exc = false;
    try
    {
      std::string val = props.get_required_property("z");
    }
    catch (...)
    {
      exc = true;
    }
    TEST("Case 13b: missing, exception thrown?", exc, true);
  }
  {
    std::cout << "\nCase 14: get_optional_property()\n";
    std::istringstream ss("{\n  a: a\n  b: b\n\n}");
    mbl_read_props_type props = mbl_read_props( ss );
    mbl_read_props_print(std::cout, props);
    std::string val1 = props.get_optional_property("a");
    TEST("Case 14a: present, correct", val1=="a", true);

    std::string val2 = props.get_optional_property("z");
    TEST("Case 14b: missing, return empty string", val2=="", true);
    std::string val3 = props.get_optional_property("z","default");
    TEST("Case 14c: missing, return default string", val3=="default", true);
  }


  std::cout << "\n\n";
}

void test_read_props_ws()
{
  std::cout << "\n***************************\n"
           <<   " Testing mbl_read_props_ws\n"
           <<   "***************************\n";
  {
    std::cout << "\nCase 1\n";
    std::istringstream ss("{}");
    mbl_read_props_type props = mbl_read_props_ws( ss );
    mbl_read_props_print(std::cout, props);
    TEST("Case 1: props[ \"a\" ] == \"\"", props["a"]=="" && !ss.fail(), true);
  }

  {
    std::cout << "\nCase 2\n";
    std::istringstream ss("{ a: a }");
    mbl_read_props_type props = mbl_read_props_ws( ss );
    mbl_read_props_print(std::cout, props);
    TEST("Case 2: props[ \"a\" ] == \"a\"", props["a"]=="a" && !ss.fail(), true);
  }

  {
    std::cout << "\nCase 3\n";
    std::istringstream ss("{\n  a: a\n }");
    mbl_read_props_type props = mbl_read_props_ws( ss );
    mbl_read_props_print(std::cout, props);
    TEST("Case 3: props[ \"a\" ] == \"a\"", props["a"]=="a" && !ss.fail(), true);
  }

  {
    std::cout << "\nCase 4\n";
    std::istringstream ss("{\n  // comment\n a: a\n }");
    mbl_read_props_type props = mbl_read_props_ws( ss );
    mbl_read_props_print(std::cout, props);
    TEST("Case 4: props[ \"a\" ] == \"a\"", props["a"]=="a" && !ss.fail(), true);
  }

  {
    std::cout << "\nCase 5\n";
    std::istringstream ss("{\n  // comment\n a: a\n b: b\n }");
    mbl_read_props_type props = mbl_read_props_ws( ss );
    mbl_read_props_print(std::cout, props);
    TEST("Case 5a: props[ \"a\" ] == \"a\"", props["a"]=="a" && !ss.fail(), true);
    TEST("Case 5b: props[ \"b\" ] == \"b\"", props["b"]=="b" && !ss.fail(), true);
  }

  {
    std::cout << "\nCase 5.5\n";
    std::string test_string("{\n // comment\n a: a    b: b\n}");
    std::istringstream ss(test_string);
    mbl_read_props_type props = mbl_read_props_ws( ss );
    std::cout<<test_string<<std::endl;
    TEST("Case 5a: props[ \"a\" ] == \"a\"", props["a"]=="a" && !ss.fail(), true);
    TEST("Case 5b: props[ \"b\" ] == \"b\"", props["b"]=="b" && !ss.fail(), true);
  }

 {
    std::cout << "\nCase 6\n";
    std::istringstream ss("{\n  // comment\n a: a\n a: b\n }");
    bool caught_exception = false;
    try
    {
      mbl_read_props_type props = mbl_read_props_ws( ss );
      mbl_read_props_print(std::cout, props);
    }
    catch (...)
    {
      caught_exception = true;
    }
    TEST("Case 6: Caught double props exception", caught_exception, true);
  }

  {
    std::cout << "\nCase 7\n";
    std::istringstream ss("{\n // comment\n a: a\n // comment\n b: b\n }");
    mbl_read_props_type props = mbl_read_props_ws( ss );
    mbl_read_props_print(std::cout, props);
    TEST("Case 7a: props[ \"a\" ] == \"a\"", props["a"]=="a" && !ss.fail(), true);
    TEST("Case 7b: props[ \"b\" ] == \"b\"", props["b"]=="b" && !ss.fail(), true);
  }

  {
    std::cout << "\nCase 8\n";
    std::istringstream ss("{\n  a: a\n  b: b\n  {\n    ba: ba\n  }\n}");
    std::string b_ans( "b\n{\n    ba: ba\n  }" );

//    std::cout << ss.str() << std::endl;
//    std::cout << b_ans << std::endl;

    mbl_read_props_type props = mbl_read_props_ws( ss );

//    std::cout << props[ "b" ] << std::endl;

    mbl_read_props_print(std::cout, props);
    TEST("Case 8a: props[ \"a\" ] == \"a\"", props["a"]=="a" && !ss.fail(), true);
    std::istringstream ssb(props[ "b" ]);
    std::string sb;
    ssb >> sb;
    TEST("Case 8b: props[ \"b\" ] == b.*", sb == "b" && !ss.fail(), true);
    mbl_read_props_type propsb = mbl_read_props_ws( ssb );
    mbl_read_props_print(std::cout, propsb);
    TEST("Case 8c: propsb[ \"ba\" ] == \"ba\"",
         propsb[ "ba" ] == "ba" && !ssb.fail(), true);
  }

  {
    std::cout << "\nCase 8.5\n";
    std::istringstream ss("{\n  a: a\n  b: b\n  {\n    ba: ba\n    bb: bb\n  }\n}");
    std::string b_ans( "b\n{\n    ba: ba\n    bb: bb\n  }" );

    mbl_read_props_type props = mbl_read_props_ws( ss );

    mbl_read_props_print(std::cout, props);
    TEST("Case 8.5a: props[ \"a\" ] == \"a\"", props["a"]=="a" && !ss.fail(), true);
    TEST("Case 8.5b: props[ \"b\" ] == b_ans",
         strip_ws(props[ "b" ]) == strip_ws(b_ans) && !ss.fail(), true);
  }

  {
    std::cout << "\nCase 8.6\n";
    std::istringstream ss("{\n  a: a\n  b: b { ba: ba bb: bb }\n}");
    std::string b_ans( "b { ba: ba bb: bb }" );

    mbl_read_props_type props = mbl_read_props_ws( ss );

    mbl_read_props_print(std::cout, props);
    TEST("Case 8.6a: props[ \"a\" ] == \"a\"", props["a"]=="a" && !ss.fail(), true);
    TEST("Case 8.6b: props[ \"b\" ] == b_ans",
         strip_ws(props[ "b" ]) == strip_ws(b_ans) && !ss.fail(), true);
  }

  {
    std::cout << "\nCase 9\n";
    std::istringstream ss("{\n  a: a\n  b: b\n  {\n    ba: ba\n  }\n  c: c\n}");
    std::string b_ans( "b\n{\n    ba: ba\n  }" );

    mbl_read_props_type props = mbl_read_props_ws( ss );

    mbl_read_props_print(std::cout, props);
    TEST("Case 9a: props[ \"a\" ] == \"a\"", props["a"]=="a" && !ss.fail(), true);
    TEST("Case 9b: props[ \"b\" ] == b_ans",
         strip_ws(props[ "b" ]) == strip_ws(b_ans) && !ss.fail(), true);
    TEST("Case 9c: props[ \"c\" ] == \"c\"", props["c"]=="c" && !ss.fail(), true);
  }

  {
    std::cout << "\nCase 10\n";
    std::istringstream ss("{\n  a: a\n  b: b\n  {\n    ba: ba\n  }\n  c: c\n  d: d\n  {\n    da: da\n  }\n}");
    std::string b_ans( "b\n{\n    ba: ba\n  }" );
    std::string d_ans( "d\n{\n    da: da\n  }" );

    mbl_read_props_type props = mbl_read_props_ws( ss );

    mbl_read_props_print(std::cout, props);
    TEST("Case 10a: props[ \"a\" ] == \"a\"", props["a"]=="a" && !ss.fail(), true);
    TEST("Case 10b: props[ \"b\" ] == b_ans",
         strip_ws(props[ "b" ]) == strip_ws(b_ans) && !ss.fail(), true);
    TEST("Case 10c: props[ \"c\" ] == \"c\"", props["c"]=="c" && !ss.fail(), true);
    TEST("Case 10d: props[ \"d\" ] == d_ans",
         strip_ws(props[ "d" ]) == strip_ws(d_ans) && !ss.fail(), true);
  }

  {
    std::cout << "\nCase 11\n";
    std::istringstream ss("{\n  a: a\n  b: b\n  {\n    ba: ba\n    bb: bb\n    {\n      bba: bba\n    }\n  }\n}");
    std::string b_ans( "b\n{\n    ba: ba\n    bb: bb\n    {\n      bba: bba\n    }\n  }" );

//    std::cout << ss.str() << std::endl;
//    std::cout << b_ans << std::endl;

    mbl_read_props_type props = mbl_read_props_ws( ss );

//    std::cout << props[ "b" ] << std::endl;

    mbl_read_props_print(std::cout, props);
    TEST("Case 11a: props[ \"a\" ] == \"a\"", props["a"]=="a" && !ss.fail(), true);
    TEST("Case 11b: props[ \"b\" ] == b_ans",
         strip_ws(props[ "b" ]) == strip_ws(b_ans) && !ss.fail(), true);
  }
#if 0
  {
    std::cout << "\nCase 12\n";
    std::istringstream ss("{\n  a: a\n  b:\n  {\n    ba: ba\n    bb: bb\n    {\n      bba: bba\n    }\n  }\n}");
    std::string b_ans( "{\n    ba: ba\n    bb: bb\n    {\n      bba: bba\n    }\n  }" );

//    std::cout << ss.str() << std::endl;
//    std::cout << b_ans << std::endl;

    mbl_read_props_type props = mbl_read_props_ws( ss );

//    std::cout << props[ "b" ] << std::endl;

    mbl_read_props_print(std::cout, props);
    TEST("Case 12a: props[ \"a\" ] == \"a\"", props["a"]=="a" && !ss.fail(), true);
    TEST("Case 12b: props[ \"b\" ] == b_ans",
         strip_ws(props[ "b" ]) == strip_ws(b_ans) && !ss.fail(), true);
  }
#endif // 0
  {
    std::cout << "\nCase 13\n";
    std::istringstream ss
      ("{\n a1: gd { nx: 8 ny: 8 nz: 8 mr: 25 md: 5 }\n  a2: tn\n a3: al {  }\n a4: 2\n a5: 2\n om: sx\n ml: 0.1\n ns: 1\n  }");
    //  "\n a4: { lo: 2 hi: 3 }"
    std::string b_ans( "gd { nx: 8 ny: 8 nz: 8 mr: 25 md: 5 }" );

    mbl_read_props_type props = mbl_read_props_ws( ss );

//    std::cout << props[ "b" ] << std::endl;

    mbl_read_props_type::const_iterator iter=props.begin();
    mbl_read_props_type::const_iterator iter_end=props.end();
    while (iter!=iter_end)
    {
      std::cout << iter->first << ',' << iter->second << std::endl;
      ++iter;
    }

    //mbl_read_props_print(std::cout, props);
    TEST("Case 13a: props[ \"a2\" ] == \"tn\"",
         props[ "a2" ] == "tn" && !ss.fail(), true);
    TEST("Case 13b: props[ \"a1\" ] == b_ans",
         strip_ws(props[ "a1" ]) == strip_ws(b_ans) && !ss.fail(), true);
  }

  std::cout << "\n\n";
}

void test_read_props()
{
  test_read_props1();
  test_read_props_ws();
}

TESTMAIN(test_read_props);
