PWD					:= $(shell pwd)
WORK_DIR			:= /var/tmp/$(LIB_NAME)

JOBS				?= $(shell expr $(shell nproc) + 1)
BUILD_TYPE			?= Debug
CXX					?= g++

OPT_BUILD_TYPE		= -DCMAKE_BUILD_TYPE=$(BUILD_TYPE)
OPT_COMPILER		= -DCMAKE_CXX_COMPILER=$(CXX)

define Options =
	-G 'CodeBlocks - Unix Makefiles'\
	$(OPT_BUILD_TYPE)\
	$(OPT_COMPILER)\
	$(ADDITIONAL_CMAKE_OPTION)
endef

ifdef MAKE_GDBINIT
	MAKE_GDBINIT_CMD := cd $(PWD); python3 $(COMMON_MAKE_PATH)/make_gdbinit.py $(PWD) $(LIB_NAME) $(WORK_DIR)/.gdbinit;
else
	MAKE_GDBINIT_CMD :=
endif

CMake = mkdir -p $(WORK_DIR); cd $(WORK_DIR); cmake $(PWD) $(Options); $(MAKE_GDBINIT_CMD) $(ADDITIONAL_CMD)
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
test:
	cd $(WORK_DIR); pwd; ctest -j$(JOBS);
