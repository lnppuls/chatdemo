#include"errcon.h"
#include<iostream>
void errif(int con,std::string source){
    if(con){
        std::cout << "error: " << source << std::endl;
        exit(0);
    }
}