#include "accounts_collection.h"

pthread_mutex_t mtx_log;
extern std::ofstream log_file;

void print_to_log(std::string str)
{
    if(pthread_mutex_lock(&mtx_log) != 0)
    {
        fprintf(stderr, "error: locking mutex\n");
        exit(1);
    }

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

    if(pthread_mutex_unlock(&mtx_log) != 0)
    {
        fprintf(stderr, "error: unlocking mutex\n");
        exit(1);
    }
}

accounts_collection::accounts_collection() : rd_cnt(0), bank_balance(0){
    if(sem_init(&rd_sem, 0, 1) < 0)
    {
        fprintf(stderr, "error: initializing semaphore\n");
        exit(1);
    }
    if(sem_init(&wr_sem, 0, 1) < 0)
    {
        fprintf(stderr, "error: initializing semaphore\n");
        exit(1);
    }
}

accounts_collection::~accounts_collection(){
    if(sem_destroy(&rd_sem) < 0)
    {
        fprintf(stderr, "error: initializing semaphore\n");
        exit(1);
    }
    if(sem_destroy(&wr_sem) < 0)
    {
        fprintf(stderr, "error: initializing semaphore\n");
        exit(1);
    }
}

void accounts_collection::add_account(int acc_num, int pswrd, int initial_blnce){
    if(sem_wait(&wr_sem) < 0)
    {
        fprintf(stderr, "error: on sem_wait syscall\n");
        exit(1);
    }
    collection[acc_num] = account(acc_num,pswrd,initial_blnce)
    if(sem_post(&wr_sem) < 0)
    {
        fprintf(stderr, "error: on sem_post syscall\n");
        exit(1);
    }
}

int accounts_collection::remove_account(int acc_num)
{
    if(sem_wait(&wr_sem) < 0)
    {
        fprintf(stderr, "error: on sem_wait syscall\n");
        exit(1);
    }

    int balance = collection.at(acc_num).get_balance_no_sleep();
    collection.erase(acc_num);

    if(sem_post(&wr_sem) < 0)
    {
        fprintf(stderr, "error: on sem_post syscall\n");
        exit(1);
    }
    return balance;
}

account& accounts_collection::get_account(int acc_num)
{
    if(sem_wait(&rd_sem) < 0)
    {
        fprintf(stderr, "error: on sem_wait syscall\n");
        exit(1);
    }
    rd_cnt++;
    if (rd_cnt == 1) 
    {
        if(sem_wait(&wr_sem) < 0)
        {
            fprintf(stderr, "error: on sem_wait syscall\n");
            exit(1);
        }
    }
    if(sem_post(&rd_sem) < 0)
    {
        fprintf(stderr, "error: on sem_post syscall\n");
        exit(1);
    } 

    auto& ret_acc = collection[acc_num];

    if(sem_wait(&rd_sem) < 0)
    {
        fprintf(stderr, "error: on sem_wait syscall\n");
        exit(1);
    }
    rd_cnt--;
    if(rd_cnt == 0) 
    {
        if(sem_post(&wr_sem) < 0)
        {
            fprintf(stderr, "error: on sem_post syscall\n");
            exit(1);
        }
    }
    if(sem_post(&rd_sem) < 0)
    {
        fprintf(stderr, "error: on sem_post syscall\n");
        exit(1);
    } 

    return ret_acc; 
}

void accounts_collection::collect_fees()
{
    int fee_percent = (std::rand()%3) + 2;

    if(sem_wait(&wr_sem) < 0)
    {
        fprintf(stderr, "error: on sem_wait syscall\n");
        exit(1);
    }

    for(auto iter_acc = collection.begin(); iter_acc != collection.end(); iter_acc++)
    {
        int fee = iter_acc->second.take_fees(fee_percent);
        bank_balance += fee;
        std::string s = "Bank: commissions of " + std::to_string(fee_percent) + " % were charged, the bank gained " 
                        + std::to_string(fee) + " $ from account " +  std::to_string(iter_acc->first);
        print_to_log(s);
    }

    if(sem_post(&wr_sem) < 0)
    {
        fprintf(stderr, "error: on sem_post syscall\n");
        exit(1);
    }
}

int accounts_collection::get_bank_balance()
{
    if(sem_wait(&rd_sem) < 0)
    {
        fprintf(stderr, "error: on sem_wait syscall\n");
        exit(1);
    }
    rd_cnt++;
    if (rd_cnt == 1) 
    {
        if(sem_wait(&wr_sem) < 0)
        {
            fprintf(stderr, "error: on sem_wait syscall\n");
            exit(1);
        }
    }
    if(sem_post(&rd_sem) < 0)
    {
        fprintf(stderr, "error: on sem_post syscall\n");
        exit(1);
    } 

    auto b = bank_balance;
        
    if(sem_wait(&rd_sem) < 0)
    {
        fprintf(stderr, "error: on sem_wait syscall\n");
        exit(1);
    }
    rd_cnt--;
    if(rd_cnt == 0) 
    {
        if(sem_post(&wr_sem) < 0)
        {
            fprintf(stderr, "error: on sem_post syscall\n");
            exit(1);
        }
    }
    if(sem_post(&rd_sem) < 0)
    {
        fprintf(stderr, "error: on sem_post syscall\n");
        exit(1);
    } 

    return b; 
}

void accounts_collection::print_accounts()
{
    if(sem_wait(&rd_sem) < 0)
    {
        fprintf(stderr, "error: on sem_wait syscall\n");
        exit(1);
    }
    rd_cnt++;
    if (rd_cnt == 1) 
    {
        if(sem_wait(&wr_sem) < 0)
        {
            fprintf(stderr, "error: on sem_wait syscall\n");
            exit(1);
        }
    }
    if(sem_post(&rd_sem) < 0)
    {
        fprintf(stderr, "error: on sem_post syscall\n");
        exit(1);
    } 

    printf(“\033[2J”);
    printf(“\033[1;1H”);
    printf("current bank status\n");
    for(auto iter_acc = collection.begin(); iter_acc != collection.end(); iter_acc++)
    {
        printf("Account %d: Balance - %d $ , Account Password - %d\n",
         iter_acc->first, iter_acc->second.get_balance_no_sleep(), iter_acc->second.get_password());
    }
    printf("the bank has %d $\n", bank_balance);

    if(sem_wait(&rd_sem) < 0)
    {
        fprintf(stderr, "error: on sem_wait syscall\n");
        exit(1);
    }
    rd_cnt--;
    if(rd_cnt == 0) 
    {
        if(sem_post(&wr_sem) < 0)
        {
            fprintf(stderr, "error: on sem_post syscall\n");
            exit(1);
        }
    }
    if(sem_post(&rd_sem) < 0)
    {
        fprintf(stderr, "error: on sem_post syscall\n");
        exit(1);
    } 
}

bool accounts_collection::acount_exists(int acc_num)
{
    if(sem_wait(&rd_sem) < 0)
    {
        fprintf(stderr, "error: on sem_wait syscall\n");
        exit(1);
    }
    rd_cnt++;
    if (rd_cnt == 1) 
    {
        if(sem_wait(&wr_sem) < 0)
        {
            fprintf(stderr, "error: on sem_wait syscall\n");
            exit(1);
        }
    }
    if(sem_post(&rd_sem) < 0)
    {
        fprintf(stderr, "error: on sem_post syscall\n");
        exit(1);
    } 
    
    bool res = (collection.find(acc_num) != collection.end());

    if(sem_wait(&rd_sem) < 0)
    {
        fprintf(stderr, "error: on sem_wait syscall\n");
        exit(1);
    }
    rd_cnt--;
    if(rd_cnt == 0) 
    {
        if(sem_post(&wr_sem) < 0)
        {
            fprintf(stderr, "error: on sem_post syscall\n");
            exit(1);
        }
    }
    if(sem_post(&rd_sem) < 0)
    {
        fprintf(stderr, "error: on sem_post syscall\n");
        exit(1);
    } 

    return res;
}