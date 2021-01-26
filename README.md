# Process Information

Tool to get details information about a specific PID.

The same information as the process listing command is returned as well as:
- Environment details
- Processes Memory maps
- Current working directory
- Exe path
- File Descriptors
- Mapped Files

Note that not all of this information will be available as low privilege user.

## Usage

You have a few options when using the tool
```
./proc_info -h
	 -p: Mandatory. Specify the PID you want more information on
	 -b: Returns basic information about the process
	 -e: Returns the environment variables set for the process
	 -f: Returns the file descriptors for the process
	 -m: Returns the memory maps for the process
	 -a: Runs all of the options above!
```

You have to provide a valid PID and at least one option. If you want everything just use -a. Note that -e, -f, -m, and some of the information from -b is dependant on whether you have the relevant permissions to read those proc files.

## Examples

Basic mode
'''
./proc_info -p 59428 -b

-------------------- Process Basic Details --------------------
Process ID: 59428
Process Parent ID: 59420
Process Group ID: 59428
Process Name: nc
Process Command Line: nc -lvnp 4444 
Process Exe Path: /usr/bin/nc.openbsd
Process State: S
Process Start Time: 1611665628
Process Threads: 1
Process Memory Size: 3338240 bytes
Process Stack Start Pos: 0x7ffe65a9ccf0
Process Env Start Pos: 0x7ffe65a9e42f
Process Env End Pos: 0x7ffe65a9efec
'''

Environment mode
'''
./proc_info -p 59428 -e
SHELL=/bin/bash
SESSION_MANAGER=local/ubuntu:@/tmp/.ICE-unix/1951,unix/ubuntu:/tmp/.ICE-unix/1951
QT_ACCESSIBILITY=1
COLORTERM=truecolor
XDG_CONFIG_DIRS=/etc/xdg/xdg-ubuntu:/etc/xdg
XDG_MENU_PREFIX=gnome-
GNOME_DESKTOP_SESSION_ID=this-is-deprecated
GNOME_SHELL_SESSION_MODE=ubuntu
...
'''

File Descriptor Mode
'''
./proc_info -p 59428 -f
-------------------- Process File Descriptors --------------------
FD 0 --> /etc/passwd
FD 1 --> /dev/pts/5
FD 2 --> /dev/pts/5
FD 3 --> socket:[431308]
'''

Memory maps mode
'''
./proc_info -p 59428 -m
-------------------- Process Maps --------------------
55e88e9d6000-55e88e9d8000 r--p 00000000 08:05 4588318                    /usr/bin/nc.openbsd
55e88e9d8000-55e88e9dd000 r-xp 00002000 08:05 4588318                    /usr/bin/nc.openbsd
55e88e9dd000-55e88e9df000 r--p 00007000 08:05 4588318                    /usr/bin/nc.openbsd
55e88e9e0000-55e88e9e1000 r--p 00009000 08:05 4588318                    /usr/bin/nc.openbsd
55e88e9e1000-55e88e9e2000 rw-p 0000a000 08:05 4588318                    /usr/bin/nc.openbsd
55e88e9e2000-55e88ea62000 rw-p 00000000 00:00 0 
55e88fdc2000-55e88fde3000 rw-p 00000000 00:00 0                          [heap]
7f21f0176000-7f21f0179000 rw-p 00000000 00:00 0 
...
'''

