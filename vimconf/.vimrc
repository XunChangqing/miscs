"""""""""""""""""""""""""""""""""""VUNDLE setting"""""""""""""""""""""""""""""""""""""""""""
set nocompatible " be iMproved
filetype off "
set rtp+=~/.vim/bundle/Vundle.vim/
call vundle#begin()

" let Vundle manage Vundle
Bundle 'gmarik/Vundle'

" vim-scripts repos
"Bundle 'minibufexpl.vim'
"Bundle 'bufexplorer.zip'
"Bundle 'taglist.vim'
Bundle 'c.vim'
"Bundle 'winmanager'
Bundle 'The-NERD-Commenter'
"Bundle 'snipMate'
"Bundle 'TxtBrowser'
"Bundle 'genutils'
"Bundle 'lookupfile'
"Bundle 'LeaderF'
Bundle 'The-NERD-tree'
"Bundle 'ctrlp.vim'
"Bundle 'cscope.vim'
"Bundle 'Tagbar'

"github repos.
Bundle 'fholgado/minibufexpl.vim'
Bundle 'powerline/powerline'
Bundle 'klen/python-mode'
"Bundle 'bling/vim-airline'
"Bundle 'tpope/vim-fugitive'
"Bundle 'Shougo/neocomplcache.vim'
"Bundle 'ervandew/supertab'
"Bundle 'simplyzhao/cscope_maps.vim'
Bundle 'vim-scripts/EasyGrep'
Bundle 'vim-scripts/a.vim'
Bundle 'vim-scripts/google.vim'
Bundle 'kien/ctrlp.vim'
Bundle 'majutsushi/tagbar'
"Bundle 'brookhong/cscope.vim'
Bundle 'scrooloose/nerdtree'
"Plugin 'Valloric/YouCompleteMe'
"file repos or git repos
"Bundle 'git://git.wincent.com/command-t.git'
"
" Add maktaba and codefmt to the runtimepath.
" " (The latter must be installed before it can be used.)
Plugin 'google/vim-maktaba'
Plugin 'google/vim-codefmtlib'
Plugin 'google/vim-codefmt'
" " Also add Glaive, which is used to configure codefmt's maktaba flags. See
" " `:help :Glaive` for usage.
Plugin 'google/vim-glaive'
"call glaive#Install()
" " Optional: Enable codefmt's default mappings on the <Leader>= prefix.
"Glaive codefmt plugin[mappings]

call vundle#end()
filetype plugin indent on " required!

"
" Brief help
" :BundleList - list configured bundles
" :BundleInstall(!) - install(update) bundles
" :BundleSearch(!) foo - search(or refresh cache first) for foo
" :BundleClean(!) - confirm(or auto-approve) removal of unused bundles
"
" see :h vundle for more details or wiki for FAQ
" NOTE: comments after Bundle command are not allowed..
""""""""""""""""""""""""END VUNDLE""""""""""""""""""""""""""



set mouse=a
syntax on
set hlsearch
set incsearch
set autoindent
set autoread
set nu
set ic
