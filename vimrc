set nocompatible        " Use Vim defaults (much better!)
set bs=2                " Allow backspacing over everything in insert mode
set history=50          " keep 50 lines of command history
set ruler               " Show the cursor position all the time

" Don't use Ex mode, use Q for formatting
map Q gq

" When doing tab completion, give the following files lower priority.
set suffixes+=.info,.aux,.log,.dvi,.bbl,.out,.o,.lo

set nomodeline
syntax on
autocmd BufRead APKBUILD set filetype=sh
set shiftwidth=4
set tabstop=4
set expandtab
