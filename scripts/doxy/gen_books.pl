#!/bin/sh
# -*- perl -*-
exec perl -w -x $0 ${1+"$@"}
#!perl
#line 6
# If Windows barfs at line 3 here, you will need to run perl -x this_file.pl
# You can set up as a permanent file association using the following commands
#  >assoc .pl=PerlScript
#  >ftype PerlScript=C:\Perl\bin\Perl.exe -x "%1" %*

#-----------------------------------------------------------
#  gen_books.pl -v vxlsrc -l ctrl_file -u -f [-o outputdir]
#  -u : Update cvs
#  -f : Force build of book
#-----------------------------------------------------------


use Cwd;
use Getopt::Std;
use File::Copy;
use File::Path;

my $havepnmutils;

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
#  update_book($vxlsrc,$doxydir,$bookbasedir,$ctrllist,$forced,$cvsup);
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
    mkpath ($bookdir,1,0777);
      $buildit="true";
  }

  if ($cvsup eq "true")
  {
    $changes = `cvs -q -n up -d -A`;
    if ($changes ne "")
    {
      $buildit="true";
      xec("cvs -q  up -d -A");
    }
    else
    {
    print " - No changes from cvs update (gen_books.pl)\n";
  }
  }

  if ($buildit eq "true")
  {

# First copy, or convert all images files in the source directory.
# texi2html will only include the images if they are already there.

    $booksrcdir = "$vxlsrc/$book/doc/book";

    print "Going to open dir: $booksrcdir\n";

    chdir $bookdir || die "Unable to chdir to $bookdir\n";

    opendir(BOOKSRCDIR,$booksrcdir) || die "Cannot open $booksrcdir: $!";

    while ($_ = readdir(BOOKSRCDIR))
    {
      if (/\.eps$/)
      {
        if ($havepnmutils ne "YES")
        {
          print "Can't process $booksrcdir/$_ because can't find pnm utils\n";
        }
        else
        {
          $ppmfile = $_;
          $ppmfile =~ s/eps$/ppm/;
          xec("pstopnm -portrait -stdout $booksrcdir/$_ > $ppmfile");
          $pngfile = $_;
          $pngfile =~ s/eps$/png/;
          unlink $pngfile;
          xec("pnmtopng $ppmfile > $pngfile");
          unlink $ppmfile;
        }
      }
    }
    closedir(BOOKSRCDIR);

    opendir(BOOKSRCDIR,$booksrcdir) || die "Cannot open $booksrcdir: $!";

    while ($_ = readdir(BOOKSRCDIR))
    {
      # print "DIR: $_\n";

      if (/\.png$/ || /\.jpg$/ || /\.jpeg$/ )
      {
        print("Copying $booksrcdir/$_ to book directory\n");
        copy("$booksrcdir/$_", $_);
      }
    }
    closedir(BOOKSRCDIR);

    $booktexi = "$vxlsrc/$book/doc/book/book.texi";
#    xec("texi2html -expandinfo -number -split_chapter $booktexi");
     $booklogfile = $book;
     $booklogfile =~ s/\//_/g;
     xec("texi2html -split=chapter -number $booktexi > $doxydir/output/texi2html_$booklogfile.out 2>&1");


  }
  else
  { print " - Not updating documentation (gen_books.pl)\n"; }

}

# "YES"/"NO" = check_pnmutils($doxyoutputdir)
sub check_pnmutils
{
  my ($doxyoutputdir) = @_;
  my $psfile = "$doxyoutputdir/psfile.dot";


  open(PSOUT, ">$psfile") || die "can't open file $psfile\n";
  print PSOUT "\n";
  close(PSOUT);

  `pstopnm $psfile > $doxyoutputdir/checkpnmutils.out  2>&1`;
  if ($? != 0)
  {
    return "NO";
  }

  open(PSOUT, ">$psfile") || die "can't open file $psfile\n";
  print PSOUT "P1\n1 1\n1\n";
  close(PSOUT);

  `pnmtopng $psfile > $doxyoutputdir/checkpnmutils.out  2>&1`;
  if ($? != 0)
  {
    return "NO";
  }

  print "found pnmutils\n";
  return "YES";
}



#-----------------------------------------------------------
# Main
#-----------------------------------------------------------


my %options;
getopts('v:s:l:o:fu', \%options);

my $vxlsrc = $options{v} || "";
my $script_dir = $options{s} || "$vxlsrc/scripts/doxy";
my $ctrllist = $options{l} || "$script_dir/data/library_list.txt";
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
  print " gen_books.pl -v vxlsrc -s script_dir -b ctrl_file [-o outputdir] -u -f\n";
  print " -u : Update cvs\n";
  print " -f : Force build of books\n";
  exit(1);
}

$bookbasedir = "$doxydir/html/books";
if (! -e $bookbasedir)
{
  print "Creating $bookbasedir\n";
  mkdir $bookbasedir,0777 || die "Can't create directory $bookbasedir\n";
}

$havepnmutils = check_pnmutils($bookbasedir);


# Check that texi2html can be invoked
#`texi2html -usage`;
#if ($? != 0)
#{
#  print "gen_books.pl: Unable to execute texi2html\n";
#  exit;
#}

# Read in list of libraries
open(BOOKS, $ctrllist) || die "can't open $ctrllist\n";
while (<BOOKS>)
{
  # ignore empty lines
  if ( /^\s*$/ ) { next; }

  # ignore comments
  if ( /^#/ ) { next; }

  # ignore non-book info lines.
  if ( ! /^book: / ) { next; }

  chomp;
  @bits = split /\s+/;
  $book = $bits[1];

  update_book($vxlsrc,$doxydir,$bookbasedir,$book,$forced,$cvsup);
}
close(BOOKS);




exit;
