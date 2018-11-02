// This is mul/mmn/tests/test_parse_arcs.cxx
#include <vector>
#include <sstream>
#include <iostream>
#include <algorithm>
#include <iterator>
#include <testlib/testlib_test.h>
#include <mmn/mmn_parse_arcs.h>
#ifdef _MSC_VER
#  include <vcl_msvc_warnings.h>
#endif


void test_parse_arcs_a()
{
  std::cout<<"========= test_parse_arcs =========\n" << std::endl;

  std::vector<std::string> nodeNames;
  nodeNames.emplace_back("L1_top_post");
  nodeNames.emplace_back("L1_top_ant");
  nodeNames.emplace_back("L1_bot_post");
  nodeNames.emplace_back("L1_bot_ant");
  nodeNames.emplace_back("L1_pedicle");

  nodeNames.emplace_back("L2_top_post");
  nodeNames.emplace_back("L2_top_ant");
  nodeNames.emplace_back("L2_bot_post");
  nodeNames.emplace_back("L2_bot_ant");
  nodeNames.emplace_back("L2_pedicle");

  std::string testStreamStr= "{\n"
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

  std::istringstream ss(testStreamStr);
  std::vector<mmn_arc> arcs;
  mmn_parse_arcs(ss,nodeNames,arcs);

  TEST("Test arc numbers",arcs.size()==15,true);
  TEST("Test first arc ",arcs[0].v1==0 && arcs[0].v2==1,true );
  TEST("Test second arc ",arcs[1].v1==0 && arcs[1].v2==2,true );
  TEST("Test last arc ",arcs.back().v1==9 && arcs.back().v2==5,true );

  std::cout<<"Arc set follows:"<<std::endl;
  std::copy(arcs.begin(),arcs.end(),
           std::ostream_iterator<mmn_arc>(std::cout,"\t"));
  std::cout<<"\n\n========= end test_parse_arcs =========\n"<<std::endl;
}

void test_parse_arcs()
{
  test_parse_arcs_a();
}

TESTMAIN(test_parse_arcs);
