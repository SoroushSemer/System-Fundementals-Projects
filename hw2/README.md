# HW2 - DEBUGGING ```PAR``` PARAGRAPH REFORMATTER #

```par``` is a simple paragraph reformatter written by Adam M. Costello and posted to Usenet in 1993. It is designed to read text from the standard input, parse the text into paragraphs, which are delimited by empty lines, chop each paragraph up into a sequence of words (forgetting about the original line breaks),
choose new line breaks to optimize some criteria that are designed to produce a pleasing result, and the finally output the paragraph with the new line breaks. 

The original program contained multiple bugs which I have fixed.

The goal for this project was to develop an understanding of debugging tools such as ```gdb``` and ```valgrind```, to learn about C memory management, and work with files and the C standard I/O library.

```
USAGE: bin/par [--version] [-w WIDTH | --width WIDTH] [-p PREFIX | --prefix PREFIX] [-s SUFFIX | --suffix SUFFIX] 
                [-h HANG | --hang HANG] [-l LAST | --last | --no-last] [-m MIN | --min | --no-min]

    --version (long form only):
    Print the version number of the program.

    -w WIDTH (short form) or --width WIDTH (long form):
    Set the output paragraph width to WIDTH.

    -p PREFIX (short form) or --prefix PREFIX (long form):
    Set the value of the "prefix" parameter to PREFIX.

    -s SUFFIX (short form) or --suffix SUFFIX (long form):
    Set the value of the "suffix" parameter to SUFFIX.

    -h HANG (short form) or --hang HANG (long form):
    Set the value of the "hang" parameter to HANG.

    -l LAST (short form) or either --last or
    --no-last (long form):
    Set the value of the boolean "last" parameter.
    For the short form, the values allowed for LAST should be either
    0 or 1.

    -m MIN (short form) or either --min or --no-min (long form).
    Set the value of the boolean "min" parameter.
    For the short form, the values allowed for MIN should be either
    0 or 1.
```
