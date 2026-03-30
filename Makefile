helper = src/helpers/tests.cpp src/helpers/search/time.cpp src/helpers/search/values.cpp src/helpers/search/pv.cpp src/helpers/evaluation/score.cpp

uci = src/uci/position.cpp src/uci/loop.cpp
uci_options = src/uci/options/spin.cpp src/uci/options/check.cpp

search = src/engine/search/search.cpp
evaluation = src/engine/evaluation/evaluate.cpp

source=src/main.cpp $(helper) $(uci) $(uci_options) $(search) $(evaluation)
out=src/output/
name=brain_dead_v0.003
cmd=-std=c++26 -pthread -O3 -march=native -Wall

ifeq ($(OS),Windows_NT)
	name := $(name).exe
else
	UNAME_S := $(shell uname -s)
	ifeq ($(UNAME_S),Linux)
		name := $(name)
	endif
	ifeq ($(UNAME_S),Darwin)
		name := $(name)
	endif
endif

all:
	g++ $(cmd) $(source) -o $(out)$(name)