vpath %.cpp ../src/caffe:../src/caffe/layers:../src/caffe/solvers:../src/caffe/util
vpath %.cu ../src/caffe:../src/caffe/layers:../src/caffe/solvers:../src/caffe/util
vpath %.cc ../src
vpath %.cpp ../src_cfiq

CXX ?= g++
NVCC ?= nvcc
RM ?= rm -f
COMMA := ,

CPU_ONLY ?= 1
TARGET_ARCH ?= x86_64
SYSROOT ?=
DEPS_ROOT ?= ../deps/$(TARGET_ARCH)
INCLUDE_ROOT ?= ../include
CFIQ_INCLUDE_DIR ?= ../include_cfiq
THIRDPART_LIB_DIR ?= $(DEPS_ROOT)/thirdpart_lib
OPENCV_LIB_DIR ?= $(DEPS_ROOT)/opencv_lib
CUDA_DIR ?= /usr/local/cuda
CUDA_INCLUDE_DIR ?= $(CUDA_DIR)/include
CUDA_LIB_DIR ?= $(CUDA_DIR)/lib64
RPATH ?= ./

SYSROOT_FLAG = $(if $(SYSROOT),--sysroot=$(SYSROOT),)
RPATH_FLAG = $(if $(RPATH),-Wl$(COMMA)-rpath=$(RPATH),)
RPATH_LINK_DIRS ?= $(THIRDPART_LIB_DIR) $(OPENCV_LIB_DIR)
RPATH_LINK_FLAGS = $(foreach dir,$(RPATH_LINK_DIRS),-Wl$(COMMA)-rpath-link=$(dir))

Protobuf_LIB ?= $(THIRDPART_LIB_DIR)/libprotobuf.so.26.0.6
Boost_LIB ?= -L$(THIRDPART_LIB_DIR)
CBLAS_LIB ?= $(THIRDPART_LIB_DIR)/cblas_LINUX.a $(THIRDPART_LIB_DIR)/blas_LINUX.a
MATH_LIB ?= $(CBLAS_LIB)
Gfortran_LIB ?= $(THIRDPART_LIB_DIR)/libgfortran.so
OPENCV_LIB ?= $(OPENCV_LIB_DIR)/libopencv_core.so.3.2 $(OPENCV_LIB_DIR)/libopencv_imgcodecs.so.3.2 $(OPENCV_LIB_DIR)/libopencv_imgproc.so.3.2
CUDA_LIB ?= -L$(CUDA_LIB_DIR) -lcuda -lcudart -lcurand -lcublas -lcudnn
CPP_LIB ?=
DL_LIB ?= -ldl

BASE_INCLUDES := -I$(INCLUDE_ROOT) -I$(CFIQ_INCLUDE_DIR)
ifeq ($(CPU_ONLY), 1)
	MODE_DEFINES := -DCPU_ONLY
	MODE_INCLUDES :=
	ThirdParty_LIB := $(CPP_LIB) $(Protobuf_LIB) $(Boost_LIB) $(OPENCV_LIB)
else
	MODE_DEFINES := -DUSE_CUDNN
	MODE_INCLUDES := -I$(CUDA_INCLUDE_DIR)
	ThirdParty_LIB := $(CPP_LIB) $(Protobuf_LIB) $(Boost_LIB) $(OPENCV_LIB) $(CUDA_LIB)
endif

CPPFLAGS += $(SYSROOT_FLAG) $(MODE_DEFINES) $(BASE_INCLUDES) $(MODE_INCLUDES) $(CPPFLAGS_EXTRA)
CXXFLAGS += -fPIC $(CXXFLAGS_EXTRA)
SONAME ?= $(notdir $(TARGET))
SONAME_FLAG = $(if $(SONAME),-Wl$(COMMA)-soname=$(SONAME),)
LDFLAGS += $(SYSROOT_FLAG) -shared $(SONAME_FLAG) $(RPATH_FLAG) $(RPATH_LINK_FLAGS) $(LDFLAGS_EXTRA)
LDLIBS += $(MATH_LIB) -pthread $(ThirdParty_LIB) $(Gfortran_LIB) $(DL_LIB) $(LDLIBS_EXTRA)

TARGET=libQualityCtrl.so
all:$(TARGET)
culib+=$(subst .cu,.cuo,$(notdir $(wildcard ../src/caffe/util/*.cu)))
culib+=$(subst .cu,.cuo,$(notdir $(wildcard ../src/caffe/layers/*.cu)))
culib+=$(subst .cu,.cuo,$(notdir $(wildcard ../src/caffe/solver/*.cu)))

cpplib+=$(subst .cpp,.co,$(notdir $(wildcard ../src/caffe/*.cpp)))
cpplib+=$(subst .cpp,.co,$(notdir $(wildcard ../src/caffe/util/*.cpp)))
cpplib+=$(subst .cpp,.co,$(notdir $(wildcard ../src/caffe/layers/*.cpp)))
cpplib+=$(subst .cpp,.co,$(notdir $(wildcard ../src/caffe/solver/*.cpp)))
cpplib+=$(subst .cpp,.co,$(notdir $(wildcard ../src_cfiq/*.cpp)))

ifeq ($(CPU_ONLY), 1)
	DEPENDENCY= caffepb.o $(cpplib)
else
	DEPENDENCY= caffepb.o $(cpplib) $(culib)
endif

objects:$(DEPENDENCY)

$(TARGET):$(DEPENDENCY)
	$(CXX) $^ -o $@ $(LDFLAGS) $(LDLIBS)

caffepb.o:caffe.pb.cc
	$(CXX) -I../include/caffe/proto $(CPPFLAGS) $(CXXFLAGS) -c $^ -o $@

%.co:%.cpp
	$(CXX) $(CPPFLAGS) $(CXXFLAGS) -c $^ -o $@

%.cuo:%.cu
# -arch -code must match the CUDA GPU version.
	$(NVCC) -arch=compute_52 -code=sm_52 -DUSE_CUDNN $(BASE_INCLUDES) $(MODE_INCLUDES) -c -Xcompiler -fPIC $^ -o $@

print-config:
	@echo "TARGET_ARCH=$(TARGET_ARCH)"
	@echo "CXX=$(CXX)"
	@echo "CPU_ONLY=$(CPU_ONLY)"
	@echo "SYSROOT=$(SYSROOT)"
	@echo "DEPS_ROOT=$(DEPS_ROOT)"
	@echo "THIRDPART_LIB_DIR=$(THIRDPART_LIB_DIR)"
	@echo "OPENCV_LIB_DIR=$(OPENCV_LIB_DIR)"

clean:
	$(RM) *.d *.o *.co *.cuo $(TARGET)

.PHONY: all clean objects print-config
