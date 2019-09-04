FLAG=-L /usr/lib64/mysql -lmysqlclient -ljsoncpp 

image_server:image_server.cc 
	g++ $^ -o $@ -std=c++11 -lpthread $(FLAG)

.PHONY:clean
clean:
	rm image_server
