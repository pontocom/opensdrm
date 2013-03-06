dnl $Id: config.m4,v 1.1 2004/12/28 10:55:30 linha2 Exp $
dnl config.m4 for extension ahcrypto

dnl Comments in this file start with the string 'dnl'.
dnl Remove where necessary. This file will not work
dnl without editing.

dnl If your extension references something external, use with:

PHP_ARG_WITH(ahcrypto, for ahcrypto support,
dnl Make sure that the comment is aligned:
[  --with-ahcrypto             Include ahcrypto support])

dnl Otherwise use enable:

dnl PHP_ARG_ENABLE(ahcrypto, whether to enable ahcrypto support,
dnl Make sure that the comment is aligned:
dnl [  --enable-ahcrypto           Enable ahcrypto support])

if test "$PHP_AHCRYPTO" != "no"; then
  dnl Write more examples of tests here...

  dnl # --with-ahcrypto -> check with-path
  dnl SEARCH_PATH="/usr/local /usr"     # you might want to change this
  dnl SEARCH_FOR="/include/ahcrypto.h"  # you most likely want to change this
  dnl if test -r $PHP_AHCRYPTO/; then # path given as parameter
  dnl   AHCRYPTO_DIR=$PHP_AHCRYPTO
  dnl else # search default path list
  dnl   AC_MSG_CHECKING([for ahcrypto files in default path])
  dnl   for i in $SEARCH_PATH ; do
  dnl     if test -r $i/$SEARCH_FOR; then
  dnl       AHCRYPTO_DIR=$i
  dnl       AC_MSG_RESULT(found in $i)
  dnl     fi
  dnl   done
  dnl fi
  dnl
  dnl if test -z "$AHCRYPTO_DIR"; then
  dnl   AC_MSG_RESULT([not found])
  dnl   AC_MSG_ERROR([Please reinstall the ahcrypto distribution])
  dnl fi

  dnl # --with-ahcrypto -> add include path
  dnl PHP_ADD_INCLUDE($AHCRYPTO_DIR/include)

  dnl # --with-ahcrypto -> check for lib and symbol presence
  dnl LIBNAME=ahcrypto # you may want to change this
  dnl LIBSYMBOL=ahcrypto # you most likely want to change this 

  dnl PHP_CHECK_LIBRARY($LIBNAME,$LIBSYMBOL,
  dnl [
  dnl   PHP_ADD_LIBRARY_WITH_PATH($LIBNAME, $AHCRYPTO_DIR/lib, AHCRYPTO_SHARED_LIBADD)
  dnl   AC_DEFINE(HAVE_AHCRYPTOLIB,1,[ ])
  dnl ],[
  dnl   AC_MSG_ERROR([wrong ahcrypto lib version or lib not found])
  dnl ],[
  dnl   -L$AHCRYPTO_DIR/lib -lm -ldl
  dnl ])
  dnl
  dnl PHP_SUBST(AHCRYPTO_SHARED_LIBADD)

  PHP_REQUIRE_CXX()
  PHP_ADD_LIBRARY(stdc++, 1, AHCRYPTO_SHARED_LIBADD)

  PHP_ADD_INCLUDE($AHCRYPTO_DIR/crypto++)
  PHP_ADD_LIBRARY_WITH_PATH(cryptopp, /home/jhht/Projects/php/php-4.3.9/ext/ahcrypto/crypto++, AHCRYPTO_SHARED_LIBADD)

  PHP_NEW_EXTENSION(ahcrypto, ahcrypto.c, $ext_shared)
fi
