GCC=gcc
GPP=g++
CFLAGS=-c -w
LDFLAGS=-pthread -ldl -w
SOURCES_CPP=web_scanner_visualizer.cpp ./src/wapiti_parser.cpp ./src/pugixml.cpp ./src/database_handler.cpp ./src/json_creator.cpp ./src/skipfish_parser.cpp ./src/helper.cpp
SOURCES_C=./src/sqlite3.c
OBJECTS_CPP=$(SOURCES_CPP:.cpp=.o)
OBJECTS_C=$(SOURCES_C:.c=.o)
EXECUTABLE=web_scanner_visualizer

all: $(SOURCES_CPP) $(SOURCES_C) $(EXECUTABLE)

$(EXECUTABLE): $(OBJECTS_CPP) $(OBJECTS_C)
	$(GPP) $(LDFLAGS) $^ -o $@

.cpp.o:
	$(GPP) $(CFLAGS) $< -o $@

.c.o:
	$(GCC) $(CFLAGS) $< -o $@

clean:
	-rm -f $(OBJECTS_CPP) $(OBJECTS_C) $(EXECUTABLE)