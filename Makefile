ALL_CPPFLAGS = $(CPPFLAGS) $(shell pkg-config opencv --cflags)
ALL_LDFLAGS = $(shell pkg-config opencv --libs)

find-face: find-face.cc
	$(CXX) $(CXXFLAGS) $(ALL_CPPFLAGS) $^ $(ALL_LDFLAGS) -o $@
