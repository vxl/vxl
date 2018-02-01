

# \author Hans J. Johnson
# Python script to ensure that std:: headers are
# listed first in the files.
#
# This is needed to provide backwards compatibility
# by ensuring that std* headers are included
# before the vcl_* definitions are included
# from vcl_compiler.h

import sys
import os
import re

std_headers_string="""algorithm bitset cassert cctype cerrno cfloat ciso646 climits clocale cmath complex csetjmp csignal cstdarg cstddef cstdio cstdlib cstring ctime cwchar cwctype deque exception exception_defines.h fstream functional iomanip ios iosfwd iostream istream iterator limits list locale map memory new numeric ostream queue set sstream stack stdexcept streambuf string typeinfo utility valarray vector"""
std_headers_list=std_headers_string.split()

with open(sys.argv[1], 'r') as infid:
    orig_file_as_string = infid.read()

file_as_string = orig_file_as_string

search_string=r'^ *# *include *([<"])([^>"]*)([>"])'
include_line_regex=re.compile(search_string)

std_headers_found=list()
std_headers_inserted = list()

for line in file_as_string.splitlines():
    gg=include_line_regex.match(line)
    if gg != None and ( len(gg.groups()) == 3 ):
        inc=os.path.basename(gg.group(2))
        if (inc in std_headers_list) and ( not (inc in std_headers_found) ):
            std_headers_found.append(inc)

out_file = ""
for line in file_as_string.splitlines():
    gg=include_line_regex.match(line)
    if gg != None and ( len(gg.groups()) == 3 ):
        inc=os.path.basename(gg.group(2))
        for stdInclude in std_headers_found:
            out_file += "#include <{0}>\n".format(stdInclude)
            std_headers_inserted.append(stdInclude)
        std_headers_found = list()
        if not( inc in std_headers_list ):
            out_file += line +"\n"

    else:
        out_file+=line + "\n"

if out_file != orig_file_as_string:
    print("PROCESSED {0}".format(sys.argv[1]))
    with open(sys.argv[1],'w') as outfid:
        outfid.write(out_file)
else:
    print("SKIPPING {0}".format(sys.argv[1]))
