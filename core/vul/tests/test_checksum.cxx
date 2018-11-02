// This is core/vul/tests/test_checksum.cxx
#include <iostream>
#include <iomanip>
#include <iterator>
#ifdef _MSC_VER
#  include <vcl_msvc_warnings.h>
#endif
#include <testlib/testlib_test.h>
#include <vul/vul_checksum.h>


void test_checksum()
{
  std::cout << "**********************\n"
           << " Testing vul_checksum\n"
           << "**********************\n";


  // Test cases taken from IBM documentation of ippsCRC32C_8u function.
  // These test cases are used to confirm compatibility with independent implementation.

  vxl_byte data[48] = {
    0x01, 0xc0, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x14, 0x00, 0x00, 0x00, 0x00, 0x00, 0x04, 0x00,
    0x00, 0x00, 0x00, 0x14, 0x00, 0x00, 0x00, 0x18,
    0x28, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x02, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00 };

  char * datac = (char*) data;
  vxl_uint_32 checksum;

  checksum = vul_checksum_crc32c(datac, datac+48);
  std::cout << "\nTest on 48 bytes of An iSCSI - SCSI Read (10) Command PDU: " << std::hex << checksum << std::endl;
  TEST("vul_checksum_crc32c", checksum, 0x563a96d9);

  for( unsigned i=0; i<32; i++) data[i] = 0;
  checksum = vul_checksum_crc32c(datac, datac+32);
  std::cout << "\nTest on 32 bytes of zeros: " << checksum << std::endl;
  TEST("vul_checksum_crc32c", checksum, 0xaa36918a);

  for( unsigned i=0; i<32; i++) data[i] = 0xff;
  checksum = vul_checksum_crc32c(datac, datac+32);
  std::cout << "\nTest on 32 bytes of 0xff: " << checksum << std::endl;
  TEST("vul_checksum_crc32c", checksum, 0x43aba862);

  for( unsigned i=0; i<32; i++) data[i] = i;
  checksum = vul_checksum_crc32c(datac, datac+32);
  std::cout << "\nTest on 32 bytes of incrementing 0 to 0x1f: " << checksum << std::endl;
  TEST("vul_checksum_crc32c", checksum, 0x4e79dd46);

  for( unsigned i=0; i<32; i++) data[i] = 31-i;
  checksum = vul_checksum_crc32c(datac, datac+32);
  std::cout << "\nTest on 32 bytes of decrementing 0x1f to 0: " << checksum << std::endl;
  TEST("vul_checksum_crc32c", checksum, 0x5cdb3f11);





  // For golden values see,
  // http://www.lammertbies.nl/comm/info/crc-calculation.html
  // But results byte swapped.

  for( unsigned i=0; i<4; i++) data[i] = 0xff;
  data[3]=0x7f;

  checksum = vul_checksum_crc32(datac, datac+4);
  std::cout << "\nTest on 0xff ff ff 7f: " << std::hex << checksum << std::endl;
  TEST("vul_checksum_crc32", checksum, 0xdf7c4712);

  for( unsigned i=0; i<40; i++) data[i] = 0xff;
  checksum = vul_checksum_crc32(datac, datac+40);
  std::cout << "\nTest on 40 bytes of 0xff: " << std::hex << checksum << std::endl;
  TEST("vul_checksum_crc32", checksum, 0x734cd08c);

  // This is the encoding of a zero polynomial.
  checksum = vul_checksum_crc32(datac, datac+4);
  std::cout << "\nTest on 4 bytes of 0xff: " << checksum << std::endl;
  TEST("vul_checksum_crc32", checksum, 0xffffffff );


  // This is the encoding of a longer zero polynomial. Only the first 32 bits are complemented.
  for( unsigned i=4; i<8; i++) data[i] = 0x00;
  checksum = vul_checksum_crc32(datac, datac+4);
  std::cout << "\nTest on 4 bytes of 0xff, then 4 of 0x00: " << checksum << std::endl;
  TEST("vul_checksum_crc32", checksum, 0xffffffff );


  char boost_data[] = { 0x31, 0x32, 0x33, 0x34, 0x35, 0x36, 0x37, 0x38, 0x39 };

  checksum = vul_checksum_crc32((char*)boost_data, (char*)boost_data+9);
  std::cout << "\nTest on 9 bytes from Boost:CRC test suite: " << std::hex << checksum << std::endl;
  TEST("vul_checksum_crc32", checksum, 0x2639f4cb); // Golden value is byte swapped version from Boost test suite


  // Same test using chars 1-9 beut using streams and iterators. Assume ASCII
  std::istringstream boost_data2("123456789");
  checksum = vul_checksum_crc32(std::istream_iterator<char>(boost_data2), std::istream_iterator<char>());
  std::cout << "\nTest on string \"123456789\": " << std::hex << checksum << std::endl;
  TEST("vul_checksum_crc32", checksum, 0x2639f4cb); // Golden value is byte swapped version from Boost test suite
}

TEST_MAIN(test_checksum);
