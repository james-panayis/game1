## Introduction

Simple WebAssembly/WebGL platformer game  
  
Use *a* and *d* to move left and right respectively  
Use *w* to jump  
Double click upon death to return to start  
Double click upon completion to try again for a higher score  
  
For demo please see: https://jamespanayis.com/game1

## Build instructions

The C++ is compiled to WebAssembly and requires a working **emscripten** install  
  
run:

	make


## Install instructions

Run a local webserver and surf to it eg:

	python -m SimpleHTTPServer 8080

Browse to http://localhost:8080/build/game1.html

OR:

Copy files to webserver root:

	PREFIX=/path/to/web/root make install
	
Browse to your webserver.

