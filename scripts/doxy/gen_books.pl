#!/bin/sh
# -*- perl -*-
exec perl -w -x $0 ${1+"$@"}
#!perl
#line 6
# If Windows barfs at line 3 here, you will need to run perl -x vxl_doxy.pl
# You can set up as a permanent file association using the following commands
#  >assoc .pl=PerlScript
#  >ftype PerlScript=C:\Perl\bin\Perl.exe -x "%1" %*

#-----------------------------------------------------------
#  gen_books.pl -v vxlsrc -b book_file -u -f [-o outputdir]
#  -u : Update cvs
#  -f : Force build of book
#-----------------------------------------------------------


use Cwd;
use Getopt::Std;

sub xec
{
  my $command = $_[0];
  print "exec [$command]\n";
  my $op = `$command`;
  print $op;
  if ( $? != 0 )
  {
    print "<$command> failed\n";
  }
}

#-----------------------------------------------------------
#  update_book($vxlsrc,$doxydir,$bookbasedir,$booklist,$forced,$cvsup);
#-----------------------------------------------------------

sub update_book
{
  my($vxlsrc,$doxydir,$bookbasedir,$book,$forced,$cvsup)=@_;
  print "book = $book\n";



  $path = "$vxlsrc/$book/doc/book";
  chdir $path || die "Unable to chdir to $path\n";
  # Only need to do the build if either
  # a) Build is forced
  # b) cvs update is allowed and indicates that changes have occurred
  $buildit = $forced;

  $bookdir = "$bookbasedir/$book";
  if (! -e $bookdir)
  {
    print "gen_books.pl: Creating $bookdir\n";
    mkdir $bookdir,0777 || die "gen_books.pl: Can't create directory $bookdir\n";
      $buildit="true";
  }

  if ($cvsup eq "true")
  {
    $changes = `cvs -q -n up -d`;
    if ($changes ne "")
    {
      $buildit="true";
      xec("cvs -q  up -d");
    }
    else
    {
	  print " - No changes from cvs update (gen_books.pl)\n";
	}
  }

  if ($buildit eq "true")
  {
    chdir $bookdir || die "Unable to chdir to $bookdir\n";

        $booktexi = "$vxlsrc/$book/doc/book/book.texi";
#    xec("texi2html -expandinfo -number -split_chapter $booktexi");
    xec("texi2html -number $booktexi > $doxydir/output/texi2html_$book.out 2>&1");
  }
  else
  { print " - Not updating documentation (gen_books.pl)\n"; }

}

#-----------------------------------------------------------
# Main
#-----------------------------------------------------------


my %options;
getopts('v:s:b:o:fu', \%options);

my $vxlsrc = $options{v} || "";
my $script_dir = $options{s} || "$vxlsrc/scripts/doxy";
my $booklist = $options{b} || "$script_dir/data/book_list.txt";
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
  print "Generate all books from source using texi2html\n";
  print "syntax is:\n";
  print " gen_books.pl -v vxlsrc -s script_dir -b book_file [-o outputdir] -u -f\n";
  print " -u : Update cvs\n";
  print " -f : Force build of books\n";
  exit(1);
}

$bookbasedir = "$doxydir/books";
if (! -e $bookbasedir)
{
  print "Creating $bookbasedir\n";
  mkdir $bookbasedir,0777 || die "Can't create directory $bookbasedir\n";
}

# Check that texi2html can be invoked
#`texi2html -usage`;
#if ($? != 0)
#{
#  print "gen_books.pl: Unable to execute texi2html\n";
#  exit;
#}

# Read in list of libraries
open(BOOKS, $booklist) || die "can't open $booklist\n";
while (<BOOKS>)
{
  # ignore empty lines
  if ( /^\s*$/ ) { next; }

  # ignore comments
  if ( /^#/ ) { next; }

  chomp;
  @bits = split /\s/;
  $book = $bits[0];

  update_book($vxlsrc,$doxydir,$bookbasedir,$book,$forced,$cvsup);
}
close(BOOKS);

exit;
