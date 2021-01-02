#include "account.h"
#include <math.h>
#include <stdio.h>

// this is just for map creation and should never be used
account::account(){}

account::account(int acc_num, int pswrd, int initial_blnce):
    account_num(acc_num), password(pswrd), balance(initial_blnce), rd_cnt(0), fast_destruct(false){
    if(sem_init(&rd_sem, 0, 1) < 0){
        fprintf(stderr, "Error: %d - failed to init account read semaphore\n", errno);
        exit(1);
    }
    if(sem_init(&wr_sem, 0, 0) < 0){
        fprintf(stderr, "Error: %d - failed to init account write semaphore\n", errno);
        exit(1);
    }
    sleep(1);
    sem_post(&wr_sem);
}

account::account(account &acc):account_num(acc.account_num), password(acc.password), balance(acc.balance), rd_cnt(acc.rd_cnt), fast_destruct(false){
    if(sem_init(&rd_sem, 0, 1) < 0){
        fprintf(stderr, "Error: %d - failed to init account read semaphore\n", errno);
        exit(1);
    }
    if(sem_init(&wr_sem, 0, 1) < 0){
        fprintf(stderr, "Error: %d - failed to init account write semaphore\n", errno);
        exit(1);
    }
}

account::account(const account &acc):account_num(acc.account_num), password(acc.password), balance(acc.balance), rd_cnt(acc.rd_cnt), fast_destruct(false){
    if(sem_init(&rd_sem, 0, 1) < 0){
        fprintf(stderr, "Error: %d - failed to init account read semaphore\n", errno);
        exit(1);
    }
    if(sem_init(&wr_sem, 0, 1) < 0){
        fprintf(stderr, "Error: %d - failed to init account write semaphore\n", errno);
        exit(1);
    }
}

account::~account(){
    if(sem_wait(&wr_sem) < 0){
        fprintf(stderr, "Error: %d - failed to lock account write semaphore\n", errno);
        exit(1);
    }
    if(sem_destroy(&rd_sem) < 0){
        fprintf(stderr, "Error: %d - failed to destroy account read semaphore\n", errno);
        exit(1);
    }
    if(!fast_destruct){
        sleep(1);
    }
    if(sem_destroy(&wr_sem) < 0){
        fprintf(stderr, "Error: %d - failed to destroy account write semaphore\n", errno);
        exit(1);
    }
}

bool account::check_password(const int pass) const {
    return (pass == password);
}

int account::get_password() const {
    return password;
}

/*assume: 
    password is correct 
    the account exists
    you are the only one touching the account after sem_wait
*/
int account::deposit(int amount){ 
    if(sem_wait(&wr_sem) < 0){
        fprintf(stderr, "Error: %d - failed to lock account write semaphore\n", errno);
        exit(1);
    }
    int new_balance;
    balance += amount;
    new_balance = balance;
    sleep(1);
    if(sem_post(&wr_sem) < 0){
        fprintf(stderr, "Error: %d - failed to unlock account write semaphore\n", errno);
        exit(1);
    }
    return new_balance;
}

int account::withdraw(int amount){
    if(sem_wait(&wr_sem) < 0){
        fprintf(stderr, "Error: %d - failed to lock account write semaphore\n", errno);
        exit(1);
    }
    int new_balance;
    if(balance >= amount){
        balance += amount;
        new_balance = balance;
    }
    else
    {
        new_balance = -1;
    }
    sleep(1);
    if(sem_post(&wr_sem) < 0){
        fprintf(stderr, "Error: %d - failed to unlock account write semaphore\n", errno);
        exit(1);
    }
    return new_balance;
}

int account::get_balance(){
    if(sem_wait(&rd_sem) < 0) {
        fprintf(stderr, "Error: %d - failed to lock account read semaphore\n", errno);
        exit(1);
    }
    rd_cnt++;
    if (rd_cnt == 1) {
        if(sem_wait(&wr_sem) < 0){
            fprintf(stderr, "Error: %d - failed to lock account write semaphore\n", errno);
            exit(1);
        }
    }
    if(sem_post(&rd_sem) < 0){
        fprintf(stderr, "Error: %d - failed to unlock account read semaphore\n", errno);
        exit(1);
    } 
    
    int ret_balance = balance;
    sleep(1);
    
    if(sem_wait(&rd_sem) < 0) {
        fprintf(stderr, "Error: %d - failed to lock account read semaphore\n", errno);
        exit(1);
    }
    rd_cnt--;
    if(rd_cnt == 0) { 
        if(sem_post(&wr_sem) < 0){
            fprintf(stderr, "Error: %d - failed to unlock account write semaphore\n", errno);
            exit(1);
        }
    }
    if(sem_post(&rd_sem) < 0){
        fprintf(stderr, "Error: %d - failed to unlock account read semaphore\n", errno);
        exit(1);
    } 
    return ret_balance; 
}

int account::get_balance_no_sleep(){
    if(sem_wait(&rd_sem) < 0) {
        fprintf(stderr, "Error: %d - failed to lock account read semaphore\n", errno);
        exit(1);
    }
    rd_cnt++;
    if (rd_cnt == 1) {
        if(sem_wait(&wr_sem) < 0){
            fprintf(stderr, "Error: %d - failed to lock account write semaphore\n", errno);
            exit(1);
        }
    }
    if(sem_post(&rd_sem) < 0){
        fprintf(stderr, "Error: %d - failed to unlock account read semaphore\n", errno);
        exit(1);
    } 

    int ret_balance = balance;

    if(sem_wait(&rd_sem) < 0) {
        fprintf(stderr, "Error: %d - failed to lock account read semaphore\n", errno);
        exit(1);
    }
    rd_cnt--;
    if(rd_cnt == 0) { 
        if(sem_post(&wr_sem) < 0){
            fprintf(stderr, "Error: %d - failed to unlock account write semaphore\n", errno);
            exit(1);
        }
    }
    if(sem_post(&rd_sem) < 0){
        fprintf(stderr, "Error: %d - failed to unlock account read semaphore\n", errno);
        exit(1);
    }

    return ret_balance; 
}

int account::take_fees(int percent)
{
    if(sem_wait(&wr_sem) < 0){
        fprintf(stderr, "Error: %d - failed to lock account write semaphore\n", errno);
        exit(1);
    }
    int fee = round((double)balance * (double)percent / (double)100);
    balance -= fee;
    if(sem_post(&wr_sem) < 0){
        fprintf(stderr, "Error: %d - failed to unlock account write semaphore\n", errno);
        exit(1);
    }
    return fee;
}
