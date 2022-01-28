CC = g++ -std=c++11

all : ProcessTrack RockSpecAnalyzer

test : testPulse testPulseArray

ProcessTrack : ProcessTrack.cpp ./src/TrackReader.cpp ./src/MCPulse.cpp ./src/MCPulseArray.cpp 
	$(CC) -I./include $^ -o $@ `root-config --cflags --glibs`

RockSpecAnalyzer : RockSpecAnalyzer.cpp
	$(CC) $^ -o $@ `root-config --cflags --glibs`

testPulse : ./test/testPulse.cpp ./src/MCPulse.cpp
	$(CC) -I./include $^ -o $@

testPulseArray : ./test/testPulseArray.cpp ./src/MCPulse.cpp ./src/MCPulseArray.cpp
	$(CC) -I./include $^ -o $@

clean :
	rm ProcessTrack testPulse testPulseArray

