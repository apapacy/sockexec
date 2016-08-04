# `sockexec`

`sockexec` is a small server for executing local processes. It accepts
connections on a UNIX socket, launches programs, and sends program output
to the connected client.

It's similar to [sockproc](https://github.com/juce/sockproc) but with a few changes:

* Instead of sending a line to run in a shell, you send an array of program arguments. No shell is launched (unless you explicitly want to).
* Input/output is communicated via [netstring](https://cr.yp.to/proto/netstrings.txt)
* Single-threaded, non-blocking
* Enforces connection limits and timeouts/deadlines.

## Installation

### Quickest and easiest install

Just download a statically-compiled binary from the releases page.

### Quick and easy install

`sockexec` requires the `skarnet` library from [skalibs](http://skarnet.org/software/skalibs/)

On most systems, simply typing `make` and `make install` will compile and
install `sockexec`.

To change any parameters, you can either override `make` variables, eg
`make CFLAGS="-I/path/to/something"`, or setup a `config.mak` file with
your custom variables.

Variables to consider:

* `CC` - defaults to `gcc`
* `LD` - defaults to `gcc`
* `CFLAGS` - defaults to `-Wall -Wextra -Werror -O3`
* `LDFLAGS` - defaults to `-lskarnet`
* `PREFIX` - defaults to `/usr/local`
* `DESTDIR` - empty
* `BINDIR` - defaults to `$(PREFIX)/bin`

## Usage

`sockexec [-m max connections] [-t timeout] /tmp/exec.sock`

This starts up `sockexec`, listening for connections at `/tmp/exec.sock`

By default, `sockexec` accepts a maximum of 100 connections and enforces a
60-second timeout. This can be changed with the `-m` and `-t` flags,
respectively.

## Protocol

### Input

After connecting, `sockexec` expects a minimum of 3 strings:

* One string for the number of program arguments (minimum 1)
* One or more strings of program arguments
* Zero or more strings of standard input
* A 0-byte termination string

All strings need to be encoded as netstrings. Some examples:

* `1:2,5:uname,2:-a,0:,`
    * A string indicating you have two arguments, the arguments themselves (`uname` and `-a`); and the zero-byte termination string

* `1:1,3:cat,5:hello,0:,`
    * A string indicating you have one argument, the argument (`cat`), a string to send to standard input (`hello`), and the zero-byte termination string.

`sockexec` won't launch any programs until it's received the 0-byte termination
string. You're not required to send all the strings at once - you could, for
example, send each of these strings individually.

### Output

Once `sockexec` launches your program, it will start sending pairs of
netstrings to the connected client. Within each pair, the first netstring
indicates the type of data, and the second is the actual data.

There's four kinds of data that `sockexec` will send:

* `stdout` - data from the program's standard output
* `stderr` - data from the program's standard error
* `exitcode` - the program's exit code (only for normal exits)
* `termsig` - the signal code that terminated the program (abnormal exits)

An example:

```
6:stdout,91:Linux jprjr 4.6.5-1 #1 SMP PREEMPT Thu Jul 28 21:58:03 UTC 2016 x86_64 GNU/Linux
,8:exitcode,1:0,
```

This has two pairs of netstrings:

* The first pair is data from `stdout`
* The second pair is the program's exit code

## Copyright and License

This program is licensed under the ISC license (see `LICENSE`).

This program includes modified code from skalibs (`src/child_spawn3.c`),
which is copyright Laurent Bercot. The original text of the skalibs license is
included as `LICENSE.skalibs`.

Static binaries compiled with `musl-libc` are available on the releases page.
`musl-libc` is copyright Rich Felker, et al and released under an MIT license.
The original text of the `musl-libc` license is included in the static
downloads.

