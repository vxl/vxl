// test_vul_psfile
// Author: Peter Vanroose
// Created: 29 January 2002
//-----------------------------------------------------------------------------
#include <testlib/testlib_test.h>

#include <vul/vul_psfile.h>
#include <vul/vul_temp_filename.h>
#include <vpl/vpl.h> // for unlink()

void test_psfile()
{
  vcl_string filename = vul_temp_filename()+".ps";
  vul_psfile f(filename.c_str());
  TEST("temp file", bool(f), true);
  vcl_cout << "Writing PostScript file to " << filename << '\n';

  // select random test:
  bool TEST_GREY = filename.c_str()[9] > 'Z';
  bool TEST_COLOUR = filename.c_str()[10] > 'Z';

  f.set_paper_type(vul_psfile::A4);
  f.set_paper_orientation(vul_psfile::PORTRAIT);
  f.set_bg_color(0.9f,0.1f,0.0f); // light red-ish
  f.set_fg_color(0.0f,0.0f,0.1f);   // dark blue-ish
  f.set_line_width(0.2f);

  f.postscript_header();

  const int wd = 32, ht = 32;

  if (TEST_GREY) {
    vcl_cout << "writing a greyscale image\n";
    f.set_scale_x(800);
    f.set_scale_y(600);
    unsigned char buf[wd*ht];
    for (int x=0; x<32; ++x) for (int y=0; y<32; ++y)
      buf[x+wd*y] = ((x-wd/2)*(y-ht/2)/16) & 0xff;
    f.print_greyscale_image(buf,wd,ht);
  }
  else if (TEST_COLOUR) {
    vcl_cout << "writing a colour image\n";
    f.set_scale_x(900);
    f.set_scale_y(700);
    unsigned char bufc[wd*ht*3];
    for (int x=0; x<32; ++x) for (int y=0; y<32; ++y) {
      bufc[3*(x+wd*y)  ] = x%(1<<8);
      bufc[3*(x+wd*y)+1] = ((x-wd/2)*(y-ht/2)/16) % (1<<8);
      bufc[3*(x+wd*y)+2] = (y/3)%(1<<8);
    }
    f.print_color_image(bufc,wd,ht);
  }
  else {
    vcl_cout << "writing a set of lines, points, circles and ellipses\n";
    for (int i=0; i<100; ++i)
      f.line(100,100+5*i,400-3*i,100);

    for (int i=0; i<100; ++i)
      f.point(300+i,400-i,0.5);

    for (int i=0; i<10; ++i)
      f.circle(210+5*i,210,10+5*i);

    for (int i=0; i<10; ++i)
      f.ellipse(150+5*i,350+5*i,10+5*i,20+10*i,45);
  }

  vcl_string command = "ghostview " + filename;
#if INTERACTIVE
  system(command.c_str());
#endif
  // remove PostScript file
  vpl_unlink(filename.c_str());
}

TESTMAIN(test_psfile);
