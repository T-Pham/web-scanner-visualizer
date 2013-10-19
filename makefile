all: web_scanner_visualizer

web_scanner_visualizer: MyProject.o wapiti_parser.o pugixml.o database_handler.o sqlite3.o json_creator.o
	g++ MyProject.o wapiti_parser.o pugixml.o database_handler.o sqlite3.o json_creator.o -o web_scanner_visualizer -pthread -ldl -w

MyProject.o: MyProject.cpp
	g++ -c MyProject.cpp -w

wapiti_parser.o: ./src/wapiti_parser.cpp
	g++ -c ./src/wapiti_parser.cpp -w

pugixml.o: ./src/pugixml.cpp
	g++ -c ./src/pugixml.cpp -w

database_handler.o: ./src/database_handler.cpp
	g++ -c ./src/database_handler.cpp -w

sqlite3.o: ./src/sqlite3.c
	gcc -c ./src/sqlite3.c -w

json_creator.o: ./src/json_creator.cpp
	gcc -c ./src/json_creator.cpp -w

clean:
	-rm *.o