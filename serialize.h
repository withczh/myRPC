#ifndef __SERIALIZE__H__
#define __SERIALIZE__H__

#define SERIALIZE_BUFFER_DEFAULT_SIZE 512

#define ADD_ID 1
#define SUB_ID 2
#define MULTIPLY_ID 3 
#define DIV_ID 4

//序列化数据缓冲区
typedef struct {
    void *buf;
    int size;
    int next;
} serialized_buffer_t;

/*标识*/
typedef struct {
    unsigned int rpc_id;
    unsigned int pay_load_size;
}rpc_hdr_t;

/*初始化序列缓冲区*/
void init_serialized_buffer(serialized_buffer_t **sbuf);
void init_serialized_buffer_of_defined_size(serialized_buffer_t **sbuf,int sizes);

/****序列化函数接口****/
int  get_serialize_buffer_size(serialized_buffer_t *sbuf);
int get_serialize_buffer_length(serialized_buffer_t *sbuf);
int get_serialize_buffer_current_ptr_offset(serialized_buffer_t *sbuf);
char *get_serialize_buffer_current_ptr(serialized_buffer_t *sbuf);
void serialize_buffer_skip(serialized_buffer_t *sbuf, int size);
void copy_in_serialized_buffer_by_offset(serialized_buffer_t *sbuf, int size, char *value, int offset);

void serialize_data(serialized_buffer_t *sbuf, char *data, int val_size);
void de_serialize_data(char *dest, serialized_buffer_t *sbuf, int val_size);

/* 释放资源*/
void free_serialize_buffer(serialized_buffer_t *sbuf);

/*重置*/
void truncate_serialize_buffer(serialized_buffer_t **sbuf);
void reset_serialize_buffer(serialized_buffer_t *sbuf);
/* 信息*/
void print_buffer_details(serialized_buffer_t *sbuf, const char *fn, int lineno);

#endif