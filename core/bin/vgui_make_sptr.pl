#! /bin/sh
# -*- perl -*-
exec perl -w -x $0 ${1+"$@"}
#!perl -w
#line 6

# Purpose: Automate generation of _sptr and _new types for tableaux.
#          If you hack the script to do something new, please consult
#          with me before committing the changes. At the end of the
#          day it is I who will be expected to maintain this script
#          so I don't think I'm being unfair.
#
# Author: fsm
#
# Usage: PROGRAM [options] name_of_class [headerfile]
#
# Options: -n     dry run
#
#
# Notes: Given a class name 'blah', the script will attempt to create
# a header file 'blah.h' and to insert (or replace) a definition of
# 'blah_new' in the given source file.
#
#blah_sptr:
#   // Forward declare the _sptr types. This could be in a header file.
#   (struct|class) blah;
#   #include <vgui/vgui_tableau_sptr.h>
#   typedef vgui_tableau_sptr_t<blah> blah_sptr;
#
#blah:
#   // Define the blah type.
#   (struct|class) blah : public [base] {
#     ctor1(...);
#     ctor2(...);
#   }
#
#blah_new:
#   // Define the _new type. The definition of blah must be visible
#   // before we can do this. Usually, the _new class should appear
#   // immediately after blah and should duplicate its constructors.
#   struct blah_new : public blah_sptr {
#     ctor1(...) : blah_sptr(new blah(...)) { }
#     ctor2(...) : blah_sptr(new blah(...)) { }
#   };
#
#
# Missing features and known bugs:
#  1. script could detect tableau base classes and derive
#     the blah_new class from base_sptr?
#
#  2. script should not barf on seeing constructors from vgui_tableau*
#

# global flags
$dry_run = 0;

# name of program used to report errors
$me = "vgui_make_sptr";

# shell commands
$os_rm = "/bin/rm";
$os_mv = "/bin/mv";
#$os_cp = "/bin/cp";
$os_diff = "/bin/diff";
$os_diff = "/usr/bin/diff" unless -x $os_diff;
die "no diff\n" unless -x $os_diff;

# _init();
exit &main(@ARGV);

#------------------------------------------------------------

# execute a shell command
sub shell {
  my ($cmd) = @_;
  print STDERR "$me (shell): $cmd\n";
  system "$cmd";
}

# remove C and C++ comments from source lines
# my @lines = &strip_comments(@lines);
sub strip_comments {
  my @lines = @_;
  my $inside_c_comment = 0;
  for (my $i=0; $i<=$#lines; ++$i) {
    if ($inside_c_comment) {
      if ($lines[$i] =~ s/.*\*\///) {
        $inside_c_comment = 0;
      }
      else {
        $lines[$i] = "";
      }
    }
    else {
      $lines[$i] =~ s/\/\*.*\*\///g;
      $lines[$i] =~ s/\/\/.*//g;
      $inside_c_comment = 1 if ($lines[$i] =~ s/\/\*.*//);
    }
  }
  die "unterminated /* ... */\n" if $inside_c_comment;
  return @lines;
}

# determine the extent of a class definition.
# ($struct_or_class, $def_begin, $def_end, $template_clause) = &find_class_def($name, @lines);
sub find_class_def {
  my ($name, @lines) = @_;
  @lines = &strip_comments(@lines);

  my $struct_or_class = "";
  my $def_begin = -1;
  my $def_end = -1;
  my $template_clause = "";

  my $brace_count = 0;
  my $brace_counted = 0;

  for (my $i=0; $i<=$#lines; ++$i) {
    # detect beginning of definition.
    if (($def_begin < 0) && ($lines[$i] =~ m/^\s*(struct|class)\s+$name\b(.*)/)) {
      $struct_or_class = $1;
      my $rest = $2;
      if    ($rest =~ m/\s*\;/) { }
      else {
        if (($i > 0) && ($lines[$i-1] =~ m/\btemplate\s*\<(.*)\>/)) {
          $def_begin = $i-1;
          $template_clause = $1;
        }
        else {
          $def_begin = $i;
          $template_clause = "";
        }
      }
    }

    # count braces.
    if (($def_begin >= 0) && ($def_end < 0)) {
      my $t = $lines[$i];
      $t =~ s/[^\{\}]//g;
      $t =~ s/\{\}//g;
      foreach my $c (split //, $t) {
        if ($c eq "{") { ++$brace_count; $brace_counted = 1; }
        if ($c eq "}") { --$brace_count; }
      }
    }

    # detect end of definition.
    if (($def_begin >= 0) && ($def_end < 0) && ($brace_counted) && ($brace_count == 0)) {
      $def_end = $i + 1;
    }
  }

  # return
  return ($struct_or_class, $def_begin, $def_end, $template_clause);
}

# return list of constructor signatures.
# my @ctor = &ctor_signatures($name, $begin, $end, @lines);
sub ctor_signatures {
  my ($name, $b, $e, @line) = @_;
  @line = &strip_comments(@line);

  my @ctor = ();
  for (my $i=$b; $i<$e; ++$i) {
    # of course this won't work if a declarator for an argument
    # type contains ( or ) or if the parameter list extends over
    # multiple lines. but that can be fixed in an ad hoc way when
    # it happens.
    if ($line[$i] =~ m/^\s*$name\s*\(([^\(\)]*)\)/) {
      my ($a) = $1;
      $a =~ s/\s\s/ /g;
      push @ctor, $a;
      #print STDERR "$me: ctor from '$a'\n";
    }
  }
  return @ctor;
}

#
sub do_sptr {
  my ($blah, $file) = @_;
  die unless ($blah && $file);

  ##############################
  # read the source file.
  die "$me: failed to open $file\n" unless open(FD, "$file");
  my (@line) = <FD>;
  close(FD);

  ##############################
  # look for // no vgui_make_sptr
  for (my $i=0; $i<=$#line; ++$i) {
    if ($line[$i] =~ m/\/\/\s+no\s+vgui_make_sptr\b/) {
      print STDERR "saw 'no vgui_make_sptr' -- aborting\n";
      return;
    }
  }

  ##############################
  # scan through the source file:

  # find extent of definition of blah:
  my ($struct_or_class, $blah_begin, $blah_end, $template_clause) = &find_class_def($blah, @line);
  die "$me: definition of '$blah' not found\n" if (($blah_begin < 0) || ($blah_end < 0));
  die "$me: no class-key found\n" if ($struct_or_class eq "");
  #print STDERR "$me:   begin line: $blah_begin\n";
  #print STDERR "$me:   end   line: $blah_end\n";
  my $template_args = "";
  if (!($template_clause eq "")) {
    my @tmp = split /,/, $template_clause;
    for (my $i=0; $i<=$#tmp; ++$i) {
      $tmp[$i] =~ s/^.*\s(\w+)$/$1/;
    }
    for (my $i=0; $i<=$#tmp; ++$i) {
      $template_args .= ", " if ($i > 0);
      $template_args .= $tmp[$i];
    }
  }
  if ($template_clause eq "") {
    print STDERR "$me: found definition of '$struct_or_class $blah':\n";
  }
  else {
    print STDERR "$me: template_clause = '$template_clause'\n";
    print STDERR "$me: template_args = '$template_args'\n";
    print STDERR "$me: found definition of 'template <${template_clause}> $struct_or_class $blah':\n";
  }

  # collect list of blah's constructor signatures:
  my @blah_ctor = &ctor_signatures($blah, $blah_begin, $blah_end, @line);
  for (my $i=0; $i<=$#blah_ctor; ++$i) {
    if ($blah_ctor[$i] =~ m/vgui_tableau\s*\*/) {
      die "$me: '$blah' has constructor from vgui_tableau *\n";
    }
  }

  # look for definition of blah_new:
  my ($discarded_value, $blah_new_begin, $blah_new_end) = &find_class_def("${blah}_new", @line);
  my @blah_new_ctor = ();
  my $gotnew;
  if (($blah_new_begin < 0) || ($blah_new_end < 0)) {
    $gotnew = 0;
    print STDERR "$me: no definition of '${blah}_new' found\n";
  }
  else {
    $gotnew = 1;
    # collect list of blah_new's constructor signatures:
    @blah_new_ctor = &ctor_signatures("${blah}_new", $blah_new_begin, $blah_new_end, @line);
    for (my $i=0; $i<=$#blah_ctor; ++$i) {
      if ($blah_ctor[$i] =~ m/vgui_tableau\s*\*/) {
        die "$me: '${blah}_new' has constructor from vgui_tableau *\n";
      }
    }
  }

  # look for #include of blah_sptr.h or typedef of blah_sptr:
  my $gotsptr = 0;
  for (my $i=0; $i<=$#line; ++$i) {
    if ($line[$i] =~ m/^\s*\#\s*include\s*([\"\<].*[\"\>])/) {
      my $h = $1;
      if ($h =~ m/\b$blah\_sptr\.h/) {
        print STDERR "$me: found #include of ${blah}_sptr.h\n";
        $gotsptr = 1;
        last;
      }
    }
    if ($line[$i] =~ m/^\s*typedef\s+.*\s+$blah\_sptr\s*\;/) {
      print STDERR "$me: found typedef of ${blah}_sptr\n";
      $gotsptr = 1;
      last;
    }
  }

  ##############################
  # Make the _sptr.h header unless the source file is not a header
  # file or the file _sptr.h already exists.
  if ($file =~ m/^(.*)\.h$/) {
    my ($stem) = $1;
    if (-f "${stem}_sptr.h") {
      print STDERR "$me: file ${stem}_sptr.h exists already\n";
    }
    else {
      if (! $dry_run) {
        print STDERR "$me: create ${stem}_sptr.h\n";
        die unless open(FD, ">${stem}_sptr.h");
        $stem =~ s/[\+\-\.]/_/g;
        print FD "#ifndef ${stem}_sptr_h_\n";
        print FD "#define ${stem}_sptr_h_\n";
        print FD "\n";
        print FD "// this is a generated file.\n";
        print FD "\n";
        print FD "#include <vgui/vgui_tableau_sptr.h>\n";
        print FD "\n";
        if ($template_clause eq "") {
          print FD "$struct_or_class ${blah};\n";
          print FD "typedef vgui_tableau_sptr_t<${blah}> ${blah}_sptr;\n";
        } else {
          print FD "template <$template_clause>\n";
          print FD "$struct_or_class ${blah};\n";
          print FD "\n";
          print FD "template <$template_clause>\n";
          print FD "struct ${blah}_sptr : public vgui_tableau_sptr_t<${blah}<$template_args> > {\n";
          print FD "  typedef vgui_tableau_sptr_t<${blah}<$template_args> > base;\n";
          print FD "  ${blah}_sptr(${blah}<$template_args> *p) : base(p) { }\n";
          print FD "};\n";
        }
        print FD "\n";
        print FD "#endif\n";
        close(FD);
      }
    }
  }
  else {
    print STDERR "$me: source file is not a header file\n";
  }


  ##############################
  # compose definition of blah_new
  if ($gotnew) {
    if ($#blah_new_ctor > $#blah_ctor) {
      # dunno what to do in this case.
      print STDERR "$me: '${blah}_new' has more constructors than '$blah'\n";
    }
  }

  my @blah_new_def = ();
  {
    #print STDERR "$me: making definition of ${blah}_new\n";

    if ($template_clause eq "") {
      push @blah_new_def, "struct ${blah}_new : public ${blah}_sptr {\n";
      push @blah_new_def, "  typedef ${blah}_sptr base;\n";
    }
    else {
      push @blah_new_def, "template <$template_clause>\n";
      push @blah_new_def, "struct ${blah}_new : public ${blah}_sptr<$template_args > {\n";
      push @blah_new_def, "  typedef ${blah}_sptr<$template_args > base;\n";
    }

    # iterate over constructor signatures.
    for (my $i=0; $i<=$#blah_ctor; ++$i) {
      my $ct = $blah_ctor[$i];
      #print STDERR "$me: ct='$ct'\n";

      # list of parameter signatures
      my @pars = split /,/, $ct;

      # list of argument names.
      my @args = ();

      # invent new argument name when none given.
      my $counter = 1000;
      for (my $j=0; $j<=$#pars; ++$j) {
        my $a = $pars[$j];

        # discard trailing whitespace
        $a =~ s/\s*$//;

        # this to cope with tab(float const min[3], float const max[3]);
        $a =~ s/\[[\d\s]*\]\s*$//g;

        # the parameter name is probably the trailing sequence of \w's
        $a =~ s/^.*[^\w](\w*)\s*$/$1/;

        #$a =~ s/^.*[\s\&]+//g;

        if ($a eq "") {
          $a = "arg$counter";
          $pars[$j] .= " $a";
          ++$counter;
        }
        push @args, $a;
        #print "$me: a = '$a'\n";
      }

      # compose line.
      my $str = "";

      # ctor signature.
      $str .= "  ${blah}_new(";
      for (my $j=0; $j<=$#pars; ++$j) {
        $str .= ", " if ($j > 0);
        $str .= $pars[$j];
      }
      $str .= ")";

      # base initializer
      if ($template_clause eq "") {
        $str .= " : base(new ${blah}(";
      }
      else {
        $str .= " : base(new ${blah}<$template_args >(";
      }
      for (my $j=0; $j<=$#args; ++$j) {
        $str .= ", " if ($j > 0);
        $str .= "$args[$j]";
      }
      $str .= "))";

      # function body
      $str .= " { }\n";

      push @blah_new_def, $str;
    }

    # special case: no constructors
    if ($#blah_ctor == -1) {
      if ($template_clause eq "") {
        push @blah_new_def, "  ${blah}_new() : base(new $blah) { }\n";
      }
      else {
        push @blah_new_def, "  ${blah}_new() : base(new $blah<$template_args >) { }\n";
      }
    }

    push @blah_new_def, "};\n";
  }
  #
  foreach $l (@blah_new_def) {
    print STDERR "$me: >> $l";
  }

  ##############################
  # make new source file.
  my (@newfile) = ();

  # first the stuff before the definition of blah:
  for (my $j=0; $j<$blah_begin; ++$j) {
    push @newfile, $line[$j];
  }

  # then declare blah_sptr
  if ($file eq "$blah.h") {
    # if it's a header file called "blah.h", insert a #include to
    # the _sptr file unless we've already seen one.
    if (! $gotsptr) {
      push @newfile, "#include \"${blah}_sptr.h\"\n";
      push @newfile, "\n";
    }
  }
  else {
    # else, insert a typedef directly unless we've already seen one
    if (! $gotsptr) {
      push @newfile, "#include <vgui/vgui_tableau_sptr.h>\n";
      if ($template_clause eq "") {
        push @newfile, "$struct_or_class ${blah};\n";
        push @newfile, "typedef vgui_tableau_sptr_t<${blah}> ${blah}_sptr;\n";
      } else {
        push @newfile, "template <$template_clause>\n";
        push @newfile, "struct ${blah}_sptr : public vgui_tableau_sptr_t<${blah}<$template_args> > {\n";
        push @newfile, "  typedef vgui_tableau_sptr_t<${blah}<$template_args> > base;\n";
        push @newfile, "  ${blah}_sptr(${blah}<$template_args> *p) : base(p) { }\n";
        push @newfile, "};\n";
      }
      push @newfile, "\n";
    }
  }

  # then emit the definition of blah
  for (my $j=$blah_begin; $j < $blah_end; ++$j) {
    push @newfile, $line[$j];
  }

  if (! $gotnew) {
    # emit the definition of blah_new just after the definition of blah:
    push @newfile, "\n";
    push @newfile, @blah_new_def;

    # emit the rest of the file
    for (my $j=$blah_end; $j<=$#line; ++$j) {
      push @newfile, $line[$j];
    }
  }
  else {
    # abort if something unexpected happens.
    die "$me: the definition of ${blah}_new must come after the definition of $blah\n"
      if ($blah_end > $blah_new_begin);

    # emit the stuff between the def of blah and the def of blah_new
    for (my $j=$blah_end; $j<$blah_new_begin; ++$j) {
      push @newfile, $line[$j];
    }
    # overwrite the old definition of blah_new:
    push @newfile, @blah_new_def;

    # emit the rest of the file
    for (my $j=$blah_new_end; $j<=$#line; ++$j) {
      push @newfile, $line[$j];
    }
  }


  # output new source file.
  print STDERR "$me: writing to $file.new\n";
  die unless open(FD, ">$file.new");
  for (my $j=0; $j<=$#newfile; ++$j) {
    print FD $newfile[$j];
  }
  close(FD);

  # show diffs
  shell("$os_diff $file $file.new");
  my $different = ($?) ? 1 : 0;

  # rename old. replace with new.
  if ($dry_run) {
    shell("$os_rm $file.new");
  }
  else {
    if ($different) {
      shell("$os_mv $file $file.old");
      shell("$os_mv $file.new $file");
    }
    else {
      print STDERR "$me: no change\n";
      shell("$os_rm $file.new");
    }
  }
}

sub main {
  my (@argv) = @_;

  if ($argv[0] eq "-n") {
    $dry_run = 1;
    shift @argv;
  }

  if    ($#argv == 0) {
    my $blah = $argv[0];
    my $file = "$blah.h";
    print STDERR "$me: assuming file is '$file'\n";
    &do_sptr($blah, $file);
  }
  elsif ($#argv == 1) {
    &do_sptr($argv[0], $argv[1]);
  }
  else {
    print STDERR "usage: [options] $me classname [filename]\n";
    return 1;
  }
}
