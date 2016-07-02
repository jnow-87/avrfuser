################
###   init   ###
################

# init source and build tree
scripts_dir := scripts/build
default_build_tree := build/
src_dirs := main/
use_config_sys := n
githooks_tree := .githooks

# init build system variables
project_type := cxx

# include build system Makefile
include $(scripts_dir)/Makefile.inc

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
all: $(lib) $(bin)

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
	$(rm) $(filter-out $(build_tree)/$(scripts_dir),$(wildcard $(build_tree)/*))

.PHONY: distclean
distclean:
	$(rm) $(build_tree)

####
## install
####
.PHONY: install-user
install-user: all
	$(mkdir) -p ~/bin
	$(cp) -au $(build_tree)/main/avrfuser ~/bin/

.PHONY: install-system
install-system: all
	$(mkdir) -p /usr/bin
	$(cp) -au $(build_tree)/main/avrfuser /usr/bin/

.PHONY: uninstall
uninstall:
	$(rm) -rf /usr/bin/avrfuser
	$(rm) -rf ~/bin/avrfuser

####
## help
####

.PHONY: help
help:
