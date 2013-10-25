GPP=g++
GCC=gcc
CFLAGS=-c -w
LDFLAGS=-pthread -ldl -w
SOURCES=main.cpp $(wildcard ./src/*.c*)
OBJECTS=$(addsuffix .o,$(basename $(SOURCES)))
EXECUTABLE=web_scanner_visualizer

all: $(SOURCES) $(EXECUTABLE)

$(EXECUTABLE): $(OBJECTS)
	$(GPP) $(LDFLAGS) $^ -o $@

.cpp.o:
	$(GPP) $(CFLAGS) $< -o $@

.c.o:
	$(GCC) $(CFLAGS) $< -o $@

clean:
	rm -f $(OBJECTS) $(EXECUTABLE)
