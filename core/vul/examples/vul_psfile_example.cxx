//:
// \file
// \brief Example use of the vul_psfile class
// \author Peter Vanroose
// \date 17 March 2004
//-----------------------------------------------------------------------------
#include <vul/vul_psfile.h>

int main(int argc, char* argv[])
{
  vul_psfile f("vul_psfile_example.ps",true);
  f.set_paper_layout(vul_psfile::CENTER);

  // Put colour images inside small circles (to be drawn later):
  unsigned char buf[300*400*3];
  for (int x=0; x<300; ++x) for (int y=0; y<400; ++y)
  {
    buf[3*(x+300*y)] = buf[3*(x+300*y)+1] = buf[3*(x+300*y)+2] = 192; // light grey
    if (y<=190 || y>=210) continue;
    int c = 15 + 30*(x/30); // centre of most nearby circle
    if ((x-c)*(x-c)+(y-200)*(y-200) >= 100) continue;
    buf[3*(x+300*y)  ] = x%256;
    buf[3*(x+300*y)+1] = ((x-150)*(y-185)/16) % 256;
    buf[3*(x+300*y)+2] = (y/3+5)%256;
  }
  f.print_color_image(buf,300,400);

  // Draw 41 horizontal red lines of length 300 at Y coordinates 0, 10, .. 400:
  f.set_fg_color(1.f,0.f,0.f);
  f.set_line_width(0.2f);
  for (int i=0; i<=40; ++i)
    f.line(0.f, 10.f*i, 300.f, 10.f*i); // (x1,y1,x2,y2)

  // Draw green circles centered at the central line and touching the neighbour lines:
  f.set_fg_color(0.f,1.f,0.f);
  f.set_line_width(0.1f);
  for (int i=0; i<10; ++i)
    f.circle(15.f+30*i,200.f,10.f); // (cx,cy,r)

  // Draw blue points inbetween the 41 lines:
  f.set_fg_color(0.f,0.f,1.f);
  for (int i=0; i<40; ++i)
    f.point(150.0f,5.f+10*i,1.5f); // (x,y,size)

  // And finally a large, thick yellow ellipse, slanted 45 degrees:
  f.set_fg_color(1.f,1.f,0.f);
  f.set_line_width(4.f);
  f.ellipse(150.0f,200.0f,70.0f,151.0f,45); // (cx,cy,A,B,slant)

  return 0;
}
