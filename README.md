# myRPC
a tiny rpc demo
### compile and run
#### compile server
gcc -o rpc_server rpc_server.c serialize.c
#### compile client
gcc -o rpc_client rpc_client.c serialize.c
#### run server
./rpc_server
#### run client
./rpc_client
