#!/bin/sh

BASEDIR=`dirname "$0"`
$BASEDIR/tlm_autocoder/autocode.sh
$BASEDIR/struct_autocoder/autocode.sh
