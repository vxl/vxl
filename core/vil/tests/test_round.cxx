#include <iostream>
#include <iomanip>
#include <limits>
#ifdef _MSC_VER
#  include <vcl_msvc_warnings.h>
#endif
#include <vil/vil_round.h>
#include <testlib/testlib_test.h>

void test_round()
{
  TEST("rnd(-8.4999)  == -8  ", vil_round_rnd(-8.4999) , -8);
  TEST("rnd(-8.4999f) == -8  ", vil_round_rnd(-8.4999f), -8);
  TEST("rnd(-8.50)    == -8/9", vil_round_rnd(-8.50)/2 , -4);
  TEST("rnd(-8.50f)   == -8/9", vil_round_rnd(-8.50f)/2, -4);
  TEST("rnd(-8.5001)  == -9  ", vil_round_rnd(-8.5001) , -9);
  TEST("rnd(-8.5001f) == -9  ", vil_round_rnd(-8.5001f), -9);
  TEST("rnd(8.4999)   ==  8  ", vil_round_rnd(8.4999)  ,  8);
  TEST("rnd(8.4999f)  ==  8  ", vil_round_rnd(8.4999f) ,  8);
  TEST("rnd(8.50)     ==  8/9", vil_round_rnd(8.50)/2  ,  4);
  TEST("rnd(8.50f)    ==  8/9", vil_round_rnd(8.50f)/2 ,  4);
  TEST("rnd(8.5001)   ==  9  ", vil_round_rnd(8.5001)  ,  9);
  TEST("rnd(8.5001f)  ==  9  ", vil_round_rnd(8.5001f) ,  9);

  TEST("rnd(-9.4999)  == -9   ", vil_round_rnd(-9.4999)     , -9);
  TEST("rnd(-9.4999f) == -9   ", vil_round_rnd(-9.4999f)    , -9);
  TEST("rnd(-9.50)    == -9/10", (vil_round_rnd(-9.50)+1)/2 , -4);
  TEST("rnd(-9.50f)   == -9/10", (vil_round_rnd(-9.50f)+1)/2, -4);
  TEST("rnd(-9.5001)  == -10  ", vil_round_rnd(-9.5001)     , -10);
  TEST("rnd(-9.5001f) == -10  ", vil_round_rnd(-9.5001f)    , -10);
  TEST("rnd(9.4999)   ==  9   ", vil_round_rnd(9.4999)      ,  9);
  TEST("rnd(9.4999f)  ==  9   ", vil_round_rnd(9.4999f)     ,  9);
  TEST("rnd(9.50)     ==  9/10", (vil_round_rnd(9.50)-1)/2  ,  4);
  TEST("rnd(9.50f)    ==  9/10", (vil_round_rnd(9.50f)-1)/2 ,  4);
  TEST("rnd(9.5001)   ==  10  ", vil_round_rnd(9.5001)      ,  10);
  TEST("rnd(9.5001f)  ==  10  ", vil_round_rnd(9.5001f)     ,  10);

  TEST("rnd_halfinttoeven(-8.4999)  == -8", vil_round_rnd_halfinttoeven(-8.4999) , -8);
  TEST("rnd_halfinttoeven(-8.4999f) == -8", vil_round_rnd_halfinttoeven(-8.4999f), -8);
  TEST("rnd_halfinttoeven(-8.50)    == -8", vil_round_rnd_halfinttoeven(-8.50)   , -8);
  TEST("rnd_halfinttoeven(-8.50f)   == -8", vil_round_rnd_halfinttoeven(-8.50f)  , -8);
  TEST("rnd_halfinttoeven(-8.5001)  == -9", vil_round_rnd_halfinttoeven(-8.5001) , -9);
  TEST("rnd_halfinttoeven(-8.5001f) == -9", vil_round_rnd_halfinttoeven(-8.5001f), -9);
  TEST("rnd_halfinttoeven(8.4999)   ==  8", vil_round_rnd_halfinttoeven(8.4999)  ,  8);
  TEST("rnd_halfinttoeven(8.4999f)  ==  8", vil_round_rnd_halfinttoeven(8.4999f) ,  8);
  TEST("rnd_halfinttoeven(8.50)     ==  8", vil_round_rnd_halfinttoeven(8.50)    ,  8);
  TEST("rnd_halfinttoeven(8.50f)    ==  8", vil_round_rnd_halfinttoeven(8.50f)   ,  8);
  TEST("rnd_halfinttoeven(8.5001)   ==  9", vil_round_rnd_halfinttoeven(8.5001)  ,  9);
  TEST("rnd_halfinttoeven(8.5001f)  ==  9", vil_round_rnd_halfinttoeven(8.5001f) ,  9);

  TEST("rnd_halfinttoeven(-9.4999)  == -9 ", vil_round_rnd_halfinttoeven(-9.4999) , -9);
  TEST("rnd_halfinttoeven(-9.4999f) == -9 ", vil_round_rnd_halfinttoeven(-9.4999f), -9);
  TEST("rnd_halfinttoeven(-9.50)    == -10", vil_round_rnd_halfinttoeven(-9.50)   , -10);
  TEST("rnd_halfinttoeven(-9.50f)   == -10", vil_round_rnd_halfinttoeven(-9.50f)  , -10);
  TEST("rnd_halfinttoeven(-9.5001)  == -10", vil_round_rnd_halfinttoeven(-9.5001) , -10);
  TEST("rnd_halfinttoeven(-9.5001f) == -10", vil_round_rnd_halfinttoeven(-9.5001f), -10);
  TEST("rnd_halfinttoeven(9.4999)   ==  9 ", vil_round_rnd_halfinttoeven(9.4999)  ,  9);
  TEST("rnd_halfinttoeven(9.4999f)  ==  9 ", vil_round_rnd_halfinttoeven(9.4999f) ,  9);
  TEST("rnd_halfinttoeven(9.50)     ==  10", vil_round_rnd_halfinttoeven(9.50)    ,  10);
  TEST("rnd_halfinttoeven(9.50f)    ==  10", vil_round_rnd_halfinttoeven(9.50f)   ,  10);
  TEST("rnd_halfinttoeven(9.5001)   ==  10", vil_round_rnd_halfinttoeven(9.5001)  ,  10);
  TEST("rnd_halfinttoeven(9.5001f)  ==  10", vil_round_rnd_halfinttoeven(9.5001f) ,  10);

  TEST("rnd_halfintup(-8.4999)  == -8", vil_round_rnd_halfintup(-8.4999) , -8);
  TEST("rnd_halfintup(-8.4999f) == -8", vil_round_rnd_halfintup(-8.4999f), -8);
  TEST("rnd_halfintup(-8.50)    == -8", vil_round_rnd_halfintup(-8.50)   , -8);
  TEST("rnd_halfintup(-8.50f)   == -8", vil_round_rnd_halfintup(-8.50f)  , -8);
  TEST("rnd_halfintup(-8.5001)  == -9", vil_round_rnd_halfintup(-8.5001) , -9);
  TEST("rnd_halfintup(-8.5001f) == -9", vil_round_rnd_halfintup(-8.5001f), -9);
  TEST("rnd_halfintup(8.4999)   ==  8", vil_round_rnd_halfintup(8.4999)  ,  8);
  TEST("rnd_halfintup(8.4999f)  ==  8", vil_round_rnd_halfintup(8.4999f) ,  8);
  TEST("rnd_halfintup(8.50)     ==  9", vil_round_rnd_halfintup(8.50)    ,  9);
  TEST("rnd_halfintup(8.50f)    ==  9", vil_round_rnd_halfintup(8.50f)   ,  9);
  TEST("rnd_halfintup(8.5001)   ==  9", vil_round_rnd_halfintup(8.5001)  ,  9);
  TEST("rnd_halfintup(8.5001f)  ==  9", vil_round_rnd_halfintup(8.5001f) ,  9);

  TEST("rnd_halfintup(-9.4999)  == -9 ", vil_round_rnd_halfintup(-9.4999) , -9);
  TEST("rnd_halfintup(-9.4999f) == -9 ", vil_round_rnd_halfintup(-9.4999f), -9);
  TEST("rnd_halfintup(-9.50)    == -9 ", vil_round_rnd_halfintup(-9.50)   , -9);
  TEST("rnd_halfintup(-9.50f)   == -9 ", vil_round_rnd_halfintup(-9.50f)  , -9);
  TEST("rnd_halfintup(-9.5001)  == -10", vil_round_rnd_halfintup(-9.5001) , -10);
  TEST("rnd_halfintup(-9.5001f) == -10", vil_round_rnd_halfintup(-9.5001f), -10);
  TEST("rnd_halfintup(9.4999)   ==  9 ", vil_round_rnd_halfintup(9.4999)  ,  9);
  TEST("rnd_halfintup(9.4999f)  ==  9 ", vil_round_rnd_halfintup(9.4999f) ,  9);
  TEST("rnd_halfintup(9.50)     ==  10", vil_round_rnd_halfintup(9.50)    ,  10);
  TEST("rnd_halfintup(9.50f)    ==  10", vil_round_rnd_halfintup(9.50f)   ,  10);
  TEST("rnd_halfintup(9.5001)   ==  10", vil_round_rnd_halfintup(9.5001)  ,  10);
  TEST("rnd_halfintup(9.5001f)  ==  10", vil_round_rnd_halfintup(9.5001f) ,  10);

  TEST("floor(8.0)      ==  8", vil_round_floor(8.0)     ,  8);
  TEST("floor(8.0f)     ==  8", vil_round_floor(8.0f)    ,  8);
  TEST("floor(8.9999)   ==  8", vil_round_floor(8.9999)  ,  8);
  TEST("floor(8.9999f)  ==  8", vil_round_floor(8.9999f) ,  8);
  TEST("floor(8.0001)   ==  8", vil_round_floor(8.0001)  ,  8);
  TEST("floor(8.0001f)  ==  8", vil_round_floor(8.0001f) ,  8);
  TEST("floor(-8.0)     == -8", vil_round_floor(-8.0)    , -8);
  TEST("floor(-8.0f)    == -8", vil_round_floor(-8.0f)   , -8);
  TEST("floor(-8.9999)  == -9", vil_round_floor(-8.9999) , -9);
  TEST("floor(-8.9999f) == -9", vil_round_floor(-8.9999f), -9);
  TEST("floor(-8.0001)  == -9", vil_round_floor(-8.0001) , -9);
  TEST("floor(-8.0001f) == -9", vil_round_floor(-8.0001f), -9);

  TEST("floor(9.0)      ==  9 ", vil_round_floor(9.0)     ,  9);
  TEST("floor(9.0f)     ==  9 ", vil_round_floor(9.0f)    ,  9);
  TEST("floor(9.9999)   ==  9 ", vil_round_floor(9.9999)  ,  9);
  TEST("floor(9.9999f)  ==  9 ", vil_round_floor(9.9999f) ,  9);
  TEST("floor(9.0001)   ==  9 ", vil_round_floor(9.0001)  ,  9);
  TEST("floor(9.0001f)  ==  9 ", vil_round_floor(9.0001f) ,  9);
  TEST("floor(-9.0)     == -9 ", vil_round_floor(-9.0)    , -9);
  TEST("floor(-9.0f)    == -9 ", vil_round_floor(-9.0f)   , -9);
  TEST("floor(-9.9999)  == -10", vil_round_floor(-9.9999) , -10);
  TEST("floor(-9.9999f) == -10", vil_round_floor(-9.9999f), -10);
  TEST("floor(-9.0001)  == -10", vil_round_floor(-9.0001) , -10);
  TEST("floor(-9.0001f) == -10", vil_round_floor(-9.0001f), -10);

  TEST("ceil(8.0)      ==  8", vil_round_ceil(8.0)     ,  8);
  TEST("ceil(8.0f)     ==  8", vil_round_ceil(8.0f)    ,  8);
  TEST("ceil(8.9999)   ==  9", vil_round_ceil(8.9999)  ,  9);
  TEST("ceil(8.9999f)  ==  9", vil_round_ceil(8.9999f) ,  9);
  TEST("ceil(8.0001)   ==  9", vil_round_ceil(8.0001)  ,  9);
  TEST("ceil(8.0001f)  ==  9", vil_round_ceil(8.0001f) ,  9);
  TEST("ceil(-8.0)     == -8", vil_round_ceil(-8.0)    , -8);
  TEST("ceil(-8.0f)    == -8", vil_round_ceil(-8.0f)   , -8);
  TEST("ceil(-8.9999)  == -8", vil_round_ceil(-8.9999) , -8);
  TEST("ceil(-8.9999f) == -8", vil_round_ceil(-8.9999f), -8);
  TEST("ceil(-8.0001)  == -8", vil_round_ceil(-8.0001) , -8);
  TEST("ceil(-8.0001f) == -8", vil_round_ceil(-8.0001f), -8);

  TEST("ceil(9.0)      ==  9", vil_round_ceil(9.0)     ,  9);
  TEST("ceil(9.0f)     ==  9", vil_round_ceil(9.0f)    ,  9);
  TEST("ceil(9.9999)   == 10", vil_round_ceil(9.9999)  , 10);
  TEST("ceil(9.9999f)  == 10", vil_round_ceil(9.9999f) , 10);
  TEST("ceil(9.0001)   == 10", vil_round_ceil(9.0001)  , 10);
  TEST("ceil(9.0001f)  == 10", vil_round_ceil(9.0001f) , 10);
  TEST("ceil(-9.0)     == -9", vil_round_ceil(-9.0)    , -9);
  TEST("ceil(-9.0f)    == -9", vil_round_ceil(-9.0f)   , -9);
  TEST("ceil(-9.9999)  == -9", vil_round_ceil(-9.9999) , -9);
  TEST("ceil(-9.9999f) == -9", vil_round_ceil(-9.9999f), -9);
  TEST("ceil(-9.0001)  == -9", vil_round_ceil(-9.0001) , -9);
  TEST("ceil(-9.0001f) == -9", vil_round_ceil(-9.0001f), -9);
}

TESTMAIN(test_round);
