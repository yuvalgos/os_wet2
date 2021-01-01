
#include "accounts_collection.h"
#include "account.h"
#include <string>
#include <pthread.h>
#include <time.h>
#include <iostream>
#include <stringstream>
#include <mutex>
#include <fstream>

accounts_collection accounts;
std::mutex mtx_log;

std::vector<std::string> &split(const std::string &s, char delim, std::vector<std::string> &elems) {
    std::stringstream ss(s);
    std::string item;
    while(std::getline(ss, item, delim)) {
        elems.push_back(item);
    }
    return elems;
}


void print_to_log_func(std::string str)
{
    mtx_log.lock();

    ofstream log_file;
    log_file.open("log.txt");
    if(log_file.is_open())
    {
        log_file << str << std::endl;
    }
    else
    {
        fprintf(sderr,  "error: could not open log file %d\n");
        exit(1);
    }
    log_file.close();

    mtx_log.unlock();
}


void bank_fees_func(void* _not_used)
{
    while(true)
    {
        sleep(3)
        accounts.collect_fees();
        printf("fees collected")
    }
}


void bank_print_func(void* _not_used)
{

}


void atm(void* file_name)
{
    std::string file_name_str = *(std::string *)file_name;
    
    std::ifstream file(file_name_str);
    if(!file.is_open)
    {
        pthread_exit(NULL);
    }
    std::string str; 
    while (std::getline(file, str))
    {
        std::vector<std::string> splited_command;
        split(str, " ", splited_command);
        
        if(splited_command[0] == "O")
        {
            printf("O");
        }
        else if(splited_command[0] == "D")
        {
            printf("D");
        }
        else if(splited_command[0] == "W")
        {
            printf("W");
        }
        else if(splited_command[0] == "B")
        {
            printf("B");
        }
        else if(splited_command[0] == "Q")
        {
            printf("Q");
        }
        else if(splited_command[0] == "T")
        {
            printf("T");
        }

        usleep(100000)
    }
    pthread_exit(NULL);
}


int main(int argc, char **argv)
{
    if(argc < 3 || std::atoi(argv[1]) != argc-2)
    {
        printf("illegal arguments");
        exit(1);
    }
    for(int i = 2; i < argc; i++)
    {
        ifstream f(argv[i]);
        if(!f.good())
        {
            printf("illegal arguments");
            exit(1);
        }
    }
    srand(time(NULL));

    int num_atms = argc-2;
    pthread_t atms = new pthread_t[num_atms];
    pthread_t bank_fees;
    pthread_t bank_printer;
   
    int rc;

    //create atms threads:
    for(int i = 0; i < num_atms; i++)
    {
        if ((rc= pthread_create(&atms[i], NULL, &atm, (void*)argv[i+2])))
        {
            fprintf(sderr,  "error: pthread_create, rc: %d\n", rc);
            exit(1);
        }
    }

    //create bank threads:
    if ((rc= pthread_create(&bank_fees, NULL, &bank_fees_func, NULL)))
    {
        fprintf(sderr,  "error: pthread_create, rc: %d\n", rc);
        exit(1);
    }
    if ((rc= pthread_create(&bank_printer, NULL, &bank_printer_func, NULL)))
    {
        fprintf(sderr,  "error: pthread_create, rc: %d\n", rc);
        exit(1);
    }

    // wait for atms to finish:
    for(int i = 0; i < num_atms; i++) 
    {
        pthread_join(atms[i], NULL);
    }

    return 0;
}
