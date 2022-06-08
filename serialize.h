#ifndef __SERIALIZE__H__
#define __SERIALIZE__H__

#define SERIALIZE_BUFFER_DEFAULT_SIZE 512
typedef struct {
    void *buf;
    int size;
    int next;
} serialized_buffer_t;

#endif