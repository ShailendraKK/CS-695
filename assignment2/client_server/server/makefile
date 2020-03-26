KVServer: server.cpp KVClientHandler.h KVStore.h KVCache.cpp KVCache.hpp
	g++ -o KVServer server.cpp ThreadPool.h KVClientHandler.cpp KVStore.h KVCache.cpp -lpthread -lrt

.Phoney:clean
clean:
	@rm -f *.dat
	@rm -f KVServer
	@rm -f Data.xml
