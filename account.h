#pragma once

#include <pthread.h>
#include <semaphore.h>

class account
{
private:
    int account_num;
    int password;
    int balance;

    sem_t* rd_sem;
    sem_t* wr_sem;
    int rd_cnt;
    
public:
    account(int acc_num, int pswrd, int initial_blnce);
    account(account &acc);
    ~account();
    bool check_password(const int pass) const;
    int deposit(int amount);
    int withdraw(int amount);
    int get_balance();

};
