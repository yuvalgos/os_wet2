#pragma once

#include "account.h"
#include <map>
#include <semaphore.h>
#include <stdlib.h>

class accounts_collection
{
private:
    std::map<int,account> collection;

    sem_t* rd_sem;
    sem_t* wr_sem;
    int rd_cnt;

    int bank_balance;

public:
    accounts_collection();
    ~accounts_collection();
    void add_account(int acc_num, int pswrd, int initial_blnce);
    void remove_account(int acc_num);
    account& get_account(int acc_num);
    void collect_fees();
    void print_accounts();
    int get_bank_balance();

};


