#include <bsta/algo/bsta_parzen_updater.txx>
#include <bsta/bsta_parzen_sphere.h>

typedef bsta_parzen_sphere<float, 3> parzen;

BSTA_PARZEN_UPDATER_INSTANTIATE(parzen);
