// This is mul/mmn/tests/test_parse_arcs.cxx
#include <testlib/testlib_test.h>
#include <vcl_vector.h>
#include <mmn/mmn_parse_arcs.h>
#include <vcl_sstream.h>
#include <vcl_algorithm.h>
#include <vcl_iterator.h>


void test_parse_arcs_a()
{
  vcl_cout<<"========= test_parse_arcs =========\n" << vcl_endl;

  vcl_vector<vcl_string> nodeNames;
  nodeNames.push_back("L1_top_post");
  nodeNames.push_back("L1_top_ant");
  nodeNames.push_back("L1_bot_post");
  nodeNames.push_back("L1_bot_ant");
  nodeNames.push_back("L1_pedicle");

  nodeNames.push_back("L2_top_post");
  nodeNames.push_back("L2_top_ant");
  nodeNames.push_back("L2_bot_post");
  nodeNames.push_back("L2_bot_ant");
  nodeNames.push_back("L2_pedicle");

  vcl_string testStreamStr= "{\n"
      "arc: { L1_top_post L1_top_ant }\n"
      "arc: { L1_top_post L1_bot_post }\n"
      "arc: { L1_bot_post L1_bot_ant }\n"
      "arc: { L1_bot_ant L1_top_ant }\n"
      "arc: { L1_pedicle L1_bot_post }\n"
      "arc: { L1_pedicle L1_top_post }\n"
      "arc: { L1_pedicle L2_pedicle }\n"
      "arc: { L1_bot_post L2_top_post }\n"
      "arc: { L1_bot_ant L2_top_ant }\n"
      "arc: { L2_top_post L2_top_ant }\n"
      "arc: { L2_top_post L2_bot_post }\n"
      "arc: { L2_bot_post L2_bot_ant }\n"
      "arc: { L2_bot_ant L2_top_ant }\n"
      "arc: { L2_pedicle L2_bot_post }\n"
      "arc: { L2_pedicle L2_top_post }\n"
      "}"
      ;

  vcl_istringstream ss(testStreamStr);
  vcl_vector<mmn_arc> arcs;
  mmn_parse_arcs(ss,nodeNames,arcs);

  TEST("Test arc numbers",arcs.size()==15,true);
  TEST("Test first arc ",arcs[0].v1==0 && arcs[0].v2==1,true );
  TEST("Test second arc ",arcs[1].v1==0 && arcs[1].v2==2,true );
  TEST("Test last arc ",arcs.back().v1==9 && arcs.back().v2==5,true );

  vcl_cout<<"Arc set follows:"<<vcl_endl;
  vcl_copy(arcs.begin(),arcs.end(),
           vcl_ostream_iterator<mmn_arc>(vcl_cout,"\t"));
  vcl_cout<<"\n\n========= end test_parse_arcs =========\n"<<vcl_endl;
}

void test_parse_arcs()
{
  test_parse_arcs_a();
}

TESTMAIN(test_parse_arcs);
