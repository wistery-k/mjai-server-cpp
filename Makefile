.DEFAULT_GOAL: a.out

.PHONY: clean

a.out: bakkjson.cpp main.cpp
	g++ -std=c++0x bakkjson.cpp main.cpp

clean:
	$(RM) a.out