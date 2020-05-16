# VCMI Project - WebAssembly Demo

DEMO: https://youtu.be/mcGTd9qQPJk


This fork is a quick attempt to compile the great VCMI project with emscripten to run in web browser.

Although it runs, the following needs to be reworked before it can be called "a port":

* Maintaining constant fps with SDL_Delay is not the way to go in web browser. I feel the game is capable of running smoothly once this is addressed.
* Networking over WebSockets proxy is glitchy as hell, didn't look into it.
* Currently the client pulls all a pack of all data files around 400mb in size :=), and this will only work well when deployed locally.

Build is not automated, see comments in CMakeLists.txt for instructions.
