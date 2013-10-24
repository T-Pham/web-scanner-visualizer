all: web_scanner_visualizer.o wapiti_parser.o pugixml.o database_handler.o sqlite3.o json_creator.o skipfish_parser.o
	g++ web_scanner_visualizer.o wapiti_parser.o pugixml.o database_handler.o sqlite3.o json_creator.o skipfish_parser.o -o web_scanner_visualizer -pthread -ldl -w

web_scanner_visualizer.o: web_scanner_visualizer.cpp
	g++ -c web_scanner_visualizer.cpp -w

wapiti_parser.o: ./src/wapiti_parser.cpp
	g++ -c ./src/wapiti_parser.cpp -w

pugixml.o: ./src/pugixml.cpp
	g++ -c ./src/pugixml.cpp -w

database_handler.o: ./src/database_handler.cpp
	g++ -c ./src/database_handler.cpp -w

sqlite3.o: ./src/sqlite3.c
	gcc -c ./src/sqlite3.c -w

json_creator.o: ./src/json_creator.cpp
	g++ -c ./src/json_creator.cpp -w

skipfish_parser.o: ./src/skipfish_parser.cpp
	g++ -c ./src/skipfish_parser.cpp

clean:
	-rm *.o