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
    # Common functions
    s/\bvil\/vil_copy\b/vil2\/vil2_copy/g;
    s/vil_copy/vil2_copy_deep/g;
    
    # Methods that have changed a lot
    s/\bget_section\(/get_view\( Fix params 
    s/\bput_section\(/put_view\( Fix params 
    
    # Methods that have changed name 
    s/\.width\(\)/\.ni\(\)/g;
    s/\.height\(\)/\.nj\(\)/g;
    
    # Classes
    s/\bvil\/vil_memory_image_of\b/vil2\/vil2_image_view/g;
    s/vil_memory_image_of/vil2_image_view/g;
    s/\bvil\/vil_image\b/vil2\/vil2_image_resource/g;
    s/vil_image/vil2_image_resource/g;

    print OUTFILE;
}
