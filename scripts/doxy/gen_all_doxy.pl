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
use File::Path;

#-----------------------------------------------------------
#  gen_all_doxy.pl -v vxlsrc -s script_dir -l ctrl_file -u -f  [-o outputdir]
#  -u : Update cvs
#  -f : Force build
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
#  cvs_changes($vxlsrc,$package,$library);
#-----------------------------------------------------------

sub cvs_changes
{
  my($vxlsrc,$package,$library)=@_;

  $path="$vxlsrc/$package/$library";

  chdir $path || die "Unable to chdir to $path\n";

  # Invoke cvs update (without actually doing update) and return the response.
  return `cvs -q -n up -d`;
}

#-----------------------------------------------------------
#  cvs_update($vxlsrc,$package,$library);
#-----------------------------------------------------------

sub cvs_update
{
  my($vxlsrc,$package,$library)=@_;

  $path="$vxlsrc/$package/$library";

  chdir $path || die "Unable to chdir to $path\n";

  # Invoke cvs update.
  xec("cvs -q  up -d");
}

#-----------------------------------------------------------
#  update_library($vxlsrc,$script_dir,$library_list,$package,$library,$doxydir,$forced,$cvsup);
#-----------------------------------------------------------

sub update_library
{
  my($vxlsrc,$script_dir,$library_list,$package,$library,$doxydir,$forced,$cvsup)=@_;
  print "package = $package Library =$library \n";

  # Only need to do the build if either
  # a) Build is forced
  # b) cvs update is allowed and indicates that changes have occurred
  $buildit = $forced;

  $packlib = $package . "/" . $library;

  # Check for special case in which libraries are directly below $vxlsrc
  # eg vcl
  if ($package eq ".")
  {
    $packlib = $library;
  }

  chdir $vxlsrc || die "Unable to chdir to $vxlsrc\n";
  
  if (! -e $package)
  {
    print "$package does not exist.  There may be a mistake in the library list file.\n";
  }

  # If the package doesn't exist, we might have to check it
  # out of the repository.  However, we'd need to know the
  # correct path to the repository for this, which isn't
  # available in the current version of the scripts.  Sorry.
  chdir $package || die "Unable to chdir to $vxlsrc/$package\n";

  if (! -e $library)
  {
    if ($cvsup ne "true")
    {
      die "$vxlsrc/$package/$library does not exist.\n";
    }

    # Check the library out of cvs
    $cmd="cvs -z3 up -d $library\n";
    xec($cmd);
    $buildit = "true";
  }

  if (! -e $doxydir)
  {
    print "Creating $doxydir\n";
    mkdir $doxydir,0777 || die "Can't create directory $doxydir\n";
    $buildit = "true";
  }

  if (! -e "$doxydir/html")
  {
    print "Creating $doxydir/html\n";
    mkdir "$doxydir/html",0777 || die "Can't create directory $doxydir/html\n";
    $buildit = "true";
  }

  # Check that package directory exists in documentation area
  chdir "$doxydir/html" || die "Unable to chdir to $doxydir/html\n";
#  chdir "$doxydir" || die "Unable to chdir to $doxydir\n";
  if (! -e $package)
  {
    print "Creating package directory: $package\n";
    mkpath ($package,1,0777);
    $buildit = "true";
  }

  # Check that library directory exists in documentation area
  chdir $package || die "Unable to chdir to $package\n";
  if (! -e $library)
  {
    print "Creating library directory: $library\n";
    mkdir $library,0777 || die "Can't create directory $library\n";
    $buildit = "true";
  }

  if ($cvsup eq "true")
  {
    $changes = cvs_changes($vxlsrc,$package,$library);
    if ($changes ne "")
    {
      $buildit="true";
      cvs_update($vxlsrc,$package,$library);
    }
    else
    { print " - No changes from cvs update\n"; }
  }

  if ($buildit eq "true")
  {
    # Go back to the source directory and create documentation
    chdir $vxlsrc || die "Unable to chdir to $vxlsrc\n";
    xec("$script_dir/rundoxy.pl -v $vxlsrc -s $script_dir -o $doxydir -l $library_list -n $packlib");
  }
  else
  { print " - Not updating documentation (not forced or no cvs changes)\n"; }
}

#-----------------------------------------------------------
# Main
#-----------------------------------------------------------

my %options;
getopts('v:s:l:o:fu', \%options);

my $vxlsrc  = $options{v} || "";
my $script_dir = $options{s} || "$vxlsrc/scripts/doxy";
my $liblist = $options{l} || "$script_dir/data/library_list.txt";
my $doxydir = $options{o} || "$vxlsrc/Doxy";

$forced="false";
if (defined($options{f}))
{
  $forced="true";
}

$cvsup="false";
if (defined($options{u}))
{
  $cvsup="true";
}

if (! $vxlsrc)
{
  print "Generate all documentation from source using doxygen\n";
  print "syntax is:\n";
  print "gen_all_doxy.pl -v vxlsrc -l ctrl_file  [-o outputdir] -u -f\n";
    print " -u : Update cvs\n";
    print " -f : Force build\n";
  exit(1);
}

# Read in list of libraries
open(MODULES, $liblist) || die "can't open $liblist\n";
while (<MODULES>)
{
  # ignore empty lines
  if ( /^\s*$/ ) { next; }

  # ignore comments
  if ( /^#/ ) { next; }

  # ignore lines containing "package:";
  if ( /package:/ ) { next; }

  # ignore lines containing "book:";
  if ( /book:/ ) { next; }

  # ignore lines containing "search:";
  if ( /search:/ ) { next; }

  chomp;
  @bits = split /\s+/;
  $package = $bits[0];
  $library = $bits[1];
#  $prefix  = $bits[2];
  $build_it = $bits[3];

  if ($build_it ne "ignore")
  {
    update_library($vxlsrc,$script_dir,$liblist,$package,$library,$doxydir,$forced,$cvsup);
  }
  else
  {
    print "gen_all_doxy.pl: Not building doc. for $package/$library (<ignore> in library list)\n";
  }
}
close(MODULES);


#-----------------------------------------------------------
