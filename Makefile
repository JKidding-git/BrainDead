helper = src/helpers/tests.cpp src/helpers/search/time.cpp src/helpers/search/values.cpp src/helpers/search/pv.cpp src/helpers/search/heuristics.cpp src/helpers/evaluation/score.cpp src/helpers/search/see.cpp src/helpers/evaluation/passer.cpp src/helpers/evaluation/isolated.cpp src/helpers/evaluation/backward.cpp src/helpers/evaluation/king_shield.cpp

uci = src/uci/position.cpp src/uci/loop.cpp
uci_options = src/uci/options/spin.cpp src/uci/options/check.cpp

search = src/engine/search/search.cpp
evaluation = src/engine/evaluation/evaluate.cpp

source = src/main.cpp $(helper) $(uci) $(uci_options) $(search) $(evaluation)

out = src/output/
name = brain_dead_v0.013
cmd = -std=c++26 -pthread -O3 -march=native -Wall
headers = $(shell find src -name '*.hpp' -type f | sort)
pch = $(headers:.hpp=.hpp.gch)


pre: $(pch)

%.hpp.gch: %.hpp
	g++ $(cmd) -x c++-header $< -o $@

all: pre
	g++ $(cmd) -Winvalid-pch $(source) -o $(out)$(name)

clean:
	rm -f $(pch) $(out)$(name)