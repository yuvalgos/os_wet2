#pragma once
#include "accounts_collection.h"
#include "account.h"
#include <string>
#include <pthread.h>
#include <time.h>
#include <iostream>
#include <sstream>
#include <mutex>
#include <fstream>
#include <vector>


typedef struct _atm_data{
    char* atm_file;
    int atm_num;
} *atm_data_p, atm_data;


void *atm(void* atm_data);