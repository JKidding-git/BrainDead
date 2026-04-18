
# BrainDead

Current ELO: ~1800

This [chess engine](https://en.wikipedia.org/wiki/Chess_engine) uses [**chess-library**](https://github.com/Disservin/chess-library) internally.

BrainDead is a [UCI](https://en.wikipedia.org/wiki/Universal_Chess_Interface)-compliant [chess engine](https://en.wikipedia.org/wiki/Chess_engine) created by a c++ beginner, so expect some questionable design choices.

BrainDead does **NOT** include a [GUI](https://en.wikipedia.org/wiki/Graphical_user_interface), if you want to use a [GUI](https://en.wikipedia.org/wiki/Graphical_user_interface). I recommend [**Cute Chess**](https://cutechess.com/) as it's easy to use.

## Building the [chess engine](https://en.wikipedia.org/wiki/Chess_engine)

To build the [chess engine](https://en.wikipedia.org/wiki/Chess_engine) itself, you **MUST** have g++. To install g++, please look elsewhere.

**NOTE**: You also need to support Makefile.

After installing g++, run ```make``` and the [chess engine](https://en.wikipedia.org/wiki/Chess_engine)'s output should be in the output folder in ```src/``` sub-directory.

## Configuration

BrainDead has ~~a lot~~ a small number of configuration options, most [GUI](https://en.wikipedia.org/wiki/Graphical_user_interface) should support some ways to set each option. If they do not, then refer to the [UCI](https://en.wikipedia.org/wiki/Universal_Chess_Interface) specification.

### move_overhead

The time buffer when playing games under time constraints. If you notice any time losses you should increase the move overhead. It caps out at 5000 milliseconds, which is 5 seconds.

### hash_mb

The amount of memory allocated to the transposition table, in megabytes. The more memory you allocate, the better the engine will perform, but it will also use more RAM.

## Special Thanks

- [**Chess Programming Wiki**](https://www.chessprogramming.org/Main_Page) - This is literally where most if not ALL of my knowledge of engine development comes from.

- [**chess-library**](https://github.com/Disservin/chess-library) - A fast c++ chess library.

- [**python-chess-engine**](https://github.com/Disservin/python-chess-engine) - Some of the search code is inspired from this engine.

## License

BrainDead is released under the **GPLv3** license, please read LICENSE for more info.
