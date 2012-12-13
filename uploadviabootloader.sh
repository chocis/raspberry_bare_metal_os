#!/usr/bin/expect -f



#spawns serial connection
spawn cu -l /dev/ttyUSB0 -s 115200
#wait when connected
expect "Connected"

#send typical ~ cu command, then wait for answer (mine is [chocis-PC]), thats why using only "]"
send -- "~"
expect "]"

#then send + cu command, which allows to insert normal shell commands (wait for + answer or else it wont work)
send -- "+"
expect  "+"

#send image via modemx protocol
send -- "sx -X kernel.img\r"

#give control back to user, so I can send commands like in normal cu usage
interact

#because (I think so) expect is running in different process, cu cant receive ~. command to exit connection,
#thats why you must press CTRL+C and only then ~. will work.
