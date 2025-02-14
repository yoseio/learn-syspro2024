SOURCES := $(wildcard *.cpp)
TARGETS := $(SOURCES:.cpp=.out)

.PHONY: all clean

all: $(TARGETS)

%.out: %.cpp
	$(CXX) -o $@ $<

clean:
	rm -f $(TARGETS)
