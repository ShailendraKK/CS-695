Running server:
	Open terminal in server folder.
	Run 'make' command
	To run server type ./KVServer in terminal. (by default runs on 8080 port no.)
	You can also provide commandline arguments as follows.
		./KVServer -port=8080 -threadPoolSize=8 -numSetsInCache=6 -sizeOfCache=10

	Output files created in server:
		Multiple .dat file will be created in which data will be stored.
		Data.xml will contain data in XML format.

Running client:
	Open terminal in client folder.
	Run 'make' command
	To run client type ./KVClient in terminal. This will run the client in interactive mode (by default runs on 8080 port no.)
	You can also provide input file and output file as follows.
		./KVClient inputfile outputfile portnumber     (This will run client in batch mode)

Note: characters <>;\n are not allowed in either key or value
	  To close the client and server press ctrl-c
	  You will know when the server has completed processing data when the last descpitor which was assigned to client is closed.

Group members:
1. Shailendra Keshav Kirtikar (193059007)
2. Harshwardhan Dilip Thorwat (193059006)
3. Vikram Barandwal (193050081)