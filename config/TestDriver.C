//:
// \file
// \brief TestDriver.C - Generic test driver program to be copied to test directories by generic.mk

#include <vcl_compiler.h>
#include <Basics/RunDynamicTests.h>
#include <vcl_iostream.h>

#if defined(VCL_SUNPRO_CC_50)
// fsm@robots.ox.ac.uk : this is a little hack to make sure that the TestDriver
// executable is linked against the fstream part of the C++ library. Without it,
// some tests will fail due to unresolved symbols.
#include <vcl_fstream.h>
void you_silly_compiler(void)
{
  vcl_ofstream f("/tmp/silly.file");
  f << "oi\n";
}
#endif

int
main(int argc, char** argv)
{
  vcl_cout << vcl_endl << "BEGIN: Generic TargetJr TestDriver: Testing - " << argv[argc-1] << vcl_endl;
  int r = RunDynamicTests::RunTests(argc, argv);
  if (r > 0) 
    vcl_cout << "*** Failures: " << r << vcl_endl;
  return r;
}
