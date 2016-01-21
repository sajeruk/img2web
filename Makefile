all: cassandra_db.o
	g++ img2web.cpp cassandra_db.o -std=c++11 -fPIC -luv -lcassandra -lfastcgi-daemon2 -shared -o libimg2web.so
cassandra_db.o:
	g++ -fPIC -c cassandra_db.cpp -o cassandra_db.o -std=c++11
