#include "accounts_collection.h"

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

void accounts_collection::remove_account(int acc_num)
{
    sem_wait(wr_sem);
    collection.erase(acc_num);
    sem_post(wr_sem);
}

account& accounts_collection::get_account(int acc_num)
{
    sem_wait(rd_sem);
    rd_cnt++;
    if (rd_cnt == 1) 
        sem_wait(wr_sem);
    sem_post(rd_sem); 

    auto& ret_acc = collection[acc_num];

    sem_wait(rd_sem);
    rd_cnt--;
    if(rd_cnt == 0) 
        sem_post(wr_sem);
    sem_post(rd_sem);

    return ret_acc; 
}

void accounts_collection::get_fees()
{
    int fee_percent = (std::rand()%3) + 2

    sem_wait(wr_sem);

    for(auto iter_acc = collection.begin(); iter_acc != collection.end(); iter_acc++)
    {
        bank_balance += iter_acc->second.take_fees(fee_percent);
    }

    sem_post(wr_sem);
}

int accounts_collection::get_bank_balance()
{
    sem_wait(rd_sem);
    rd_cnt++;
    if (rd_cnt == 1) 
        sem_wait(wr_sem);
    sem_post(rd_sem); 

    auto b = bank_balance;

    sem_wait(rd_sem);
    rd_cnt--;
    if(rd_cnt == 0) 
        sem_post(wr_sem);
    sem_post(rd_sem);

    return b; 
}

void accounts_collection::print_accounts()
{
    sem_wait(rd_sem);
    rd_cnt++;
    if (rd_cnt == 1) 
        sem_wait(wr_sem);
    sem_post(rd_sem); 

    printf("current bank status");

    sem_wait(rd_sem);
    rd_cnt--;
    if(rd_cnt == 0) 
        sem_post(wr_sem);
    sem_post(rd_sem);
}