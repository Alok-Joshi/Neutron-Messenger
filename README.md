# Neutron-Messenger
-GTK required
-Use the following terminal command (Linux) to compile the server and the client:
 
 CLIENT:
 cc -Wno-format -o Client  Client.c -Wno-deprecated-declarations -Wno-format-security -lm `pkg-config --cflags --libs gtk+-3.0` -export-dynamic
 
 SERVER:
 cc -Wno-format -o Server  Server.c -Wno-deprecated-declarations -Wno-format-security -lm `pkg-config --cflags --libs gtk+-3.0` -export-dynamic
