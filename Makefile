all: base64
	g++ img2web.cpp -std=c++11 -fPIC -luv -lcassandra -lfastcgi-daemon2 -shared -o libimg2web.so base64.o
base64:
	g++ -fPIC -c base64.cpp
