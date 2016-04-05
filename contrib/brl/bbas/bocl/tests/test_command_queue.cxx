#include <testlib/testlib_test.h>
#include <testlib/testlib_root_dir.h>
#include "bocl_command_queue_mgr.h"

static void test_command_queue()
{
  std::cout<<"TESTING COMMAND QUEUE ASYNC WRITING"<<std::endl;
   bocl_command_queue_mgr &mgr = bocl_command_queue_mgr::instance();
   mgr.init_kernel();
   mgr.test_async_command_queue();
}

TESTMAIN(test_command_queue);
