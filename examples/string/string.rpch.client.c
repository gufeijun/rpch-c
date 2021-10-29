// This is code generated by hgen. DO NOT EDIT!!!
// hgen version: v0.1.7
// source: string.gfj

#include "string.rpch.client.h"

#include <stdint.h>
#include <string.h>
#include <stdlib.h>

#include "argument.h"
#include "cJSON.h"
#include "error.h"
#include "client.h"



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


char* string_toupper(char* arg1, client_t* client) {
	int free_data = 1;
    struct client_request req;
    struct argument resp;
	error_t* err = &client->err;char* v = NULL;

	client_request_init(&req, "string", "toupper", 1);
	arg1 = arg1 == NULL ? "" : arg1;
	argument_init_with_option(req.args + 0, 0, "string", arg1, strlen(arg1));
	client_call(client, &req, &resp);
    if (!client->err.null) goto end;
	CHECK_ARG_TYPE("string", resp.type_name)
	v = resp.data;
	free_data = 0;
end:
    if (resp.data && free_data) free(resp.data);
    if (resp.type_name) free(resp.type_name);
    return v;
}

char* string_tolower(char* arg1, client_t* client) {
	int free_data = 1;
    struct client_request req;
    struct argument resp;
	error_t* err = &client->err;char* v = NULL;

	client_request_init(&req, "string", "tolower", 1);
	arg1 = arg1 == NULL ? "" : arg1;
	argument_init_with_option(req.args + 0, 0, "string", arg1, strlen(arg1));
	client_call(client, &req, &resp);
    if (!client->err.null) goto end;
	CHECK_ARG_TYPE("string", resp.type_name)
	v = resp.data;
	free_data = 0;
end:
    if (resp.data && free_data) free(resp.data);
    if (resp.type_name) free(resp.type_name);
    return v;
}

char* string_concat(char* arg1, char* arg2, client_t* client) {
	int free_data = 1;
    struct client_request req;
    struct argument resp;
	error_t* err = &client->err;char* v = NULL;

	client_request_init(&req, "string", "concat", 2);
	arg1 = arg1 == NULL ? "" : arg1;
	argument_init_with_option(req.args + 0, 0, "string", arg1, strlen(arg1));
	arg2 = arg2 == NULL ? "" : arg2;
	argument_init_with_option(req.args + 1, 0, "string", arg2, strlen(arg2));
	client_call(client, &req, &resp);
    if (!client->err.null) goto end;
	CHECK_ARG_TYPE("string", resp.type_name)
	v = resp.data;
	free_data = 0;
end:
    if (resp.data && free_data) free(resp.data);
    if (resp.type_name) free(resp.type_name);
    return v;
}

int32_t string_atoi(char* arg1, client_t* client) {
	int free_data = 1;
    struct client_request req;
    struct argument resp;
	error_t* err = &client->err;int32_t v = 0;

	client_request_init(&req, "string", "atoi", 1);
	arg1 = arg1 == NULL ? "" : arg1;
	argument_init_with_option(req.args + 0, 0, "string", arg1, strlen(arg1));
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
