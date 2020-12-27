#pragma once

#include "account.h"
#include <map>
#include <semaphore.h>

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
    int get_fees();
    void print_accounts();

};

accounts_collection::accounts_collection() : rd_cnt(0), bank_balance(0){
    sem_init(rd_sem, 0, 1);
    sem_init(wr_sem, 0, 1);
}

accounts_collection::~accounts_collection(){
    sem_destroy(rd_sem);
    sem_destroy(wr_sem);
}

void accounts_collection::add_account(int acc_num, int pswrd, int initial_blnce){
    sem_wait(wr_sem);
    auto new_account = account(acc_num,pswrd,initial_blnce);
    collection[acc_num] = new_account;
    sem_post(wr_sem);
}

void accounts_collection::remove_account(int acc_num);

account& accounts_collection::get_account(int acc_num);

int accounts_collection::get_fees();

void accounts_collection::print_accounts();
