#!/usr/bin/perl -w

# usage: convertviltovil1.run_me_first.pl file_or_directory_name

# usage: convertviltovil1.run_me_first.pl file_or_directory_name
# Modifies files so as to deal with vil to vil1 renaming.


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
        if (/\bvil_/ || /\bVIL_/ || /<vil\// || /\bvil\b/)
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
        s/\bvil_/vil1_/g;
        s/\bVIL_/VIL1_/g;
        s/<vil\//<vil1\//;
        s/\bvil\b/vil1/g;
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
    if ($filename =~ /\.svn/) {return;}
#    print "Found3 $filename\n";
    if (mentions_vil($filename))
    {
        print "Fixing $filename\n";
        # Create backup
        $backup = "$filename.old";
        rename $filename, $backup or die "Unable to create backup $backup";
        # Copy backup into original file
        convert $backup, $filename;
    }
}


print "Searching $ARGV[0]\n";
# Traverse desired filesystems
File::Find::find({ wanted => \&wanted, follow => 0, no_chdir => 1 } , $ARGV[0]);
exit;
