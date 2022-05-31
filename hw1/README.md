# HW 1 - ```ARGO``` JSON PARSER #

```argo``` is a command-line utility which can validate JSON input and transform JSON input into JSON output in a "canonical" form.

The goal of this project was to familiarize myself with File I/O, Bitwise Manipulations, and Use of Pointers in C.

```USAGE: bin/argo [-h] [-c|-v] [-p|-p INDENT]
   -h       Help: displays this help menu.
   -v       Validate: the program reads from standard input and checks whether
            it is syntactically correct JSON.  If there is any error, then a message
            describing the error is printed to standard error before termination.
            No other output is produced.
   -c       Canonicalize: once the input has been read and validated, it is
            re-emitted to standard output in 'canonical form'.  Unless -p has been
            specified, the canonicalized output contains no whitespace (except within
            strings that contain whitespace characters).
   -p       Pretty-print:  This option is only permissible if -c has also been specified.
            In that case, newlines and spaces are used to format the canonical output
            in a more human-friendly way.  For the precise requirements on where this
            whitespace must appear, see the assignment handout.
            The INDENT is an optional nonnegative integer argument that specifies the
            number of additional spaces to be output at the beginning of a line for each
            for each increase in indentation level.  If no value is specified, then a
            default value of 4 is used.```
