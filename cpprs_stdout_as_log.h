#pragma once

#define LOG_ERRORs {puts("ERROR: " XSTR(__FILE__) " " XSTR(__LINE__));}
#define LOG_ERROR_INTs(i) {LOG_ERRORs printf("%d\n",i);}
#define LOG_ERROR_LITERALs(lit) {LOG_ERRORs puts(lit);}
