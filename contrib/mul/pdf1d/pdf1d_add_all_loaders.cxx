#include <pdf1d/pdf1d_gaussian.h>
#include <pdf1d/pdf1d_gaussian_builder.h>
#include <pdf1d/pdf1d_mixture.h>
#include <pdf1d/pdf1d_mixture_builder.h>

void pdf1d_add_all_loaders()
{
  vsl_add_to_binary_loader(pdf1d_gaussian());
  vsl_add_to_binary_loader(pdf1d_gaussian_builder());
  vsl_add_to_binary_loader(pdf1d_mixture());
  vsl_add_to_binary_loader(pdf1d_mixture_builder());
}
