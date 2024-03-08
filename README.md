# socket_programming
## Compiling code
* run `make`
## Usage
* For server program, run `./udp_server` 

* For client program, run `./udp_client` `127.0.0.1` `5555`

    The two command-line parameters are the server's ip and the server's port for the client connecting to.


## Cleaning compiled files
* Run `make clean`

## Preparing the environment for testing in Debian
* Install the docker deskstop in windows 11, refering to https://docs.docker.com/desktop/install/windows-install/
* Open the Windows Powershell as administrator
  1. Run `docker pull debian:10-slim`
  2. Run `docker run -it -v  C:\Users\USER\Desktop\project:/source debian:10-slim` (C:\Users\USER\Desktop\project is the folder where the files and the programs are placing)
  3. Run `cd source` and execute the programs
 
## How the echo server program works
1. Run server program and server listening on to the port `5555` is ready for receiving data.
<img width="474" alt="server_ready" src="https://github.com/chavichen/socket_programming/assets/27536513/e3082954-9cd9-4298-95c5-500f917279b0">

2. Server receives the data from a client and replies the same message to the client.
<img width="392" alt="server_receive_msg" src="https://github.com/chavichen/socket_programming/assets/27536513/320aef9c-8a84-46f9-a6d8-a27beda8d064">

## How the client program works
1. Run client program and the user needs to enter the message.

<img width="502" alt="client_ready" src="https://github.com/chavichen/socket_programming/assets/27536513/7d3a0ed2-e91f-4b46-afc7-490b68d53236">

2-1. The client sends the message and waits for the reply from the server. If receiving the reply from the server, and the client program exited with code 0.

<img width="502" alt="client_reply" src="https://github.com/chavichen/socket_programming/assets/27536513/1693c316-9a82-453b-bba4-e30afa5e615a">

2-2. The client sends the message and waits for the reply from the server. If not receiving the reply from the server, the client will resend the message until reaching the maximum of retries, and the client program exited with code 1.

<img width="474" alt="client_resend" src="https://github.com/chavichen/socket_programming/assets/27536513/3ef06af4-2839-406b-9e04-bea8a5963f18">
