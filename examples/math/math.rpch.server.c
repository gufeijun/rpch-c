// This is code generated by hgen. DO NOT EDIT!!!
// hgen version: v0.1.7
// source: math.gfj

#include "math.rpch.server.h"

#include <stdint.h>
#include <stdlib.h>
#include <string.h>

#include "argument.h"
#include "cJSON.h"
#include "error.h"
#include "request.h"
#include "server.h"

static inline __attribute__((always_inline)) void Quotient_init(struct Quotient*);
static inline __attribute__((always_inline)) void Quotient_destroy(struct Quotient*);
static inline __attribute__((always_inline)) void TwoNum_init(struct TwoNum*);
static inline __attribute__((always_inline)) void TwoNum_destroy(struct TwoNum*);

void Quotient_init(struct Quotient* data) {}
void Quotient_destroy(struct Quotient* data) {}
struct Quotient* Quotient_create() {
	struct Quotient* v = malloc(sizeof(struct Quotient));
	Quotient_init(v);
	return v;
}
void TwoNum_init(struct TwoNum* data) {}
void TwoNum_destroy(struct TwoNum* data) {}
struct TwoNum* TwoNum_create() {
	struct TwoNum* v = malloc(sizeof(struct TwoNum));
	TwoNum_init(v);
	return v;
}
struct Quotient* Quotient_clone(struct Quotient* src) {
	if (src == NULL) return NULL;
	struct Quotient* dst = malloc(sizeof(struct Quotient));		
	dst->Quo = src->Quo;		
	dst->Rem = src->Rem;
	return dst;
}
struct TwoNum* TwoNum_clone(struct TwoNum* src) {
	if (src == NULL) return NULL;
	struct TwoNum* dst = malloc(sizeof(struct TwoNum));		
	dst->A = src->A;		
	dst->B = src->B;
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

static cJSON* Quotient_marshal(struct Quotient* arg, error_t* err);
static cJSON* TwoNum_marshal(struct TwoNum* arg, error_t* err);

cJSON* Quotient_marshal(struct Quotient* data, error_t* err) {
	cJSON* root = NULL;
	
	if (data == NULL) goto bad;
    root = cJSON_CreateObject();
    if (!root) goto bad;
    if (cJSON_AddNumberToObject(root, "Quo", (double)data->Quo) == NULL) goto bad;
    if (cJSON_AddNumberToObject(root, "Rem", (double)data->Rem) == NULL) goto bad;
	return root;
bad:
	if (!err->null) MARSHAL_FAILED("Quotient")
    if (root) cJSON_Delete(root);
	return NULL;
}

cJSON* TwoNum_marshal(struct TwoNum* data, error_t* err) {
	cJSON* root = NULL;
	
	if (data == NULL) goto bad;
    root = cJSON_CreateObject();
    if (!root) goto bad;
    if (cJSON_AddNumberToObject(root, "A", (double)data->A) == NULL) goto bad;
    if (cJSON_AddNumberToObject(root, "B", (double)data->B) == NULL) goto bad;
	return root;
bad:
	if (!err->null) MARSHAL_FAILED("TwoNum")
    if (root) cJSON_Delete(root);
	return NULL;
}
static void Quotient_unmarshal(struct Quotient* dst, char* data, error_t* err);
static void TwoNum_unmarshal(struct TwoNum* dst, char* data, error_t* err);

void Quotient_unmarshal(struct Quotient* dst, char* data, error_t* err) {
    cJSON* root = NULL;
    cJSON* item = NULL;

    root = cJSON_Parse(data);
    if (!root) goto bad;
    item = cJSON_GetObjectItemCaseSensitive(root, "Quo");
	if (!item || !cJSON_IsNumber(item)) goto bad;
	dst->Quo = (uint64_t)item->valueint;
    item = cJSON_GetObjectItemCaseSensitive(root, "Rem");
	if (!item || !cJSON_IsNumber(item)) goto bad;
	dst->Rem = (uint64_t)item->valueint;
    cJSON_Delete(root);
    return;
bad:
    if (!err->null) UNMARSHAL_FAILED("Quotient");
    if (root) cJSON_Delete(root);
}

void TwoNum_unmarshal(struct TwoNum* dst, char* data, error_t* err) {
    cJSON* root = NULL;
    cJSON* item = NULL;

    root = cJSON_Parse(data);
    if (!root) goto bad;
    item = cJSON_GetObjectItemCaseSensitive(root, "A");
	if (!item || !cJSON_IsNumber(item)) goto bad;
	dst->A = (int32_t)item->valueint;
    item = cJSON_GetObjectItemCaseSensitive(root, "B");
	if (!item || !cJSON_IsNumber(item)) goto bad;
	dst->B = (int32_t)item->valueint;
    cJSON_Delete(root);
    return;
bad:
    if (!err->null) UNMARSHAL_FAILED("TwoNum");
    if (root) cJSON_Delete(root);
}

void Math_Add_handler(request_t* req, error_t* err, struct argument* resp) {
	uint32_t arg1;
	uint32_t arg2;
	uint32_t res;

	CHECK_ARG_CNT(2, req->argcnt)
	CHECK_ARG_TYPE("uint32", req->args[0].type_name)
	CHECK_ARG_SIZE("uint32", 4, req->args[0].data_len)
	CHECK_ARG_TYPE("uint32", req->args[1].type_name)
	CHECK_ARG_SIZE("uint32", 4, req->args[1].data_len)
	arg1 = *(uint32_t*)req->args[0].data;
	arg2 = *(uint32_t*)req->args[1].data;
	res = Math_Add(arg1, arg2, err);
	if (!err->null) goto end;
	build_resp(resp, 0, "uint32", 4, (char*)&res);
end:
	return;
}

void Math_Sub_handler(request_t* req, error_t* err, struct argument* resp) {
	int32_t arg1;
	int32_t arg2;
	int32_t res;

	CHECK_ARG_CNT(2, req->argcnt)
	CHECK_ARG_TYPE("int32", req->args[0].type_name)
	CHECK_ARG_SIZE("int32", 4, req->args[0].data_len)
	CHECK_ARG_TYPE("int32", req->args[1].type_name)
	CHECK_ARG_SIZE("int32", 4, req->args[1].data_len)
	arg1 = *(int32_t*)req->args[0].data;
	arg2 = *(int32_t*)req->args[1].data;
	res = Math_Sub(arg1, arg2, err);
	if (!err->null) goto end;
	build_resp(resp, 0, "int32", 4, (char*)&res);
end:
	return;
}

void Math_Multiply_handler(request_t* req, error_t* err, struct argument* resp) {
	struct TwoNum arg1;
	int32_t res;

	CHECK_ARG_CNT(1, req->argcnt)
	CHECK_ARG_TYPE("TwoNum", req->args[0].type_name)
	
	TwoNum_init(&arg1);
	TwoNum_unmarshal(&arg1, req->args[0].data, err);
	if (!err->null) goto end;
	res = Math_Multiply(&arg1, err);
	if (!err->null) goto end;
	build_resp(resp, 0, "int32", 4, (char*)&res);
end:
	TwoNum_destroy(&arg1);
	return;
}

void Math_Divide_handler(request_t* req, error_t* err, struct argument* resp) {
	uint64_t arg1;
	uint64_t arg2;
	struct Quotient* res = NULL;
	cJSON* root = NULL;

	CHECK_ARG_CNT(2, req->argcnt)
	CHECK_ARG_TYPE("uint64", req->args[0].type_name)
	CHECK_ARG_SIZE("uint64", 8, req->args[0].data_len)
	CHECK_ARG_TYPE("uint64", req->args[1].type_name)
	CHECK_ARG_SIZE("uint64", 8, req->args[1].data_len)
	arg1 = *(uint64_t*)req->args[0].data;
	arg2 = *(uint64_t*)req->args[1].data;
	res = Math_Divide(arg1, arg2, err);
	if (!err->null) goto end;
	root = Quotient_marshal(res, err);
    if (!err->null) goto end;
    char* data = cJSON_Print(root);
    build_resp(resp, 2, "Quotient", strlen(data), data);
end:
	if (res) Quotient_destroy(res);
	free(res);
	if (root) cJSON_Delete(root);
	return;
}


void register_Math_service(server_t* svr) {
	server_register(svr, "Math.Add", Math_Add_handler);
	server_register(svr, "Math.Sub", Math_Sub_handler);
	server_register(svr, "Math.Multiply", Math_Multiply_handler);
	server_register(svr, "Math.Divide", Math_Divide_handler);
}