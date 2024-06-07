#include "cerde.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

str_builder str_builder_init(u32 cap)
{
    str_builder result;
    result.data = malloc(cap);
    result.cap = cap;
    result.len = 0;
    return result;
}

void str_append(str_builder* a, str b)
{
    if (a->len + b.len >= a->cap) {
        a->cap *= 1.5;
        a->data = realloc(a->data, a->cap);
    }
    memcpy(a->data + a->len, b.data, b.len);
    a->len += b.len;
}

void str_append_c(str_builder* a, char* b, u32 b_len)
{
    if (b_len == 0) b_len = strlen(b);
    if (a->len + b_len >= a->cap) {
        a->cap *= 1.5;
        a->data = realloc(a->data, a->cap);
    }
    memcpy(a->data + a->len, b, b_len);
    a->len += b_len;
}

void str_append_char(str_builder* a, char b)
{
    if (a->len + 1 == a->cap) {
        a->cap *= 1.5;
        a->data = realloc(a->data, a->cap);
    }
    a->data[a->len++] = b;
}

str str_builder_finalize(str_builder* b)
{
    str result;
    result.data = realloc(b->data, b->len+1);
    result.data[b->len] = 0;
    result.len = b->len;
    return result;
}

// TODO:
str str_concat( str a, str b);
str str_concat_c(str a, char* b, u32 b_len);
str char_concat_s(char* a, u32 a_len, str b);

cd_arena arena_init(u32 bucket_size)
{
    cd_arena result;
    result.bucket_count = 1;
    result.bucket_size = bucket_size;
    result.buckets = malloc(sizeof(cd_arena_bucket) * 2);
    cd_arena_bucket* b = result.buckets;
    b->data = malloc(bucket_size);
    b->cur = b->data;
    b->last_alloc_size = 0;
    return result;
}

void* arena_alloc(cd_arena* a, u32 size)
{
    cd_arena_bucket* b = &a->buckets[a->bucket_count-1];
    if (b->cur + size >= b->data + a->bucket_size) {
        // add new bucket
        a->bucket_count++;
        a->buckets = realloc(a->buckets, sizeof(cd_arena_bucket) * a->bucket_count);
        b = &a->buckets[a->bucket_count-1];
        b->data = malloc(a->bucket_size);
        b->cur = b->data;
    }
    void* result = b->cur;
    b->cur += size;
    b->last_alloc_size = size;
    return result;
}

void arena_free_last(cd_arena* a)
{
    cd_arena_bucket* b = &a->buckets[a->bucket_count-1];
    b->cur -= b->last_alloc_size;
    b->last_alloc_size = 0;
}

void arena_reset(cd_arena* a)
{
    for (int i = 1; i < a->bucket_count; i++) {
        cd_arena_bucket* b = &a->buckets[i];
        free(b->data);
    }
    a->buckets = realloc(a->buckets, sizeof(cd_arena_bucket));
    a->bucket_count = 1;
    a->buckets[0].cur = a->buckets[0].data;
    a->buckets[0].last_alloc_size = 0;
}

void arena_destroy(cd_arena* a)
{
    for (int i = 0; i < a->bucket_count; i++) {
        cd_arena_bucket* b = &a->buckets[i];
        free(b->data);
    }
    free(a->buckets);
}

// ==== MAIN ====
cd_arena cd_main_arena;

void cd_init()
{
    cd_main_arena = arena_init(20000);
}

void cd_add_char(cd_val* root, str key, char* value, u32 len)
{
    if (root->kind != CD_VALUE_ARRAY && root->kind != CD_VALUE_OBJECT) { return; }
    
    if (len == 0) {
        len = strlen(value);
    }
    cd_val* v = arena_alloc(&cd_main_arena, sizeof(cd_val));

    v->key = key;
    v->kind = CD_VALUE_STRING; 
    v->string = make_str(value, len);

    if (root->child_count > 0) {
        root->children[root->child_count-1].next = v;
    } else {
        root->children = v;
    }
    root->child_count++;
}

#define cd_add_type_impl(type, enum_name, field_name) \
    void cd_add_##type(cd_val* root, str key, type value) { \
        if (root->kind != CD_VALUE_ARRAY && root->kind != CD_VALUE_OBJECT) { return; } \
        cd_val* v = arena_alloc(&cd_main_arena, sizeof(cd_val)); \
        v->key = key; \
        v->kind = enum_name; \
        v->field_name = value; \
        v->next = null; \
        if (root->child_count > 0) { \
            root->children[root->child_count-1].next = v; \
        } else { \
            root->children = v; \
        } \
        root->child_count++; \
    }

cd_add_type_impl(double, CD_VALUE_DOUBLE, number);
cd_add_type_impl(bool, CD_VALUE_BOOL, boolean);
cd_add_type_impl(str, CD_VALUE_STRING, string);

#define cd_add_val_impl(name, enum_type) \
    cd_val* cd_add_##name(cd_val* root, str key) { \
        if (root) { \
            if (root->kind != CD_VALUE_ARRAY && root->kind != CD_VALUE_OBJECT) { return null; } \
        } \
        cd_val* result = arena_alloc(&cd_main_arena, sizeof(cd_val)); \
        result->key = key; \
        result->kind = enum_type; \
        result->child_count = 0; result->children = null; \
        result->next = null;\
        if (root) { \
            if (root->child_count > 0) { \
                root->children[root->child_count-1].next = result; \
            } else { \
                root->children = result; \
            } \
            root->child_count++; \
        } \
        return result;\
    }

cd_add_val_impl(obj, CD_VALUE_OBJECT);
cd_add_val_impl(array, CD_VALUE_ARRAY);

void cd_serialize_double(str_builder* result, double value)
{
    char buf[50];
    int len = snprintf(buf, 50, "%lf", value);
    char* end = &buf[len-1];
    while (*end == '0') end--;
    if (*end == '.') end--;
    len = (u64)end - (u64)buf+1;
    buf[len] = 0;
    str_append_c(result, buf, len);
}

void cd_serialize_val(cd_val* val, str_builder* result, bool print_keys) 
{
    if (print_keys) {
        str_append_char(result, '\"');
        str_append(result, val->key); 
        str_append_char(result, '\"');
        str_append_char(result, ':');
    }
    switch (val->kind) {
        case CD_VALUE_BOOL: {
            if (val->boolean) {
                str_append(result, make_sstr("true"));
            } else {
                str_append(result, make_sstr("false"));
            }
        } break;
        case CD_VALUE_STRING: {
            str_append_char(result, '\"');
            str_append(result, val->string);
            str_append_char(result, '\"');
        } break;
        case CD_VALUE_NULL: {
            str_append(result, make_sstr("null"));
        } break;
        case CD_VALUE_DOUBLE: {
            cd_serialize_double(result, val->number);
        } break;
        case CD_VALUE_ARRAY: {
            str_append_char(result, '[');
            if (val->children) {
                cd_val* cur = val->children;
                cd_serialize_val(cur, result, false);
                while (cur->next) {
                    cur = cur->next;
                    str_append_char(result, ',');
                    cd_serialize_val(cur, result, false);
                }
            }
            str_append_char(result, ']');
        } break;
        case CD_VALUE_OBJECT: {
            str_append_char(result, '{');
            if (val->children) {
                cd_val* cur = val->children;
                cd_serialize_val(cur, result, true);
                while (cur->next) {
                    cur = cur->next;
                    str_append_char(result, ',');
                    cd_serialize_val(cur, result, true);
                }
            }
            str_append_char(result, '}');
        } break;
    }
}

str cd_serialize(cd_val* root)
{
    str_builder result = str_builder_init(100);
    cd_serialize_val(root, &result, false);
    return str_builder_finalize(&result);
}