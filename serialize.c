#include "common.h"
#include "serialize.h"

void init_serialized_buffer(serialized_buffer_t **sbuf)
{   
    (*sbuf)=(serialized_buffer_t*)malloc(sizeof(serialized_buffer_t));
    (*sbuf)->buf=calloc(1,SERIALIZE_BUFFER_DEFAULT_SIZE);
    (*sbuf)->next=0;
    (*sbuf)->size=SERIALIZE_BUFFER_DEFAULT_SIZE;
}

void init_serialized_buffer_of_defined_size(serialized_buffer_t **sbuf,int sizes)
{
    (*sbuf)=(serialized_buffer_t*)malloc(sizeof(serialized_buffer_t));
    (*sbuf)->buf=calloc(1,SERIALIZE_BUFFER_DEFAULT_SIZE);
    (*sbuf)->next=0;
    (*sbuf)->size=sizes;
}

int  get_serialize_buffer_size(serialized_buffer_t *sbuf)
{
    return sbuf->next;
}

int get_serialize_buffer_length(serialized_buffer_t *sbuf)
{
    return sbuf->size;
}

int get_serialize_buffer_current_ptr_offset(serialized_buffer_t *sbuf)
{
    if(!sbuf) return -1;
    return sbuf->next;
}

char *get_serialize_buffer_current_ptr(serialized_buffer_t *sbuf)
{
    if(!sbuf) return NULL;
    return sbuf->buf+sbuf->next;
}

void serialize_buffer_skip(serialized_buffer_t *sbuf, int size)
{
    int available_size=sbuf->size-sbuf->next;
    if(available_size>=size)
    {
        sbuf->next+=size;
        return;
    }

    while(available_size<size)
    {
        sbuf->size=sbuf->size*2;
        available_size=sbuf->size-sbuf->next;
    }

    sbuf->buf=realloc(sbuf->buf,sbuf->size);
    sbuf->next+=size;
}

void copy_in_serialized_buffer_by_offset(serialized_buffer_t *sbuf, int size, char *value, int offset)
{
    if(offset>sbuf->size)
    {
        printf("%s():error:out of boundaries\n",__FUNCTION__);
    }

    memcpy(sbuf->buf+offset,value,size);
}

//序列化数据
void serialize_data(serialized_buffer_t *sbuf, char *data, int val_size)
{
    if (sbuf == NULL) assert(0);

    int available_size = sbuf->size - sbuf->next;
    char isResize = 0;

    while(available_size < val_size){
        sbuf->size = sbuf->size * 2;
        available_size = sbuf->size - sbuf->next;
        isResize = 1;
    }

    if(isResize == 0){
        memcpy((char *)sbuf->buf + sbuf->next, data, val_size);
        sbuf->next += val_size;
        return;
    }

    // resize of the buffer
    sbuf->buf = realloc(sbuf->buf, sbuf->size); 
    memcpy((char *)sbuf->buf + sbuf->next, data, val_size);
    sbuf->next += val_size;
    return;
}

//反序列化数据
void de_serialize_data(char *dest, serialized_buffer_t *sbuf, int val_size)
{
	if(!sbuf || !sbuf->buf) assert(0);
	if(!val_size) return;	
	if((sbuf->size - sbuf->next)< val_size) assert(0);	 

	memcpy(dest, sbuf->buf + sbuf->next, val_size);
	sbuf->next += val_size;
}

void free_serialize_buffer(serialized_buffer_t *sbuf)
{
    free(sbuf->buf);
    free(sbuf);
}

void truncate_serialize_buffer(serialized_buffer_t **sbuf)
{
	serialized_buffer_t *clone = NULL;

	if((*sbuf)->next == (*sbuf)->size){
		return;
	}
	init_serialized_buffer_of_defined_size(&clone, (*sbuf)->next);
	memcpy(clone->buf, (*sbuf)->buf, (*sbuf)->next);
	clone->next = clone->size;
	free_serialize_buffer(*sbuf);
	*sbuf = clone;
}

void reset_serialize_buffer(serialized_buffer_t *sbuf)
{
    sbuf->next=0;
}

void print_buffer_details(serialized_buffer_t *sbuf, const char *fn, int lineno)
{
    printf("%s():%d : starting address = %p\n", fn, lineno, sbuf);
    printf("size = %d\n", sbuf->size);
    printf("next = %d\n", sbuf->next);
}