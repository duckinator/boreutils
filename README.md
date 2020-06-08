# boreutils

An implementation of some of the [utilities](https://pubs.opengroup.org/onlinepubs/9699919799/idx/utilities.html) specified in [POSIX.1-2017](https://pubs.opengroup.org/onlinepubs/9699919799/toc.htm).

## Excluded utilities

Boreutils focuses on non-optional utilities.
Many POSIX utility categories and XSI (X/Open Systems Interfaces) utilities are excluded.

Things explicitly excluded include:

- Things that make more sense as shell builtins, including:
  * Things that are only really usable in a shell: `cd`, `hash`, `read`, etc.
  * Things related to job control: `bg`, `fg`, `jobs`, etc.
  * Things used exclusively for printing: `lp`, etc.
- The following [Margin Codes](https://pubs.opengroup.org/onlinepubs/9699919799/help/codes.html):
  * `CD`: C-Language Development Utilities
  * `FD`: FORTRAN Development Utilities
  * `FR`: FORTRAN Runtime Utilities
  * `OB`: Obsolescent
  * `SD`: Software Development Utilities
  * `UU`: UUCP Utilities
- Things that are very heavily influenced by other software, including:
  * Things that have terminal-specific behavior, like `tput`.

Things not excluded, but prone to be put off forever include:

- Things where the standard doesn't provide enough information:
  * `mesg`: "The mechanism by which the message status of the terminal is changed is unspecified"
- Things where there's no standard for the underlying functionality:
  * `ps`: On Linux, you read /proc. On BSD, you use `kvm_getproc*()`. Elsewhere? Who the heck knows.
- Obscenely complex things, such as those involving creating archives:
  * `pax`


## Utilities, and current status

Below is the list of [POSIX.1-2017 utilities](https://pubs.opengroup.org/onlinepubs/9699919799/idx/utilities.html) that are planned to be implemented.

Utilities that are checked off have been implemented.

- [ ] at
- [ ] awk
- [x] basename
- [ ] batch
- [ ] bc
- [ ] cal (September 1752 and later works)
- [x] cat
- [ ] chgrp
- [ ] chmod
- [ ] chown
- [ ] cksum
- [ ] cmp
- [ ] comm
- [ ] cp
- [ ] crontab
- [ ] csplit
- [ ] cut
- [x] date
- [ ] dd
- [ ] df
- [ ] diff
- [x] dirname
- [ ] du
- [x] echo
- [ ] ed
- [x] env
- [ ] ex
- [ ] expand
- [ ] expr
- [x] false
- [ ] file
- [ ] find
- [ ] fold
- [ ] gencat
- [ ] getconf
- [ ] getopts
- [ ] grep
- [x] head
- [ ] iconv
- [ ] id
- [ ] join
- [x] kill
- [x] link
- [ ] ln
- [ ] locale
- [ ] localedef
- [ ] logger
- [ ] logname
- [ ] ls
- [ ] m4
- [ ] mailx
- [ ] make
- [x] man (kind of)
- [ ] mkdir
- [ ] mkfifo
- [ ] more
- [ ] mv
- [ ] newgrp
- [ ] nice
- [ ] nl
- [ ] nohup
- [ ] od
- [ ] paste
- [ ] patch
- [ ] pathchk
- [ ] pax
- [ ] pr
- [ ] printf
- [ ] ps
- [x] pwd
- [ ] renice
- [x] rm
- [x] rmdir
- [ ] sed
- [ ] sh
- [x] sleep
- [ ] sort
- [ ] split
- [ ] strings
- [ ] stty
- [ ] tabs
- [x] tail
- [ ] talk
- [ ] tee
- [ ] test
- [ ] time
- [ ] touch
- [ ] tr
- [x] true
- [ ] tsort
- [x] tty
- [ ] umask
- [ ] uname
- [ ] unexpand
- [ ] uniq
- [x] unlink
- [ ] uudecode
- [ ] uuencode
- [ ] vi
- [ ] wait
- [ ] wc
- [ ] xargs

Here are non-POSIX.1-2017 utilities which are provided:

- [x] ish (an extremely basic shell; not POSIX-compliant)
- [x] whoami
- [x] yes
