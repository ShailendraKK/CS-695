KVServer: server.cpp KVClientHandler.h KVStore.h KVCache.cpp KVCache.hpp HelperFunctions.cpp Node.cpp ip_addr.c
	g++ -g -o KVServer server.cpp ThreadPool.h KVClientHandler.cpp KVStore.cpp KVCache.cpp sha1.cpp HelperFunctions.cpp Node.cpp ip_addr.c -lpthread -lrt

.Phoney:clean
clean:
	@rm -f *.dat
	@rm -f KVServer
	@rm -f Data.xml
