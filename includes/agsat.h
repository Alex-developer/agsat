#ifndef PHP_AGSAT_H
#define PHP_AGSAT_H 1

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include "php.h"
#include "php_ini.h"

#include "../src/includes/norad.h"

#define PHP_AGSAT_VERSION "1.0"
#define PHP_AGSAT_EXTNAME "agsat"

typedef struct {int hh; int mm; float ss;} hours;
typedef struct {int year; int month; int day;} date;
typedef struct {int deg; int mm; float ss;} degrees;

PHP_FUNCTION(agsat_track);
PHP_FUNCTION(agsat_validatetle);

void do_track(tle_t tle, int debug);
void JulianDay2Calendar(double JD, date *calendar, hours *UTC);

extern zend_module_entry agsat_module_entry;
#define phpext_agsat_ptr &agsat_module_entry

#endif
