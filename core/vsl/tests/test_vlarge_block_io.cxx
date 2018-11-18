// This is core/vsl/tests/test_vlarge_block_io.cxx
#include <iostream>
#include <new>
#include <algorithm>
#include <limits>
#include <vector>
#include <cstdlib>
#ifdef _MSC_VER
#  include <vcl_msvc_warnings.h>
#endif
#include <vsl/vsl_binary_io.h>
#include <vsl/vsl_block_binary.h>
#include <testlib/testlib_test.h>

void free_blocks(std::vector<void *> &blocks)
{
  for (unsigned i=0, n=blocks.size(); i!=n; ++i)
    std::free(blocks[i]);
}

// scale should be 0 or 1.
template <class T>
void test_vlarge_block(void * block, std::size_t s, T scale)
{
  // fill one of the blocks with large numbers.
  T * const numbers = static_cast<T *>(block);
  std::size_t n = s / sizeof(T);

  for (std::size_t i=0; i<n; ++i)
    numbers[i] = static_cast<T>(std::numeric_limits<T>::max() - T(i) * scale) ;

  vsl_b_ofstream bfs_out("vsl_vlarge_block_io_test.bvl.tmp");
  TEST("Created vsl_vlarge_block_io_test.bvl.tmp for writing", (!bfs_out), false);
  try { vsl_block_binary_write(bfs_out, numbers, n); }
  catch(...) { TEST ("vsl_block_binary_write didn't throw exception", true, false); }
  vsl_b_write(bfs_out, 0xdeadbeefu);
  TEST("Stream still ok", (!bfs_out), false);
  bfs_out.close();

  std::fill(numbers, numbers+n, T());

  vsl_b_ifstream bfs_in("vsl_vlarge_block_io_test.bvl.tmp");
  TEST("Opened vsl_vlarge_block_io_test.bvl.tmp for reading", (!bfs_in), false);
  unsigned sentinel;
  try { vsl_block_binary_read(bfs_in, numbers, n); }
  catch(...) { TEST ("vsl_block_binary_read didn't throw exception", true, false); }
  vsl_b_read(bfs_in, sentinel);
  TEST("sentinel matched", sentinel, 0xdeadbeefu);
  TEST("Stream still ok", (!bfs_in), false);
  bfs_in.close();

  std::size_t errors=0;
  for (std::size_t i=0; i<n; ++i)
    if (numbers[i] != static_cast<T>(std::numeric_limits<T>::max() - T(i) * scale) )
      errors++;

  TEST_NEAR("No errors in stored numbers", static_cast<double>(errors), 0, 0);
}


void test_vlarge_block_io()
{
  std::cout << "*********************************\n"
           << "Testing vsl_block_binary_write io\n"
           << "*********************************\n";

  // Try and find a block size that allows us to
  // control the availabililty of memory.
  std::size_t s = std::numeric_limits<std::size_t>::max() / 16;

  std::cout << "Start by trying to allocate " << (s/(1024*1024))+1 << "MiB" << std::endl;
  while (true)
  {
    char *block = nullptr;
    block = (char *)std::malloc(s);
    if (block != nullptr)
    {
      std::free(block);
      break;
    }
    s /= 4;
    if (s==0)
    {
      std::cout << "ERROR: Unable to allocate any memory" << std::endl;
      std::exit(3);
    }
  }

  std::cout << "Succeeded allocating " <<(s+1)/(1024*1024)<<"MiB" << std::endl;


  // Now try to use up most of memory.

  s /= 1024*4; // a sensible block size.
  s *= 1024;
  unsigned max_blocks = std::min<std::size_t>(1024*1024,
                                            std::numeric_limits<std::size_t>::max() / (s/2) );

  std::cout << "Try to allocate up to " << max_blocks << " blocks of "
           << s/1024 << "KiB" << std::endl;
  std::vector<void *> blocks;
  blocks.reserve(max_blocks+2);
  while (true)
  {
    if (blocks.size() >= max_blocks+1)
    {
      std::cout << "ERROR: Unable to force out of memory, after allocating\n"
               << "         " << blocks.size() << " blocks of "
               << s/1024<<"KiB" << std::endl;
      free_blocks(blocks);
      std::exit(3);
    }
    void * block = nullptr;
    block = std::malloc(s);
    // if we have run out of memory, go on to next section.
    if (block == nullptr)
      break;
    //otherwise store memory pointer, and try and use up some more.
    blocks.push_back(block);
  }

  std::cout << "Run out of memory after allocating " << blocks.size()
           << " blocks of " << s/1024 << " KiB" << std::endl;
  if (blocks.empty())
  {
    std::cout << "ERROR: Unable to use chosen block size" << std::endl;
    free_blocks(blocks);
    std::exit(3);
  }


  test_vlarge_block(blocks.back(), s, (int) 0);
  test_vlarge_block(blocks.back(), s-1, (int) 0);
  test_vlarge_block(blocks.back(), s-1019, (int) 0);
  test_vlarge_block(blocks.back(), s, (int) 1);
  test_vlarge_block(blocks.back(), s-1, (int) 1);
  test_vlarge_block(blocks.back(), s-1019, (int) 1);
  test_vlarge_block(blocks.back(), s, (unsigned long) 0);
  test_vlarge_block(blocks.back(), s, (char) 0);
  test_vlarge_block(blocks.back(), s, (short) 0);
  test_vlarge_block(blocks.back(), s, (float) 0);
  test_vlarge_block(blocks.back(), s-1, (float) 0);
  test_vlarge_block(blocks.back(), s-1019, (float) 0);
  test_vlarge_block(blocks.back(), s, (double) 0);
  test_vlarge_block(blocks.back(), s, (vxl_int_64) 0);

  free_blocks(blocks);
}

TESTMAIN(test_vlarge_block_io);
