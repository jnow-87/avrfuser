################
###   init   ###
################

# init source and built tree
default_built_tree := built/
src_dirs := main/

# init build system variables
project_type := cxx

# include build system Makefile
include scripts/Makefile.inc

# init default flags
cflags := $(CFLAGS)
cxxflags := $(CXXFLAGS)
cppflags := $(CPPFLAGS) -I"include/" -Wall -O2
ldflags := $(LDFLAGS)
ldrflags := $(LDRFLAGS)
asflags := $(ASFLAGS)
archflags := $(ARCHFLAGS)

yaccflags := $(YACCFLAGS)
lexflags := $(LEXFLAGS)
gperfflags := $(GPERFFLAGS)

###################
###   targets   ###
###################

####
## build
####
.PHONY: all
all: check_tools check_config $(lib) $(bin)

.PHONY: debug
debug: cflags += -g
debug: cxxflags += -g
debug: asflags += -g
debug: all

####
## cleanup
####
.PHONY: clean
clean:
	$(rm) $(built_tree)

.PHONY: distclean
distclean:
	$(rm) $(built_tree)

####
## install
####
.PHONY: install-user
install-user: all
	$(mkdir) -p ~/bin
	$(cp) -au built/main/avrfuser ~/bin/

.PHONY: install-system
install-system: all
	$(mkdir) -p /usr/bin
	$(cp) -au built/main/avrfuser /usr/bin/

.PHONY: uninstall
uninstall:
	$(rm) -rf /usr/bin/avrfuser
	$(rm) -rf ~/bin/avrfuser

####
## help
####

.PHONY: help
help:
