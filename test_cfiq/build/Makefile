VPATH=./

CXX ?= g++
RM ?= rm -f
COMMA := ,

TARGET_ARCH ?= x86_64
SYSROOT ?=
DEPS_ROOT ?= ../../deps/$(TARGET_ARCH)
INCLUDE_DIR ?= ../include
NIRVANA_INCLUDE_DIR ?= ../include/nirvana
RUNTIME_LIB_DIR ?= $(DEPS_ROOT)/test_cfiq/lib
THIRDPART_LIB_DIR ?= $(DEPS_ROOT)/thirdpart_lib
RPATH ?= .

SYSROOT_FLAG = $(if $(SYSROOT),--sysroot=$(SYSROOT),)
RPATH_FLAG = $(if $(RPATH),-Wl$(COMMA)-rpath=$(RPATH),)
RPATH_LINK_DIRS ?= $(RUNTIME_LIB_DIR) $(THIRDPART_LIB_DIR)
RPATH_LINK_FLAGS = $(foreach dir,$(RPATH_LINK_DIRS),-Wl$(COMMA)-rpath-link=$(dir))

CPPFLAGS += $(SYSROOT_FLAG) -I$(INCLUDE_DIR) -I$(NIRVANA_INCLUDE_DIR) $(CPPFLAGS_EXTRA)
CXXFLAGS += -O3 -Wall -Wextra -pthread -D_REENTRANT -fmessage-length=0 -fPIC $(CXXFLAGS_EXTRA)
LDFLAGS += $(SYSROOT_FLAG) $(RPATH_FLAG) $(RPATH_LINK_FLAGS) $(LDFLAGS_EXTRA)
LDLIBS += -L$(RUNTIME_LIB_DIR) -l:libnirvana-kernel.so.8.15.1 -l:libQualityCtrl.so $(THIRDPART_LIB_DIR)/libgfortran.so $(LDLIBS_EXTRA)

CommonFiles=main.o ExtractMnt.o ScanNSortDirectory.o MakeBmp.o

Target=FingerQualityCtrlTest

ALL:$(Target)

objects:$(CommonFiles)

ExtractMnt.o:../src/ExtractMnt.cpp
	$(CXX) $(CPPFLAGS) $(CXXFLAGS) -c $^ -o $@
ScanNSortDirectory.o:../src/ScanNSortDirectory.cpp
	$(CXX) $(CPPFLAGS) $(CXXFLAGS) -c $^ -o $@
MakeBmp.o:../src/MakeBmp.cpp
	$(CXX) $(CPPFLAGS) $(CXXFLAGS) -c $^ -o $@
main.o:../src/main.cpp
	$(CXX) $(CPPFLAGS) $(CXXFLAGS) -c $^ -o $@
$(Target):$(CommonFiles)
	$(CXX) $^ $(LDFLAGS) $(LDLIBS) -o $@

print-config:
	@echo "TARGET_ARCH=$(TARGET_ARCH)"
	@echo "CXX=$(CXX)"
	@echo "SYSROOT=$(SYSROOT)"
	@echo "DEPS_ROOT=$(DEPS_ROOT)"
	@echo "RUNTIME_LIB_DIR=$(RUNTIME_LIB_DIR)"
	@echo "THIRDPART_LIB_DIR=$(THIRDPART_LIB_DIR)"

clean:
	$(RM) *.o $(Target)

.PHONY: ALL clean objects print-config
