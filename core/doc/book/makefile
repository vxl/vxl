
# You'll need texi2html version 1.64 from
# http://www.mathematik.uni-kl.de/~obachman/Texi2html/


ifeq ($(OS),Windows_NT)
GS = gswin32c
PNGTOPNM = pngtopnm.exe
PNMCROP = pnmcrop.exe
PNMTOPNG = pnmtopng.exe
MAKEINFO= makeinfo.exe # exe to avoid conflict with cygnus makeinfo sh script
TEXI2HTML= T2H_HOME=/pkg/Texi2html perl /pkg/texi2html/texi2html.pl
TEX = tex --c-style-errors --quiet
else
GS = gs
PNGTOPNM = pngtopnm
PNMCROP = pnmcrop
PNMTOPNG = pnmtopng
MAKEINFO = makeinfo
T2H_HOME = false
TEXI2HTML = texi2html
TEX = tex
endif

MAKEINFOFLAGS=-Dmakeinfo --no-validate

all: book.html book.dvi

thelot: book.info book.html book.dvi book.hpj 

ifdef USE_MAKEINFO
book.html: book.texi chapter_vsl.texi rosenbrock.png
	$(MAKEINFO) $(MAKEINFOFLAGS) book.texi --html -o $@ --number-sections
else
book.html: book.texi chapter_vsl.texi rosenbrock.png
	$(TEXI2HTML) -expand info -menu -number -split chapter -nosec_nav -frames book.texi
endif

# Generate figure for HTML
rosenbrock.png: rosenbrock.eps
	$(GS) -sDEVICE=png16m -sOutputFile=tmp.png -dQUIET -dNOPAUSE rosenbrock.eps -c showpage -c quit
	$(PNGTOPNM) tmp.png | $(PNMCROP) | $(PNMTOPNG) > $@
	rm tmp.png

# TeX for paper printing
book.dvi: book.texi atexinfo.tex
	$(TEX) book.texi || touch -t 010100001970 book.dvi


# Windows help files. Process hpj the help compiler 
book.hpj book.rtf: book.texi
	-mkinfrtf.exe $(MAKEINFOFLAGS) book --force -o book.rtf --hpj-output book.hpj

# Emacs info file
book.info: book.texi
	$(MAKEINFO) $(MAKEINFOFLAGS) book --force -o book.info

## awf private
publish:
	 rcp index.html book*.html volumnia.robots.ox.ac.uk.awf:WWW/vxl/book

## for debugging makefile
echo-%:
	echo '$(@:echo-%=%)=$($(@:echo-%=%))'
