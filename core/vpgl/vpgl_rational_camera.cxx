// This is core/vpgl/vpgl_rational_camera.cxx
#include "vpgl_rational_camera.h"

// std::vector from vpgl_rational_order
std::vector<unsigned>
vpgl_rational_order_func::to_vector(vpgl_rational_order choice)
{
  std::vector<unsigned> order(20, 0);
  switch (choice)
  {

    // order[VXL_INDEX] = VXL_INDEX
    case vpgl_rational_order::VXL:
    {
      order[0] = 0;   // xxx
      order[1] = 1;   // xxy
      order[2] = 2;   // xxz
      order[3] = 3;   // xx
      order[4] = 4;   // xyy
      order[5] = 5;   // xyz
      order[6] = 6;   // xy
      order[7] = 7;   // xzz
      order[8] = 8;   // xz
      order[9] = 9;   // x
      order[10] = 10; // yyy
      order[11] = 11; // yyz
      order[12] = 12; // yy
      order[13] = 13; // yzz
      order[14] = 14; // yz
      order[15] = 15; // y
      order[16] = 16; // zzz
      order[17] = 17; // zz
      order[18] = 18; // z
      order[19] = 19; // 1
      break;
    }

    // order[VXL_INDEX] = RPC00B_INDEX
    case vpgl_rational_order::RPC00B:
    {
      order[0] = 11;  // xxx
      order[1] = 14;  // xxy
      order[2] = 17;  // xxz
      order[3] = 7;   // xx
      order[4] = 12;  // xyy
      order[5] = 10;  // xyz
      order[6] = 4;   // xy
      order[7] = 13;  // xzz
      order[8] = 5;   // xz
      order[9] = 1;   // x
      order[10] = 15; // yyy
      order[11] = 18; // yyz
      order[12] = 8;  // yy
      order[13] = 16; // yzz
      order[14] = 6;  // yz
      order[15] = 2;  // y
      order[16] = 19; // zzz
      order[17] = 9;  // zz
      order[18] = 3;  // z
      order[19] = 0;  // 1
      break;
    }

    // order[VXL_INDEX] = RPC00A_INDEX
    case vpgl_rational_order::RPC00A:
    {
      order[0] = 11;  // xxx
      order[1] = 12;  // xxy
      order[2] = 13;  // xxz
      order[3] = 8;   // xx
      order[4] = 14;  // xyy
      order[5] = 7;   // xyz
      order[6] = 4;   // xy
      order[7] = 17;  // xzz
      order[8] = 5;   // xz
      order[9] = 1;   // x
      order[10] = 15; // yyy
      order[11] = 16; // yyz
      order[12] = 9;  // yy
      order[13] = 18; // yzz
      order[14] = 6;  // yz
      order[15] = 2;  // y
      order[16] = 19; // zzz
      order[17] = 10; // zz
      order[18] = 3;  // z
      order[19] = 0;  // 1
      break;
    }

    default:
    {
      throw std::invalid_argument("vpgl_rational_order not recognized");
    }
  }
  return order;
}

// string from vpgl_rational_order
std::string
vpgl_rational_order_func::to_string(vpgl_rational_order choice)
{
  switch (choice)
  {
    case vpgl_rational_order::VXL:
      return "VXL";
    case vpgl_rational_order::RPC00B:
      return "RPC00B";
    case vpgl_rational_order::RPC00A:
      return "RPC00A";
    default:
      throw std::invalid_argument("vpgl_rational_order not recognized");
  }
}

// vpgl_rational_order from string
vpgl_rational_order
vpgl_rational_order_func::from_string(const std::string & buf)
{
  if (buf.find("VXL") != std::string::npos)
    return vpgl_rational_order::VXL;
  else if (buf.find("RPC00B") != std::string::npos)
    return vpgl_rational_order::RPC00B;
  else if (buf.find("RPC00A") != std::string::npos)
    return vpgl_rational_order::RPC00A;
  else
    throw std::invalid_argument("string not recognized as vpgl_rational_order");
}

// define vpgl_rational_order_func static initializer_list in namespace
constexpr std::array<vpgl_rational_order, 3> vpgl_rational_order_func::initializer_list;
