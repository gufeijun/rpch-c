// This is code generated by hgen. DO NOT EDIT!!!
// hgen version: v0.1.7
// source: book.gfj

#include "book.rpch.server.h"

#include <stdint.h>
#include <stdlib.h>
#include <string.h>

#include "argument.h"
#include "cJSON.h"
#include "error.h"
#include "request.h"
#include "server.h"

static inline __attribute__((always_inline)) void Account_init(struct Account*);
static inline __attribute__((always_inline)) void Account_destroy(struct Account*);
static inline __attribute__((always_inline)) void Book_init(struct Book*);
static inline __attribute__((always_inline)) void Book_destroy(struct Book*);
static inline __attribute__((always_inline)) void Price_init(struct Price*);
static inline __attribute__((always_inline)) void Price_destroy(struct Price*);

void Account_init(struct Account* data) {
	data->userName = NULL;
}
void Account_destroy(struct Account* data) {
	free(data->userName);
}
struct Account* Account_create() {
	struct Account* v = malloc(sizeof(struct Account));
	Account_init(v);
	return v;
}
void Book_init(struct Book* data) {
	data->price = malloc(sizeof(struct Price));
	Price_init(data->price);
	data->name = NULL;
}
void Book_destroy(struct Book* data) {
	Price_destroy(data->price);
	free(data->price);
	free(data->name);
}
struct Book* Book_create() {
	struct Book* v = malloc(sizeof(struct Book));
	Book_init(v);
	return v;
}
void Price_init(struct Price* data) {
	data->unit = NULL;
}
void Price_destroy(struct Price* data) {
	free(data->unit);
}
struct Price* Price_create() {
	struct Price* v = malloc(sizeof(struct Price));
	Price_init(v);
	return v;
}
struct Account* Account_clone(struct Account* src) {
	if (src == NULL) return NULL;
	struct Account* dst = malloc(sizeof(struct Account));		
	dst->userName = src->userName == NULL? NULL : strdup(src->userName);		
	dst->balance = src->balance;
	return dst;
}
struct Book* Book_clone(struct Book* src) {
	if (src == NULL) return NULL;
	struct Book* dst = malloc(sizeof(struct Book));		
	dst->price = Price_clone(src->price);		
	dst->name = src->name == NULL? NULL : strdup(src->name);
	return dst;
}
struct Price* Price_clone(struct Price* src) {
	if (src == NULL) return NULL;
	struct Price* dst = malloc(sizeof(struct Price));		
	dst->unit = src->unit == NULL? NULL : strdup(src->unit);		
	dst->num = src->num;
	return dst;
}

#define invalid_argcnt(err, want, got) \
    errorf(err, "expected count of arugments is %d, but got %d", want, got)
#define invalid_type(err, want, got) \
    errorf(err, "expected argument type: %s, but got %s", want, got)
#define invalid_type_size(err, t, want, got) \
    errorf(err, "expected size for type %s is %d, but got %d", t, want, got)
#define CHECK_ARG_CNT(want, got)                   \
    {                                              \
        if (got != want) {                         \
            invalid_argcnt(err, got, req->argcnt); \
			return;                                \
        }                                          \
    }
#define CHECK_ARG_TYPE(want, got)         \
    {                                     \
        if (strcmp(want, got) != 0) {     \
            invalid_type(err, want, got); \
			return;                       \
        }                                 \
    }
#define CHECK_ARG_SIZE(t, want, got)              \
    {                                             \
        if (want != got) {                        \
            invalid_type_size(err, t, want, got); \
			return;                               \
        }                                         \
    }
#define MARSHAL_FAILED(obj) error_put(err, "marshal struct " obj " failed");
#define UNMARSHAL_FAILED(obj) error_put(err, "unmarshal struct " obj " failed")

static cJSON* Account_marshal(struct Account* arg, error_t* err);
static cJSON* Book_marshal(struct Book* arg, error_t* err);
static cJSON* Price_marshal(struct Price* arg, error_t* err);

cJSON* Account_marshal(struct Account* data, error_t* err) {
	cJSON* root = NULL;
	
	if (data == NULL) goto bad;
    root = cJSON_CreateObject();
    if (!root) goto bad;
	if (data->userName == NULL) data->userName = strdup("");
    if (cJSON_AddStringToObject(root, "userName", data->userName) == NULL) goto bad;
    if (cJSON_AddNumberToObject(root, "balance", (double)data->balance) == NULL) goto bad;
	return root;
bad:
	if (!err->null) MARSHAL_FAILED("Account")
    if (root) cJSON_Delete(root);
	return NULL;
}

cJSON* Book_marshal(struct Book* data, error_t* err) {
	cJSON* root = NULL;
	cJSON* item = NULL;
	
	if (data == NULL) goto bad;
    root = cJSON_CreateObject();
    if (!root) goto bad;
    if (data->price == NULL) {
        if (cJSON_AddNullToObject(root, "price") == NULL) goto bad;
    } else {
		item = Price_marshal(data->price, err);
		if (!err->null) goto bad;
    	if (!cJSON_AddItemToObject(root, "price", item)) goto bad;
    }
	if (data->name == NULL) data->name = strdup("");
    if (cJSON_AddStringToObject(root, "name", data->name) == NULL) goto bad;
	return root;
bad:
	if (!err->null) MARSHAL_FAILED("Book")
    if (root) cJSON_Delete(root);
	return NULL;
}

cJSON* Price_marshal(struct Price* data, error_t* err) {
	cJSON* root = NULL;
	
	if (data == NULL) goto bad;
    root = cJSON_CreateObject();
    if (!root) goto bad;
	if (data->unit == NULL) data->unit = strdup("");
    if (cJSON_AddStringToObject(root, "unit", data->unit) == NULL) goto bad;
    if (cJSON_AddNumberToObject(root, "num", (double)data->num) == NULL) goto bad;
	return root;
bad:
	if (!err->null) MARSHAL_FAILED("Price")
    if (root) cJSON_Delete(root);
	return NULL;
}
static void Account_unmarshal(struct Account* dst, char* data, error_t* err);
static void Book_unmarshal(struct Book* dst, char* data, error_t* err);
static void Price_unmarshal(struct Price* dst, char* data, error_t* err);

void Account_unmarshal(struct Account* dst, char* data, error_t* err) {
    cJSON* root = NULL;
    cJSON* item = NULL;

    root = cJSON_Parse(data);
    if (!root) goto bad;
    item = cJSON_GetObjectItemCaseSensitive(root, "userName");
	if (!item || !cJSON_IsString(item)) goto bad;
	dst->userName = strdup(cJSON_GetStringValue(item));
    item = cJSON_GetObjectItemCaseSensitive(root, "balance");
	if (!item || !cJSON_IsNumber(item)) goto bad;
	dst->balance = (float)item->valuedouble;
    cJSON_Delete(root);
    return;
bad:
    if (!err->null) UNMARSHAL_FAILED("Account");
    if (root) cJSON_Delete(root);
}

void Book_unmarshal(struct Book* dst, char* data, error_t* err) {
    cJSON* root = NULL;
    cJSON* item = NULL;

    root = cJSON_Parse(data);
    if (!root) goto bad;
    item = cJSON_GetObjectItemCaseSensitive(root, "price");
    if (cJSON_IsNull(item))
        dst->price = NULL;
    else {
		if (!item || !cJSON_IsObject(item)) goto bad;
    	data = cJSON_Print(item);
		Price_unmarshal(dst->price, data, err);
		if (!err->null) goto bad;
    }
    item = cJSON_GetObjectItemCaseSensitive(root, "name");
	if (!item || !cJSON_IsString(item)) goto bad;
	dst->name = strdup(cJSON_GetStringValue(item));
    cJSON_Delete(root);
    return;
bad:
    if (!err->null) UNMARSHAL_FAILED("Book");
    if (root) cJSON_Delete(root);
}

void Price_unmarshal(struct Price* dst, char* data, error_t* err) {
    cJSON* root = NULL;
    cJSON* item = NULL;

    root = cJSON_Parse(data);
    if (!root) goto bad;
    item = cJSON_GetObjectItemCaseSensitive(root, "unit");
	if (!item || !cJSON_IsString(item)) goto bad;
	dst->unit = strdup(cJSON_GetStringValue(item));
    item = cJSON_GetObjectItemCaseSensitive(root, "num");
	if (!item || !cJSON_IsNumber(item)) goto bad;
	dst->num = (float)item->valuedouble;
    cJSON_Delete(root);
    return;
bad:
    if (!err->null) UNMARSHAL_FAILED("Price");
    if (root) cJSON_Delete(root);
}

void bookMarket_getBook_handler(request_t* req, error_t* err, struct argument* resp) {
	char* arg1;
	struct Book* res = NULL;
	cJSON* root = NULL;

	CHECK_ARG_CNT(1, req->argcnt)
	CHECK_ARG_TYPE("string", req->args[0].type_name)
	
	arg1 = req->args[0].data;
	res = bookMarket_getBook(arg1, err);
	if (!err->null) goto end;
	root = Book_marshal(res, err);
    if (!err->null) goto end;
    char* data = cJSON_Print(root);
    build_resp(resp, 2, "Book", strlen(data), data);
end:
	if (res) Book_destroy(res);
	free(res);
	if (root) cJSON_Delete(root);
	return;
}

void bookMarket_consume_handler(request_t* req, error_t* err, struct argument* resp) {
	struct Book arg1;
	struct Account arg2;
	struct Account* res = NULL;
	cJSON* root = NULL;

	CHECK_ARG_CNT(2, req->argcnt)
	CHECK_ARG_TYPE("Book", req->args[0].type_name)
	
	CHECK_ARG_TYPE("Account", req->args[1].type_name)
	
	Book_init(&arg1);
	Account_init(&arg2);
	Book_unmarshal(&arg1, req->args[0].data, err);
	if (!err->null) goto end;
	Account_unmarshal(&arg2, req->args[1].data, err);
	if (!err->null) goto end;
	res = bookMarket_consume(&arg1, &arg2, err);
	if (!err->null) goto end;
	root = Account_marshal(res, err);
    if (!err->null) goto end;
    char* data = cJSON_Print(root);
    build_resp(resp, 2, "Account", strlen(data), data);
end:
	Book_destroy(&arg1);
	Account_destroy(&arg2);
	if (res) Account_destroy(res);
	free(res);
	if (root) cJSON_Delete(root);
	return;
}

void bookMarket_updateBookPrice_handler(request_t* req, error_t* err, struct argument* resp) {
	char* arg1;
	struct Price arg2;

	CHECK_ARG_CNT(2, req->argcnt)
	CHECK_ARG_TYPE("string", req->args[0].type_name)
	
	CHECK_ARG_TYPE("Price", req->args[1].type_name)
	
	Price_init(&arg2);
	arg1 = req->args[0].data;
	Price_unmarshal(&arg2, req->args[1].data, err);
	if (!err->null) goto end;
	bookMarket_updateBookPrice(arg1, &arg2, err);
	if (!err->null) goto end;
	build_resp(resp, 4, "", 0, NULL);
end:
	Price_destroy(&arg2);
	return;
}

void bookMarket_shelve_handler(request_t* req, error_t* err, struct argument* resp) {
	struct Book arg1;

	CHECK_ARG_CNT(1, req->argcnt)
	CHECK_ARG_TYPE("Book", req->args[0].type_name)
	
	Book_init(&arg1);
	Book_unmarshal(&arg1, req->args[0].data, err);
	if (!err->null) goto end;
	bookMarket_shelve(&arg1, err);
	if (!err->null) goto end;
	build_resp(resp, 4, "", 0, NULL);
end:
	Book_destroy(&arg1);
	return;
}


void register_bookMarket_service(server_t* svr) {
	server_register(svr, "bookMarket.getBook", bookMarket_getBook_handler);
	server_register(svr, "bookMarket.consume", bookMarket_consume_handler);
	server_register(svr, "bookMarket.updateBookPrice", bookMarket_updateBookPrice_handler);
	server_register(svr, "bookMarket.shelve", bookMarket_shelve_handler);
}