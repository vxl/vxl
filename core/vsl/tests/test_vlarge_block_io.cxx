// This is core/vsl/tests/test_vlarge_block_io.cxx
#include <vcl_iostream.h>
#include <vcl_new.h>
#include <vcl_limits.h>
#include <vcl_vector.h>
#include <vcl_cstdlib.h>
#include <vsl/vsl_binary_io.h>
#include <vsl/vsl_block_binary.h>
#include <testlib/testlib_test.h>
#include <vpl/vpl.h>


void free_blocks(vcl_vector<int *> &blocks)
{
  for (unsigned i=0, n=blocks.size(); i!=n; ++i)
    vcl_free(blocks[i]);
}


void test_vlarge_block_io()
{
  vcl_cout << "*********************************\n"
           << "Testing vsl_block_binary_write io\n"
           << "*********************************\n";

  // Try and find a block size that allows us to
  // control the availabililty of memory.
  vcl_size_t s = vcl_numeric_limits<vcl_size_t>::max() / 16;

  vcl_cout << "Start by trying to allocate " << (s/(1024*1024))+1 << "MiB" << vcl_endl;
  while (true)
  {
    char *block = 0;
    block = (char *)vcl_malloc(s);
    if (block != 0)
    {
      vcl_free(block);
      break;
    }
    s /= 4;
    if (s==0)
    {
      vcl_cout << "ERROR: Unable to allocate any memory" << vcl_endl;
      vcl_exit(3);
    }

  }

  vcl_cout << "Succeeded allocating " <<(s+1)/(1024*1024)<<"MiB" << vcl_endl;


  // Now try to use up most of memory.

  s /= sizeof(int)*4; // a sensible block size.
  unsigned max_blocks= vcl_numeric_limits<vcl_size_t>::max() / (s/4);
  vcl_cout << "Try to allocate up to " << max_blocks << " blocks of " <<
    (s+1)*sizeof(int)/(1024*1024)<<"MiB" << vcl_endl;
  vcl_vector<int *> blocks;
  blocks.reserve(max_blocks+2);
  while (true)
  {
    if (blocks.size() >= max_blocks+1)
    {
      vcl_cout << "ERROR: Unable to force out of memory, after allocating\n"
        "         " << blocks.size() << " blocks of " << 
        (s+1)*sizeof(int)/(1024*1024)<<"MiB" << vcl_endl;
      free_blocks(blocks);
      vcl_exit(3);
    }
    int * block = 0;
    block = (int *)vcl_malloc(s*sizeof(int));
    // if we have run out of memory, go on to next section.
    if (block == 0)
      break;
    //otherwise store memory pointer, and try and use up some more.
    blocks.push_back(block);
  }

  vcl_cout << "Run out of memory after allocating " << blocks.size() <<
    " blocks of " << s << " ints at " << sizeof(int) << "bytes each" << vcl_endl; 
  if (blocks.empty())
  {
    vcl_cout << "ERROR: Unable to use chosen block size" << vcl_endl;
    free_blocks(blocks);
    vcl_exit(3);
  }


  // fill one of the blocks with large numbers.
  int * const useful_block = blocks.back();

  for (unsigned i=0; i<s; ++i)
    useful_block[i] = vcl_numeric_limits<int>::max() - i;
  
  vsl_b_ofstream bfs_out("vsl_vlarge_block_io_test.bvl.tmp");
  TEST("Created vsl_vlarge_block_io_test.bvl.tmp for writing", (!bfs_out), false);
#if VCL_HAS_EXCEPTION
  try
  {
    vsl_block_binary_write(bfs_out, useful_block, s);
    vsl_b_write(bfs_out, 0xdeadbeefu);
  }
  catch(...)
  {
    TEST ("vsl_block_binary_write didn't throw exception", true, false);
  }
#else
    vsl_block_binary_write(bfs_out, useful_block, s);
    vsl_b_write(bfs_out, 0xdeadbeefu);
#endif
  TEST("Stream still ok", (!bfs_out), false);
  bfs_out.close();


  vsl_b_ifstream bfs_in("vsl_vlarge_block_io_test.bvl.tmp");
  TEST("Opened vsl_vlarge_block_io_test.bvl.tmp for reading", (!bfs_in), false);
  unsigned sentinel;
#if VCL_HAS_EXCEPTION 
  try
  {
    vsl_block_binary_read(bfs_in, blocks.front(), s);
    vsl_b_read(bfs_in, sentinel);
  }
  catch(...)
  {
    TEST ("vsl_block_binary_read didn't throw exception", true, false);
  }
#else
  vsl_block_binary_read(bfs_in, blocks.front(), s);
  vsl_b_read(bfs_in, sentinel);
#endif 
  TEST("Stream still ok", (!bfs_in), false);
  TEST("sentinel matched", sentinel, 0xdeadbeefu);
  bfs_in.close();

  free_blocks(blocks);
}

TESTMAIN(test_vlarge_block_io);

