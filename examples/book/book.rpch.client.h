// This is code generated by hgen. DO NOT EDIT!!!
// hgen version: v0.1.5
// source: book.gfj

#ifndef __book_RPCH_CLIENT_H_
#define __book_RPCH_CLIENT_H_

#include <stdint.h>
#include "client.h"

struct Account;
struct Book;
struct Price;

struct Account{		
	char* userName;		
	float balance;
};

struct Book{		
	struct Price* price;		
	char* name;
};

struct Price{		
	char* unit;		
	float num;
};

void Book_delete(struct Book*);
void Account_delete(struct Account*);

struct Book* bookMarket_getBook(char*, client_t*);
struct Account* bookMarket_consume(struct Book*, struct Account*, client_t*);
void bookMarket_updateBookPrice(char*, struct Price*, client_t*);
void bookMarket_shelve(struct Book*, client_t*);

#endif