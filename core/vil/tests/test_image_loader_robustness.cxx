// This is core/vil/tests/test_image_loader_robustness.cxx
#include <cstddef>
#include <iostream>
#ifdef _MSC_VER
#  include "vcl_msvc_warnings.h"
#endif
#include "vil/vil_load.h"
#include "vil/vil_stream_core.h"
#include "vil/vil_stream_section.h"
#include "vil/vil_stream.h"
#include "testlib/testlib_test.h"

using vil_stream_sptr = vil_smart_ptr<vil_stream>;

static unsigned long rand_next;

void
test_at_file_size(const std::size_t n)
{
  auto *          fs_core_p = new vil_stream_core();
  vil_stream_sptr fs_core = fs_core_p;

  auto * data = new unsigned char[n];
  // Use dodgy ANSI-C suggested rand() for repeatability.
  for (unsigned i = 0; i < n; ++i)
  {
    rand_next = rand_next * 1103515245 + 12345;
    data[i] = static_cast<unsigned char>(((rand_next / 65536) % 32768) % 256);
  }

  fs_core_p->m_transfer((char *)data, 0, n, false);

  vil_stream_sptr         fs_section = new vil_stream_section(fs_core.ptr(), 0, int(n));
  vil_image_resource_sptr ir = vil_load_image_resource_raw(fs_section.ptr());
  std::cout << "Trying file of size " << n << std::endl;
  TEST("Should fail to load", static_cast<bool>(ir), false);
  delete[] data;
}

void
test_image_loader_robustness()
{
  test_at_file_size(0);

  for (unsigned i = 0; i < 100; ++i)
    test_at_file_size(256);

  for (unsigned i = 0; i < 25; ++i)
    test_at_file_size(1024);

  for (unsigned i = 0; i < 10; ++i)
    test_at_file_size(10240);
}

TESTMAIN(test_image_loader_robustness);
