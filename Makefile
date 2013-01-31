ALL_CPPFLAGS = $(CPPFLAGS) $(shell pkg-config opencv --cflags)
ALL_CXXFLAGS = $(CXXFLAGS) -Wall
ALL_LDFLAGS = $(shell pkg-config opencv --libs)

OPENCV_2_1 = $(shell pkg-config --modversion opencv | grep ^2.1)
ifneq ($(OPENCV_2_1),)
  ALL_CPPFLAGS += -DOPENCV_2_1
endif

find-face: find-face.cc Makefile
	$(CXX) $(ALL_CXXFLAGS) $(ALL_CPPFLAGS) $< $(ALL_LDFLAGS) -o $@
