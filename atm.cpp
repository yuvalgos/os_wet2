#include "atm.h"


extern accounts_collection accounts;
extern pthread_mutex_t mtx_log;


std::vector<std::string> &split(const std::string &s, char delim, std::vector<std::string> &elems) {
    std::stringstream ss(s);
    std::string item;
    while(std::getline(ss, item, delim)) {
        elems.push_back(item);
    }
    return elems;
}


void *atm(void* atm_data)
{
    int atm_id = ((atm_data_p)atm_data)->atm_num;
    std::string file_name_str (((atm_data_p)atm_data)->atm_file);
    std::ifstream file(file_name_str.c_str());
    if(!file.is_open())
    {
        printf("error opening file\n");
        pthread_exit(NULL);
    }
    std::string str; 
    while (std::getline(file, str))
    {
        std::vector<std::string> splited_command;
        split(str, ' ', splited_command);

        if(splited_command[0] == "O")
        {
            printf("O\n");
            int acc_id = std::atoi(splited_command[1].c_str());
            int acc_pass = std::atoi(splited_command[2].c_str());
            int acc_amount = std::atoi(splited_command[3].c_str());
            if(accounts.acount_exists(acc_id)){
                std::string s = "Error " + std::to_string(atm_id) + ": Your transaction failed – account with the same id exists"; 
                print_to_log(s);
            }
            else {
                accounts.add_account(acc_id, acc_pass, acc_amount);
                
                std::string s = std::to_string(atm_id) + ": New account id is " + splited_command[1] + " with password " + splited_command[2] + " and initial balance " + splited_command[3]; 
                print_to_log(s);
            }
        }
        else if(splited_command[0] == "D")
        {
            printf("D\n");
            int acc_id = std::atoi(splited_command[1].c_str());
            int acc_pass = std::atoi(splited_command[2].c_str());
            int acc_amount = std::atoi(splited_command[3].c_str());
            if(!accounts.acount_exists(acc_id)){
                std::string s = "Error " + std::to_string(atm_id) + ": Your transaction failed – account id " + splited_command[1] + " does not exist"; 
                print_to_log(s);
            } else {
                account acc = accounts.get_account(acc_id);
                if(!acc.check_password(acc_pass)){
                    std::string s = "Error " + std::to_string(atm_id) + ": Your transaction failed – password for account id " + splited_command[1] + " is incorrect"; 
                    print_to_log(s);
                }
                else
                {
                    int new_balance = acc.deposit(acc_amount);

                    std::string s = std::to_string(atm_id) + ": Account " + splited_command[1] + " new balance is " + 
                                    std::to_string(new_balance) + " after " + splited_command[3] + " $ was deposited";
                    print_to_log(s);
                }
            }
        }
        else if(splited_command[0] == "W")
        {
            printf("W\n");
            int acc_id = std::atoi(splited_command[1].c_str());
            int acc_pass = std::atoi(splited_command[2].c_str());
            int acc_amount = std::atoi(splited_command[3].c_str());
            if(!accounts.acount_exists(acc_id)){
                std::string s = "Error " + std::to_string(atm_id) + ": Your transaction failed – account id " + splited_command[1] + " does not exist"; 
                print_to_log(s);
            } else {
                account acc = accounts.get_account(acc_id);
                if(!acc.check_password(acc_pass)){
                    std::string s = "Error " + std::to_string(atm_id) + ": Your transaction failed – password for account id " + splited_command[1] + " is incorrect"; 
                    print_to_log(s);
                }
                else
                {
                    int new_balance = acc.withdraw(acc_amount);
                    if(new_balance < 0) {
                        std::string s = "Error " + std::to_string(atm_id) + ": Your transaction failed – account id " + splited_command[1] + 
                                        " balance is lower than " + splited_command[3]; 
                        print_to_log(s);
                    }
                    else {
                        std::string s = std::to_string(atm_id) + ": Account " + splited_command[1] + " new balance is " + 
                                        std::to_string(new_balance) + " after " + splited_command[3] + " $ was withdrew";
                        print_to_log(s);
                    }
                }
            }
        }
        else if(splited_command[0] == "B")
        {
            printf("B\n");
            int acc_id = std::atoi(splited_command[1].c_str());
            int acc_pass = std::atoi(splited_command[2].c_str());
            
            if(!accounts.acount_exists(acc_id))
            {
                std::string s = "Error " + std::to_string(atm_id) + ": Your transaction failed – account id " + splited_command[1] + " does not exist"; 
                print_to_log(s);
            }
            else
            {
                account acc = accounts.get_account(acc_id);
                if(!acc.check_password(acc_pass)){
                    std::string s = "Error " + std::to_string(atm_id) + ": Your transaction failed – password for account id " + splited_command[1] + " is incorrect"; 
                    print_to_log(s);
                }
                else
                {
                    int balance = acc.get_balance();
                    std::string s = std::to_string(atm_id) + ": Account " + splited_command[1] + " balance is " + balance;
                    print_to_log(s);
                }                
            }        
        }
        else if(splited_command[0] == "Q")
        {
            printf("Q\n");
            
            int acc_id = std::atoi(splited_command[1].c_str());
            int acc_pass = std::atoi(splited_command[2].c_str());
            
            if(!accounts.acount_exists(acc_id))
            {
                std::string s = "Error " + std::to_string(atm_id) + ": Your transaction failed – account id " + splited_command[1] + " does not exist"; 
                print_to_log(s);
            }
            else
            {
                account acc = accounts.get_account(acc_id);
                if(!acc.check_password(acc_pass)){
                    std::string s = "Error " + std::to_string(atm_id) + ": Your transaction failed – password for account id " + splited_command[1] + " is incorrect"; 
                    print_to_log(s);
                }
                else
                {
                    int balance = accounts.remove_account(acc_id);
                    std::string s = std::to_string(atm_id) + ": Account " + splited_command[1] + " is now closed. Balance was " + balance;
                    print_to_log(s);
                }                
            }        
        }
        else if(splited_command[0] == "T")
        {
            printf("T\n");
            int source_acc_id = std::atoi(splited_command[1].c_str());
            int source_acc_pass = std::atoi(splited_command[2].c_str());
            int target_acc_id = std::atoi(splited_command[3].c_str());
            int amount = std::atoi(splited_command[4].c_str());
            if(!accounts.acount_exists(source_acc_id))
            {
                std::string s = "Error " + std::to_string(atm_id) + ": Your transaction failed – account id " + splited_command[1] + " does not exist"; 
                print_to_log(s);
            }
            else if(!accounts.acount_exists(target_acc_id))
            {
                std::string s = "Error " + std::to_string(atm_id) + ": Your transaction failed – account id " + splited_command[3] + " does not exist"; 
                print_to_log(s);
            }
            else if(source_acc_id == target_acc_id)
            {
                std::string s = "Error " + std::to_string(atm_id) + ": Your transaction failed - accounts numbers are the same"; 
                print_to_log(s);
            }
            else
            {
                account source_acc = accounts.get_account(source_acc_id);
                account target_acc = accounts.get_account(target_acc_id);
                if(!acc.check_password(source_acc_pass)){
                    std::string s = "Error " + std::to_string(atm_id) + ": Your transaction failed – password for account id " + splited_command[1] + " is incorrect"; 
                    print_to_log(s);
                }
                else
                {
                    int new_source_balance = source_acc.withdraw(amount);
                    if(new_source_balance < 0) 
                    {
                        std::string s = "Error " + std::to_string(atm_id) + ": Your transaction failed – account id " + splited_command[1] + 
                                        " balance is lower than " + splited_command[4]; 
                        print_to_log(s);
                    }
                    else 
                    {
                        int new_target_balance = target_acc.deposit(amount);
                        std::string s = std::to_string(atm_id) + ": Transfer " + splited_command[4] + " from account " 
                                        + splited_command[1] + " to account " + splited_command[3] + " new account balance is " 
                                        + std::to_string(new_source_balance) + " new target account balance is " + std::to_string(new_target_balance);
                        print_to_log(s);
                    }
                }                
            }        
        }
        usleep(100000);
    }
    pthread_exit(NULL);
}
