#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "../book.rpch.server.h"
#include "error.h"
#include "hashmap.h"

hashmap_t* book_map;

struct Account* bookMarket_consume(struct Book* book, struct Account* account,
                                   error_t* err) {
    float price = 0;
    if (strcmp(book->price->unit, "yuan") == 0) {
        price = book->price->num;
    } else if (strcmp(book->price->unit, "dime") == 0) {
        price = book->price->num / 10;
    } else {
        price = book->price->num / 100;
    }
    account->balance -= price;
    struct Account* acc = Account_create();
    acc->balance = account->balance;
    acc->userName = strdup(account->userName);
    return acc;
}

void bookMarket_shelve(struct Book* book, error_t* err) {
    struct Book* b = Book_create();
    b->name = strdup(book->name);
    b->price->num = book->price->num;
    b->price->unit = strdup(book->price->unit);
    hashmap_set(book_map, book->name, b);
}

void bookMarket_updateBookPrice(char* bookName, struct Price* price,
                                error_t* err) {
    struct Book* book = (struct Book*)hashmap_get(book_map, bookName);
    if (book == NULL) {
        return;
    }
    book->price->num = price->num;
    book->price->unit = strdup(price->unit);
    book->name = bookName;
}

struct Book* bookMarket_getBook(char* bookName, error_t* err) {
    struct Book* book = (struct Book*)hashmap_get(book_map, bookName);
    if (book == NULL) {
        errorf(err, "do not exsit book %s\n", bookName);
        return NULL;
    }
    return book;
}

void book_destroy(void* arg) {
    struct Book* book = arg;
    free(book->price->unit);
    free(book->price);
    free(book);
}

int main() {
    book_map = hashmap_init(book_destroy, Type_String);
    error_t err = error_new();
    server_t* svr = server_create();
    register_bookMarket_service(svr);
    server_listen(svr, "127.0.0.1:8080", NULL, &err);
    if (!err.null) {
        printf("err occurred: %s\n", err.msg);
        return -1;
    }
    server_destroy(svr);
    return 0;
}
