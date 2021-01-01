
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

accounts_collection accounts;
extern pthread_mutex_t mtx_log;

std::ofstream log_file;

typedef struct _atm_data{
    char* atm_file;
    int atm_num;
} *atm_data_p, atm_data;

std::vector<std::string> &split(const std::string &s, char delim, std::vector<std::string> &elems) {
    std::stringstream ss(s);
    std::string item;
    while(std::getline(ss, item, delim)) {
        elems.push_back(item);
    }
    return elems;
}


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


void *atm(void* atm_data)
{
    int atm_id = ((atm_data_p)atm_data)->atm_num;
    std::string file_name_str (((atm_data_p)atm_data)->atm_file);
    std::ifstream file(file_name_str.c_str());
    if(!file.is_open())
    {
        printf("error opening file\n");
        pthread_exit(NULL);
    }
    std::string str; 
    while (std::getline(file, str))
    {
        std::vector<std::string> splited_command;
        split(str, ' ', splited_command);

        if(splited_command[0] == "O")
        {
            printf("O\n");
            int acc_id = std::atoi(splited_command[1].c_str());
            int acc_pass = std::atoi(splited_command[2].c_str());
            int acc_amount = std::atoi(splited_command[3].c_str());
            if(accounts.acount_exists(acc_id)){
                std::string s = "Error " + std::to_string(atm_id) + ": Your transaction failed – account with the same id exists"; 
                print_to_log(s);
            }
            else {
                accounts.add_account(acc_id, acc_pass, acc_amount);
                
                std::string s = std::to_string(atm_id) + ": New account id is " + splited_command[1] + " with password " + splited_command[2] + " and initial balance " + splited_command[3]; 
                print_to_log(s);
            }
        }
        else if(splited_command[0] == "D")
        {
            printf("D\n");
            int acc_id = std::atoi(splited_command[1].c_str());
            int acc_pass = std::atoi(splited_command[2].c_str());
            int acc_amount = std::atoi(splited_command[3].c_str());
            if(!accounts.acount_exists(acc_id)){
                std::string s = "Error " + std::to_string(atm_id) + ": Your transaction failed – account " + splited_command[1] + " does not exist"; 
                print_to_log(s);
            } else {
                account acc = accounts.get_account(acc_id);
                if(!acc.check_password(acc_pass)){
                    std::string s = "Error " + std::to_string(atm_id) + ": Your transaction failed – password for account id " + splited_command[1] + " is incorrect"; 
                    print_to_log(s);
                }
                else
                {
                    int new_balance = acc.deposit(acc_amount);

                    std::string s = std::to_string(atm_id) + ": Account " + splited_command[1] + " new balance is " + 
                                    std::to_string(new_balance) + " after " + splited_command[3] + " $ was deposited";
                    print_to_log(s);
                }
            }
        }
        else if(splited_command[0] == "W")
        {
            printf("W\n");
            int acc_id = std::atoi(splited_command[1].c_str());
            int acc_pass = std::atoi(splited_command[2].c_str());
            int acc_amount = std::atoi(splited_command[3].c_str());
            if(!accounts.acount_exists(acc_id)){
                std::string s = "Error " + std::to_string(atm_id) + ": Your transaction failed – account " + splited_command[1] + " does not exist"; 
                print_to_log(s);
            } else {
                account acc = accounts.get_account(acc_id);
                if(!acc.check_password(acc_pass)){
                    std::string s = "Error " + std::to_string(atm_id) + ": Your transaction failed – password for account id " + splited_command[1] + " is incorrect"; 
                    print_to_log(s);
                }
                else
                {
                    int new_balance = acc.withdraw(acc_amount);
                    if(new_balance < 0) {
                        std::string s = "Error " + std::to_string(atm_id) + ": Your transaction failed – account id " + splited_command[1] + 
                                        " balance is lower than " + splited_command[3]; 
                        print_to_log(s);
                    }
                    else {
                        std::string s = std::to_string(atm_id) + ": Account " + splited_command[1] + " new balance is " + 
                                        std::to_string(new_balance) + " after " + splited_command[3] + " $ was withdrew";
                        print_to_log(s);
                    }
                }
            }
        }
        else if(splited_command[0] == "B")
        {
            printf("B\n");
        }
        else if(splited_command[0] == "Q")
        {
            printf("Q\n");
        }
        else if(splited_command[0] == "T")
        {
            printf("T\n");
        }

        usleep(100000);
    }
    pthread_exit(NULL);
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
