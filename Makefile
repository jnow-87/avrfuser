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
include $(scripts_dir)/main.make

# init default flags
cflags := $(CFLAGS)
cxxflags := $(CXXFLAGS)
cppflags := $(CPPFLAGS) -I"include/" -I"$(build_tree)/" -Wall -O2
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
	$(rm) $(filter-out $(patsubst %/,%,$(dir $(build_tree)/$(scripts_dir))),$(wildcard $(build_tree)/*))

.PHONY: distclean
distclean:
	$(rm) $(build_tree)

####
## install
####

include $(scripts_dir)/install.make

.PHONY: install
install: all
	$(call install,$(build_tree)/main/avrfuser)

.PHONY: uninstall
uninstall:
	$(call uninstall,$(PREFIX)/avrfuser)
