/******************************************************************************
 * Copyright 2013-2014 Espressif Systems (Wuxi)
 *
 * FileName: user_main.c
 *
 * Description: entry file of user application
 *
 * Modification history:
 *     2014/1/1, v1.0 create this file.
*******************************************************************************/
#include "ets_sys.h"
#include "osapi.h"
#include "user_interface.h"

#include "user_devicefind.h"

void user_rf_pre_init(void){}



/******************************************************************************
 * FunctionName : user_init
 * Description  : entry of user application, init user function here
 * Parameters   : none
 * Returns      : none
*******************************************************************************/
void user_init(void)
{
#if DEBUG_MODE
    os_printf(" \n");
#endif

#if DEBUG_MODE
    os_printf("SDK version is :%s\n", system_get_sdk_version());
#endif


}

