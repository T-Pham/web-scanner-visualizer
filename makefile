all: web_scanner_visualizer

web_scanner_visualizer: MyProject.o wapiti_parser.o pugixml.o database_handler.o sqlite3.o urls.o
	g++ MyProject.o wapiti_parser.o pugixml.o database_handler.o sqlite3.o urls.o -o web_scanner_visualizer -pthread -ldl -w

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

urls.o: ./src/urls.cpp
	g++ -c ./src/urls.cpp -w
clean:
	-rm *.o