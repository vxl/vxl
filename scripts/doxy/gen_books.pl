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
#  gen_books.pl -v vxlsrc -b ctrl_file -u -f [-o outputdir]
#  -u : Update cvs
#  -f : Force build of book
#-----------------------------------------------------------


use Cwd;
use Getopt::Std;
use File::Copy;

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

    $booksrcdir = "$vxlsrc/$book/doc/book";

    opendir(BOOKSRCDIR,$booksrcdir) || die "Cannot open $booksrcdir: $!";


# Now copy, or convert all images files in the source directory.
    while (readdir(BOOKSRCDIR))
    {
      if (/\.png$/ || /\.jpg$/ || /\.jpeg$/ )
      { copy("$booksrcdir/$_", $_); }

      if (/\.eps$/)
      {
        if ($havepnmutils ne "YES")
        {
          print "Can't process $booksrcdir/$_ because can't find pnm utils\n";
        }
        else
        {
          xec("ps2pnm $booksrcdir/$_");
          $pngfile = $_;
          $pngfile =~ s/eps$/png/;
          xec("pnm2png $_001.ppm > $pngfile");
        }
      }
    }
    closedir(BOOKSRCDIR);



  }
  else
  { print " - Not updating documentation (gen_books.pl)\n"; }

}

# "YES"/"NO" = check_pnmutils($doxyoutputdir)
sub check_pnmutils
{
  my ($doxyoutputdir) = @_;
  my $dotfile = "$doxyoutputdir/dotfile.dot";

  `pstopnm > $doxyoutputdir/checkpnmutils.out  2>&1`;
  if ($? != 0)
  {
    return "NO";
  }

  `pnmtopng > $doxyoutputdir/checkpnmutils.out  2>&1`;
  if ($? != 0)
  {
    return "NO";
  }


  return "YES";
}



#-----------------------------------------------------------
# Main
#-----------------------------------------------------------


my %options;
getopts('v:s:b:o:fu', \%options);

my $vxlsrc = $options{v} || "";
my $script_dir = $options{s} || "$vxlsrc/scripts/doxy";
my $booklist = $options{b} || "$script_dir/data/library_list.txt";
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
open(BOOKS, $booklist) || die "can't open $booklist\n";
while (<BOOKS>)
{
  # ignore empty lines
  if ( /^\s*$/ ) { next; }

  # ignore comments
  if ( /^#/ ) { next; }

  # ignore non-book info lines.
  if ( ! /^book: / ) { next; }

  chomp;
  @bits = split /\s/;
  $book = $bits[1];

  update_book($vxlsrc,$doxydir,$bookbasedir,$book,$forced,$cvsup);
}
close(BOOKS);




exit;
