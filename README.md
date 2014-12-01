ScratchMonkey
=============

Arduino software programmer sketch, supporting ISP, HVSP, and HVPP

For details, please consult the [User Manual](http://microtherion.github.com/ScratchMonkey/)

This branch adds experimental support for TPI programming (For ATtiny4/5/9/10). However, to actually use it, you will have to patch avrdude as well (since current STK600 support assumes the programmer runs over direct USB, as real STK600s do). I'm not quite ready to approach the avrdude developers with my patches until I've head the opportunity to flesh them out a bit more.
