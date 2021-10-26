// This is code generated by hgen. DO NOT EDIT!!!
// hgen version: v0.1.5
// source: string.gfj

#ifndef __string_RPCH_SERVER_H_
#define __string_RPCH_SERVER_H_

#include <stdint.h>
#include "error.h"
#include "server.h"



// server should implement following functions for service: string
//**********************************************************
char* string_toupper(char*, error_t*);
char* string_tolower(char*, error_t*);
char* string_concat(char*, char*, error_t*);
int32_t string_atoi(char*, error_t*);
//**********************************************************
void register_string_service(server_t*);

#endif