#include "accounts_collection.h"

pthread_mutex_t mtx_log;
extern std::ofstream log_file;

void print_to_log(std::string str)
{
    pthread_mutex_lock(&mtx_log);

    log_file.open("log.txt" , std::ios_base::app);
    if(log_file.is_open())
    {
        log_file << str << std::endl;
    }
    else
    {
        fprintf(stderr, "error: could not open log file\n");
        exit(1);
    }
    log_file.close();

    pthread_mutex_unlock(&mtx_log);
}

accounts_collection::accounts_collection() : rd_cnt(0), bank_balance(0){
    sem_init(&rd_sem, 0, 1);
    sem_init(&wr_sem, 0, 1);
}

accounts_collection::~accounts_collection(){
    sem_destroy(&rd_sem);
    sem_destroy(&wr_sem);
}

void accounts_collection::add_account(int acc_num, int pswrd, int initial_blnce){
    sem_wait(&wr_sem);
    account new_account(acc_num,pswrd,initial_blnce);
    collection[acc_num] = new_account;
    sem_post(&wr_sem);
}

void accounts_collection::remove_account(int acc_num)
{
    sem_wait(&wr_sem);
    collection.erase(acc_num);
    sem_post(&wr_sem);
}

account& accounts_collection::get_account(int acc_num)
{
    sem_wait(&rd_sem);
    rd_cnt++;
    if (rd_cnt == 1) 
        sem_wait(&wr_sem);
    sem_post(&rd_sem); 

    auto& ret_acc = collection[acc_num];

    sem_wait(&rd_sem);
    rd_cnt--;
    if(rd_cnt == 0) 
        sem_post(&wr_sem);
    sem_post(&rd_sem);

    return ret_acc; 
}

void accounts_collection::collect_fees()
{
    int fee_percent = (std::rand()%3) + 2;

    sem_wait(&wr_sem);

    for(auto iter_acc = collection.begin(); iter_acc != collection.end(); iter_acc++)
    {
        int fee = iter_acc->second.take_fees(fee_percent);
        bank_balance += fee;
        std::string s = "Bank: commissions of " + std::to_string(fee_percent) + " % were charged, the bank gained " 
                        + std::to_string(fee) + " $ from account " +  std::to_string(iter_acc->first);
        print_to_log(s);
    }

    sem_post(&wr_sem);
}

int accounts_collection::get_bank_balance()
{
    sem_wait(&rd_sem);
    rd_cnt++;
    if (rd_cnt == 1) 
        sem_wait(&wr_sem);
    sem_post(&rd_sem); 

    auto b = bank_balance;

    sem_wait(&rd_sem);
    rd_cnt--;
    if(rd_cnt == 0) 
        sem_post(&wr_sem);
    sem_post(&rd_sem);

    return b; 
}

void accounts_collection::print_accounts()
{
    sem_wait(&rd_sem);
    rd_cnt++;
    if (rd_cnt == 1) 
        sem_wait(&wr_sem);
    sem_post(&rd_sem); 

    printf("current bank status");

    sem_wait(&rd_sem);
    rd_cnt--;
    if(rd_cnt == 0) 
        sem_post(&wr_sem);
    sem_post(&rd_sem);
}

bool accounts_collection::acount_exists(int acc_num){
    sem_wait(&rd_sem);
    rd_cnt++;
    if (rd_cnt == 1) 
        sem_wait(&wr_sem);
    sem_post(&rd_sem); 

    bool res = (collection.find(acc_num) != collection.end());

    sem_wait(&rd_sem);
    rd_cnt--;
    if(rd_cnt == 0) 
        sem_post(&wr_sem);
    sem_post(&rd_sem);

    return res;
}