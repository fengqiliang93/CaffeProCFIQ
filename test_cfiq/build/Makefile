VPATH=./

Include=../include

CXXFlags=-O3 -Wall -Wextra -Wl,-rpath-link=./lib -pthread -D_REENTRANT -fmessage-length=0 -fPIC -I${Include} -I../include/nirvana -L../lib -lQualityCtrl

CommonFiles=main.o ExtractMnt.o ScanNSortDirectory.o MakeBmp.o ../lib/libnirvana-kernel.so.8

Target=FingerQualityCtrlTest

ALL:${Target}

ExtractMnt.o:../src/ExtractMnt.cpp
	g++ -c $^ ${CXXFlags} -o $@
ScanNSortDirectory.o:../src/ScanNSortDirectory.cpp
	g++ -c $^ ${CXXFlags} -o $@
MakeBmp.o:../src/MakeBmp.cpp
	g++ -c $^ ${CXXFlags} -o $@
main.o:../src/main.cpp
	g++ -c $^ ${CXXFlags} -o $@
FingerQualityCtrlTest:${CommonFiles}
	g++ $^ ${CXXFlags} -o $@
clean:
	rm *.o ${Target} -f
