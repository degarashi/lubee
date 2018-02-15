PWD			:= $(shell pwd)
LIB_NAME	:= lubee
WORK_DIR	:= /var/tmp/$(LIB_NAME)
JOBS		:= 5

BUILD_TYPE			?= Debug
CXX					?= g++

OPT_BUILD_TYPE		= -DCMAKE_BUILD_TYPE=$(BUILD_TYPE)
OPT_COMPILER		= -DCMAKE_CXX_COMPILER=$(CXX)

define Options =
	-G 'CodeBlocks - Unix Makefiles'\
	$(OPT_BUILD_TYPE)\
	$(OPT_COMPILER)
endef

CMake = mkdir -p $(WORK_DIR); cd $(WORK_DIR); cmake $(PWD) $(Options); python3 $(PWD)/make_gdbinit.py $(PWD) $(LIB_NAME) $(WORK_DIR)/.gdbinit;
Make = cd $(WORK_DIR); make -j$(JOBS);
Clean = cd $(WORK_DIR); make clean; rm -f Makefile CMakeCache.txt;

.PHONY: cmake clean tags
all: $(WORK_DIR)/Makefile
	$(call Make)
cmake:
	$(call CMake)
$(WORK_DIR)/Makefile:
	$(call CMake)
clean:
	$(call Clean)
tags:
	@ctags -R -f ./.git/ctags .
	@cscope -b -f ./.git/cscope.out
