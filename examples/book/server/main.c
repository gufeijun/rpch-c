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
    // account会在该函数结束后被销毁，不能直接返回account
    // 应该在堆区分配内存，对account进行深拷贝
    return Account_clone(account);
}

void bookMarket_shelve(struct Book* book, error_t* err) {
    // book会在当前请求完成后销毁，所以我们需要对其深拷贝后保存
    hashmap_set(book_map, book->name, Book_clone(book));
}

void bookMarket_updateBookPrice(char* bookName, struct Price* price,
                                error_t* err) {
    struct Book* book = (struct Book*)hashmap_get(book_map, bookName);
    if (book == NULL) {
        return;
    }
    free(book->price->unit);
    free(book->price);
    book->price = Price_clone(price);
}

struct Book* bookMarket_getBook(char* bookName, error_t* err) {
    struct Book* book = (struct Book*)hashmap_get(book_map, bookName);
    if (book == NULL) {
        errorf(err, "do not exsit book %s\n", bookName);
        return NULL;
    }
    //深拷贝book
    return Book_clone(book);
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
