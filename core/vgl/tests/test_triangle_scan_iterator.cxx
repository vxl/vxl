#include <vcl/vcl_iostream.h>

#include <vgl/vgl_triangle_scan_iterator.h>
#include <vgl/vgl_polygon_scan_iterator.h>

int main(int, char **)
{
  float x[3] = { 10.23, 20.12, 30.73 };
  float y[3] = { 20.54, 10.39, 30.11 };

  vgl_triangle_scan_iterator ti;
  ti.a.x = x[0]; ti.a.y = y[0];
  ti.b.x = x[1]; ti.b.y = y[1];
  ti.c.x = x[2]; ti.c.y = y[2];
  
  vgl_polygon p(x, y, 3);
  vgl_polygon_scan_iterator pi(p, false);

  cerr << "triangle:" << endl;
  for (ti.reset(); ti.next(); )
    cerr << ti.scany() << ' ' << ti.startx() << ' ' << ti.endx() << endl;
  cerr << endl;

  cerr << "polygon:" << endl;
  for (pi.reset(); pi.next(); )
    cerr << pi.scany() << ' ' << pi.startx() << ' ' << pi.endx() << endl;
  cerr << endl;

  return 0;
}
