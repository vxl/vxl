// acal/io/tests/test_match_graph_io.cxx
#include "test_generic_io.h"
#include <acal/io/acal_io_match_graph.h>

static void
test_match_graph_io()
{
  // match_params io test
  match_params mp(10, 10, 0.5f, 0.5f);
  test_generic_io(mp, "match_params");

  // acal_match_graph io test
  // TODO

}

TESTMAIN(test_match_graph_io);
