# Web Server

CMPT 371, Data Communication I, SFU

## What the Server Does

  1. Respond requests from browsers
  2. Parse the GET request, then respond with correct HTTP code
  3. If not "404 Not Found", the server will send requested html file through specific port

## Features
  The server is equipped with a queue, capacity 6 request maximum, to serve multiple clients as first-come-first-serve scheme
