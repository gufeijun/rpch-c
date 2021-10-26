#include <assert.h>
#include <stdio.h>

#include "../book.rpch.client.h"
#include "client.h"
#include "error.h"

void check_err(client_t* cli) {
    if (client_failed(cli)) {
        printf("err: %s\n", client_failed_msg(cli));
        exit(-1);
    }
}

void addBook(client_t* cli, char* name, char* price_unit, float num) {
    struct Book book;
    struct Price price;
    price.num = num;
    price.unit = price_unit;
    book.name = name;
    book.price = &price;
    bookMarket_shelve(&book, cli);
    check_err(cli);
}

int main() {
    error_t err = error_new();
    client_t* cli = client_dial("127.0.0.1:8080", &err);
    if (!err.null) {
        printf("err occurred: %s\n", err.msg);
        return -1;
    }
    addBook(cli, "线性代数", "yuan", 20);
    struct Book* book = bookMarket_getBook("线性代数", cli);
    check_err(cli);
    assert(strcmp(book->name, "线性代数") == 0 && book->price->num == 20);

    struct Account account;
    account.userName = "jack";
    account.balance = 100;
    {
        struct Account* res = bookMarket_consume(book, &account, cli);
        check_err(cli);
        assert(res->balance == 80 && strcmp(res->userName, "jack") == 0);
        Account_delete(res);
    }
    {
        struct Price price;
        price.num = 2500;
        price.unit = "cent";
        bookMarket_updateBookPrice("线性代数", &price, cli);
        check_err(cli);
    }
    Book_delete(book);
    book = bookMarket_getBook("线性代数", cli);
    check_err(cli);
    assert(strcmp(book->name, "线性代数") == 0 && book->price->num == 2500 &&
           strcmp(book->price->unit, "cent") == 0);
    {
        account.balance = 80;
        struct Account* res = bookMarket_consume(book, &account, cli);
        check_err(cli);
        assert(res->balance == 55 && strcmp(res->userName, "jack") == 0);
        Account_delete(res);
    }
    printf("test succ!\n");
}
