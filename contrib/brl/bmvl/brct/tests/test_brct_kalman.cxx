#include <vcl_iostream.h>
#include <bmvl/brct/kalman_filter.h>

int main()
{
  kalman_filter kf("data/curves.txt");
  return 0;
}
