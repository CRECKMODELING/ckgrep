" ckgrep.vim - search CHEMKIN reaction mechanisms by chemistry from Vim
" Maintainer: Timoteo Dinelli <timoteo.dinelli@polimi.it>
" License:    GPL-3.0-or-later (same as ckgrep)

if exists('g:loaded_ckgrep')
  finish
endif
let g:loaded_ckgrep = 1

" :Ckgrep  "OH+CH4=" [files/dirs...]
" ckgrep flags (-e, -c, -p) pass straight through. With no files the current
" buffer's file is searched; :Ckgrep! searches the current directory
" recursively instead, exactly like the command line tool.
command! -bang -nargs=+ -complete=file Ckgrep call ckgrep#search(<bang>0, <q-args>)
