all: testserver testclient

testserver: testserver.cpp
	g++ -g -o $@ $<

testclient: testclient.cpp
	g++ -g -o $@ $<

clean:
	rm -rf testserver testclient
