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
#  gen_doxy_index.pl -v vxlsrc -s script_dir -l ctrl_file [-o outputdir]
#-----------------------------------------------------------

#use File::Find;
use Cwd;
use Getopt::Std;
#use File::Basename;

#-----------------------------------------------------------
# get_summary_line($vxl,$package,$library)
#-----------------------------------------------------------

sub get_summary_line
{
   my($vxlsrc,$package,$library)=@_;

   $default_line = $library;
   $intro_file = "$vxlsrc/$package/$library/introduction_doxy.txt";

   open(INTRO, $intro_file) || return $default_line;
   while (<INTRO>)
   {
     if (/mainpage/)
     {
       chomp;
       s/\\mainpage//;
       s/\/\*\!//;
       return $_;
     }
   }

   return $default_line;
}

#-----------------------------------------------------------
# do_dirs($vxlsrc,$library_list)
#-----------------------------------------------------------
sub do_dirs
{
  my($vxlsrc,$library_list)=@_;
  my $label;

  my $firstpackage=1;
  my $cvsroot;

  print OF  "<h2>Library Documentation</h2>\n";

  # Read in list of libraries
  open(MODULES, $library_list) || die "can't open $library_list\n";
  while (<MODULES>)
  {
    # ignore empty lines
    if ( /^\s*$/ ) { next; }

    # ignore comments
    if ( /^#/ ) { next; }


    chomp;
    @bits = split /\s+/;

    if ($bits[0] eq "book:") {next;}

    if ($bits[0] eq "search:") {next;}

    if ($bits[0] eq "package:")
    {
      # Get package name and description
      ($label,$cvsroot,$package, @descrip) = split /\s+/;
      if (!$firstpackage)
      {
        print OF "</blockquote>\n";
      }
      print OF "<h3>$package : @descrip</h3>\n";
            print OF "<blockquote>\n";
            $firstpackage=0;
      next;
    }


    $package = $bits[0];
    $library = $bits[1];

    $link = "$package/$library/html/index.html";

    # Attempt to get one line description from $library/introduction_doxy.txt
    $linktext = get_summary_line($vxlsrc,$package,$library);

    print_link($linktext, $link);
    print OF "<br>\n";
  }

  print OF "</blockquote>\n";

  close(MODULES);
}

#-----------------------------------------------------------
#      print_link

sub print_link
{
  my ($text, $link)  = @_;
  print OF "<a href=\"";
  print OF $link;
  print OF "\"> $text";
  print OF '</a>' , "\n";

}

#-----------------------------------------------------------

sub print_header

{
my ($ctrl_list) = @_;

# print OF  "<!DOCTYPE HTML PUBLIC \"-//W3C//DTD HTML 4.01 Transitional//EN\">";
print OF  "<!DOCTYPE html PUBLIC \"-//W3C//DTD HTML 4.01 Transitional//EN\" \"http://www.w3.org/TR/html401/loose.dtd\"> \n";


print OF  "<html>\n";
print OF  "<HEAD>\n";
print OF  "<TITLE>VXL Documentation</TITLE>\n";
print OF  "<meta http-equiv=\"Content-Type\" content=\"text/html; charset=utf-8\">";
print OF  "</HEAD>\n";
print OF  "<body>\n";
print OF  "<h1>VXL Documentation</h1>\n";
print OF  "<p>C++ Libraries for Computer Vision Research and Implementation</p>\n";


# Read in control file for search term.
open(CTRLFILE, $ctrl_list) || die "can't open $ctrl_list\n";
while (<CTRLFILE>)
{
  # ignore empty lines
  if ( /^\s*$/ ) { next; }

  # ignore comments
  if ( /^#/ ) { next; }

  # ignore non-book info lines.
  if ( /^search: / )
  {
    chomp;
    ($command, $search_ref, @textlinebits) = split /\s+/;
    $textline = join(' ', @textlinebits);
    print OF  '<p>';
    print_link ($textline, $search_ref);
    print OF "</p>\n";
  }
}
close(CTRLFILE);

print OF  "<HR>";
}

#-----------------------------------------------------------
# print_book_index_links($vxlsrc,$ctrl_list)
#-----------------------------------------------------------

sub print_book_index_links
{
  my($vxlsrc,$ctrl_list)=@_;
  my @textline;

  print OF "<h2>Overview Documentation</h2>\n";

  print_link("VXL Homepage","http://vxl.sourceforge.net");
  print OF "<br><br>\n";
  print OF "<p>Overviews are compiled from the doc/book directories of each package.</p>\n";

  # Read in list of books
  open(BOOKS, $ctrl_list) || die "can't open $ctrl_list\n";
  while (<BOOKS>)
  {
    # ignore empty lines
    if ( /^\s*$/ ) { next; }

    # ignore comments
    if ( /^#/ ) { next; }

    # ignore non-book info lines.
    if ( ! /^book: / ) { next; }

    chomp;
    ($command, $module, @textline) = split /\s+/;

      $link = "books/$module/book.html";
      $ltext = join(' ',@textline);
      print_link("$module $ltext",$link);
      print OF "<br>";

  }
  close(BOOKS);

  print OF "<br><hr>";
}

sub print_tail()
{
print OF '<p>';
print OF  "<HR>";
print OF  "<h2>Download</h2>\n";
print OF "See the ";
print_link("VXL Homepage","http://vxl.sourceforge.net/");
print OF "\n";
print OF "The source for VXL can be downloaded from ";
print_link("sourceforge.net/projects/vxl","http://sourceforge.net/projects/vxl");
print OF "\n";
print OF '<p>';
print OF  "<HR>";
print OF  "Index generated by <em>gen_doxy_index.pl</em> on \n";
$date = localtime(time());
print OF $date;
print OF  "<HR>";
print OF  "</body>";
print OF  "</html>\n";
}

#-----------------------------------------------------------
# Main
#-----------------------------------------------------------

my %options;
getopts('v:s:l:o:fu', \%options);

my $vxlsrc  = $options{v} || "";
my $script_dir = $options{s} || "$vxlsrc/scripts/doxy";
my $ctrl_list = $options{l} || "$script_dir/data/library_list.txt";
my $doxydir = $options{o} || "$vxlsrc/Doxy";

if (!$vxlsrc)
{
  print "Generate index of all documentation from source\n";
  print "syntax is:\n";
  print "gen_doxy_index.pl -v vxlsrc -s script_dir -l lib_file [-o outputdir]\n";
  exit(1);
}

$index = "$doxydir/html/index.html";
open(OF, ">$index");
print_header($ctrl_list);
print_book_index_links($vxlsrc,$ctrl_list);
do_dirs($vxlsrc,$ctrl_list);
print_tail();
close(OF);

print "Wrote index file to $index\n";

#-----------------------------------------------------------
