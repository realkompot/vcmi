# VCMI Project - WebAssembly Demo

DEMO: https://youtu.be/mcGTd9qQPJk


This fork is a quick attempt to compile the great VCMI project with emscripten to run in web browser.

Although it runs, the following needs to be reworked:

* Maintaining constant fps with SDL_Delay is not the way to go in web browser. I feel the game is capable of running smoothly once this is addressed.
* Client <-> Server connection is glitchy, doesn't work well.
* Currently the client pulls a pack of all data files (around 400mb in size), not acceptable in web environment.

Notes:
1. wasm binary size is ~12mb.
2. WebSockify proxy (or similar tool) should be used to proxy WebSocket connections to vcmiserver.
3. This is demo, build is not fully automated, see comments in CMakeLists.txt for build/run instructions.
