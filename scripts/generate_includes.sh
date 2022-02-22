#! /bin/bash

realpath() {
  [[ ${1} = /* ]] && echo "${1}" || echo "${PWD}/${1#./}";
}

my_dir=$(dirname $(realpath ${0}));
if [ -f "${my_dir}/../config.h.in" ]; then
  srcdir="${my_dir}/..";
else
  echo "unable to determine srcdir";
  exit 1;
fi;

platform_cmake=${srcdir}/cmake/Platform.cmake

if [ ! -f "${platform_cmake}" ]; then
  echo "cmake/Platform.cmake not found";
  exit 1;
fi;

includes=$(grep -E '^(cyan_check_include_files)' ${platform_cmake} | \
    sed -e 's#cyan_check_include_files(##g' -e 's#)##g' -e 's#"##g');
IFS=$'\n';
for include in ${includes}; do
  header=$(echo ${include} | awk '{ print $1; }');
  macro=$(echo ${include} | awk '{ print $2; }');
  echo "#ifdef ${macro}";
  echo "#include <${header}>";
  echo "#endif // ${macro}";
  echo "";
done;

for include in ${includes}; do
  macro=$(echo ${include} | awk '{ print $2; }');
  echo "#cmakedefine ${macro} 1";
done;


structs=$(grep -E '^(check_struct_size)' ${platform_cmake} | \
    sed -e 's#check_struct_size(##g' -e 's#)##g' -e 's#"##g');
IFS=$'\n';
for struct in ${structs}; do
  st=$(echo ${struct} | awk '{ print $1; }');
  macro=$(echo ${struct} | awk '{ print $2; }');
  echo "#ifdef HAVE_STRUCT_${macro}";
  echo "using ${st}_type = struct ::${st};";
  echo "#endif // HAVE_STRUCT_${macro}";
  echo "";
done;

for struct in ${structs}; do
  macro=$(echo ${struct} | awk '{ print $2; }');
  echo "#cmakedefine HAVE_STRUCT_${macro} 1";
  echo "#cmakedefine SIZEOF_STRUCT_${macro} @SIZEOF_STRUCT_${macro}@";
  echo "";
done;




