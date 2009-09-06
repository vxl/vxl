#include <vcl_iostream.h>
#include <vcl_iomanip.h>
#include <vcl_limits.h> // for infinity()
#include <vil/vil_round.h>
#include <testlib/testlib_test.h>

void test_round()
{
  testlib_test_assert("rnd(-8.4999)  == -8  ", vil_round_rnd(-8.4999)  == -8);
  testlib_test_assert("rnd(-8.4999f) == -8  ", vil_round_rnd(-8.4999f) == -8);
  testlib_test_assert("rnd(-8.50)    == -8/9", vil_round_rnd(-8.50)/2  == -4);
  testlib_test_assert("rnd(-8.50f)   == -8/9", vil_round_rnd(-8.50f)/2 == -4);
  testlib_test_assert("rnd(-8.5001)  == -9  ", vil_round_rnd(-8.5001)  == -9);
  testlib_test_assert("rnd(-8.5001f) == -9  ", vil_round_rnd(-8.5001f) == -9);
  testlib_test_assert("rnd(8.4999)   ==  8  ", vil_round_rnd(8.4999)   ==  8);
  testlib_test_assert("rnd(8.4999f)  ==  8  ", vil_round_rnd(8.4999f)  ==  8);
  testlib_test_assert("rnd(8.50)     ==  8/9", vil_round_rnd(8.50)/2   ==  4);
  testlib_test_assert("rnd(8.50f)    ==  8/9", vil_round_rnd(8.50f)/2  ==  4);
  testlib_test_assert("rnd(8.5001)   ==  9  ", vil_round_rnd(8.5001)   ==  9);
  testlib_test_assert("rnd(8.5001f)  ==  9  ", vil_round_rnd(8.5001f)  ==  9);

  testlib_test_assert("rnd(-9.4999)  == -9   ", vil_round_rnd(-9.4999)      == -9);
  testlib_test_assert("rnd(-9.4999f) == -9   ", vil_round_rnd(-9.4999f)     == -9);
  testlib_test_assert("rnd(-9.50)    == -9/10", (vil_round_rnd(-9.50)+1)/2  == -4);
  testlib_test_assert("rnd(-9.50f)   == -9/10", (vil_round_rnd(-9.50f)+1)/2 == -4);
  testlib_test_assert("rnd(-9.5001)  == -10  ", vil_round_rnd(-9.5001)      == -10);
  testlib_test_assert("rnd(-9.5001f) == -10  ", vil_round_rnd(-9.5001f)     == -10);
  testlib_test_assert("rnd(9.4999)   ==  9   ", vil_round_rnd(9.4999)       ==  9);
  testlib_test_assert("rnd(9.4999f)  ==  9   ", vil_round_rnd(9.4999f)      ==  9);
  testlib_test_assert("rnd(9.50)     ==  9/10", (vil_round_rnd(9.50)-1)/2   ==  4);
  testlib_test_assert("rnd(9.50f)    ==  9/10", (vil_round_rnd(9.50f)-1)/2  ==  4);
  testlib_test_assert("rnd(9.5001)   ==  10  ", vil_round_rnd(9.5001)       ==  10);
  testlib_test_assert("rnd(9.5001f)  ==  10  ", vil_round_rnd(9.5001f)      ==  10);

  testlib_test_assert("rnd_halfinttoeven(-8.4999)  == -8", vil_round_rnd_halfinttoeven(-8.4999) == -8);
  testlib_test_assert("rnd_halfinttoeven(-8.4999f) == -8", vil_round_rnd_halfinttoeven(-8.4999f)== -8);
  testlib_test_assert("rnd_halfinttoeven(-8.50)    == -8", vil_round_rnd_halfinttoeven(-8.50)   == -8);
  testlib_test_assert("rnd_halfinttoeven(-8.50f)   == -8", vil_round_rnd_halfinttoeven(-8.50f)  == -8);
  testlib_test_assert("rnd_halfinttoeven(-8.5001)  == -9", vil_round_rnd_halfinttoeven(-8.5001) == -9);
  testlib_test_assert("rnd_halfinttoeven(-8.5001f) == -9", vil_round_rnd_halfinttoeven(-8.5001f)== -9);
  testlib_test_assert("rnd_halfinttoeven(8.4999)   ==  8", vil_round_rnd_halfinttoeven(8.4999)  ==  8);
  testlib_test_assert("rnd_halfinttoeven(8.4999f)  ==  8", vil_round_rnd_halfinttoeven(8.4999f) ==  8);
  testlib_test_assert("rnd_halfinttoeven(8.50)     ==  8", vil_round_rnd_halfinttoeven(8.50)    ==  8);
  testlib_test_assert("rnd_halfinttoeven(8.50f)    ==  8", vil_round_rnd_halfinttoeven(8.50f)   ==  8);
  testlib_test_assert("rnd_halfinttoeven(8.5001)   ==  9", vil_round_rnd_halfinttoeven(8.5001)  ==  9);
  testlib_test_assert("rnd_halfinttoeven(8.5001f)  ==  9", vil_round_rnd_halfinttoeven(8.5001f) ==  9);

  testlib_test_assert("rnd_halfinttoeven(-9.4999)  == -9 ", vil_round_rnd_halfinttoeven(-9.4999) == -9);
  testlib_test_assert("rnd_halfinttoeven(-9.4999f) == -9 ", vil_round_rnd_halfinttoeven(-9.4999f)== -9);
  testlib_test_assert("rnd_halfinttoeven(-9.50)    == -10", vil_round_rnd_halfinttoeven(-9.50)   == -10);
  testlib_test_assert("rnd_halfinttoeven(-9.50f)   == -10", vil_round_rnd_halfinttoeven(-9.50f)  == -10);
  testlib_test_assert("rnd_halfinttoeven(-9.5001)  == -10", vil_round_rnd_halfinttoeven(-9.5001) == -10);
  testlib_test_assert("rnd_halfinttoeven(-9.5001f) == -10", vil_round_rnd_halfinttoeven(-9.5001f)== -10);
  testlib_test_assert("rnd_halfinttoeven(9.4999)   ==  9 ", vil_round_rnd_halfinttoeven(9.4999)  ==  9);
  testlib_test_assert("rnd_halfinttoeven(9.4999f)  ==  9 ", vil_round_rnd_halfinttoeven(9.4999f) ==  9);
  testlib_test_assert("rnd_halfinttoeven(9.50)     ==  10", vil_round_rnd_halfinttoeven(9.50)    ==  10);
  testlib_test_assert("rnd_halfinttoeven(9.50f)    ==  10", vil_round_rnd_halfinttoeven(9.50f)   ==  10);
  testlib_test_assert("rnd_halfinttoeven(9.5001)   ==  10", vil_round_rnd_halfinttoeven(9.5001)  ==  10);
  testlib_test_assert("rnd_halfinttoeven(9.5001f)  ==  10", vil_round_rnd_halfinttoeven(9.5001f) ==  10);

  testlib_test_assert("rnd_halfintup(-8.4999)  == -8", vil_round_rnd_halfintup(-8.4999) == -8);
  testlib_test_assert("rnd_halfintup(-8.4999f) == -8", vil_round_rnd_halfintup(-8.4999f)== -8);
  testlib_test_assert("rnd_halfintup(-8.50)    == -8", vil_round_rnd_halfintup(-8.50)   == -8);
  testlib_test_assert("rnd_halfintup(-8.50f)   == -8", vil_round_rnd_halfintup(-8.50f)  == -8);
  testlib_test_assert("rnd_halfintup(-8.5001)  == -9", vil_round_rnd_halfintup(-8.5001) == -9);
  testlib_test_assert("rnd_halfintup(-8.5001f) == -9", vil_round_rnd_halfintup(-8.5001f)== -9);
  testlib_test_assert("rnd_halfintup(8.4999)   ==  8", vil_round_rnd_halfintup(8.4999)  ==  8);
  testlib_test_assert("rnd_halfintup(8.4999f)  ==  8", vil_round_rnd_halfintup(8.4999f) ==  8);
  testlib_test_assert("rnd_halfintup(8.50)     ==  9", vil_round_rnd_halfintup(8.50)    ==  9);
  testlib_test_assert("rnd_halfintup(8.50f)    ==  9", vil_round_rnd_halfintup(8.50f)   ==  9);
  testlib_test_assert("rnd_halfintup(8.5001)   ==  9", vil_round_rnd_halfintup(8.5001)  ==  9);
  testlib_test_assert("rnd_halfintup(8.5001f)  ==  9", vil_round_rnd_halfintup(8.5001f) ==  9);

  testlib_test_assert("rnd_halfintup(-9.4999)  == -9 ", vil_round_rnd_halfintup(-9.4999) == -9);
  testlib_test_assert("rnd_halfintup(-9.4999f) == -9 ", vil_round_rnd_halfintup(-9.4999f)== -9);
  testlib_test_assert("rnd_halfintup(-9.50)    == -9 ", vil_round_rnd_halfintup(-9.50)   == -9);
  testlib_test_assert("rnd_halfintup(-9.50f)   == -9 ", vil_round_rnd_halfintup(-9.50f)  == -9);
  testlib_test_assert("rnd_halfintup(-9.5001)  == -10", vil_round_rnd_halfintup(-9.5001) == -10);
  testlib_test_assert("rnd_halfintup(-9.5001f) == -10", vil_round_rnd_halfintup(-9.5001f)== -10);
  testlib_test_assert("rnd_halfintup(9.4999)   ==  9 ", vil_round_rnd_halfintup(9.4999)  ==  9);
  testlib_test_assert("rnd_halfintup(9.4999f)  ==  9 ", vil_round_rnd_halfintup(9.4999f) ==  9);
  testlib_test_assert("rnd_halfintup(9.50)     ==  10", vil_round_rnd_halfintup(9.50)    ==  10);
  testlib_test_assert("rnd_halfintup(9.50f)    ==  10", vil_round_rnd_halfintup(9.50f)   ==  10);
  testlib_test_assert("rnd_halfintup(9.5001)   ==  10", vil_round_rnd_halfintup(9.5001)  ==  10);
  testlib_test_assert("rnd_halfintup(9.5001f)  ==  10", vil_round_rnd_halfintup(9.5001f) ==  10);

  testlib_test_assert("floor(8.0)      ==  8", vil_round_floor(8.0)     ==  8);
  testlib_test_assert("floor(8.0f)     ==  8", vil_round_floor(8.0f)    ==  8);
  testlib_test_assert("floor(8.9999)   ==  8", vil_round_floor(8.9999)  ==  8);
  testlib_test_assert("floor(8.9999f)  ==  8", vil_round_floor(8.9999f) ==  8);
  testlib_test_assert("floor(8.0001)   ==  8", vil_round_floor(8.0001)  ==  8);
  testlib_test_assert("floor(8.0001f)  ==  8", vil_round_floor(8.0001f) ==  8);
  testlib_test_assert("floor(-8.0)     == -8", vil_round_floor(-8.0)    == -8);
  testlib_test_assert("floor(-8.0f)    == -8", vil_round_floor(-8.0f)   == -8);
  testlib_test_assert("floor(-8.9999)  == -9", vil_round_floor(-8.9999) == -9);
  testlib_test_assert("floor(-8.9999f) == -9", vil_round_floor(-8.9999f)== -9);
  testlib_test_assert("floor(-8.0001)  == -9", vil_round_floor(-8.0001) == -9);
  testlib_test_assert("floor(-8.0001f) == -9", vil_round_floor(-8.0001f)== -9);

  testlib_test_assert("floor(9.0)      ==  9 ", vil_round_floor(9.0)     ==  9);
  testlib_test_assert("floor(9.0f)     ==  9 ", vil_round_floor(9.0f)    ==  9);
  testlib_test_assert("floor(9.9999)   ==  9 ", vil_round_floor(9.9999)  ==  9);
  testlib_test_assert("floor(9.9999f)  ==  9 ", vil_round_floor(9.9999f) ==  9);
  testlib_test_assert("floor(9.0001)   ==  9 ", vil_round_floor(9.0001)  ==  9);
  testlib_test_assert("floor(9.0001f)  ==  9 ", vil_round_floor(9.0001f) ==  9);
  testlib_test_assert("floor(-9.0)     == -9 ", vil_round_floor(-9.0)    == -9);
  testlib_test_assert("floor(-9.0f)    == -9 ", vil_round_floor(-9.0f)   == -9);
  testlib_test_assert("floor(-9.9999)  == -10", vil_round_floor(-9.9999) == -10);
  testlib_test_assert("floor(-9.9999f) == -10", vil_round_floor(-9.9999f)== -10);
  testlib_test_assert("floor(-9.0001)  == -10", vil_round_floor(-9.0001) == -10);
  testlib_test_assert("floor(-9.0001f) == -10", vil_round_floor(-9.0001f)== -10);

  testlib_test_assert("ceil(8.0)      ==  8", vil_round_ceil(8.0)     ==  8);
  testlib_test_assert("ceil(8.0f)     ==  8", vil_round_ceil(8.0f)    ==  8);
  testlib_test_assert("ceil(8.9999)   ==  9", vil_round_ceil(8.9999)  ==  9);
  testlib_test_assert("ceil(8.9999f)  ==  9", vil_round_ceil(8.9999f) ==  9);
  testlib_test_assert("ceil(8.0001)   ==  9", vil_round_ceil(8.0001)  ==  9);
  testlib_test_assert("ceil(8.0001f)  ==  9", vil_round_ceil(8.0001f) ==  9);
  testlib_test_assert("ceil(-8.0)     == -8", vil_round_ceil(-8.0)    == -8);
  testlib_test_assert("ceil(-8.0f)    == -8", vil_round_ceil(-8.0f)   == -8);
  testlib_test_assert("ceil(-8.9999)  == -8", vil_round_ceil(-8.9999) == -8);
  testlib_test_assert("ceil(-8.9999f) == -8", vil_round_ceil(-8.9999f)== -8);
  testlib_test_assert("ceil(-8.0001)  == -8", vil_round_ceil(-8.0001) == -8);
  testlib_test_assert("ceil(-8.0001f) == -8", vil_round_ceil(-8.0001f)== -8);

  testlib_test_assert("ceil(9.0)      ==  9", vil_round_ceil(9.0)     ==  9);
  testlib_test_assert("ceil(9.0f)     ==  9", vil_round_ceil(9.0f)    ==  9);
  testlib_test_assert("ceil(9.9999)   == 10", vil_round_ceil(9.9999)  == 10);
  testlib_test_assert("ceil(9.9999f)  == 10", vil_round_ceil(9.9999f) == 10);
  testlib_test_assert("ceil(9.0001)   == 10", vil_round_ceil(9.0001)  == 10);
  testlib_test_assert("ceil(9.0001f)  == 10", vil_round_ceil(9.0001f) == 10);
  testlib_test_assert("ceil(-9.0)     == -9", vil_round_ceil(-9.0)    == -9);
  testlib_test_assert("ceil(-9.0f)    == -9", vil_round_ceil(-9.0f)   == -9);
  testlib_test_assert("ceil(-9.9999)  == -9", vil_round_ceil(-9.9999) == -9);
  testlib_test_assert("ceil(-9.9999f) == -9", vil_round_ceil(-9.9999f)== -9);
  testlib_test_assert("ceil(-9.0001)  == -9", vil_round_ceil(-9.0001) == -9);
  testlib_test_assert("ceil(-9.0001f) == -9", vil_round_ceil(-9.0001f)== -9);
}

TESTMAIN(test_round);
