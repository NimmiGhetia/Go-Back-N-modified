ACN Assignment 1
Name : Nimmi Ghetia
ID : 2018H1120276P

Packet description:
    type : describes the type of packet i.e segment or ack packet
    length : length of data
    isLastPacket : whether it is terminal packet or not
    payload : actual data
    sequence No : to ensure that packets are in order

File read:
    Reading whole file in one go

File write:
    writting one packet at a time in file

Features : 
    - random time delay in sending ack at receiver side using delay function
    - receiver buffer stores whole file then write it to output.txt in one go
    - sender sends ws packets at a time and maintains base (next expected ack)
    - asynchronous calls used for timeout timer using alarm system call

To run:
make makefile sender
make makefile receiver

Start receiver:
Usage:  %s <UDP SERVER PORT> 
./receiver 9999

Start sender:
Usage: %s <Server IP> <Server Port> <File Name> <Packet Size> <Window Size>
./sender 127.0.0.1 9999 input.txt 256 6

Note : in case receive from fails please retry the commands.

Output : 
./receiver 9999
./sender 127.0.0.1 9999 input.txt 128 6

Sender :
received first packet with sequence number : 0
--------  Sending ACK 0  --------
received packet with sequence number : 1
--------  Sending ACK 1  --------
received packet with sequence number : 2
--------  Sending ACK 2  --------
received packet with sequence number : 3
--------  Sending ACK 3  --------
received packet with sequence number : 4
--------  Sending ACK 4  --------
received packet with sequence number : 5
--------  Sending ACK 5  --------
received packet with sequence number : 6
--------  Sending ACK 6  --------
received packet with sequence number : 7
--------  Sending ACK 7  --------
received packet with sequence number : 8
--------  Sending ACK 8  --------
received packet with sequence number : 9
--------  Sending ACK 9  --------
received packet with sequence number : 10
received terminal packet with sequence number : 10

Receiver:
Attempting to Send to: 
IP:          127.0.0.1
Port:        9999
File:        input.txt
Window Size: 6
Packet Size:  128
Packets:      11
sending packet with sequence number : 0
sending packet with sequence number : 1
sending packet with sequence number : 2
sending packet with sequence number : 3
sending packet with sequence number : 4
sending packet with sequence number : 5
window is full
-------- recieved teardown ack: 0 --------
sending packet with sequence number : 6
window is full
-------- recieved teardown ack: 1 --------
sending packet with sequence number : 7
window is full
-------- recieved teardown ack: 2 --------
sending packet with sequence number : 8
window is full
-------- recieved teardown ack: 3 --------
sending packet with sequence number : 9
window is full
-------- recieved teardown ack: 4 --------
sending terminal packet with sequence number : 10
window is full
-------- recieved teardown ack: 5 --------
sending terminal packet with sequence number : 10
window is full
-------- recieved teardown ack: 6 --------
sending terminal packet with sequence number : 10
window is full
-------- recieved teardown ack: 7 --------
sending terminal packet with sequence number : 10
window is full
sequence number :  8
timeout occurred. resending
sending packet with sequence number: 8
sending packet with sequence number: 9
sending terminal packet with sequence number : 10 
-------- recieved teardown ack: 8 --------
sending terminal packet with sequence number : 10
window is full
-------- recieved teardown ack: 9 --------
sending terminal packet with sequence number : 10
window is full
-------- recieved terminal ack : 10  --------


