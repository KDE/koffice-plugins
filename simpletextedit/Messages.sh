#! /bin/sh
$EXTRACTRC *.ui >> rc.cpp || exit 11
$XGETTEXT *.cpp  -o $podir/koffice-simpletextedit.pot
