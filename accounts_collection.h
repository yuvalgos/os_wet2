#pragma once

#include "account.h"
#include <map>
#include <semaphore.h>
#include <stdlib.h>
#include <string>
#include <fstream>
#include <mutex>
#include <sstream>

void print_to_log(std::string str);
class accounts_collection
{
private:
    int rd_cnt;
    int bank_balance;

    sem_t rd_sem;
    sem_t wr_sem;
    
    std::map<int,account> collection;

public:
    accounts_collection();
    ~accounts_collection();
    void add_account(int acc_num, int pswrd, int initial_blnce);
    int remove_account(int acc_num);
    account& get_account(int acc_num);
    void collect_fees();
    void print_accounts();
    int get_bank_balance();
    bool acount_exists(int acc_num);

};


