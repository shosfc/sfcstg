all:
	g++ -o sfcstg_server sfcstg_server.o ServerNetworkManager.o Unit.o Game.o
	g++ -o sfcstg sfcstg_client.o sfcColor.o sfcLibR.o Client.o ClientNetworkManager.o

sfcstg: sfcstg_client.o sfcColor.o sfcLibR.o Client.o ClientNetworkManager.o
	g++ -o sfcstg sfcstg_client.o sfcColor.o sfcLibR.o Client.o ClientNetworkManager.o

sfcstg_server: sfcstg_server.o ServerNetworkManager.o Unit.o Game.o
	g++ -o sfcstg_server sfcstg_server.o ServerNetworkManager.o Unit.o Game.o

sfcstg_client.o: sfcstg_client.cpp
	g++ -c sfcstg_client.cpp

sfcstg_server.o: sfcstg_server.cpp
	g++ -c sfcstg_server.cpp

Client.o: Client.cpp
	g++ -c Client.cpp

Unit.o: Unit.cpp
	g++ -c Unit.cpp

Game.o: Game.cpp
	g++ -c Game.cpp

sfcColor.o: sfcColor.cpp
	g++ -c sfcColor.cpp

sfcLibR.o: sfcLibR.cpp
	g++ -c sfcLibR.cpp

ClientNetworkManager.o: ClientNetworkManager.cpp
	g++ -c ClientNetworkManager.cpp

ServerNetworkManager.o: ServerNetworkManager.cpp
	g++ -c ServerNetworkManager.cpp

clean:
	rm -f sfcstg sfcstg_server sfcstg.o Unit.o Game.o sfcColor.o sfcLibR.o ClientNetworkManager.o ServerNetworkManager.o Client.o sfcstg_client.o sfcstg_server.o