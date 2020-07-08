#! /bin/bash

IFS=$'\n';
for opt in $(grep "define" socket_option.h | grep "SO_"); do
  _type=$(echo ${opt} | awk '{ print $5 }');
  _def=$(echo ${opt} | awk '{ print $3 }');
  echo "using $(echo ${_def} | sed -e 's#SO_##g' | tr [A-Z] [a-z]) = cyan::net::detail::${_type}_socket_option<CYAN_OS_DEF(SOL_SOCKET), CYAN_OS_DEF(${_def})>;";
done;
