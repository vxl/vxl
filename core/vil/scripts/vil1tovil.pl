#! /bin/sh
exec perl -w -pi.old -x $0 ${1+"$@"}
#!perl
#line 5
# Does global search and replace of vil1 code with vil equivalent in named file
# Backup saved to filename.old
undef $/;
if (undef($msg) && $#ARGV<1)
{
    $msg=1;
    print STDERR "vil1tovil.pl filename [filename ...]\n";
    print STDERR "Performs numerous substitutions to convert from vil1 to vil in named file\n";
    print STDERR "(Backup saved to filename.old)\n";
    print STDERR "BEWARE:\nthis is only a first, rough conversion; hand editing will probably be necessary\n";
}

# just filenames and macros
s!\b(vxl|core)/vil1(/\S+)*/vil1_\b!core/vil$1/vil_!g;
s!\b(vxl|core)/vil1\b!core/vil!g;
s!\bvil1/vil1_!vil/vil_!g;
s/\bVIL1_/VIL_/g;

# Common functions
s/\bvil1_copy\b/vil_copy_deep/g;
s!\bvil/vil_skip\b!vil/vil_decimate!g;
s/\bvil1_skip/vil_decimate/g;
s/\bvil1_clamp/vil_clamp/g;
s!\bvil/vil_convolve(_simple)?\b!vil/algo/vil_convolve_2d!g;
s/\bvil1_convolve_1d_x/vil_convolve_1d/g;
s/\bvil1_crop/vil_crop/g;
s!\bvil/vil_flipud\b!vil/vil_flip!g;
s/\bvil1_flipud/vil_flip_ud/g;
s/\bvil1_load(_raw)?\b/vil_load/g;
s/\bvil1_save/vil_save/g;
s!\bvil/vil_image_as\b!vil/vil_convert!g;
s/\bvil1_image_as_byte/vil_convert_cast<vxl_byte>/g;
s/\bvil1_image_as_([a-z0-9_]+)/vil_convert_cast<$1>/g;
s!\bvil/vil_ncc\b!vil/algo/vil_normalised_correlation_2d!g;
s/\bvil1_ncc\b/vil_normalised_correlation_2d</g;
s!\bvil/vil_resample\b!vil/vil_resample_bilin!g;
s/\bvil1_new/vil_new/g;
s!\bvil/vil_ssd\b!vil/vil_math!g;
s/\bvil1_ssd/vil_math_ssd/g;
s!\bvil/vil_scale_intensities\b!vil/vil_math!g;
s/\bvil1_scale_intensities/vil_math_scale_and_offset_values/g;
s!(.*)vil/vil_(16|32)bit\b(.*)!$1vil/vil_stream_read$3\n$1vil/vil_stream_write$3!g;
s/\bvil1_(16|32)bit_/vil_stream_/g;


# Methods that have changed a lot
s/\bget_section\s*\(/get_view\( \/* Fix params : remove first (becomes return value) and interchange third and fourth *\/ : /g;
s/\bput_section\s*\(/put_view\( \/* Fix params : combine first, fourth and fifth into new first argument *\/ : /g;
s/\bvil1_convolve_simple\s*\(/vil_convolve_2d( \/* Fix params *\/ : /g;
s/\bvil1_pixel_format\s*\(\s*([^)]*)\)/$1.vil_pixel_format()/g;
s/\b(\S+)\s*(\.|-\>)\s*components\s*\(\s*\)/vil_pixel_format_num_components($1$2pixel_format())/g;
s/\bvil1_resample\s*\(/vil_resample_bilin( \/* Fix params : return value becomes second argument *\/ : /g;

# Methods that have changed name
s/(\.|-\>)\s*width\(\s*\)/$1ni()/g;
s/(\.|-\>)\s*height\(\s*\)/$1nj()/g;
s/(\.|-\>)\s*planes\(\s*\)/$1nplanes()/g;

# Classes
s!\bvil/vil_image\b!vil/vil_image_view!g;
s!\bvil/vil_memory_image_of\b!vil/vil_image_view!g;
s!\bvil/vil_memory_image\b!vil/vil_image_view_base!g;
s!\bvil/vil_pyramid\b!vimt/vimt_image_pyramid!g;
s!\bvil/vil_image_impl\b!vil/vil_image_resource!g;
s/\bvil1_image_impl\s*\*/vil_image_resource_sptr/g;
s/\bvil1_image_impl\b/vil_image_resource/g;
s/\bvil1_memory_image_of\b/vil_image_view/g;
s/\bvil1_memory_image\b/vil_image_view_base_sptr/g;
s/\bvil1_image\b/vil_image_resource/g;
s/\bvil1_pyramid/vimt_image_pyramid/g;
s/\bvil1_file_format/vil_file_format/g;

# types
s/\bvil1_property_/vil_property_/g;
s/\bvil1_component_format\b/vil_pixel_format/g;

s/\bVIL_(|RGB_|RGBA_)(BYTE|FLOAT|DOUBLE)\b/VIL_PIXEL_FORMAT_$1$2/g;
s/\bVIL_(RGB_UINT|UINT)(16|32)\b/VIL_PIXEL_FORMAT_$1_$2/g;
s/\bVIL_COMPLEX\b/VIL_PIXEL_FORMAT_COMPLEX_FLOAT/g;
s/\bVIL_COMPONENT_FORMAT_UNKNOWN\b/VIL_PIXEL_FORMAT_UNKNOWN/g;
s/\bVIL_COMPONENT_FORMAT_UNSIGNED_INT\b/VIL_PIXEL_FORMAT_UINT_XXX/g;
s/\bVIL_COMPONENT_FORMAT_SIGNED_INT\b/VIL_PIXEL_FORMAT_INT_XXX/g;
s/\bVIL_COMPONENT_FORMAT_IEEE_FLOAT\b/VIL_PIXEL_FORMAT_FLOAT_OR_DOUBLE/g;
s/\bVIL_COMPONENT_FORMAT_COMPLEX\b/VIL_PIXEL_FORMAT_COMPLEX_XXX/g;

s!\bvil/vil_byte\b!vxl_config!g;
s/\bvil1_byte\b/vxl_byte/g;
s!\bvil/vil_rgb_byte\b!vil/vil_rgb!g;
s/\bvil1_rgb_byte/vil_rgb<vxl_byte>/g;
s/\bvil1_rgb\s*\<\s*unsigned\s+char\s*\>/vil_rgb<vxl_byte>/g;

# Stuff that was just copied from vil1 to vil
s/\bvil1_rgba</vil_rgba</g;
s/\bvil1_rgb</vil_rgb</g;
s/\bvil1_stream/vil_stream/g;
s/\bvil1_open/vil_open/g;
