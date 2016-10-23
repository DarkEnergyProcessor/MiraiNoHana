# Only uses GCC, unlike HonokaMiku which can be configured to use MSVC
# To set flags, use CXXFLAGS instead of CFLAGS

all: default

default: TEXBFetch.o TEXBModify.o TEXBPixel.o TEXBSave.o TIMG.o Not_Umi_Song.o
	-mkdir bin
	$(CXX) -o bin/MiraiNoHana $(CXXFLAGS) $(CPPFLAGS) TEXBFetch.o TEXBModify.o TEXBPixel.o TEXBSave.o TIMG.o Not_Umi_Song.o
	-rm *.o

TEXBFetch.o: itsudemo-libtexb/TEXBFetch.cpp
	$(CXX) -c $(CXXFLAGS) $(CPPFLAGS) itsudemo-libtexb/TEXBFetch.cpp

TEXBModify.o: itsudemo-libtexb/TEXBModify.cpp
	$(CXX) -c $(CXXFLAGS) $(CPPFLAGS) itsudemo-libtexb/TEXBModify.cpp

TEXBPixel.o: itsudemo-libtexb/TEXBPixel.cpp
	$(CXX) -c $(CXXFLAGS) $(CPPFLAGS) itsudemo-libtexb/TEXBPixel.cpp

TEXBSave.o: itsudemo-libtexb/TEXBSave.cpp
	$(CXX) -c $(CXXFLAGS) $(CPPFLAGS) itsudemo-libtexb/TEXBSave.cpp

TIMG.o: itsudemo-libtexb/TIMG.cpp
	$(CXX) -c $(CXXFLAGS) $(CPPFLAGS) itsudemo-libtexb/TIMG.cpp

Not_Umi_Song.o: src/Not_Umi_Song.cpp
	$(CXX) -c $(CXXFLAGS) $(CPPFLAGS) -Iitsudemo-libtexb src/Not_Umi_Song.cpp