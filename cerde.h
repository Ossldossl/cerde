#pragma once

#include <stdbool.h>

typedef signed char        i8;
typedef short              i16;
typedef int                i32;
typedef long long          i64;
typedef unsigned char      u8;
typedef unsigned short     u16;
typedef unsigned int       u32;
typedef unsigned long long u64;

#define null NULL
#define max(a, b) (a>b?a:b)
#define min(a, b) (a<b?a:b)
#define for_in(i, to) for(int _i##to = i; i < to; i++)
#define for_to(i, to) for(int i=0;i<to;i++)

#define array_of(type) array

//==== STR ====
typedef struct {
    char* data;
    u8 len;
} str8;

typedef struct {
    char* data;
    u16 len;
} str16;

typedef struct {
    char* data;
    u32 len;
} str;

#define make_str(data_, len_) (str){.data=data_,.len=len_}
#define make_sstr(data_) (str){.data=data_,.len=sizeof(data_)-1}

typedef struct {
    char* data;
    u32 len;
    u32 cap;
} str_builder;

str_builder str_builder_init(u32 cap);
void str_append(str_builder* a, str b);
void str_append_c(str_builder* a, char* b, u32 b_len);
void str_append_char(str_builder* a, char b);
str str_builder_finalize(str_builder* b);
str str_concat(str a, str b);
str str_concat_c(str a, char* b, u32 b_len);
str char_concat_s(char* a, u32 a_len, str b);

// TODO: implement allocators
// TODO: implement strings
// TODO: implement arrays
// TODO: implement serialization
// TODO: implement deserialization

// ==== ARENA ====
typedef struct cd_arena_bucket {
    char* data;
    char* cur;
    u32 last_alloc_size;
} cd_arena_bucket;

typedef struct {
    cd_arena_bucket* buckets;
    u32 bucket_size;
    u8 bucket_count;
} cd_arena;

cd_arena arena_init(u32 bucket_size);
void* arena_alloc(cd_arena* a, u32 size);
void arena_free_last(cd_arena* a);
void arena_reset(cd_arena* a);
void arena_destroy(cd_arena* a);

// ==== ARRAY ====
typedef struct {
    void* data;
    u32 cap;
    u16 used;
    u16 element_size;
} array;

// TODO: implement arrays
array array_init(u16 element_size, u16 reserve_amount);
void* array_push(array* a);
void* array_pop(array* a);
void* array_get(array* a, u16 index);
void* array_destroy(array* a);

// ==== JSON ====
typedef enum {
    CD_VALUE_STRING,
    CD_VALUE_DOUBLE,
    CD_VALUE_BOOL,
    CD_VALUE_NULL,
    CD_VALUE_OBJECT,
    CD_VALUE_ARRAY,
} cd_val_kind;

typedef struct cd_val {
    cd_val_kind kind;
    str key;
    union {
        str string;
        double number;
        bool boolean;
        struct {
            struct cd_val* children;
            u32 child_count;
        };
    };
    struct cd_val* next;
} cd_val;

void cd_init();

// Serialization
#define cd_add_for_type(type) void cd_add_##type(cd_val* root, str key, type value);
cd_add_for_type(bool);
cd_add_for_type(double);
cd_add_for_type(str);

void cd_add_char(cd_val* root, str key, char* value, u32 len);
//void cd_add_val(cd_val* root, str key, cd_val value);
cd_val* cd_add_obj(cd_val* root, str key);
cd_val* cd_add_array(cd_val* root, str key);

#define cd_add(root, key, value) _Generic((value),  \
                                        _Bool: cd_add_bool, \
                                        int: cd_add_double, \
                                        float: cd_add_double, \
                                        double: cd_add_double, \
                                        char*: cd_add_char, \
                                        str: cd_add_str)(root, key, value)

#define cd_add_element(root, value) cd_add(root, make_sstr(""), value)

str cd_serialize(cd_val* root);