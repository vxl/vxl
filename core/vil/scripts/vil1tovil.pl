#!/usr/bin/perl
# Does global search and replace of vil1 code with vil equivalent in named file
# Backup saved to filename.old
$filename=$ARGV[0];
if (@ARGV!=1)
{
    print "vil1tovil.pl filename\n";
    print "Performs numerous substitutions to convert from vil1 to vil in named file\n";
    print "(Backup saved to filename.old)\n";
    exit 0;
}

# Create backup
$backup = "$filename.old";
rename $filename, $backup or die "Unable to create backup $backup";
# Copy backup into original file
open(OUTFILE,">$filename") or die "Can't open $filename";
open(INFILE,"<$backup") or die "Can't open $backup";

while (<INFILE>)
{
    # just Filenames
    s/\b(vxl|core)\/vil1\b/core\/vil/g;
    s/\bvil\/vil1_fwd/vil\/vil_fwd/g;

    # Common functions
    s/\bvil1\/vil1_copy\b/vil\/vil_copy/g;
    s/vil1_copy/vil_copy_deep/g;
    s/\bvil1\/vil1_skip\b/vil\/vil_decimate/g;
    s/vil1_skip/vil_decimate/g;
    s/\bvil1\/vil1_clamp\b/vil\/vil_clamp/g;
    s/vil1_clamp/vil_clamp/g;
    s/\bvil1\/vil1_convolve\b/vil\/algo\/vil_convolve_1d/g;
    s/vil1_convolve_1d_x/vil_convolve_1d/g;
    s/\bvil1\/vil1_crop\b/vil\/vil_crop/g;
    s/vil1_crop/vil_crop/g;
    s/\bvil1\/vil1_flipud\b/vil\/vil_flip/g;
    s/vil1_flipud/vil_flip_ud/g;
    s/\bvil1\/vil1_load\b/vil\/vil_load/g;
    s/vil1_load/vil_load/g;
    s/\bvil1\/vil1_save\b/vil\/vil_save/g;
    s/vil1_save/vil_save/g;
    s/\bvil1\/vil1_image_as\b/vil\/vil_convert/g;
    s/vil1_image_as_/vil_convert_cast</g;
    s/\bvil1\/vil1_ncc\b/vil\/algo\/vil_normalised_correlation_2d/g;
    s/vil1_ncc/vil_normalised_correlation_2d</g;
    s/\bvil1\/vil1_new\b/vil\/vil_new/g;
    s/vil1_new/vil_new/g;
    s/\bvil1\/vil1_ssd\b/vil\/vil_math/g;
    s/vil1_ssd/vil_math_ssd/g;
    s/\bvil1\/vil1_scale_intensities\b/vil\/vil_math/g;
    s/vil1_scale_intensities/vil_math_scale_and_offset_values/g;
    s/\bvil1\/vil1_32bit\b/vil\/vil_stream_32bit/g;
    s/vil1_32bit_/vil_stream_32bit_/g;
    s/\bvil1\/vil1_16bit\b/vil\/vil_stream_16bit/g;
    s/vil1_16bit_/vil_stream_16bit_/g;


    # Methods that have changed a lot
    s/\bget_section\(/get_view\( Fix params/g;
    s/\bput_section\(/put_view\( Fix params/g;

    # Methods that have changed name
    s/\.width\(\)/.ni()/g;
    s/\.height\(\)/.nj()/g;

    # Classes
    s/\bvil1\/vil1_memory_image_of\b/vil\/vil_image_view/g;
    s/\bvil_memory_image_of\b/vil_image_view/g;
    s/\bvil1\/vil1_memory_image\b/vil\/vil_image_view_base/g;
    s/\bvil_memory_image\b/vil_image_view_base_sptr/g;
    s/\bvil1\/vil1_image\b/vil\/vil_image_resource/g;
    s/\bvil_image\b/vil_image_resource/g;
    s/\bvil1\/vil1_pyramid\b/vimt\/vimt_image_pyramid/g;
    s/vil1_pyramid/vimt_image_pyramid/g;
    s/\bvil1\/vil1_file_format\b/vil\/vil_file_format/g;
    s/vil1_file_format/vil_file_format/g;

    # types
    s/\bvil1\/vil1_property\b/vil\/vil_property/g;
    s/vil1_property_/vil_property_/g;

    s/\bvil1\/vil1_byte\b/vxl_config/g;
    s/\bvil1_byte\b/vxl_byte/g;
    s/\bvil1\/vil1_rgb_byte\b/vil\/vil_rgb/g;
    s/vil1_rgb_byte/vil_rgb<vxl_byte>/g;

    # Stuff that was just copied from vil1 to vil
    s/\bvil1\/vil1_rgba\b/vil\/vil_rgba/g;
    s/vil1_rgba</vil_rgba</g;
    s/\bvil1\/vil1_rgb\b/vil\/vil_rgb/g;
    s/vil1_rgb</vil_rgb</g;

    s/\bvil1\/vil1_stream/vil\/vil_stream/g;
    s/vil1_stream/vil_stream/g;

    s/\bvil\/vil1_open\b/vil\/vil_open/g;
    s/vil1_open/vil_open/g;


    print OUTFILE;
}
