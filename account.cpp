#include "account.h"
#include <math.h>

account::account(int acc_num, int pswrd, int initial_blnce):
    account_num(acc_num), password(pswrd), balance(initial_blnce), rd_cnt(0){
    sem_init(rd_sem, 0, 1);
    sem_init(wr_sem, 0, 0);
    sleep(1);
    sem_post(wr_sem);
}

account::account(account &acc):account_num(acc.account_num), password(acc.password), balance(acc.balance), rd_cnt(acc.rd_cnt){
    sem_init(rd_sem, 0, 1);
    sem_init(wr_sem, 0, 1);
}

account::~account(){
    sem_wait(wr_sem);
    sleep(1)
    sem_destroy(rd_sem);
    sem_destroy(wr_sem);
}

bool account::check_password(const int pass) const {
    return (pass == password);
}

/*assume: 
    password is correct 
    the account exists
    you are the only one touching the account after sem_wait
*/
int account::deposit(int amount){ 
    sem_wait(wr_sem);
    int new_balance;
    balance += amount;
    new_balance = balance;
    sleep(1);
    sem_post(wr_sem);
    return new_balance;
}

int account::withdraw(int amount){
    sem_wait(wr_sem);
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
    sem_post(wr_sem);
    return new_balance;
}

int account::get_balance(){
    sem_wait(rd_sem);
    rd_cnt++;
    if (rd_cnt == 1) 
        sem_wait(wr_sem);
    sem_post(rd_sem); 
    int ret_balance = balance;
    sleep(1);
    sem_wait(rd_sem);
    rd_cnt--;
    if(rd_cnt == 0) 
        sem_post(wr_sem);
    sem_post(rd_sem);
    return ret_balance; 
}

int account::take_fees(int percent)
{
    sem_wait(wr_sem);
    int fee = round((double)balance * (double)percent / (double)100);
    balance -= fee;
    sem_post(wr_sem);
    return fee;
}
