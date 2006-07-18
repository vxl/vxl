// This is core/vul/tests/test_regexp.cxx
#include <vul/vul_reg_exp.h>
#include <testlib/testlib_test.h>
//:
// \file
// \brief  Thorough testing of class vul_reg_exp (regular expression matching)
// \author Peter Vanroose, KULeuven, ESAT/PSI
// \date   13 June 2002
// \history Adapted from the original Texas Instruments COOL test suite,
//          but with lots of tests added.

#include <vcl_iostream.h>

void test_regexp()
{
  vcl_cout << "\n\tGENERAL TESTS OF MEMBER FUNCTIONS FOR REGEXP CLASS.\n";

  // Test the vul_reg_exp(char*) constructor [and thus also the compile(char*)
  // member function], and do a very basic test on the find(char*) function.

  vul_reg_exp rd1("Test string\tto match");
  TEST("rd1.is_valid()", rd1.is_valid(), true);
  rd1.set_invalid();
  TEST("rd1.is_valid()", rd1.is_valid(), false);
  vul_reg_exp rrr;
  TEST("rrr.is_valid()", rrr.is_valid(), false);
  rrr.compile("hello");
  TEST("rrr.is_valid()", rrr.is_valid(), true);

  vul_reg_exp r1("Test string\tto match");
  TEST("vul_reg_exp r1(Test string\tto match)", 0, 0);
  TEST("r1.find(\"It is not here\")", r1.find("It is not here"), false);
  const char * strng = "garbage\n49r*%Test string\tto matchAnd more Garbage";
  TEST("r1.find(strng)", r1.find(strng), true);
  TEST("r1.start() == 13", r1.start(), 13);
  TEST("r1.end() == 33", r1.end(), 33);

  // Test the copy constructor, and the == and deep_equal functions.

  vul_reg_exp r2(r1);
  TEST("vul_reg_exp r2(r1);", 0, 0);
  TEST("r1==r2", r1==r2, true);
  TEST("r2==r1", r2==r1, true);
  TEST("r2.deep_equal(r1)", r2.deep_equal(r1), true);
  TEST("r1.deep_equal(r2)", r1.deep_equal(r2), true);
  TEST("r2.find(strng)", r2.find(strng), true);
  TEST("r1==r2", r1, r2);
  TEST("r2==r1", r2, r1);
  TEST("r2.deep_equal(r1)", r2.deep_equal(r1), true);
  TEST("r1.deep_equal(r2)", r1.deep_equal(r2), true);
  TEST("r2.find(\"It's not here\")", r2.find("It's not here"), false);
  TEST("r1==r2", r1, r2);
  TEST("r2==r1", r2, r1);
  TEST("r2.deep_equal(r1)", r2.deep_equal(r1), true);
  TEST("r1.deep_equal(r2)", r1.deep_equal(r2), true);
  TEST("r2.find(\"Another Test string\tto matchstring.\")",
       r2.find("Another Test string\tto matchstring."), true);
  TEST("r1==r2", r1==r2, true);
  TEST("r2==r1", r2==r1, true);
  TEST("r1!=r2", r1!=r2, false);
  TEST("r2!=r1", r2!=r1, false);
  TEST("r2.deep_equal(r1)", r2.deep_equal(r1), false);
  TEST("r1.deep_equal(r2)", r1.deep_equal(r2), false);

  // Tests the vul_reg_exp() constructor and finishes up the testing on
  // the == operator.

  vul_reg_exp r3;
  TEST("vul_reg_exp r3;", 0, 0);
  r3.compile("hELl");
  TEST("r3.compile(\"hELl\")", 0, 0);
  const char * strng2 = "o, hELlo";
  TEST("r3.find(strng2)", r3.find(strng2), true);
  TEST("r1==r3", r1==r3, false);
  TEST("r1.deep_equal(r3)", r1.deep_equal(r3), false);
  TEST("r3.start()", r3.start(), 3);
  TEST("r3.end()", r3.end(), 7);
  vul_reg_exp r4(r3);
  const char * strng3 = "I don't think it's here";
  TEST("r3.find(strng3)", r3.find(strng3), false);
  TEST("r4==r3", r4, r3);
  TEST("r4.deep_equal(r3)", r4.deep_equal(r3), false);

  // THIS COMPLETES THE GENERAL TESTS OF THE MEMBER FUNCTIONS.

  vcl_cout << "\n\tTESTS FOR compile AND find WITH VARIOUS REGULAR EXPRESSIONS.\n";

//:
// A regular expression allows a programmer to specify complex patterns that
// can be searched for and matched against the character string of a std::string
// object.  In its simplest case, a regular expression is a sequence of
// characters with which you can search for exact character matches. That is
// what was done in the tests above.
// However, many times you may not know the exact sequence you want to find,
// or you may only want to find a match at the beginning or end of a string.
// The vul_reg_exp object allows specification of such patterns by utilizing
// the following regular expressions:
//
// meta-characters:
//
// -       ^    Match at beginning of line
// -       $    Match at end of line
// -       .    Match any single character
// -       [ ]  Match any one character inside the brackets
// -       -    Match any character in range on either side of dash
// -       *    Match preceding pattern zero or more times
// -       +    Match preceding pattern one or more times
// -       ?    Match preceding pattern zero or once only
// -       ()   Save a matched expression and use it in a further match.
//
// Below are tests for the regular expressions using the symbols listed
// above.  They provide good examples of regular expression use.
//
// Other good examples of how regular expressions are used, can be
// found in documentation for ed, sed, grep, awk and perl.
//
// The above symbols can be used together to form complex regular expressions
// for very unusual matching.  There are a few examples of this type of
// use in the following tests.

  vcl_cout << "\nTESTS FOR REGULAR EXPRESSIONS WITH ^\n";

  const char * s = "str at front";
  vul_reg_exp rxp("^str");
  TEST("vul_reg_exp rxp(^str)", 0, 0);
  TEST("rxp.find(\"str at front\")", rxp.find(s), true);
  TEST("rxp.start() == 0", rxp.start(), 0);
  TEST("rxp.end() == 3", rxp.end(), 3);
  TEST("rxp.find(\"Str does not match\")", rxp.find("Str does not match"), false);
  TEST("rxp.find(\"not at front: str\")", rxp.find("not at front: str"), false);
  TEST("rxp.find(\"not at front: \\nstr\")", rxp.find("not at front: \nstr"), false);

  vcl_cout << "\nTESTS FOR REGULAR EXPRESSIONS WITH $\n";

  rxp.compile("str$");
  TEST("rxp.compile(\"str$\")", 0, 0);
  TEST("rxp.find(\"str not at end\")", rxp.find("str not at end"), false);
  TEST("rxp.find(\"str\\n not at end\")", rxp.find("str\n not at end"), false);
  TEST("rxp.find(\"not present: Str\")", rxp.find("not present: Str"), false);
  TEST("rxp.find(\"at end str\")", rxp.find("at end str"), true);
  TEST("rxp.start() == 7", rxp.start(), 7);
  TEST("rxp.end() == 10", rxp.end(), 10);

  vcl_cout << "\nTESTS FOR REGULAR EXPRESSIONS WITH .\n";

  rxp.compile("s..t.r");
  TEST("rxp.compile(\"s..t.r\")", 0, 0);
  TEST("rxp.find(\" so three\")", rxp.find(" so three"), true);
  TEST("rxp.find(\"str too close\")", rxp.find("str too close"), false);
  TEST("rxp.find(\"dl 32 s*\\nt0r ugh\")", rxp.find("dl 32 s*\nt0r ugh"), true);
  TEST("rxp.find(\"too far s  t  r\")", rxp.find("too far s  t  r"), false);

  vcl_cout << "\nTESTS FOR REGULAR EXPRESSIONS WITH []\n";

  rxp.compile("[b1-9]");
  TEST("rxp.compile(\"[b1-9]\")", 0, 0);
  TEST("rxp.find(\"6 is the num\")", rxp.find("6 is the num"), true);
  TEST("rxp.start() == 0", rxp.start(), 0);
  TEST("rxp.end() == 1", rxp.end(), 1);
  TEST("rxp.find(\"ugh b is not 1.\")", rxp.find("ugh b is not 1."), true);
  TEST("rxp.start() == 4", rxp.start(), 4);
  TEST("rxp.end() == 5 (not 14!)", rxp.end(), 5);
  TEST("rxp.find(\"this will not match\")", rxp.find("this will not match"), false);
  rxp.compile("[b1-9].*[b1-9]");
  TEST("rxp.compile(\"[b1-9].*[b1-9]\")", 0, 0);
  TEST("rxp.find(\"ugh b is not 1.\")", rxp.find("ugh b is not 1."), true);
  TEST("rxp.start() == 4", rxp.start(), 4);
  TEST("rxp.end() == 14", rxp.end(), 14);

  vcl_cout << "TESTS FOR REGULAR EXPRESSIONS WITH [^]\n";

  rxp.compile("[^ab1-9]");
  TEST("rxp.compile(\"[^ab1-9]\")", 0, 0);
  TEST("rxp.find(\"ab123QQ59ba\")", rxp.find("ab123QQ59ba"), true);
  TEST("rxp.start() == 5", rxp.start(), 5);
  TEST("rxp.end() == 6", rxp.end(), 6);
  TEST("rxp.find(\"ab123456789ba\")", rxp.find("ab123456789ba"), false);
  rxp.compile("[^ab1-9]+");
  TEST("rxp.compile(\"[^ab1-9]+\")", 0, 0);
  TEST("rxp.find(\"ab123QQ59ba\")", rxp.find("ab123QQ59ba"), true);
  TEST("rxp.start() == 5", rxp.start(), 5);
  TEST("rxp.end() == 7", rxp.end(), 7);
  rxp.compile("[^ab1-9]*");
  TEST("rxp.compile(\"[^ab1-9]*\")", 0, 0);
  TEST("rxp.find(\"ab123QQ59ba\")", rxp.find("ab123QQ59ba"), true);
  TEST("rxp.start() == 0 (not 5!)", rxp.start(), 0);
  TEST("rxp.end() == 0", rxp.end(), 0);

  vcl_cout << "\nTESTS FOR REGULAR EXPRESSIONS WITH * + ?\n";

  rxp.compile("a *l");
  TEST("rxp.compile(\"a *l\")", 0, 0);
  s = "rrra  lddd";
  TEST("rxp.find(\"rrra  lddd\")", rxp.find(s), true);
  TEST("rxp.start() == 3", rxp.start(), 3);
  TEST("rxp.end() == 7", rxp.end(), 7);
  TEST("rxp.find(\"hello there\")", rxp.find("hello there"), false);
  TEST("rxp.find(\"hallo there\")", rxp.find("hallo there"), true);

  rxp.compile("a +l");
  TEST("rxp.compile(\"a +l\")", 0, 0);
  TEST("rxp.find(\"rrra  lddd\")", rxp.find(s), true);
  TEST("rxp.start() == 3", rxp.start(), 3);
  TEST("rxp.end() == 7", rxp.end(), 7);
  TEST("rxp.find(\"hello there\")", rxp.find("hello there"), false);
  TEST("rxp.find(\"hallo there\")", rxp.find("hallo there"), false);

  rxp.compile("a ? ? ?l");
  TEST("rxp.compile(\"a ? ? ?l\")", 0, 0);
  TEST("rxp.find(\"rrra  lddd\")", rxp.find(s), true);
  TEST("rxp.start() == 3", rxp.start(), 3);
  TEST("rxp.end() == 7", rxp.end(), 7);
  TEST("rxp.find(\"hello there\")", rxp.find("hello there"), false);
  TEST("rxp.find(\"hallo there\")", rxp.find("hallo there"), true);

  vcl_cout << "\nTESTS FOR REGULAR EXPRESSIONS WITH TEMPORARY STORAGE USING ()\n";

  // finds an expression ending with pb and beginning with whatever
  // the two characters before the first p encountered in the line were.

  rxp.compile("(..p)b");
  TEST("rxp.compile(\"(..p)b\")", 0, 0);
  s = "rep drepa qrepb";
  TEST("rxp.find(\"rep drepa qrepb\")", rxp.find(s), true);
  TEST("rxp.start()", rxp.start(), 11);
  TEST("rxp.end()", rxp.end(), 15);
  TEST("rxp.match(1) == \"rep\"", rxp.match(1), "rep");
  TEST("rxp.find(\"repreprep\")", rxp.find("repreprep"), false);

  // finds an expression ending with p, beginning with d, and having
  // two characters in between that are the same as the two characters
  // before the first p encountered in the line.

  rxp.compile("(..p).*d(..p)");
  TEST("rxp.compile(\"(..p).*d(..p)\")", 0, 0);
  TEST("rxp.find(\"rep drepa qrepb\")", rxp.find(s), true);
  TEST("rxp.match(1) == rxp.match(2)", rxp.match(1), rxp.match(2));
  TEST("rxp.start()", rxp.start(), 0);
  TEST("rxp.end()", rxp.end(), 8);
  TEST("rxp.find(\"rap drepa qrapb\")", rxp.find("rap drepa qrapb"), true);
  TEST("rxp.match(1) != rxp.match(2)", rxp.match(1) == rxp.match(2), false);
  TEST("rxp.find(\"repreprep\")", rxp.find("repreprep"), false);

  // and now for a test on a long-standing bug in match(), when there is an
  // optional group, i.e., "(...)" followed by "?".

  rxp.compile("(..p).*d(..p)?");
  TEST("rxp.compile(\"(..p).*d(..p)?\")", 0, 0);
  TEST("rxp.find(\"rep drepa qrepb\")", rxp.find(s), true);
  TEST("rxp.match(1) == rxp.match(2)", rxp.match(1), rxp.match(2));
  TEST("rxp.find(\"rep dreba qrepb\")", rxp.find("rep dreba qrepb"), true);
  TEST("rxp.match(1) != rxp.match(2)", rxp.match(1) == rxp.match(2), false);
  TEST("rxp.match(2) == \"\"", rxp.match(2), "");
}

//TESTMAIN(test_regexp);
int test_regexp(int, char*[])
{
  testlib_test_start("test_regexp");

  test_regexp();

  return testlib_test_summary();
}
