#ifndef __MY_ERR_H
#define __MY_ERR_H

#include <errno.h>
#include <stdlib.h>
#include <iostream>
#include <string>

void print_err(std::string&& str,int line,int err_no);

#endif /* __MY_ERR_H */