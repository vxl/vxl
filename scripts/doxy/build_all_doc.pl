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

#---------------------------------------------------------------------
#   build_all_doc.pl -v vxlsrc -s script_dir -l lib_file -b book_file [-o outputdir] -u -f
#  -u : Update cvs
#  -f : Force build
#---------------------------------------------------------------------

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
#-----------------------------------------------------------
# Main
#-----------------------------------------------------------

my %options;
getopts('v:l:s:b:o:fu', \%options);

my $vxlsrc = $options{v} || "";
my $script_dir = $options{s} || "$vxlsrc/scripts/doxy";
my $library_list = $options{l} || "$script_dir/data/library_list.txt";
my $book_list = $options{b} || "$script_dir/data/library_list.txt";
my $doxydir = $options{o} || "$vxlsrc/Doxy";

$forcedflag="";
if (defined($options{f}))
{
  $forcedflag="-f";
}

$cvsupflag="";
if (defined($options{u}))
{
  $cvsupflag="-u";
}

if ($vxlsrc eq "")
{
  print "Update documentation files on webserver.\n";
    print "syntax is:\n";
    print "build_all_doc.pl -v VXLSRC -s script_dir -l lib_file -b book_file [-o outputdir] -u -f\n\n";
    print "  where VXLSRC is the path to the root of the vxl source tree.\n";
    print "  ie the directory which contains vcl,vxl,mul etc\n\n";
    print "  outputdir (default VXLSRC/Doxy) indicates where the documentation \n";
    print "  is to be placed.\n";
    print "  script_dir gives the location of the scripts\n";
    print "    default is VXLSRC/scripts/doxy\n";
    print "  If lib_file is not supplied it is assumed to be\n";
    print "  script_dir/library_list.txt\n";
    print "  If book_file is not supplied it is assumed to be\n";
    print "  script_dir/library_list.txt\n";
    print "  Other options:\n";
    print "  -u : Update cvs\n";
    print "  -f : Force build\n";
    exit(1);
}

# Update the scripts directory
if ($cvsupflag)
{
  chdir $script_dir || die "Unable to chdir to $script_dir\n";

  print "Checking for update of documentation scripts\n";
  $changes = `cvs -q up -d`;

  if ($changes)
  {
    print "Scripts have changed, so forcing rebuild of all documentation\n";
    $forcedflag="-f";
  }
}

$params = "-v $vxlsrc -s $script_dir -o $doxydir $forcedflag $cvsupflag";

$gen_all_doxy_cmd = "$script_dir/gen_all_doxy.pl $params -l $library_list";
xec($gen_all_doxy_cmd );

$gen_book_cmd = "$script_dir/gen_books.pl $params -b $book_list";
xec($gen_book_cmd );

$gen_index_cmd = "$script_dir/gen_doxy_index.pl -v $vxlsrc -s $script_dir -l $library_list -b $book_list -o $doxydir";
xec($gen_index_cmd );
