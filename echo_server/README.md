# echo server

Barebones little echo server that sends back received messages. Works with TCP and UDP.

## Usage

`server (-t | -u)`

`-t`  
 Use TCP.

`-u`  
 Use UDP.

## What I learned

- Practice with sockets
- Practice with pthreads
- Good review of the differences between TCP and UDP (connection state)
- Got to experience and resolve IPv4 VS IPv6 weirdness
  - Since TCP uses a connection, a failed IPv6 connection can simply retry with IPv4 (which netcat does). However, with UDP, there is no connection, so no retry and the echo simply fails.
