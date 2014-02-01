#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include "php.h"
#include "includes/agsat.h"
#include "includes/norad.h"

static function_entry agsat_functions[] = {
    PHP_FE(agsat_track, NULL)
    PHP_FE(agsat_validatetle, NULL)
    {NULL, NULL, NULL}
};

zend_module_entry agsat_module_entry = {
#if ZEND_MODULE_API_NO >= 20010901
    STANDARD_MODULE_HEADER,
#endif
    PHP_AGSAT_EXTNAME,
    agsat_functions,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
#if ZEND_MODULE_API_NO >= 20010901
    PHP_AGSAT_VERSION,
#endif
    STANDARD_MODULE_PROPERTIES
};

PHP_INI_BEGIN()
PHP_INI_ENTRY("agsat.defaultmodel", "sgp", PHP_INI_ALL, NULL)
PHP_INI_END()

PHP_MINIT_FUNCTION(agsat)
{
    REGISTER_INI_ENTRIES();

    return SUCCESS;
}

PHP_MSHUTDOWN_FUNCTION(agsat)
{
    UNREGISTER_INI_ENTRIES();

    return SUCCESS;
}

#ifdef COMPILE_DL_AGSAT
ZEND_GET_MODULE(agsat)
#endif

 
PHP_FUNCTION(agsat_track)
{
  int debug = 1;
  
  
  char *line1;
  char *line2;
  char *model;
  int line1_len;
  int line2_len;

  zval *options, **data;;
  HashTable *options_hash, *arr_hash_tmp;
  int array_count;
  HashPosition pointer;

  zval **tleLine1, **tleLine2, **tlemodel;
  
  tle_t tle;

   
 zval temp;
        char *key;
        int key_len;
        long index;
	
  /**
   * Check we have a single array an a paramater
   */
  if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "a", &options) == FAILURE) {
    RETURN_NULL();
  }

  if (debug == 1) {
    php_printf("%s\n","Options array found");
  }

  /**
  * Get the array and size
  */
  options_hash = Z_ARRVAL_P(options);
  array_count = zend_hash_num_elements(options_hash);

  /**
   * Loop over the array looking for valid satellites to track. if found run the tarcking
   */
  for(zend_hash_internal_pointer_reset_ex(options_hash, &pointer); zend_hash_get_current_data_ex(options_hash, (void**) &data, &pointer) == SUCCESS; zend_hash_move_forward_ex(options_hash, &pointer)) {
    if (Z_TYPE_PP(data) == IS_ARRAY) {
      if (zend_hash_get_current_key_ex(options_hash, &key, &key_len, &index, 0, &pointer) == HASH_KEY_IS_STRING) {
	if (debug == 1) {
	  php_printf("Found Options for %s\n",key);
	}
      } else {
	if (debug == 1) {
	  php_printf("Found Options for %ld\n",index);
	}
      }

      if (Z_TYPE_PP(data) == IS_ARRAY) {
	arr_hash_tmp = Z_ARRVAL_PP(data);

	if (zend_symtable_find(arr_hash_tmp,"line1",sizeof("line1"),(void**)&tleLine1) == SUCCESS) {
	  if (Z_TYPE_PP(tleLine1) == IS_STRING) {
	    line1 = Z_STRVAL_PP(tleLine1);
	    if (debug == 1) {
	      php_printf("Line1 %s\n",line1);
	    }	    

	    if (zend_symtable_find(arr_hash_tmp,"line2",sizeof("line2"),(void**)&tleLine2) == SUCCESS) {
	      if (Z_TYPE_PP(tleLine1) == IS_STRING) {
		  //PHPWRITE(Z_STRVAL_PP(tleLine2), Z_STRLEN_PP(tleLine2));
		line2 = Z_STRVAL_PP(tleLine2);
		if (debug == 1) {
		  php_printf("Line2 %s\n",line2);
		}
		
		if (zend_symtable_find(arr_hash_tmp,"model",sizeof("model"),(void**)&tlemodel) == SUCCESS) {
		  if (Z_TYPE_PP(tlemodel) == IS_STRING) {
		    model = Z_STRVAL_PP(tlemodel);
		    if (debug == 1) {
		      php_printf("Model %s\n",model);
		    }
		    
		  }
		} else {
		  model = "sgp";		  
		  if (debug == 1) {
		    php_printf("Defaulted Model %s\n",model);
		  };
		}
		
		if( parse_elements( line1, line2, &tle) >= 0) {
		  do_track(tle, debug);
		}
		
		
	      }
	    }   
	  }
	}	   
      }
      
    }
  }  
  

    

  
    RETURN_STRING("Hello World", 1);
}

PHP_FUNCTION(agsat_validatetle)
{
    RETURN_STRING("Hello World", 1);
}

/**
 * Track an object
 * 
 * tle The tle structure
 * times array of times
 * emph The ephemeris model to use
 * 
 */
//void do_track(tle_t tle, double times[], int ephem) {
void do_track(tle_t tle, int debug) {
  
  int ephem = TLE_EPHEMERIS_TYPE_SGP4; /* Default empherisis to SGP4 */
  char *ephem_names[5] = { "SGP ", "SGP4", "SGP8", "SDP4", "SDP8" };  
  double sat_params[N_SAT_PARAMS];
  int is_deep;
  double time, currentTime, RightAscension, declination, radius, sign;;
  hours RA, UTC;
  degrees dec;
  date calendar;
   
  double vel[3], pos[3];
double times[400];
   int n_times = 2;
   int i;
   
   
    /**
     * Adjust the Empherisis model
     */
    is_deep = select_ephemeris(&tle);  
    if( is_deep && (ephem == 1 || ephem == 2)) {
      ephem += 2;    /* switch to an SDx */
    }
    if( !is_deep && (ephem == 3 || ephem == 4)) {
      ephem -= 2;    /* switch to an SGx */
    }
    
    //ephem = TLE_EPHEMERIS_TYPE_SDP8;
    
    if (debug == 1) {
      if( is_deep) {       
	php_printf("Deep Space Emph: %s Selected\n",ephem_names[ephem]);
      } else {
	php_printf("Near Earth Emph: %s Selected\n",ephem_names[ephem]);
      }
    }   
   
    if (debug == 1) {
         php_printf("#  Julian Day      Date    Time (UTC) ---------- Position (GEI/J2000) ------------     RA           Dec      \n");
         php_printf("#               MM/DD/YYYY  HH:MM:SS     x(km)      y(km)      z(km)    Radius(km)(hh:mm:ss.ss)(deg:mm:ss.s)\n");
    }

    /**
     * Initialise the Empherisis model
     */
    switch( ephem) {
      case TLE_EPHEMERIS_TYPE_SGP:
	  SGP_init( sat_params, &tle);
	  break;
      case TLE_EPHEMERIS_TYPE_SGP4:
	  SGP4_init( sat_params, &tle);
	  break;
      case TLE_EPHEMERIS_TYPE_SGP8:
	  SGP8_init( sat_params, &tle);
	  break;
      case TLE_EPHEMERIS_TYPE_SDP4:
	  SDP4_init( sat_params, &tle);
	  break;
      case TLE_EPHEMERIS_TYPE_SDP8:
	  SDP8_init( sat_params, &tle);
	  break;
    }
   
   
   
   /* iterate over time steps */
   double startTime=0;
   double endTime=10;
   double stepTime=1;
   
   for(time=startTime; time<=endTime; time+=stepTime)
   {
      currentTime=(tle.epoch)+(time/24.0/60.0); /* compute JD of current time */
      switch( ephem)
      {
         case 0:
            SGP(time, &tle, sat_params, pos, vel);
            break;
         case 1:
            SGP4(time, &tle, sat_params, pos, vel);
            break;
         case 2:
            SGP8(time, &tle, sat_params, pos, vel);
            break;
         case 3:
            SDP4(time, &tle, sat_params, pos, vel);
            break;
         case 4:
            SDP8(time, &tle, sat_params, pos, vel);
            break;
      }

      /* compute additional coordinate points */
      radius=sqrt(pos[0]*pos[0]+pos[1]*pos[1]+pos[2]*pos[2]); /* km */
      declination=(180.0/M_PI)*asin(pos[2]/radius);
      RightAscension=(180.0/M_PI)*atan2(pos[1],pos[0]);
      if(RightAscension<0.0) RightAscension+=360.0;
      /* compute date & UTC */
      JulianDay2Calendar(currentTime, &calendar, &UTC);

            php_printf(" %13.5f  %02d/%02d/%04d  %02d:%02d:%02.0f  "
                   "%10.2f %10.2f %10.2f %10.2f  %11.5f  %11.5f\n",
                   currentTime, calendar.month, calendar.day, calendar.year, UTC.hh, UTC.mm, UTC.ss,
                   pos[0],pos[1],pos[2], radius, RightAscension, declination );

   } /* End of time loop  */   
   
  
}

void JulianDay2Calendar(double JD, date *calendar, hours *UTC) {
   /* Convert double precision Julian Day to calendar date & UTC */
   double dayfrac, JulianDay;
   int l, n, yr, mn;
   
   dayfrac = modf(JD+0.5,&JulianDay); /* JD starts at noon */
   
   UTC->hh = (int)(24.0*dayfrac);
   UTC->mm = (int)(60.0*(24.0*dayfrac-UTC->hh));
   UTC->ss = (int)((60.0*(24.0*dayfrac-UTC->hh)-UTC->mm)*60.0);
   /* compute month day year */
   /* based on routine in IDL Astro DAYCNV routine */
   l = (int)(JulianDay) + 68569;
   n = 4 * l / 146097;
   /* printf("\tl,n=%d,%d\n",l,n);*/
   l = l - (146097 * n + 3)/ 4;
   yr = 4000*(l + 1) / 1461001;
   /*printf("\tl,yr=%d,%d\n",l,yr);*/
   l = l - 1461*yr / 4 + 31;  /* 1461 = 365.25 * 4 */
   mn = 80 * l / 2447;
   /* printf("\tl,mn=%d,%d\n",l,mn); */
   calendar->day = (int)(l - 2447 * mn / 80);
   l = mn/11;
   calendar->month = (int)(mn + 2 - 12 * l);
   calendar->year = (int)(100*(n - 49) + yr + l);
   /* printf("year,month,day: %d, %d, %d\n",calendar->year,calendar->month,calendar->day); */
}