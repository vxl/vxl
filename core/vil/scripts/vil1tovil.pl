#!/usr/bin/perl
# Does global search and replace of vil code with vil2 equivalent in named file
# Backup saved to filename.old
$filename=$ARGV[0];
if (@ARGV!=1)
{
    print "viltovil2.pl filename\n";
    print "Performs numerous substitutions to convert from vil to vil2 in named file\n";
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
    s/\bvxl\/vil\b/contrib\/mul\/vil/g;
    s/\bvil\/vil_fwd/vil2\/vil2_fwd/g;

    # Common functions
    s/\bvil\/vil_copy\b/vil2\/vil2_copy/g;
    s/vil_copy/vil2_copy_deep/g;
    s/\bvil\/vil_skip\b/vil2\/vil2_decimate/g;
    s/vil_skip/vil2_decimate/g;
    s/\bvil\/vil_clamp\b/vil2\/vil2_clamp/g;
    s/vil_clamp/vil2_clamp/g;
    s/\bvil\/vil_convolve\b/vil2\/algo\/vil2_convolve_1d/g;
    s/vil_convolve_1d_x/vil2_convolve_1d/g;
    s/\bvil\/vil_crop\b/vil2\/vil2_crop/g;
    s/vil_crop/vil2_crop/g;
    s/\bvil\/vil_flipud\b/vil2\/vil2_flip/g;
    s/vil_flipud/vil2_flip_ud/g;
    s/\bvil\/vil_load\b/vil2\/vil2_load/g;
    s/vil_load/vil2_load/g;
    s/\bvil\/vil_save\b/vil2\/vil2_save/g;
    s/vil_save/vil2_save/g;
    s/\bvil\/vil_image_as\b/vil2\/vil2_convert/g;
    s/vil_image_as_/vil2_convert_cast</g;
    s/\bvil\/vil_ncc\b/vil2\/algo\/vil2_normalised_correlation_2d/g;
    s/vil_ncc/vil2_normalised_correlation_2d</g;
    s/\bvil\/vil_new\b/vil2\/vil2_new/g;
    s/vil_new/vil2_new/g;
    s/\bvil\/vil_ssd\b/vil2\/vil2_math/g;
    s/vil_ssd/vil2_math_ssd/g;
    s/\bvil\/vil_scale_intensities\b/vil2\/vil2_math/g;
    s/vil_scale_intensities/vil2_math_scale_and_offset_values/g;
    s/\bvil\/vil_32bit\b/vil2\/vil2_stream_32bit/g;
    s/vil_32bit_/vil2_stream_32bit_/g;
    s/\bvil\/vil_16bit\b/vil2\/vil2_stream_16bit/g;
    s/vil_16bit_/vil2_stream_16bit_/g;
    
    
    # Methods that have changed a lot
    s/\bget_section\(/get_view\( Fix params/g;
    s/\bput_section\(/put_view\( Fix params/g; 
    
    # Methods that have changed name 
    s/\.width\(\)/.ni()/g;
    s/\.height\(\)/.nj()/g;
    
    # Classes
    s/\bvil\/vil_memory_image_of\b/vil2\/vil2_image_view/g;
    s/\bvil_memory_image_of\b/vil2_image_view/g;
    s/\bvil\/vil_memory_image\b/vil2\/vil2_image_view_base/g;
    s/\bvil_memory_image\b/vil2_image_view_base_sptr/g;
    s/\bvil\/vil_image\b/vil2\/vil2_image_resource/g;
    s/\bvil_image\b/vil2_image_resource/g;
    s/\bvil\/vil_pyramid\b/vimt\/vimt_image_pyramid/g;
    s/vil_pyramid/vimt_image_pyramid/g;
    s/\bvil\/vil_file_format\b/vil2\/vil2_file_format/g;
    s/vil_file_format/vil2_file_format/g;
    
    # types
    s/\bvil\/vil_property\b/vil2\/vil2_property/g;
    s/vil_property_/vil2_property_/g;

    s/\bvil\/vil_byte\b/vxl_config/g;
    s/\bvil_byte\b/vxl_byte/g;
    s/\bvil\/vil_rgb_byte\b/vil2\/vil2_rgb/g;
    s/vil_rgb_byte/vil2_rgb<vxl_byte>/g;

    # Stuff that was just copied from vil to vil2
    s/\bvil\/vil_rgba\b/vil2\/vil2_rgba/g;
    s/vil_rgba</vil2_rgba</g;
    s/\bvil\/vil_rgb\b/vil2\/vil2_rgb/g;
    s/vil_rgb</vil2_rgb</g;

    s/\bvil\/vil_stream/vil2\/vil2_stream/g;
    s/vil_stream/vil2_stream/g;

    s/\bvil\/vil_open\b/vil2\/vil2_open/g;
    s/vil_open/vil2_open/g;
	

    print OUTFILE;
}
