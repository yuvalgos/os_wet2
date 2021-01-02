#pragma once

#include <pthread.h>
#include <semaphore.h>
#include <unistd.h>

class account
{
private:
    int account_num;
    int password;
    int balance;

    sem_t rd_sem;
    sem_t wr_sem;
    int rd_cnt;
    
public:
    account();
    account(const account &acc);
    account(int acc_num, int pswrd, int initial_blnce);
    account(account &acc);
    ~account();
    bool check_password(const int pass) const;
    int check_password() const;
    int deposit(int amount);
    int withdraw(int amount);
    int get_balance();
    int take_fees(int percent);

    int get_balance_no_sleep();

};
