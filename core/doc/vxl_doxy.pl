#!/bin/sh
# -*- perl -*-
exec perl -w -x $0 ${1+"$@"}
#!perl
#line 6

# Script to change the perceps documentation format to Doxygen (JavaDoc) format
# Authors:
#         Dave Cooper
#         Maarten Vergauwen
# Date:   
#      17/02/2000

# get the filename from the args
$filename = $ARGV[0];

# patterns to be matched
$verbpatt = "\verbatim";
$endverbpatt = "\endverbatim";
$namepatt = ".NAME";
$authorpatt = ".SECTION Author";
$modificationpatt = ".SECTION Modifications";
$descriptionpatt = ".SECTION Description";
$slashslashpatt = "//";
$slashslashcolonpatt = "//:";
$slashslashspacedashdashpatt = "// --";
$slashstarstarpatt = "/**";
$spacespacepatt = "  ";
$starpatt = "*";
$starslashpatt = "*/";

# variables that keep state:

# comment found -> first line should start with /**, next lines with *, last line with */
$comment = 0;

# verbatim found -> lines should not start with * (visible in Doxygen)
$verbatim = 0;

# file-header found -> we have to deal with .SECTION NAME etc
$weareinfile = 0;

# .SECTION Author found -> search following lines for names until eol found
$gotonextlineauthor = 0;

# list of authornames
@authornames = ();

# .SECTION Description found -> search following lines for description until .SECTION or //----eol found
$gotonextlinedescription = 0;

# list of descriptions
@descriptions = ();

# .SECTION Modifications found -> search following lines for modifications until //-----eol found
$gotonextlinemod = 0;


# mainloop	   
while (<>)
  {
    # found verbatim ?
    if ( /$verbpatt/ ) { $verbatim = 1; };
    
    # found endverbatim ?
    if ( /$endverbpatt/ ) { $verbatim = 0; };
    
    # found start of comment: either "//:" or "// --" ?
    if ( s/$slashslashcolonpatt/$slashstarstarpatt/ || s/$slashslashspacedashdashpatt/$slashstarstarpatt/ )
      {
	chomp; print; print ".\n";
	$comment = 1;
	next;
      }
    
    # found continuation of comment WITH verbatim -> no "*"
    if ( /$slashslashpatt/ && $verbatim && $comment)
      {
	s/$slashslashpatt/$spacespacepatt/;
	print; next;
      }
    
    # found continuation of comment WITHOUT verbatim -> start line with "*"
    if ( /$slashslashpatt/ && $comment )
      {
	s/$slashslashpatt/$starpatt/;
	print;
	next;
      }
    
    # found end of comment -> start line with */
    if ( $comment && ! /$slashslashpatt/  )
      {
	print "$starslashpatt\n";
	$comment = 0;
      }
    
    # found .NAME -> header found !
    if ( /$namepatt/ && !$weareinfile)
      {
	$weareinfile = 1;
	@listname = split /\s+/;
	shift @listname;
	shift @listname;
	shift @listname;
	$brief = "";
	foreach $name ( @listname )
	  {
	    $brief .= "$name ";
	  }
	print "/** \\file \"$filename\"\n";
	print " * \\brief $brief\n";
	next;
      }

    # in header and found AUTHOR -> start scanning lines for authors
    if ( $weareinfile && /$authorpatt/ && !$gotonextlinedescription)
      {
	$gotonextlineauthor = 1;
	next;
      }
    
    # scan lines for authors
    if ($gotonextlineauthor)
      {
	# end of authors by "//eol" -> print authors
	if (/\/\/\s*$/)
	  {
	    foreach $name ( @authornames )
	      {
		print " * \\author $name\n";
	      }
	    print " * \n";
	    $gotonextlineauthor = 0;
	    next;
	  }
	# end of authors by ".SECTION Description -> print authors and start searching for Description
	if (/$descriptionpatt/)
	  {
	    foreach $name ( @authornames )
	      {
		print " * \\author $name,\n";
	      }
	    print " *\n";
	    $gotonextlineauthor = 0;
	    $gotonextlinedescription = 1;
	    next; 
	  }

	# end of authors by ".SECTION Modifications -> print authors and start searching for Modifications
	if (/$modificationpatt/)
	  {
	    foreach $name ( @authornames )
	      {
		print " * \\author $name\n";
	      }
	    print " * \n";
	    $gotonextlineauthor = 0;
	    $gotonextlinemod = 1;
	    next; 
	  }
	
	# get authorname(s) on this line and add to authornamelist
	if (($f,$authorname) = ($_ =~ /^\s*(\S+)\s+(.*)/))
	  {
	    $f = ""; # to avoid warning of unused variable
	    push (@authornames,$authorname);
	    next;
	  }
      }

    # in header and found MODIFICATION -> start scanning lines for modifications
    if ( $weareinfile && /$modificationpatt/)
      {
	$gotonextlinemod = 1;
	print " * \\bug\n";
	next;
      }

    # scan lines for modifications
    if ($gotonextlinemod)
      {
	# end of modifications because "//---eol" -> print */ and end header
	if (/\/\/\s*-*$/)
	  {
	    print " *\/ \n";
	    $gotonextlinemod = 0;
	    $weareinfile = 0;
	    next;
	  }
	else
	  # strip "//" from line and print line
	  {
	   s!/*!!;
	   print " * ";
	   print;
	   next;
	  }
      }
    
    # .SESCTION Description found
    if ( $weareinfile && /$descriptionpatt/)
      {
      	$gotonextlinedescription = 1;
	next;
      }
    
    # scan lines for description
    if ($gotonextlinedescription)
      {
	# end of description by ".SECTION Modifications -> print description and start searching for Modifications
	if (/$modificationpatt/)
	  {
	    print " * \n";
	    foreach $name ( @descriptions )
	      {
		print " * $name\n";
	      }
	    print " * \n";
	    $gotonextlinedescription = 0;
	    $gotonextlinemod = 1;
	    next; 
	  }

	# end of description by ".SECTION Author -> print description and start searching for Authors
	if (/$authorpatt/)
	  {
	    print " * \n";
	    foreach $name ( @descriptions )
	      {
		print " * $name\n";
	      }
	    print " * \n";
	    $gotonextlinedescription = 0;
	    $gotonextlineauthor = 1;
	    next; 
	  }
	
	# get description on this line and add to descriptionlist
	if (($f,$des) = ($_ =~ /^\s*(\S+)\s+(.*)/))
	  {
	    $f = ""; # to avoid warning of unused variable
	    push (@descriptions,$des);
	    next;
	  }
      }

    # in header and found MODIFICATION -> start scanning lines for modifications
    if (  $weareinfile && /$modificationpatt/)
      {
	$gotonextlinemod = 1;
	print " * \\bug\n";
	next;
      }

    # scan lines for modifications


    # in header and found end ( "//---------eol") -> stop header
    if ( $weareinfile && /\/\/\s*-+$/)
      {
	print " *\/ \n";
	$weareinfile = 0;
	$gotonextlinemod = 0;
	$gotonextlineauthor = 0;
	$gotonextlinedescription = 0;
	next;
      }	
    
    # just print line if not in comment or in file
    if ( !$comment && !$weareinfile) { print;}

  }
