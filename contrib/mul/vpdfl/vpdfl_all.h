#ifndef vpdfl_all_h_
#define vpdfl_all_h_

#include <vpdfl/vpdfl_axis_gaussian.h>
#include <vpdfl/vpdfl_axis_gaussian_builder.h>
#include <vpdfl/vpdfl_axis_gaussian_sampler.h>
#include <vpdfl/vpdfl_builder_base.h>
#include <vpdfl/vpdfl_gaussian.h>
#include <vpdfl/vpdfl_gaussian_builder.h>
#include <vpdfl/vpdfl_gaussian_sampler.h>
#include <vpdfl/vpdfl_mixture.h>
#include <vpdfl/vpdfl_mixture_builder.h>
#include <vpdfl/vpdfl_mixture_sampler.h>
#include <vpdfl/vpdfl_pc_gaussian.h>
#include <vpdfl/vpdfl_pc_gaussian_builder.h>
#include <vpdfl/vpdfl_pc_gaussian_sampler.h>
#include <vpdfl/vpdfl_pdf_base.h>
#include <vpdfl/vpdfl_prob_chi2.h>
#include <vpdfl/vpdfl_sampler_base.h>

static void vpdfl_add_all_loaders()
{
  vsl_add_to_binary_loader(vpdfl_axis_gaussian());
  vsl_add_to_binary_loader(vpdfl_axis_gaussian_builder());
  vsl_add_to_binary_loader(vpdfl_gaussian());
  vsl_add_to_binary_loader(vpdfl_gaussian_builder());
  vsl_add_to_binary_loader(vpdfl_mixture());
  vsl_add_to_binary_loader(vpdfl_mixture_builder());
  vsl_add_to_binary_loader(vpdfl_pc_gaussian());
  vsl_add_to_binary_loader(vpdfl_pc_gaussian_builder());
}

#endif
