PHP_ARG_ENABLE(agsat, whether to enable the agsat lib support,
[ --enable-agsat   Enable agsat support])

if test "$PHP_AGSAT" = "yes"; then
echo $abs_builddir
  AC_DEFINE(HAVE_AGSAT, 1, [Whether you have agsat])
  PHP_NEW_EXTENSION(agsat, src/agsat.c /src/sgp/sgp.c /src/sgp4/sgp4.c /src/sgp8/sgp8.c /src/sdp4/sdp4.c /src/sdp8/sdp8.c /src/common/get_el.c /src/common/common.c /src/common/basic.c /src/common/deep.c, $ext_shared)
  PHP_ADD_BUILD_DIR([$abs_builddir/src], 1)
fi
