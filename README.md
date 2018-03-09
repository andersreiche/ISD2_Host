# ISD2_Host
Part of a distributed system: Daemon process that takes a TCP connection on localhost:1955 and uses commands from a client to set a temperature

This is a deamon process, it logs to the syslog. To see the log execute: "tail -f /var/log/syslog | grep Daemon"

for debug purposes you can send commands directly to the process with "telnet localhost 1955"

commands: set <tempetature between 20 and 50>
