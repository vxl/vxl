#!/bin/sh
# -*- perl -*-
exec perl -w -x $0 ${1+"$@"}
#!perl
#line 6
# If Windows barfs at line 3 here, you will need to run perl -x this_file.pl
# You can set up as a permanent file association using the following commands
#  >assoc .pl=PerlScript
#  >ftype PerlScript=C:\Perl\bin\Perl.exe -x "%1" %*

use Cwd;
use Getopt::Std;

#-----------------------------------------------------------
#  gen_all_doxy.pl -v vxlsrc -p package_list
#-----------------------------------------------------------
sub xec
{
  my $command = $_[0];
  print "exec [$command]\n";
  my $op = `$command`;
  print $op;
  if ( $? != 0 )
  {
    print "<$command> failed\n";
    exit(1);
  }
}

#-----------------------------------------------------------
# Main
#-----------------------------------------------------------

my %options;
getopts('v:p:', \%options);

my $vxlsrc  = $options{v} || "";
my $package_list = $options{p} || "$vxlsrc/scripts/doxy/data/library_list.txt";

if (! $vxlsrc)
{
  print "Check out all packages into $vxlsrc\n";
  print "Syntax is:\n";
  print "gen_all_doxy.pl -v vxlsrc -p ctrl_file\n";
  exit(1);
}

chdir $vxlsrc || die "Unable to chdir to $vxlsrc\n";

# Keep list of packages checked out already so that we don't do any twice.
my (%packages_so_far);
my $label;
my @textline;

# Read in list of packages
open(PACKAGES, $package_list) || die "can't open $package_list\n";
while (<PACKAGES>)
{
  # ignore empty lines
  if ( /^\s*$/ ) { next; }

  # ignore comments
  if ( /^#/ ) { next; }

  if ( /^package:/ )
  {
    chomp;
    ($label,$cvsroot,$package,@textline) = split;

    if ( exists $packages_so_far{$package} ) {next;}
    $packages_so_far{$package} = 1;

    print "Checking for $package (CVSROOT=$cvsroot)\n";
    if (! -e $package)
    {
      xec("cvs -z3 -d$cvsroot co $package");
    }
  }
}
close(PACKAGES);


#-----------------------------------------------------------
