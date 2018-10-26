#include <testlib/testlib_test.h>
#include <brdb/brdb_value.h>
#include <brdb/brdb_query.h>
#include <brdb/brdb_query_aptr.h>
#include <brdb/brdb_relation.h>
#include <brdb/brdb_selection.h>
#include <brdb/brdb_relation_sptr.h>
#include <brdb/brdb_selection_sptr.h>

static void test_query()
{
  TEST("comparison complements", true,
       (brdb_query::NONE == ~brdb_query::ALL ) &&
       (brdb_query::ALL  == ~brdb_query::NONE) &&
       (brdb_query::EQ   == ~brdb_query::NEQ ) &&
       (brdb_query::NEQ  == ~brdb_query::EQ  ) &&
       (brdb_query::GT   == ~brdb_query::LEQ ) &&
       (brdb_query::LEQ  == ~brdb_query::GT  ) &&
       (brdb_query::LT   == ~brdb_query::GEQ ) &&
       (brdb_query::GEQ  == ~brdb_query::LT  ) );

  brdb_query_aptr q1 = brdb_query_comp_new("attr", brdb_query::EQ, 1);
  brdb_query_aptr q2 = brdb_query_comp_new("attr", brdb_query::EQ, 3);
  brdb_query_aptr q1or2 = std::move(q1) | std::move(q2);
}

TESTMAIN(test_query);
