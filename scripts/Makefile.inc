include $(scripts_dir)/Makefile.helper
include $(scripts_dir)/Makefile.compile

###############################
###   verbosity and debug   ###
###############################

V ?= 0

ifeq ($(V),0)
  QBUILD := @
  QUTIL := @
endif

ifeq ($(call cond_ge,$(V),1),1)
  QBUILD :=
  QUTIL := @
endif

ifeq ($(call cond_ge,$(V),2),1)
  QBUILD :=
  QUTIL :=
endif

# print DEBUG message
#
#	$(call pdebug,<msg>)
ifeq ($(call cond_ge,$(V),3),1)
  define pdebug0
    $(info $1)
  endef
endif

ifeq ($(call cond_ge,$(V),4),1)
  define pdebug1
    $(info $1)
  endef
endif

ifeq ($(call cond_ge,$(V),5),1)
  define pdebug2
    $(info $1)
  endef
endif


################
###   init   ###
################

# include config file if defined
ifneq ("$(config)","")
-include $(config)
endif

# check project type
ifneq ($(project_type),c)
  ifneq ($(project_type),cxx)
    $(error invalid $$(project_type), choose either 'c' or 'cxx')
  endif
endif

# init build system variables
all_flags := cflags cxxflags cppflags asflags ldflags ldrflags archflags hostcflags hostcxxflags hostcppflags hostasflags hostldflags hostarchflags yaccflags lexflags gperfflags

obj_types := obj obj-nobuiltin hostobj hostobj-nobuiltin
lib_types := lib hostlib
bin_types := bin hostbin
all_types := $(obj_types) $(lib_types) $(bin_types)

all_tools := cc cxx as ld ar hostcc hostcxx hostas hostld hostar lex yacc gperf

# init global variables for list of objects, libraries and executables
$(foreach type,$(supported_types), \
	$(eval $(type) :=) \
)

# disable built-in rules
.SUFFIXES:

# disable removal of temporary files
.SECONDARY:

# init source and built tree
$(call set_default,BUILT_TREE, $(default_built_tree))
$(call set_default,SRC_TREE, .)

built_tree := $(patsubst %/,%,$(BUILT_TREE))
src_tree := $(patsubst %/,%,$(SRC_TREE))

# init variables for directory traversal
build := $(scripts_dir)/Makefile.build
included :=

# set default values for tools
$(call set_default,CC, gcc)
$(call set_default,CXX, g++)
$(call set_default,AS, as)
$(call set_default,LD, ld)
$(call set_default,AR, ar)

$(call set_default,HOSTCC, gcc)
$(call set_default,HOSTCXX, g++)
$(call set_default,HOSTAS, as)
$(call set_default,HOSTLD, ld)
$(call set_default,HOSTAR, ar)

$(call set_default,LEX, flex)
$(call set_default,YACC, bison)
$(call set_default,GPERF, gperf)

cc := $(QBUILD)$(CC)
cxx := $(QBUILD)$(CXX)
as := $(QBUILD)$(AS)
ld := $(QBUILD)$(LD)
ar := $(QBUILD)$(AR)

hostcc := $(QBUILD)$(HOSTCC)
hostcxx := $(QBUILD)$(HOSTCXX)
hostas := $(QBUILD)$(HOSTAS)
hostld := $(QBUILD)$(HOSTLD)
hostar := $(QBUILD)$(HOSTAR)

lex := $(QBUILD)$(LEX)
yacc := $(QBUILD)$(YACC)
gperf := $(QBUILD)$(GPERF)
gperf_c_header := $(QBUILD)$(scripts_dir)/gperf_c_header.sh
gperf_cxx_header := $(QBUILD)$(scripts_dir)/gperf_cxx_header.sh

echo := @echo
printf := @printf
rm := $(QUTIL)rm -rf
mkdir := $(QUTIL)mkdir -p
touch := $(QUTIL)touch
cp := $(QUTIL)cp
sym_link := $(QUTIL)ln -s


###########################
###   default targets   ###
###########################

# fake target as default target
.PHONY: all
all: check_tools check_config

# check if all tools are available
check_tools_error := "please make sure that $${id} or CONFIG_$${id} is initialised correctly, current value is \"$${val}\""

tools :=
$(foreach tool,$(all_tools), \
	$(eval tools += $(call upper_case,$(tool))=$(subst @,,$($(tool)))) \
)

.PHONY: check_tools
check_tools:
	@r=0; \
	for tool in $(tools); do \
		id=$$(echo $${tool} | cut -d '=' -f 1); \
		val=$$(echo $${tool} | cut -d '=' -f 2); \
		test -n "$$(which $${val})" || (echo $(check_tools_error); exit 1); \
		test $${?} -eq 1 && r=1; \
	done; \
	exit $${r}

# check if $(config) file is present
.PHONY: check_config
check_config:
ifneq ($(shell test -e $(config) && echo $(config)),$(config))
	$(call error,$(config) does not exist, please run $$make menuconfig or $$make defconfig_<target> first)
endif

# config system targets
ifeq ("$(use_config_sys)","y")
  ifneq ("$(config_ftype)","")
    config_header := $(built_tree)/config/config.h

    mconfig_src := $(scripts_dir)/mconf
    mconfig := $(built_tree)/$(mconfig_src)/mconfig
    confheader_src := $(scripts_dir)/mconf
    confheader := $(built_tree)/$(confheader_src)/confheader
    fixdep_src := $(scripts_dir)/fixdep
    fixdep := $(built_tree)/$(fixdep_src)/fixdep

    config_tools := $(fixdep) $(mconfig) $(confheader)

    mconfig := $(QBUILD)$(mconfig)
    confheader := $(QBUILD)$(confheader)
    fixdep := $(QBUILD)$(fixdep)

.PHONY: configtools
configtools: do_basic := y
configtools: $(config_tools)

.PHONY: check_configheader
check_configheader: $(config_header)

$(config_header): $(config) $(config_tools)
	$(confheader) $(config_ftype) $(dir $(config_header)) $(config_header) 

.PHONY: menuconfig
menuconfig: check_tools configtools
	$(mconfig) $(config_ftype)

all: configtools check_configheader
  endif
else
  do_basic := y
endif

# default config targets
ifneq ("$(config_tree)","")
config_files = $(notdir $(wildcard $(config_tree)/*))

$(foreach cfg, $(config_files), \
	$(call gen_rule_basic,cmd_defconfig,defconfig_$(cfg),$(config_tree)/$(cfg)) \
)
endif

# include dependency files
include $(shell find $(built_tree)/ -name \*.d 2>/dev/null)

# help for the build system
help-buildsys:
	$(printf) "   \033[1m\033[4msub-directory Makefile syntax - available identifiers\033[0m\n"
	$(printf) "\n"
	$(printf) "      \033[1mbuild targets\033[0m\n"
	$(printf) "         %25s\t%s\n" "[host]obj-y" "define target object files, also combined into ./obj.o"
	$(printf) "         %25s\t%s\n" "[host]lib-y" "define target libraries"
	$(printf) "         %25s\t%s\n" "[host]bin-y" "define target binaries"
	$(printf) "         %25s\t%s\n" "<target>-y" "specify the objects required to build the compound object <target>"
	$(printf) "\n"
	$(printf) "             - prefixed \"host\" indicates to use host compiler tools\n"
	$(printf) "             - [host]obj-y can take a directory as \"obj-y := <dir>/\"\n"
	$(printf) "                  this builds <dir>/obj.o and adds <dir>/ to subdir-y\n"
	$(printf) "\n"
	$(printf) "      \033[1msub-directories\033[0m\n"
	$(printf) "         %25s\t%s\n" "subdir-y" "define sub-directories to include"
	$(printf) "\n"
	$(printf) "      \033[1mflags\033[0m\n"
	$(printf) "         %25s\t%s\n" "[host]<flags>-y" "specify <flags> to be used for all targets within the current directory"
	$(printf) "         %25s\t%s\n" "<target>-[host]<flags>" "specify <flags> only applied for that <target>"
	$(printf) "         %25s\t%s\n" "subdir-[host]<flags>" "specify <flags> apply to all sub-directories"
	$(printf) "         %25s\t%s\n" "<subdir>-[host]<flags>" "specify <flags> apply to all sub-directories"
	$(printf) "\n"
	$(printf) "         %25s\t%s\n" "cflags" "c compiler flags"
	$(printf) "         %25s\t%s\n" "cxxflags" "c++ compiler flags"
	$(printf) "         %25s\t%s\n" "cppflags" "c pre-processor flags"
	$(printf) "         %25s\t%s\n" "asflags" "assembler flags"
	$(printf) "         %25s\t%s\n" "ldflags" "linker flags"
	$(printf) "         %25s\t%s\n" "ldrflags" "linker flags for creating relocatable output (ld -r)"
	$(printf) "         %25s\t%s\n" "archflags" "architecture specific flags"
	$(printf) "         %25s\t%s\n" "yaccflags" "yacc flags"
	$(printf) "         %25s\t%s\n" "lexflags" "lex flags"
	$(printf) "         %25s\t%s\n" "gperfflags" "gperf flags"
	$(printf) "\n\n"
	$(printf) "   \033[1m\033[4muser defineable variables\033[0m\n"
	$(printf) "\n"
	$(printf) "         %25s\t%s\n" "V" "change verbosity, default = 0"
	$(printf) "         %25s\t%s\n" "BUILT_TREE" "define output directory"
	$(printf) "         %25s\t%s\n" "SRC_TREE" "define source directory"
	$(printf) "\n"
	$(printf) "         %25s\t%s\n" "[HOST]CC"
	$(printf) "         %25s\t%s\n" "[HOST]CXX"
	$(printf) "         %25s\t%s\n" "[HOST]AS"
	$(printf) "         %25s\t%s\n" "[HOST]LD"
	$(printf) "         %25s\t%s\n" "[HOST]AR"
	$(printf) "         %25s\t%s\n" "LEX"
	$(printf) "         %25s\t%s\n" "YACC"
	$(printf) "         %25s\t%s\n" "GPERF"
	$(printf) "\n"
	$(printf) "         %25s\t%s\n" "[HOST]CFLAGS"
	$(printf) "         %25s\t%s\n" "[HOST]CXXFLAGS"
	$(printf) "         %25s\t%s\n" "[HOST]CPPFLAGS"
	$(printf) "         %25s\t%s\n" "[HOST]LDFLAGS"
	$(printf) "         %25s\t%s\n" "[HOST]ASFLAGS"
	$(printf) "         %25s\t%s\n" "[HOST]ARCHFLAGS"
	$(printf) "         %25s\t%s\n" "LEXFLAGS"
	$(printf) "         %25s\t%s\n" "YACCFLAGS"
	$(printf) "         %25s\t%s\n" "GPERFFLAGS"
	$(printf) "\n\n"
	$(printf) "   \033[1m\033[4mspecial targets\033[0m\n"
	$(printf) "\n"
	$(printf) "         %25s\t%s\n" "<path>/<target>.i" "build pre-processed target file"
	$(printf) "         %25s\t%s\n" "<path>/<target>.S" "build assembly target file"
	$(printf) "\n"
	$(printf) "         %25s\t%s\n" "menuconfig" "graphical configuration"
	$(printf) "         %25s\t%s\n" "defconfig_*" "apply default configuration"
	$(printf) "\n"
	$(printf) "         %25s\t%s\n" "check_tools" "test if the variables defined in 'all_tools' define valid binaries"
	$(printf) "         %25s\t%s\n" "check_config" "test if the config file defined in 'config' is accesable"


######################################
###   finally traverse Makefiles   ###
######################################

# start subdirectory traversal
# 	if subdir-y is empty include '.' (within $(src_tree))
$(if $(src_dirs), \
	$(call dinclude,$(src_dirs) $(mconfig_src) $(fixdep_src) $(confheader_src)) \
	, \
	$(error $$(src_dirs) is empty, please define initial source directories) \
)