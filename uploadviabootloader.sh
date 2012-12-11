#!/usr/bin/expect -f

set force_conservative 0  ;# set to 1 to force conservative mode even if

if {$force_conservative} {
	set send_slow {1 .1}
	proc send {ignore arg} {
		sleep .1
		exp_send -s -- $arg
	}
}


set timeout -1
spawn cu -l /dev/ttyUSB0 -s 115200
match_max 100000
expect "Connected"
send -- "~"
expect "chocis-PC"
send -- "+"
expect  "+"
send -- "sx -X kernel.img\r"
expect "UART"
expect eof
