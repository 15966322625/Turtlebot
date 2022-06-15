#include "my_err.h"
#include <stdio.h>
#include <errno.h>
#include <string.h>


void print_err(std::string&& str,int line,int err_no)
{
    std::cout<<line<<','<<str<<':'<<strerror(err_no)<<std::endl;
    str.erase();
    exit(-1);
}