
# You'll need texi2html from
# http://www.mathematik.uni-kl.de/~obachman/Texi2Html/Distrib


ifeq ($(OS),Windows_NT)
GS = gswin32c
PNGTOPNM = pngtopnm.exe
PNMCROP = pnmcrop.exe
PNMTOPNG = pnmtopng.exe
MAKEINFO= makeinfo.exe # exe to avoid conflict with cygnus makeinfo sh script
TEXI2HTML= T2H_HOME=/pkg/Texi2html perl /pkg/texi2html/texi2html.pl
else
GS = gs
PNGTOPNM = pngtopnm
PNMCROP = pnmcrop
PNMTOPNG = pnmtopng
MAKEINFO = makeinfo
T2H_HOME = false
TEXI2HTML = texi2html
endif

MAKEINFOFLAGS=-Dmakeinfo --no-validate

all: book.html book.dvi

thelot: book.info book.html book.dvi book.hpj 

ifdef aaa
book.html: book.texi rosenbrock.png
	$(MAKEINFO) $(MAKEINFOFLAGS) book.texi --html -o $@ --number-sections
else
book.html: book.texi rosenbrock.png
	$(TEXI2HTML) book.texi \
              -expand info \
              -number -split chapter -no-section_navigation -frames
endif

# Generate figure for HTML
rosenbrock.png: rosenbrock.eps
	$(GS) -sDEVICE=png16m -sOutputFile=tmp.png -dQUIET -dNOPAUSE rosenbrock.eps -c showpage -c quit
	$(PNGTOPNM) tmp.png | $(PNMCROP) | $(PNMTOPNG) > $@
	rm tmp.png

# TeX for paper printing
book.dvi: book.texi atexinfo.tex
	tex --c-style-errors --quiet book.texi || touch -t 010100001970 book.dvi


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
