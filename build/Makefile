vpath %.cpp ../src/caffe:../src/caffe/layers:../src/caffe/solvers:../src/caffe/util
vpath %.cu ../src/caffe:../src/caffe/layers:../src/caffe/solvers:../src/caffe/util
vpath %.cc ../src
vpath %.cpp ../src_cfiq


CPU_ONLY=1

CUDA_DIR=/usr/local/cuda
CUDA_LIB=-L${CUDA_DIR}/lib64 -lcuda -lcudart -lcurand -lcublas -lcudnn
#CPP_LIB=-lboost_system -lboost_filesystem -lboost_thread -lglog -lgflags -lhdf5_serial -lhdf5_serial_hl  -lcblas -latlas
#CPP_LIB=-lboost_system -lboost_filesystem -lboost_thread -lglog  -lhdf5_serial -lhdf5_serial_hl  -lcblas -latlas
#CPP_LIB=-lboost_system -lboost_filesystem -lboost_thread -lglog  -lcblas -latlas
#CPP_LIB=-lboost_system -lboost_filesystem -lboost_thread  -lcblas -latlas
CPP_LIB= -lcblas -latlas

Include=-I../include -I/usr/local/cuda-10.0/include -I../include_cfiq
#CXXFlags=-std=c++11 -O3 -Wall -Wextra -MMD -MP -pthread -D_REENTRANT -fmessage-length=0 -fPIC ${Include} ${CUDA_LIB} ${CPP_LIB}
ifeq ($(CPU_ONLY), 1)
#	CXXFlags= -std=c++11 -O3 -MMD -MP -Wl,-rpath=./ -DCPU_ONLY -pthread -fPIC ${Include} ${CPP_LIB}
#	CXXFlags=-O3 -Wall -Wextra -Wl,-rpath=./ -pthread -D_REENTRANT -fmessage-length=0 -fPIC -DCPU_ONLY ${Include} ${CPP_LIB}
#	CXXFlags= -Wl,-rpath=./  -fPIC -DCPU_ONLY ${Include} -lstdc++ ${CPP_LIB} -lprotobuf -lboost_thread ${OPENCV_LIB} 
	CXXFlags= -Wl,-rpath=./  -fPIC -DCPU_ONLY ${Include} -lstdc++ ${CPP_LIB} -lprotobuf ${OPENCV_LIB} 
else
#	CXXFlags= -std=c++11 -O3 -MMD -MP -Wl,-rpath=./ -DUSE_CUDNN -pthread -fPIC ${Include} ${CPP_LIB} ${CUDA_LIB}
#	CXXFlags=-O3 -Wall -Wextra -Wl,-rpath=./ -pthread -D_REENTRANT -fmessage-length=0 -fPIC -DUSE_CUDNN ${Include} ${CPP_LIB} ${CUDA_LIB}
#	CXXFlags= -Wl,-rpath=./  -fPIC -DUSE_CUDNN ${Include} lstdc++ ${CPP_LIB} -lprotobuf -lboost_thread ${OPENCV_LIB} ${CUDA_LIB}
	CXXFlags= -Wl,-rpath=./  -fPIC -DUSE_CUDNN ${Include} -lstdc++ ${CPP_LIB} -lprotobuf ${OPENCV_LIB} ${CUDA_LIB}
endif

TARGET=libQualityCtrl.so
all:${TARGET}
culib+=$(subst .cu,.cuo,$(notdir $(wildcard ../src/caffe/util/*.cu)))
culib+=$(subst .cu,.cuo,$(notdir $(wildcard ../src/caffe/layers/*.cu)))
culib+=$(subst .cu,.cuo,$(notdir $(wildcard ../src/caffe/solver/*.cu)))

cpplib+=$(subst .cpp,.co,$(notdir $(wildcard ../src/caffe/*.cpp)))
cpplib+=$(subst .cpp,.co,$(notdir $(wildcard ../src/caffe/util/*.cpp)))
cpplib+=$(subst .cpp,.co,$(notdir $(wildcard ../src/caffe/layers/*.cpp)))
cpplib+=$(subst .cpp,.co,$(notdir $(wildcard ../src/caffe/solver/*.cpp)))
cpplib+=$(subst .cpp,.co,$(notdir $(wildcard ../src_cfiq/*.cpp)))

#OPENCV_LIB = ../opencv_lib/libopencv_core.so.3.2 ../opencv_lib/libopencv_imgcodecs.so.3.2 ../opencv_lib/libopencv_imgproc.so.3.2
OPENCV_LIB=-lopencv_core -lopencv_highgui -lopencv_imgproc -lopencv_imgcodecs

NIRVANA_LIB=../nirvana_lib/libnirvana-kernel.so.8

ifeq ($(CPU_ONLY), 1)
	DEPENDENCY= caffepb.o ${cpplib}
else
	DEPENDENCY= caffepb.o ${cpplib} ${culib}
endif
libQualityCtrl.so:${DEPENDENCY}
	g++  -fPIC -pthread ${NIRVANA_LIB} $^ -shared -o $@  ${CXXFlags} 
##g++ $^ -o $@ `pkg-config --cflags --libs protobuf` ${CXXFlags} 
##g++ $^ -o $@-lprotobuf 

caffepb.o:caffe.pb.cc 
	g++ -I../include/caffe/proto ${CXXFlags}  -c $^ -o $@

%.co:%.cpp
	g++ ${CXXFlags} -c $^ -o $@

%.cuo:%.cu
#-arch -code 一定根据CUDA GPU版本来
	nvcc -arch=compute_52 -code=sm_52 -DUSE_CUDNN ${Include} -c -Xcompiler -fPIC $^ -o $@
##nvcc -arch=compute_30 -code=sm_30 -ccbin=/usr/bin/g++ -Xcompiler -fPIC -DUSE_CUDNN ${Include} -c $^ -o $@
##nvcc -DUSE_CUDNN ${Include} -c $^ -o $@
	
clean:
	rm -rf *.d *.o *.co *.cuo ${TARGET}
