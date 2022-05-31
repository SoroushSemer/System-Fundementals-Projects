# HW4 - ```MUSH``` INTERPRETER FOR A SCRIPTING LANGUAGE #

```mush``` is an interpreter for a simple scripting language that is capable of managing multiple concurrently executing "jobs".

The goal of this project was to gain an advanced understanding of the following:
  * process execution: forking, executing, and reaping.
  * signal handling.
  * the use of "dup" to perform I/O redirection.
  * Unix commands and the command line.
  * C libraries and system calls.

```
mush commands
    | delete <lineno> , <lineno>
    | run
    | cont
    | <lineno> stop
    | <optional_lineno> set <name> = <expr>
    | <optional_lineno> unset <name>
    | <optional_lineno> goto <lineno>
    | <optional_lineno> if <expr> goto <lineno>
    | <optional_lineno> source <file>
    | <optional_lineno> <pipeline>
    | <optional_lineno> <pipeline> &
    | <optional_lineno> wait <expr>
    | <optional_lineno> poll <expr>
    | <optional_lineno> cancel <expr>
    | <optional_lineno> pause
```
