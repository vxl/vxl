// This is core/vsl/tests/test_vlarge_block_io.cxx
#include <vcl_iostream.h>
#include <vcl_new.h>
#include <vcl_algorithm.h>
#include <vcl_limits.h>
#include <vcl_vector.h>
#include <vcl_cstdlib.h>
#include <vsl/vsl_binary_io.h>
#include <vsl/vsl_block_binary.h>
#include <testlib/testlib_test.h>
#include <vpl/vpl.h>


void free_blocks(vcl_vector<void *> &blocks)
{
  for (unsigned i=0, n=blocks.size(); i!=n; ++i)
    vcl_free(blocks[i]);
}

template <class T>
void test_vlarge_block(void * block, vcl_size_t s, T /*dummy*/)
{
  // fill one of the blocks with large numbers.
  T * const numbers = static_cast<T *>(block);
  vcl_size_t n = s / sizeof(T);
  assert(s % sizeof(T) == 0);

  for (vcl_size_t i=0; i<n; ++i)
    numbers[i] = static_cast<T>(vcl_numeric_limits<T>::max() - i) ;
  
  vsl_b_ofstream bfs_out("vsl_vlarge_block_io_test.bvl.tmp");
  TEST("Created vsl_vlarge_block_io_test.bvl.tmp for writing", (!bfs_out), false);
#if VCL_HAS_EXCEPTION
  try { vsl_block_binary_write(bfs_out, numbers, n); }
  catch(...) { TEST ("vsl_block_binary_write didn't throw exception", true, false); }
#else
  vsl_block_binary_write(bfs_out, numbers, n);
#endif
  vsl_b_write(bfs_out, 0xdeadbeefu);
  TEST("Stream still ok", (!bfs_out), false);
  bfs_out.close();

  vcl_fill(numbers, numbers+n, T());

  vsl_b_ifstream bfs_in("vsl_vlarge_block_io_test.bvl.tmp");
  TEST("Opened vsl_vlarge_block_io_test.bvl.tmp for reading", (!bfs_in), false);
  unsigned sentinel;
#if VCL_HAS_EXCEPTION 
  try { vsl_block_binary_read(bfs_in, numbers, n); }
  catch(...) { TEST ("vsl_block_binary_read didn't throw exception", true, false); }
#else
  vsl_block_binary_read(bfs_in, numbers, n);
#endif 
  vsl_b_read(bfs_in, sentinel);
  TEST("sentinel matched", sentinel, 0xdeadbeefu);
  TEST("Stream still ok", (!bfs_in), false);
  bfs_in.close();

  vcl_size_t errors=0;
  for (vcl_size_t i=0; i<n; ++i)
    if (numbers[i] != static_cast<T>(vcl_numeric_limits<T>::max() - i) )
      errors++;

  TEST_NEAR("No errors in stored numbers", errors, 0, 0);
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

  s /= 1024*4; // a sensible block size.
  s *= 1024;
  unsigned max_blocks= vcl_numeric_limits<vcl_size_t>::max() / (s/2);
  vcl_cout << "Try to allocate up to " << max_blocks << " blocks of " <<
    s/1024<<"KiB" << vcl_endl;
  vcl_vector<void *> blocks;
  blocks.reserve(max_blocks+2);
  while (true)
  {
    if (blocks.size() >= max_blocks+1)
    {
      vcl_cout << "ERROR: Unable to force out of memory, after allocating\n"
        "         " << blocks.size() << " blocks of " << 
        s/1024<<"KiB" << vcl_endl;
      free_blocks(blocks);
      vcl_exit(3);
    }
    void * block = 0;
    block = vcl_malloc(s);
    // if we have run out of memory, go on to next section.
    if (block == 0)
      break;
    //otherwise store memory pointer, and try and use up some more.
    blocks.push_back(block);
  }

  vcl_cout << "Run out of memory after allocating " << blocks.size() <<
    " blocks of " << s/1024 << " KiB" << vcl_endl; 
  if (blocks.empty())
  {
    vcl_cout << "ERROR: Unable to use chosen block size" << vcl_endl;
    free_blocks(blocks);
    vcl_exit(3);
  }


  test_vlarge_block(blocks.back(), s, int());
  test_vlarge_block(blocks.back(), s, float());
  free_blocks(blocks);
}

TESTMAIN(test_vlarge_block_io);

