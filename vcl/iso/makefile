#
# fsm@robots.ox.ac.uk
#

HEADERS := $(wildcard vcl_*.hhh)
HEADERS := $(HEADERS:%.hhh=%.h)

all: $(HEADERS)

clean:
	rm -f $(HEADERS)

$(HEADERS) : %.h : %.hhh
	@echo "making $@ from $<"
	@cat $< | ./zap.pl > $@~
	@mv $@~ $@
