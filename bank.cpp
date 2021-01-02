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
#include "atm.h"

accounts_collection accounts;
extern pthread_mutex_t mtx_log;

std::ofstream log_file;


void *bank_fees_func(void* _not_used)
{
    while(true)
    {
        sleep(3);
        accounts.collect_fees();
    }
}


void *bank_printer_func(void* _not_used)
{
    while (true)
    {
        usleep(500000);
        accounts.print_accounts();
    }
}

int main(int argc, char **argv)
{
    if(argc < 3 || std::atoi(argv[1]) != argc-2)
    {
        printf("illegal arguments\n");
        exit(1);
    }
    for(int i = 2; i < argc; i++)
    {
        std::ifstream f(argv[i]);
        if(!f.good())
        {
            printf("illegal arguments\n");
            exit(1);
        }
    }
    srand(time(NULL));
    // clear log file
    log_file.open("log.txt");
    if(log_file.is_open())
    {
        log_file << "";
    }
    else
    {
        fprintf(stderr, "error: could not open log file\n");
        exit(1);
    }
    log_file.close();
    // initiate log file mutex
    pthread_mutex_init(&mtx_log, NULL);
    
    // init thread data
    int num_atms = argc-2;
    std::vector<pthread_t> atms(num_atms);
    std::vector<atm_data> atms_data(num_atms);
    pthread_t bank_fees;
    pthread_t bank_printer;
   
    int rc;

    //create atms threads:
    for(int i = 0; i < num_atms; i++)
    {
        atms_data[i].atm_file = argv[i+2];
        atms_data[i].atm_num = i + 1;
        if ((rc= pthread_create(&atms[i], NULL, &atm, (void*)&atms_data[i])))
        {
            fprintf(stderr,  "error: pthread_create, rc: %d\n", rc);
            exit(1);
        }
    }

    //create bank threads:
    if ((rc= pthread_create(&bank_fees, NULL, &bank_fees_func, NULL)))
    {
        fprintf(stderr,  "error: pthread_create, rc: %d\n", rc);
        exit(1);
    }
    if ((rc= pthread_create(&bank_printer, NULL, &bank_printer_func, NULL)))
    {
        fprintf(stderr,  "error: pthread_create, rc: %d\n", rc);
        exit(1);
    }

    // wait for atms to finish:
    for(int i = 0; i < num_atms; i++) 
    {
        pthread_join(atms[i], NULL);
    }

    

    pthread_mutex_destroy(&mtx_log);    
    return 0;
}
