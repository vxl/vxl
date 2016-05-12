// This is core/vul/vul_checksum.h
#ifndef vul_checksum_h_
#define vul_checksum_h_


//:
// \file
// \brief Contains function for calculating checksum
// \author Ian Scott, Imorphics.


#include <vxl_config.h>





//: Caluclate the recommended CRC32C checksum.
// As used by iSCSI, Btrfs, ext4.
// \param begin,end The sequence of chars to be checksummed.
// \note that this checksum is not cryptographically secure - it is easy to find
//  collisions with the original data.
template <class IT> vxl_uint_32 vul_checksum_crc32c(IT begin, IT end);

//: Caluclate the popular CRC32 checksum as used by SATA, MPEG-2, PKZIP, Gzip, Bzip2.
// \param begin,end The sequence of chars to be checksummed.
// \note that this checksum is not cryptographically secure - it is easy to find
//  collisions with the original data.
template <class IT> vxl_uint_32 vul_checksum_crc32(IT begin, IT end);




// Copied from "Fast CRC32 in Software" R Black, 1994, http://www.cl.cam.ac.uk/research/srg/bluebook/21/crc/node6.html
// Software original is copyright (c) 1993 Richard Black. All rights are reserved. You may use this code only if it includes a statement to that effect.
// Heavily modified to handle byte swapping and bit reversing issues, and implement CRC32C

namespace
{
  inline vxl_uint_32 byte_swap (vxl_uint_32 val)
  {
    val = ((val << 8) & 0xFF00FF00 ) | ((val >> 8) & 0xFF00FF );
    return (val << 16) | (val >> 16);
  }


  #if VXL_LITTLE_ENDIAN
  //! Byte swap unsigned int
  inline vxl_uint_32 to_bigendian( vxl_uint_32 val )
  {
    return byte_swap (val);
  }

  #else
  inline vxl_uint_32 to_bigendian( vxl_uint_32 val )
  {
    return val;
  }

  #endif

  //: reverse this (8-bit) byte
  // From http://graphics.stanford.edu/~seander/bithacks.html
  inline char bit_reverse(char b)
  {
    return (char)((((unsigned char)(b) * 0x0202020202ULL) & 0x010884422010ULL) % 1023);
  }



// There is a faster implementation in the above reference but I don't have time to fix the endian issues.
template <class IT> vxl_uint_32 vul_checksum_crc32x(IT p, IT end, vxl_uint_32 quotient)
{
  vxl_uint_32 result=-1;
  unsigned char octet;


  // The running result of the polymnomial long division is result*x^(current_bit_position-32) xor remaining_message
  while (p!=end)
  {
    octet = bit_reverse(*(p++));
    for (unsigned j=0; j<8; j++)
    {
      if ((octet >> 7) ^ (result >> 31))  // xor not power
        result = (result << 1) ^ quotient;
      else
        result = (result << 1);
      octet <<= 1;
    }
  }
  // After we have finished, result contains the remainer.
  result = ~result;  //The complement of the remainder

  char* result_carray = reinterpret_cast<char*>(&result);
  result_carray[0] = bit_reverse(result_carray[0]);
  result_carray[1] = bit_reverse(result_carray[1]);
  result_carray[2] = bit_reverse(result_carray[2]);
  result_carray[3] = bit_reverse(result_carray[3]);

  return result;
}


}


template <class IT> vxl_uint_32 vul_checksum_crc32c(IT begin, IT end)
{
  return vul_checksum_crc32x(begin, end, 0x1EDC6F41);
}


template <class IT> vxl_uint_32 vul_checksum_crc32(IT begin, IT end)
{
  return vul_checksum_crc32x(begin, end, 0x04c11db7);
}
#endif // vul_checksum_h_
