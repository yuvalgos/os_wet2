
#include "accounts_collection.h"
#include "account.h"
#include <string>
#include <pthread.h>
#include <time.h>
#include <iostream>
#include <ifstream>
#include <stringstream>

std::vector<std::string> &split(const std::string &s, char delim, std::vector<std::string> &elems) {
    std::stringstream ss(s);
    std::string item;
    while(std::getline(ss, item, delim)) {
        elems.push_back(item);
    }
    return elems;
}


void atm(void* file_name)
{
    std::string &file_name_str = *(std::string *)file_name;
    
    std::ifstream file(file_name_str);
    if(!file.is_open)
    {
        pthread_exit(NULL);
    }
    std::string str; 
    while (std::getline(file, str))
    {
        std::vector<std::string> splited_command;
        split(str, " ", splited_command);
        
        if(splited_command[0] == "O")
        {

        }
        else if(splited_command[0] == "D")
        {

        }
        else if(splited_command[0] == "W")
        {
            
        }
        else if(splited_command[0] == "B")
        {
            
        }
        else if(splited_command[0] == "Q")
        {
            
        }
        else if(splited_command[0] == "T")
        {
            
        }


        usleep(100000)
    }
    pthread_exit(NULL);
}

int main()
{
    srand(time(NULL));


}
