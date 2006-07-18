//:
// \file
// \brief Testing vul_psfile class
// \author Peter Vanroose
// \date 29 January 2002
// \verbatim
// Modifications
//  7 Jan 2003 - Peter Vanroose - complete rewrite: full coverage of vul_psfile
// \endverbatim
//-----------------------------------------------------------------------------
#include <testlib/testlib_test.h>

#include <vul/vul_psfile.h>
#include <vul/vul_temp_filename.h>
#include <vpl/vpl.h> // for unlink()

const int wd = 171, ht = 323;

static void write_greyscale(vul_psfile& f)
{
  vcl_cout << "writing a greyscale image\n";
  unsigned char buf[wd*ht];
  for (int x=0; x<wd; ++x) for (int y=0; y<ht; ++y)
    buf[x+wd*y] = ((x-wd/2)*(y-ht/2)/16) & 0xff;
  f.print_greyscale_image(buf,wd,ht);
}

static void write_colour(vul_psfile& f)
{
  vcl_cout << "writing a colour image\n";
  unsigned char buf[wd*ht*3];
  for (int x=0; x<wd; ++x) for (int y=0; y<ht; ++y) {
    buf[3*(x+wd*y)  ] = x%(1<<8);
    buf[3*(x+wd*y)+1] = ((x-wd/2)*(y-ht/2)/16) % (1<<8);
    buf[3*(x+wd*y)+2] = (y/3)%(1<<8);
  }
  f.print_color_image(buf,wd,ht);
}

static void write_geometry(vul_psfile& f)
{
  vcl_cout << "writing a set of lines, points, circles and ellipses\n";
  for (int i=0; i<100; ++i)
    f.line(100.0f,100.0f+5*i,400.0f-3*i,100.0f);

  for (int i=0; i<100; ++i)
    f.point(300.0f+i,400.0f-i,0.5f);

  f.set_line_width(f.line_width()*1.5f);
  for (int i=0; i<10; ++i)
    f.circle(210.0f+5*i,210.0f,10.0f+5*i);

  f.set_line_width(f.line_width()*0.5f);
  for (int i=0; i<10; ++i)
    f.ellipse(150.0f+5*i,350.0f+5*i,10.0f+5*i,20.0f+10*i,45);
}

static void test_psfile_grey_portrait()
{
  vcl_string filename = vul_temp_filename()+".ps";
  vul_psfile f(filename.c_str(), true);
  TEST("temp file", bool(f), true);

  f.set_paper_type(vul_psfile::A4);
  f.set_paper_orientation(vul_psfile::PORTRAIT);
  f.set_bg_color(0.9f,0.1f,0.0f); // light red-ish
  f.set_fg_color(0.0f,0.0f,0.1f);   // dark blue-ish
  f.set_line_width(0.2f); // has no effect here: only for geometry objects
  f.set_scale_x(150); f.set_scale_y(75); // default scale is 100

  write_greyscale(f);
  vcl_cout << "Writing PostScript file to " << filename << '\n';

#ifdef INTERACTIVE
  vcl_string command = "ghostview " + filename;
  system(command.c_str());
#endif
#ifndef LEAVE_PSFILE_BEHIND
  // remove PostScript file
  vpl_unlink(filename.c_str());
#endif
}

static void test_psfile_colour_portrait()
{
  vcl_string filename = vul_temp_filename()+".ps";
  vul_psfile f(filename.c_str(), true);
  TEST("temp file", bool(f), true);

  f.set_paper_type(vul_psfile::A3);
  f.set_paper_orientation(vul_psfile::PORTRAIT);
  f.set_bg_color(0.1f,0.9f,0.0f);
  f.set_fg_color(0.1f,0.0f,0.1f);
  f.set_reduction_factor(3);

  write_colour(f);
  vcl_cout << "Writing PostScript file to " << filename << '\n';

#ifdef INTERACTIVE
  vcl_string command = "ghostview " + filename;
  system(command.c_str());
#endif
#ifndef LEAVE_PSFILE_BEHIND
  // remove PostScript file
  vpl_unlink(filename.c_str());
#endif
}

static void test_psfile_geometry_portrait()
{
  vcl_string filename = vul_temp_filename()+".ps";
  vul_psfile f(filename.c_str(), true);
  TEST("temp file", bool(f), true);

  f.set_paper_type(vul_psfile::ELEVEN_BY_SEVENTEEN);
  f.set_paper_orientation(vul_psfile::PORTRAIT);
  f.set_bg_color(0.0f,0.1f,0.9f);
  f.set_fg_color(0.0f,0.1f,0.0f);
  f.set_line_width(0.2f);

  write_geometry(f);
  vcl_cout << "Writing PostScript file to " << filename << '\n';

#ifdef INTERACTIVE
  vcl_string command = "ghostview " + filename;
  system(command.c_str());
#endif
#ifndef LEAVE_PSFILE_BEHIND
  // remove PostScript file
  vpl_unlink(filename.c_str());
#endif
}

static void test_psfile_grey_landscape()
{
  vcl_string filename = vul_temp_filename()+".ps";
  vul_psfile f(filename.c_str(), true);
  TEST("temp file", bool(f), true);

  f.set_paper_type(vul_psfile::US_NORMAL);
  f.set_paper_orientation(vul_psfile::LANDSCAPE);
  f.set_paper_layout(vul_psfile::MAX); // scale to US_NORMAL
  f.set_bg_color(0.5f,0.9f,0.9f);
  f.set_fg_color(0.5f,0.0f,0.0f);
  f.set_reduction_factor(2);
  f.set_scale_x(120); f.set_scale_y(120); // default scale is 100

  write_greyscale(f);
  vcl_cout << "Writing PostScript file to " << filename << '\n';

#ifdef INTERACTIVE
  command = "ghostview " + filename;
  system(command.c_str());
#endif
#ifndef LEAVE_PSFILE_BEHIND
  // remove PostScript file
  vpl_unlink(filename.c_str());
#endif
}

static void test_psfile_colour_landscape()
{
  vcl_string filename = vul_temp_filename()+".ps";
  vul_psfile f(filename.c_str(), true);
  TEST("temp file", bool(f), true);

  f.set_paper_type(vul_psfile::B5);
  f.set_paper_orientation(vul_psfile::LANDSCAPE);
  f.set_paper_layout(vul_psfile::MAX); // scale to B5
  f.set_bg_color(0.9f,0.5f,0.9f);
  f.set_fg_color(0.0f,0.5f,0.0f);

  write_colour(f);
  vcl_cout << "Writing PostScript file to " << filename << '\n';

#ifdef INTERACTIVE
  command = "ghostview " + filename;
  system(command.c_str());
#endif
#ifndef LEAVE_PSFILE_BEHIND
  // remove PostScript file
  vpl_unlink(filename.c_str());
#endif
}

static void test_psfile_geometry_landscape()
{
  vcl_string filename = vul_temp_filename()+".ps";
  vul_psfile f(filename.c_str(), true);
  TEST("temp file", bool(f), true);

  f.set_paper_type(vul_psfile::US_LEGAL);
  f.set_paper_orientation(vul_psfile::LANDSCAPE);
  f.set_paper_layout(vul_psfile::MAX); // scale to US_LEGAL
  f.set_bg_color(0.9f,0.9f,0.5f);
  f.set_fg_color(0.0f,0.0f,0.5f);
  f.set_line_width(0.8f);
  f.set_reduction_factor(4); // should have no effect: only on images

  write_geometry(f);
  vcl_cout << "Writing PostScript file to " << filename << '\n';

#ifdef INTERACTIVE
  command = "ghostview " + filename;
  system(command.c_str());
#endif
#ifndef LEAVE_PSFILE_BEHIND
  // remove PostScript file
  vpl_unlink(filename.c_str());
#endif
}

static void test_psfile_combined()
{
  vcl_string filename = vul_temp_filename()+".ps";
  vul_psfile f(filename.c_str(), true);
  TEST("temp file", bool(f), true);

  write_greyscale(f);
  write_colour(f);
  write_geometry(f);
  vcl_cout << "Writing PostScript file to " << filename << '\n';

#ifdef INTERACTIVE
  command = "ghostview " + filename;
  system(command.c_str());
#endif
#ifndef LEAVE_PSFILE_BEHIND
  // remove PostScript file
  vpl_unlink(filename.c_str());
#endif
}

static void test_psfile() {
  test_psfile_grey_portrait();
  test_psfile_colour_portrait();
  test_psfile_geometry_portrait();
  test_psfile_grey_landscape();
  test_psfile_colour_landscape();
  test_psfile_geometry_landscape();
  test_psfile_combined();
}

//TESTMAIN(test_psfile);
int test_psfile(int, char*[])
{
  testlib_test_start("test_psfile");

  test_psfile();

  return testlib_test_summary();
}
