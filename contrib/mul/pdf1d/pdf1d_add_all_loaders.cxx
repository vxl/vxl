#include <pdf1d/pdf1d_all.h>

void pdf1d_add_all_loaders()
{
  vsl_add_to_binary_loader(pdf1d_gaussian());
  vsl_add_to_binary_loader(pdf1d_gaussian_builder());
  vsl_add_to_binary_loader(pdf1d_mixture());
  vsl_add_to_binary_loader(pdf1d_mixture_builder());
}
