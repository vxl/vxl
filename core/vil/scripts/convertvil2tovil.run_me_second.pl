#!/bin/sh
# -*- perl -*-
exec perl -w -x $0 ${1+"$@"}
#!perl
#line 6
# If Windows barfs at line 3 here, you will need to run perl -x this_file.pl
# You can set up as a permanent file association using the following commands
#  >assoc .pl-PerlScript
#  >ftype PerlScript=Perl=C:\Perl\bin\Perl.exe -x "%1" %*


# usage: convertvil2tovil.run_me_second.pl file_or_directory_name
# Modifies files so as to deal with vil2 to vil renaming.
# Only run this script after running convertviltovil1.run_me_first.pl

use strict;
use File::Find ();
use File::Copy ();

# Set the variable $File::Find::dont_use_nlink if you're using AFS,
# since AFS cheats.






sub mentions_vil {
    my ($filename)= ($_);
#    print "Checking $filename\n";
    open(INFILE,"<$filename") or die "Can't open $filename";

    while (<INFILE>)
    {
        if (/\bvil2_/ || /\bVIL2_/ || /<vil2\// || /\bvil2\b/)
        {
          close INFILE;
          return 1;
        }
    }
    close INFILE;
    return 0;
}

sub convert {
    my ($src, $dest)= @_;
    open(OUTFILE,">$dest") or die "Can't open $src";
    open(INFILE,"<$src") or die "Can't open $dest";

    while (<INFILE>)
    {
        s/\bvil2_/vil_/g;
        s/\bVIL2_/VIL_/g;
        s/<vil2\//<vil\//;
        s/\bvil2\b/vil/g;
        print OUTFILE;
    }
    close OUTFILE;
    close INFILE;

}



sub wanted {
    my ($filename) = $File::Find::name;
    my $backup;

#    print "Found1 $filename\n";
    if (! -f $filename)  {return;}
#    print "Found2 $filename\n";
    if ($filename =~ /CVS/) {return;}
    if ($filename =~ /\.old/) {return;}
    if ($filename =~ /\.svn/) {return;}
#    print "Found3 $filename\n";

    if (mentions_vil($filename))
    {
        print "Fixing $filename\n";
        # Create backup
        $backup = "$filename.old2";
        rename $filename, $backup or die "Unable to create backup $backup";
        # Copy backup into original file
        convert $backup, $filename;
    }
}


print "Searching $ARGV[0]\n";
# Traverse desired filesystems
File::Find::find({ wanted => \&wanted, follow => 0, no_chdir => 1 } , $ARGV[0]);
exit;
