// This is mul/vpdfl/vpdfl_add_all_binary_loaders.cxx
#include "vpdfl_add_all_binary_loaders.h"
//:
// \file

#include <vpdfl/vpdfl_axis_gaussian.h>
#include <vpdfl/vpdfl_axis_gaussian_builder.h>
#include <vpdfl/vpdfl_gaussian.h>
#include <vpdfl/vpdfl_gaussian_builder.h>
#include <vpdfl/vpdfl_mixture.h>
#include <vpdfl/vpdfl_mixture_builder.h>
#include <vpdfl/vpdfl_pc_gaussian.h>
#include <vpdfl/vpdfl_pc_gaussian_builder.h>
#include <vpdfl/vpdfl_gaussian_kernel_pdf.h>
#include <vpdfl/vpdfl_gaussian_kernel_pdf_builder.h>

void vpdfl_add_all_binary_loaders()
{
  vsl_add_to_binary_loader(vpdfl_axis_gaussian());
  vsl_add_to_binary_loader(vpdfl_axis_gaussian_builder());
  vsl_add_to_binary_loader(vpdfl_gaussian());
  vsl_add_to_binary_loader(vpdfl_gaussian_builder());
  vsl_add_to_binary_loader(vpdfl_mixture());
  vsl_add_to_binary_loader(vpdfl_mixture_builder());
  vsl_add_to_binary_loader(vpdfl_pc_gaussian());
  vsl_add_to_binary_loader(vpdfl_pc_gaussian_builder());
  vsl_add_to_binary_loader(vpdfl_gaussian_kernel_pdf());
  vsl_add_to_binary_loader(vpdfl_gaussian_kernel_pdf_builder());
}
