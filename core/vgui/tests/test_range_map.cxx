// This is core/vgui/tests/test_range_map.cxx
#include <testlib/testlib_test.h>
#include <vxl_config.h>
#include <vgui/vgui_range_map.h>
#include <vil/vil_pixel_traits.h>
#include <vcl_vector.h>
static void test_range_map()
{
  //Test the range map parameter class
  long double ma = 0, mb = 1, mc = 2, md = 3, me = 4, mf = 5;
  vgui_range_map_params_sptr rmpL =  new vgui_range_map_params(ma, mb);
  vgui_range_map_params_sptr rmpL1 =  new vgui_range_map_params(*rmpL);
  vgui_range_map_params_sptr rmpL2 =  new vgui_range_map_params(ma, mc);

  rmpL->print(vcl_cout);
  rmpL1->print(vcl_cout);
  rmpL2->print(vcl_cout);
  bool equal = *rmpL==*rmpL1;
  bool not_equal = *rmpL!=*rmpL2;
  TEST("range map params luminosity", equal&&not_equal, true);

  vgui_range_map_params_sptr rmpRGB =
    new vgui_range_map_params(ma, mb, ma, mc, ma, md);
  vgui_range_map_params_sptr rmpRGB1 =  new vgui_range_map_params(*rmpRGB);
  vgui_range_map_params_sptr rmpRGB2 =
    new vgui_range_map_params(ma, mb, ma, mc, ma, me);

  rmpRGB->print(vcl_cout);
  rmpRGB1->print(vcl_cout);
  rmpRGB2->print(vcl_cout);
  bool RGBequal = *rmpRGB==*rmpRGB1;
  bool RGBnot_equal = *rmpRGB!=*rmpRGB2;
  TEST("range map params RGB", RGBequal&&RGBnot_equal, true);

  vgui_range_map_params_sptr rmpRGBA =
    new vgui_range_map_params(ma, mb, ma, mc, ma, md, ma, me);
  vgui_range_map_params_sptr rmpRGBA1 =  new vgui_range_map_params(*rmpRGBA);
  vgui_range_map_params_sptr rmpRGBA2 =
    new vgui_range_map_params(ma, mb, ma, mc, ma, md, ma, mf);

  rmpRGBA->print(vcl_cout);
  rmpRGBA1->print(vcl_cout);
  rmpRGBA2->print(vcl_cout);
  bool RGBAequal = *rmpRGBA==*rmpRGBA1;
  bool RGBAnot_equal = *rmpRGBA!=*rmpRGBA2;
  TEST("range map params RGBA", RGBAequal&&RGBAnot_equal, true);


  //The following types are the most likely to be used in practice
  //
  // bool pixel type
  //
  vgui_range_map_params_sptr Lrmpb =  new vgui_range_map_params(0,1);
  vgui_range_map<vxl_byte> Lrmb(*Lrmpb);
  vxl_byte valfalse = Lrmb.map_L_pixel(0);
  vxl_byte valtrue = Lrmb.map_L_pixel(1);
  TEST("bool values", !valfalse&&valtrue==255, true);

  //
  //unsigned 8 bit (unsigned char)
  //
  long double min = 0, max = 255;
  vgui_range_map_params_sptr Lrmpuc = new vgui_range_map_params(min, max);
  vgui_range_map<vxl_byte> Lrmuc(*Lrmpuc);
  vxl_byte val8 = Lrmuc.map_L_pixel(127);
  vcl_vector<vxl_byte> map8 = Lrmuc.Lmap();
  vcl_vector<float> fmap8 = Lrmuc.fLmap();
  vxl_byte val8f = (vxl_byte)(fmap8[127]*255 + 0.5);
  unsigned size8 = Lrmuc.map_size();
  bool map_good = false;
  if(Lrmuc.mapable())
    {
      map_good = true;
      for(unsigned i = 0; i<size8; ++i)
        if(i>=0&&i<=255)
          map_good = map_good&&map8[i]==(vxl_byte)i;
    }
  TEST("vxl_byte values", val8==127&&val8f==127&&!Lrmuc.offset()&&map_good, true);

  //
  //signed 8 bit (signed char)
  //
  vgui_range_map_params_sptr Lrmpsc = new vgui_range_map_params(-128,127);
  vgui_range_map<vxl_sbyte> Lrmsc(*Lrmpsc);
  vxl_byte valsc = Lrmsc.map_L_pixel(-64);
  vcl_vector<vxl_byte> mapsc = Lrmsc.Lmap();
  unsigned sizesc = Lrmsc.map_size();
  int offsc = Lrmsc.offset();
  map_good = false;
  if(Lrmsc.mapable())
    {
      map_good = true;
      for(int i = -128; i<=127; ++i)
        map_good = map_good&&(mapsc[i+offsc]==(i+offsc));
    }
  TEST("signed char values", sizesc == 256&&valsc==64&&Lrmsc.offset()==128&&map_good, true);

//
// signed 16 bit (short)
//
  vgui_range_map_params_sptr Lrmpsh = new vgui_range_map_params(-128,127);
  vgui_range_map<short> Lrmsh(*Lrmpsh);
  vxl_byte valsh = Lrmsh.map_L_pixel(0);
  vcl_vector<vxl_byte> mapsh = Lrmsh.Lmap();
  unsigned sizesh = Lrmsh.map_size();
  int offsh = Lrmsh.offset();
  map_good = false;
  if(Lrmsh.mapable())
    {
	  map_good = true;
      for(int i = -offsh; i<=(offsh-1); ++i)
        if(i>=-128 && i<=127)
          map_good = map_good&&mapsc[i+offsc]==(i+offsc);
    }
  TEST("short values", sizesh==65536&&valsh==128&&Lrmsh.offset()==32768&&map_good, true);

//
// unsigned 16 bit (unsigned short)
//
  vgui_range_map_params_sptr Lrmpush = new vgui_range_map_params(0,255);
  vgui_range_map<unsigned short> Lmush(*Lrmpush);
  vxl_byte valush = Lmush.map_L_pixel(128);
  vcl_vector<vxl_byte> mapush = Lmush.Lmap();
  unsigned sizeush = Lmush.map_size();
  int offush = Lmush.offset();
  map_good = false;
  if(Lmush.mapable())
    {
      map_good = true;
      for(int i = 0; i<=(sizeush-1); ++i)
        if(i>=0 && i<=255)
          map_good = map_good&&mapush[i]==(i);
    }
  TEST("unsigned short values", sizeush==65536&&valsh==(vxl_byte)128&&offush==0&&map_good, true);

//
// float
//
  vgui_range_map_params_sptr Lrmpf = new vgui_range_map_params(-128,127);
  vgui_range_map<float> Lrmf(*Lrmpf);
  map_good = true;
  for (float x = -128.0f; x<128.0f; ++x)
    map_good = map_good&&(Lrmf.map_L_pixel(x)==(vxl_byte)(x+128.0f));

  TEST("float values", map_good, true);

  //
  // RGB unsigned char
  //
  long double minR = 0, maxR = 255, minG=minR, maxG=maxR, minB=minR, maxB=maxR;
  vgui_range_map_params_sptr RGBrmpuc =
    new vgui_range_map_params(minR, maxR, minG, maxG, minB, maxB);
  vgui_range_map<vxl_byte> RGBrmuc(*RGBrmpuc);
  vxl_byte Rval = RGBrmuc.map_R_pixel(127);
  vxl_byte Gval = RGBrmuc.map_G_pixel(127);
  vxl_byte Bval = RGBrmuc.map_B_pixel(127);
  vcl_vector<vxl_byte> Rmap = RGBrmuc.Rmap();
  vcl_vector<vxl_byte> Gmap = RGBrmuc.Gmap();
  vcl_vector<vxl_byte> Bmap = RGBrmuc.Bmap();
  vcl_vector<float> fRmap = RGBrmuc.fRmap();
  vcl_vector<float> fGmap = RGBrmuc.fGmap();
  vcl_vector<float> fBmap = RGBrmuc.fBmap();
  unsigned RGBsize8 = RGBrmuc.map_size();
  map_good = false;
  if (RGBrmuc.mapable())
  {
    map_good = true;
    for (unsigned i = 0; i<RGBsize8 && i<256; ++i)
      map_good = map_good &&
                 (vxl_byte)(fRmap[i]*255 + 0.5)==Rmap[i] &&
                 (vxl_byte)(fGmap[i]*255 + 0.5)==Gmap[i] &&
                 (vxl_byte)(fBmap[i]*255 + 0.5)==Bmap[i];
  }

  TEST("RGB vxl_byte values", Rval==127&&Gval==127&&Bval==127&&!RGBrmuc.offset()&&map_good, true);
}

TESTMAIN(test_range_map);
