// This is code generated by hgen. DO NOT EDIT!!!
// hgen version: v0.1.7
// source: math.gfj

#include "math.rpch.client.h"

#include <stdint.h>
#include <string.h>
#include <stdlib.h>

#include "argument.h"
#include "cJSON.h"
#include "error.h"
#include "client.h"

static inline __attribute__((always_inline)) void Quotient_init(struct Quotient*);
static inline __attribute__((always_inline)) void Quotient_destroy(struct Quotient*);
static inline __attribute__((always_inline)) void TwoNum_init(struct TwoNum*);
static inline __attribute__((always_inline)) void TwoNum_destroy(struct TwoNum*);

void Quotient_init(struct Quotient* data) {}
void Quotient_destroy(struct Quotient* data) {}
void Quotient_delete(struct Quotient* arg) {
	Quotient_destroy(arg);
	free(arg);
}
void TwoNum_init(struct TwoNum* data) {}
void TwoNum_destroy(struct TwoNum* data) {}
void TwoNum_delete(struct TwoNum* arg) {
	TwoNum_destroy(arg);
	free(arg);
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
			goto end;                                \
        }                                          \
    }
#define CHECK_ARG_TYPE(want, got)         \
    {                                     \
        if (strcmp(want, got) != 0) {     \
            invalid_type(err, want, got); \
			goto end;                       \
        }                                 \
    }
#define CHECK_ARG_SIZE(t, want, got)              \
    {                                             \
        if (want != got) {                        \
            invalid_type_size(err, t, want, got); \
			goto end;                               \
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

uint32_t Math_Add(uint32_t arg1, uint32_t arg2, client_t* client) {
	int free_data = 1;
    struct client_request req;
    struct argument resp;
	error_t* err = &client->err;uint32_t v = 0;

	client_request_init(&req, "Math", "Add", 2);
	argument_init_with_option(req.args + 0, 0, "uint32", &arg1, 4);
	argument_init_with_option(req.args + 1, 0, "uint32", &arg2, 4);
	client_call(client, &req, &resp);
    if (!client->err.null) goto end;
	CHECK_ARG_TYPE("uint32", resp.type_name)
	CHECK_ARG_SIZE("uint32", 4, resp.data_len)
	memcpy(&v, resp.data, 4);
end:
    if (resp.data && free_data) free(resp.data);
    if (resp.type_name) free(resp.type_name);
    return v;
}

int32_t Math_Sub(int32_t arg1, int32_t arg2, client_t* client) {
	int free_data = 1;
    struct client_request req;
    struct argument resp;
	error_t* err = &client->err;int32_t v = 0;

	client_request_init(&req, "Math", "Sub", 2);
	argument_init_with_option(req.args + 0, 0, "int32", &arg1, 4);
	argument_init_with_option(req.args + 1, 0, "int32", &arg2, 4);
	client_call(client, &req, &resp);
    if (!client->err.null) goto end;
	CHECK_ARG_TYPE("int32", resp.type_name)
	CHECK_ARG_SIZE("int32", 4, resp.data_len)
	memcpy(&v, resp.data, 4);
end:
    if (resp.data && free_data) free(resp.data);
    if (resp.type_name) free(resp.type_name);
    return v;
}

int32_t Math_Multiply(struct TwoNum* arg1, client_t* client) {
	int free_data = 1;
    struct client_request req;
    struct argument resp;
	error_t* err = &client->err;
	char* data = NULL;
    cJSON* node1 = NULL;int32_t v = 0;

	client_request_init(&req, "Math", "Multiply", 1);
	node1 = TwoNum_marshal(arg1, &client->err);
	if (client_failed(client)) return v;
	data = cJSON_Print(node1);
	argument_init_with_option(req.args + 0, 2, "TwoNum", data, strlen(data));
	
	client_call(client, &req, &resp);
    if (!client->err.null) goto end;
	CHECK_ARG_TYPE("int32", resp.type_name)
	CHECK_ARG_SIZE("int32", 4, resp.data_len)
	memcpy(&v, resp.data, 4);
end:
    if (resp.data && free_data) free(resp.data);
    if (resp.type_name) free(resp.type_name);
    if (node1) cJSON_Delete(node1);
    return v;
}

struct Quotient* Math_Divide(uint64_t arg1, uint64_t arg2, client_t* client) {
	int free_data = 1;
    struct client_request req;
    struct argument resp;
	error_t* err = &client->err;struct Quotient* v = NULL;

	client_request_init(&req, "Math", "Divide", 2);
	argument_init_with_option(req.args + 0, 0, "uint64", &arg1, 8);
	argument_init_with_option(req.args + 1, 0, "uint64", &arg2, 8);
	client_call(client, &req, &resp);
    if (!client->err.null) goto end;
	CHECK_ARG_TYPE("Quotient", resp.type_name)
	v = malloc(sizeof(struct Quotient));
	Quotient_init(v);
	Quotient_unmarshal(v, resp.data, &client->err);
	
end:
    if (resp.data && free_data) free(resp.data);
    if (resp.type_name) free(resp.type_name);
    return v;
}
