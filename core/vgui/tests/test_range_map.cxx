// This is core/vgui/tests/test_range_map.cxx
#include <testlib/testlib_test.h>
#include <vxl_config.h>
#include <vgui/vgui_range_map.h>
#include <vil/vil_pixel_traits.h>

static void test_range_map()
{
  //The following types are the most likely to be used in practice
  //
  // bool pixel type
  //
  vgui_range_map_params_sptr rmpb =  new vgui_range_map_params(0,1,1,false);
  vgui_range_map<vxl_byte> rmb(*rmpb);
  vxl_byte valfalse = rmb.map_pixel(0);
  vxl_byte valtrue = rmb.map_pixel(1);
  TEST("bool values", !valfalse&&valtrue==255, true);

  //
  //unsigned 8 bit (unsigned char)
  //
  vgui_range_map_params_sptr rmpuc = new vgui_range_map_params(0,255,1,false);
  vgui_range_map<vxl_byte> rmuc(*rmpuc);
  vxl_byte val8 = rmuc.map_pixel(127);
  vxl_byte* map8 = rmuc.rmap();
  unsigned size8 = rmuc.map_size();
  bool map_good = false;
  if (rmuc.mapable())
  {
    map_good = true;
    for (unsigned i = 0; i<size8; ++i)
      if (i>=0&&i<=255)
        map_good = map_good&&map8[i]==(vxl_byte)i;
  }
  TEST("vxl_byte values", val8==127&&!rmuc.offset()&&map_good, true);

  //
  //signed 8 bit (signed char)
  //
  vgui_range_map_params_sptr rmpsc = new vgui_range_map_params(-128,127,1,false);
  vgui_range_map<vxl_sbyte> rmsc(*rmpsc);
  vxl_byte valsc = rmsc.map_pixel(-64);
  vxl_byte* mapsc = rmsc.rmap();
  unsigned sizesc = rmsc.map_size();
  int offsc = rmsc.offset();
  map_good = false;
  if (rmsc.mapable())
  {
    map_good = true;
    for (int i = -128; i<=127; ++i)
      map_good = map_good&&(mapsc[i+offsc]==(i+offsc));
  }
  TEST("signed char values", valsc==64&&rmsc.offset()==128&&map_good, true);

  //
  // signed 16 bit (short)
  //
  vgui_range_map_params_sptr rmpsh = new vgui_range_map_params(-128,127,1,false);
  vgui_range_map<short> rmsh(*rmpsh);
  vxl_byte valsh = rmsh.map_pixel(0);
  vxl_byte* mapsh = rmsh.rmap();
  unsigned sizesh = rmsh.map_size();
  int offsh = rmsh.offset();
  map_good = false;
  if (rmsh.mapable())
  {
    map_good = true;
    for (int i = -offsh; i<=(offsh-1); ++i)
      if (i>=-128 && i<=127)
        map_good = map_good&&mapsc[i+offsc]==(i+offsc);
  }
  TEST("short values", valsh==128&&rmsh.offset()==32768&&map_good, true);

  //
  // unsigned 16 bit (unsigned short)
  //
  vgui_range_map_params_sptr rmpush = new vgui_range_map_params(0,255,1,false);
  vgui_range_map<unsigned short> rmush(*rmpush);
  vxl_byte valush = rmush.map_pixel(128);
  vxl_byte* mapush = rmush.rmap();
  unsigned sizeush = rmush.map_size();
  int offush = rmush.offset();
  map_good = false;
  if (rmush.mapable())
  {
    map_good = true;
    for (int i = 0; i<=(sizeush-1); ++i)
      if (i>=0 && i<=255)
        map_good = map_good&&mapsc[i]==(i);
  }
  TEST("unsigned short values", valsh==(vxl_byte)128&&offush==0&&map_good, true);

  //
  // float
  //
  vgui_range_map_params_sptr rmpf = new vgui_range_map_params(-128,127,1,false);
  vgui_range_map<float> rmf(*rmpsh);
  map_good = true;
  for (float x = -128.0f; x<128.0f; ++x)
    map_good = map_good&&(rmf.map_pixel(x)==(vxl_byte)(x+128.0f));

  TEST("float values", map_good, true);
}

TESTMAIN(test_range_map);
