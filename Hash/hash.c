/*
   Copyright (C) Information Equipment co.,LTD
   All rights reserved.
   Code by JaeHyuk Cho <mailto:minzkn@infoeq.com>
   CVSTAG="$Header: /usr/local/mutihost/joinc/modules/moniwiki/data/text/RCS/Code_2fC_2fmz_5fhash,v 1.1 2007/01/09 02:46:10 root Exp root $"
 */

#include <stdio.h> 
#include <stdlib.h> 
#include <string.h> 
#include <unistd.h> 

#define mzapi_export 
#define mzapi_fastcall 

typedef unsigned long t_mzapi_crc32;
typedef void* t_mzapi_vector;
#define t_mzapi_hash_key t_mzapi_crc32 

struct ts_mzapi_hash_node
{
    struct ts_mzapi_hash_node* prev;
    struct ts_mzapi_hash_node* next;
    t_mzapi_hash_key key;
    t_mzapi_vector vector;
};

struct ts_mzapi_hash
{
    int table_count;
    size_t table_size;
    t_mzapi_hash_key seed;
    struct ts_mzapi_hash_node** head;
    struct ts_mzapi_hash_node** tail;
    /* member function */
    t_mzapi_hash_key(mzapi_fastcall* function)(struct ts_mzapi_hash*, const void*, size_t);
    int (mzapi_fastcall* index)(t_mzapi_hash_key, int);
    struct ts_mzapi_hash_node* (mzapi_fastcall* search_by_key)(struct ts_mzapi_hash*, t_mzapi_hash_key);
    struct ts_mzapi_hash_node* (mzapi_fastcall* next_search)(struct ts_mzapi_hash*, struct ts_mzapi_hash_node*);
    struct ts_mzapi_hash_node* (mzapi_fastcall* prev_search)(struct ts_mzapi_hash*, struct ts_mzapi_hash_node*);
    struct ts_mzapi_hash_node* (mzapi_fastcall* add)(struct ts_mzapi_hash*, t_mzapi_hash_key, t_mzapi_vector);
    int (mzapi_fastcall* del)(struct ts_mzapi_hash*, struct ts_mzapi_hash_node*);
    int (mzapi_fastcall* info)(struct ts_mzapi_hash*);
};

static t_mzapi_crc32(mzapi_fastcall __mzhash_crc32__)(t_mzapi_crc32 s_crc32, unsigned char* s_buffer, int s_size);
static t_mzapi_hash_key(mzapi_fastcall __mzhash_function__)(struct ts_mzapi_hash* s_hash, const void* s_data, size_t s_size);
static int (mzapi_fastcall __mzhash_index__)(t_mzapi_hash_key s_key, int s_table_count);
static struct ts_mzapi_hash_node* (mzapi_fastcall __mzhash_search_by_key__)(struct ts_mzapi_hash* s_hash, t_mzapi_hash_key s_key);
static struct ts_mzapi_hash_node* (mzapi_fastcall __mzhash_next_search__)(struct ts_mzapi_hash* s_hash, struct ts_mzapi_hash_node* s_hash_node);
static struct ts_mzapi_hash_node* (mzapi_fastcall __mzhash_prev_search__)(struct ts_mzapi_hash* s_hash, struct ts_mzapi_hash_node* s_hash_node);
static struct ts_mzapi_hash_node* (mzapi_fastcall __mzhash_add__)(struct ts_mzapi_hash* s_hash, t_mzapi_hash_key s_key, t_mzapi_vector s_vector);
static int (mzapi_fastcall __mzhash_del__)(struct ts_mzapi_hash* s_hash, struct ts_mzapi_hash_node* s_hash_node);
static int (mzapi_fastcall __mzhash_info__)(struct ts_mzapi_hash* s_hash);

mzapi_export struct ts_mzapi_hash* (mzapi_fastcall mzapi_open_hash)(int s_table_count);
mzapi_export struct ts_mzapi_hash* (mzapi_fastcall mzapi_close_hash)(struct ts_mzapi_hash* s_hash);

static t_mzapi_crc32(mzapi_fastcall __mzhash_crc32__)(t_mzapi_crc32 s_crc32, unsigned char* s_buffer, int s_size)
{
    static t_mzapi_crc32 sg_crc32_table[1 << 8];
    static int sg_given_crc32_table = 0;
    t_mzapi_crc32 s_value;
    int s_index, s_sub_index;
    if (sg_given_crc32_table == 0)
    { /* make crc32 table */
        sg_given_crc32_table = 1;
        for (s_index = 0; s_index < (sizeof(sg_crc32_table) / sizeof(t_mzapi_crc32)); s_index++)
        {
            s_value = (t_mzapi_crc32)s_index;
            for (s_sub_index = 0; s_sub_index < 8; s_sub_index++)
            {
                if (s_value & 0x00000001ul)s_value = 0xedb88320ul ^ (s_value >> 1);
                else s_value >>= 1;
            }
            sg_crc32_table[s_index] = s_value;
        }
    }
    s_value = s_crc32 ^ ((t_mzapi_crc32)0xfffffffful);
    for (s_index = 0; s_index < s_size; s_index++)s_value = sg_crc32_table[(s_value ^ ((t_mzapi_crc32)s_buffer[s_index])) & ((t_mzapi_crc32)0x000000fful)] ^ (s_value >> 8);
    return(s_value ^ ((t_mzapi_crc32)0xfffffffful));
}

static t_mzapi_hash_key(mzapi_fastcall __mzhash_function__)(struct ts_mzapi_hash* s_hash, const void* s_data, size_t s_size)
{
    return((t_mzapi_hash_key)__mzhash_crc32__((s_hash != ((struct ts_mzapi_hash*)0)) ? s_hash->seed : ((t_mzapi_hash_key)0), (unsigned char*)s_data, s_size));
}

static int (mzapi_fastcall __mzhash_index__)(t_mzapi_hash_key s_key, int s_table_count)
{
    return((int)(s_key % ((t_mzapi_hash_key)s_table_count)));
}

static struct ts_mzapi_hash_node* (mzapi_fastcall __mzhash_search_by_key__)(struct ts_mzapi_hash* s_hash, t_mzapi_hash_key s_key)
{
    struct ts_mzapi_hash_node* s_hash_node;
    if (s_hash == ((struct ts_mzapi_hash*)0))return((struct ts_mzapi_hash_node*)0);
    s_hash_node = s_hash->head[s_hash->index(s_key, s_hash->table_count)];
    while (s_hash_node != ((struct ts_mzapi_hash_node*)0))
    {
        if (s_hash_node->key == s_key)break;
        s_hash_node = s_hash_node->next;
    }
    return(s_hash_node);
}

static struct ts_mzapi_hash_node* (mzapi_fastcall __mzhash_next_search__)(struct ts_mzapi_hash* s_hash, struct ts_mzapi_hash_node* s_hash_node)
{
    t_mzapi_hash_key s_key;
    if (s_hash_node != ((struct ts_mzapi_hash_node*)0))
    {
        s_key = s_hash_node->key;
        s_hash_node = s_hash_node->next;
        while (s_hash_node != ((struct ts_mzapi_hash_node*)0))
        {
            if (s_hash_node->key == s_key)break;
            s_hash_node = s_hash_node->next;
        }
    }
    return(s_hash_node);
}

static struct ts_mzapi_hash_node* (mzapi_fastcall __mzhash_prev_search__)(struct ts_mzapi_hash* s_hash, struct ts_mzapi_hash_node* s_hash_node)
{
    t_mzapi_hash_key s_key;
    if (s_hash_node != ((struct ts_mzapi_hash_node*)0))
    {
        s_key = s_hash_node->key;
        s_hash_node = s_hash_node->prev;
        while (s_hash_node != ((struct ts_mzapi_hash_node*)0))
        {
            if (s_hash_node->key == s_key)break;
            s_hash_node = s_hash_node->prev;
        }
    }
    return(s_hash_node);
}

static struct ts_mzapi_hash_node* (mzapi_fastcall __mzhash_add__)(struct ts_mzapi_hash* s_hash, t_mzapi_hash_key s_key, t_mzapi_vector s_vector)
{
    struct ts_mzapi_hash_node* s_hash_node;
    int s_index;
    if (s_hash == ((struct ts_mzapi_hash*)0))return((struct ts_mzapi_hash_node*)0);
    s_hash_node = (struct ts_mzapi_hash_node*)malloc((size_t)sizeof(struct ts_mzapi_hash_node));
    if (s_hash_node != ((struct ts_mzapi_hash_node*)0))
    {
        s_hash_node->key = s_key;
        s_hash_node->vector = s_vector;
        s_index = s_hash->index(s_key, s_hash->table_count);
        if (s_hash->tail[s_index] == ((struct ts_mzapi_hash_node*)0))
        {
            s_hash_node->prev = s_hash_node->next = (struct ts_mzapi_hash_node*)0;
            s_hash->head[s_index] = s_hash_node;
            s_hash->tail[s_index] = s_hash_node;
        }
        else
        {
            s_hash_node->prev = s_hash->tail[s_index];
            s_hash_node->next = (struct ts_mzapi_hash_node*)0;
            s_hash->tail[s_index]->next = s_hash_node;
            s_hash->tail[s_index] = s_hash_node;
        }
    }
    return(s_hash_node);
}

static int (mzapi_fastcall __mzhash_del__)(struct ts_mzapi_hash* s_hash, struct ts_mzapi_hash_node* s_hash_node)
{
    int s_index;
    if (s_hash == ((struct ts_mzapi_hash*)0))return(-1);
    if (s_hash_node == ((struct ts_mzapi_hash_node*)0))return(0);
    s_index = s_hash->index(s_hash_node->key, s_hash->table_count);
    if (s_hash_node->prev == ((struct ts_mzapi_hash_node*)0))s_hash->head[s_index] = s_hash_node->next;
    else s_hash_node->prev->next = s_hash_node->next;
    if (s_hash_node->next == ((struct ts_mzapi_hash_node*)0))s_hash->tail[s_index] = s_hash_node->prev;
    else s_hash_node->next->prev = s_hash_node->prev;
    free((void*)s_hash_node);
    return(1);
}

static int (mzapi_fastcall __mzhash_info__)(struct ts_mzapi_hash* s_hash)
{
    int s_index;
    int s_hash_count;
    struct ts_mzapi_hash_node* s_hash_node;
    if (s_hash == ((struct ts_mzapi_hash*)0))return(-1);
    (void)fprintf(stdout,
        "\nhash info\n=========\n\n"
        "table_count = %d\n"
        "table_size  = %lu\n"
        "seed        = %08lXH\n\n",
        s_hash->table_count,
        (unsigned long)s_hash->table_size,
        (unsigned long)s_hash->seed);
    for (s_index = 0; s_index < s_hash->table_count; s_index++)
    {
        s_hash_count = 0;
        s_hash_node = s_hash->head[s_index];
        while (s_hash_node != ((struct ts_mzapi_hash_node*)0))
        {
            s_hash_count++;
            s_hash_node = s_hash_node->next;
        }
        (void)fprintf(stdout, " [%5d] hash_nodes=%d\n", s_index, s_hash_count);
    }
    (void)fputs("\nEnd of hash info.\n", stdout);
    return(1);
}

struct ts_mzapi_hash* (mzapi_fastcall mzapi_open_hash)(int s_table_count)
{
    struct ts_mzapi_hash* s_hash;
    int s_index;
    s_hash = (struct ts_mzapi_hash*)malloc((size_t)sizeof(struct ts_mzapi_hash));
    if (s_hash != ((struct ts_mzapi_hash*)0))
    {
        if (s_table_count <= 0)s_table_count = 256; /* default */
        s_hash->table_count = s_table_count;
        s_hash->table_size = (size_t)(s_hash->table_count * sizeof(struct ts_mzapi_hash_node*));
        s_hash->seed = (t_mzapi_hash_key)0; /* default seed key */
        s_hash->head = (struct ts_mzapi_hash_node**)malloc(s_hash->table_size);
        /* mapping member function */
        s_hash->function = __mzhash_function__;
        s_hash->index = __mzhash_index__;
        s_hash->search_by_key = __mzhash_search_by_key__;
        s_hash->next_search = __mzhash_next_search__;
        s_hash->prev_search = __mzhash_prev_search__;
        s_hash->add = __mzhash_add__;
        s_hash->del = __mzhash_del__;
        s_hash->info = __mzhash_info__;
        /* initialize hash table */
        if (s_hash->head != ((struct ts_mzapi_hash_node**)0))
        {
            for (s_index = 0; s_index < s_hash->table_count; s_index++)s_hash->head[s_index] = (struct ts_mzapi_hash_node*)0;
            s_hash->tail = (struct ts_mzapi_hash_node**)malloc(s_hash->table_size);
            if (s_hash->tail != ((struct ts_mzapi_hash_node**)0))
            {
                for (s_index = 0; s_index < s_hash->table_count; s_index++)s_hash->tail[s_index] = (struct ts_mzapi_hash_node*)0;
                /* ok */
            }
            else s_hash = mzapi_close_hash(s_hash);
        }
        else
        {
            s_hash->tail = (struct ts_mzapi_hash_node**)0;
            s_hash = mzapi_close_hash(s_hash);
        }
    }
    return(s_hash);
}

struct ts_mzapi_hash* (mzapi_fastcall mzapi_close_hash)(struct ts_mzapi_hash* s_hash)
{
    int s_index;
    struct ts_mzapi_hash_node* s_prev_node;
    if (s_hash != ((struct ts_mzapi_hash*)0))
    {
        if (s_hash->tail != ((struct ts_mzapi_hash_node**)0))free((void*)s_hash->tail);
        if (s_hash->head != ((struct ts_mzapi_hash_node**)0))
        {
            for (s_index = 0; s_index < s_hash->table_count; s_index++)
            {
                while (s_hash->head[s_index] != ((struct ts_mzapi_hash_node*)0))
                {
                    s_prev_node = s_hash->head[s_index];
                    s_hash->head[s_index] = s_hash->head[s_index]->next;
                    free((void*)s_prev_node);
                }
            }
            free((void*)s_hash->head);
        }
        free((void*)s_hash);
    }
    return((struct ts_mzapi_hash*)0);
}

#if 1 /* simple test code -------------------------------------------------- */ 
struct ts_data
{
    struct ts_data* next;
    int index;
    size_t size;
    void* ptr;
};

static struct ts_data* test_data(FILE* s_file)
{
    struct ts_data* s_data, * s_last, * s_new;
    unsigned char s_buffer[32 << 10];
    char* s_line;
    size_t s_len;
    int s_index = 0;
    s_data = s_last = (struct ts_data*)0;
    do
    { /* indexing from stdin */
        s_line = fgets((char*)(&s_buffer[0]), sizeof(s_buffer), stdin);
        if (s_line == (char*)0)break;
        s_index++;
        s_len = strlen(s_line);
        while (s_len > 0)
        { /* strip \n */
            if (s_line[s_len - 1] == '\n')s_line[--s_len] = '\0';
            else break;
        }
        s_new = (struct ts_data*)malloc((size_t)sizeof(struct ts_data));
        if (s_new != ((struct ts_data*)0))
        {
            s_new->index = s_index;
            s_new->size = s_len;
            s_new->ptr = (void*)malloc(s_new->size + ((size_t)1));
            if (s_new->ptr != ((void*)0))(void)strcpy((char*)s_new->ptr, s_line);
            s_new->next = (struct ts_data*)0;
            if (s_last == ((struct ts_data*)0))s_data = s_new;
            else s_last->next = s_new;
            s_last = s_new;
        }
    } while (1);
    return(s_data);
}

static void free_data(struct ts_data* s_data)
{
    struct ts_data* s_prev;
    while (s_data != ((struct ts_data*)0))
    {
        s_prev = s_data;
        s_data = s_data->next;
        if (s_prev->ptr != ((void*)0))free(s_prev->ptr);
        free((void*)s_prev);
    }
}

int (main)(int s_argc, char** s_argv)
{
    struct ts_mzapi_hash* s_hash;
    struct ts_mzapi_hash_node* s_hash_node;
    struct ts_data* s_data;
    struct ts_data* s_temp;
    int s_arg_index;
    if (s_argc <= 1) (void)fprintf(stdout, "usage: %s <keyword> [...]\n", s_argv[0]);
    else
    {
        s_hash = mzapi_open_hash(128 /* table_count */);
        if (s_hash != ((struct ts_mzapi_hash*)0))
        {
            s_data = test_data(stdin);
            if (s_data != ((struct ts_data*)0))
            {
                /* indexing hash */
                (void)fprintf(stdout, "enter: index\n");
                s_temp = s_data;
                while (s_temp != ((struct ts_data*)0))
                {
                    s_hash_node = s_hash->add(s_hash, s_hash->function(s_hash, s_temp->ptr, s_temp->size), (t_mzapi_vector)s_temp);
                    if (s_hash_node != ((struct ts_mzapi_hash_node*)0))
                        (void)fprintf(stdout, " indexing [key=%08lXH, line=%d] \"%s\"\n", (unsigned long)s_hash_node->key, s_temp->index, (char*)s_temp->ptr);
                    s_temp = s_temp->next;
                }
                (void)fprintf(stdout, "leave: index\n");

#if 1 /* hash view */ 
                s_hash->info(s_hash);
#endif 
                (void)fprintf(stdout, "\n");

                for (s_arg_index = 1; s_arg_index < s_argc; s_arg_index++)
                {
                    /* search */
                    (void)fprintf(stdout, "enter: search[%d] \"%s\"\n", s_arg_index, s_argv[s_arg_index]);
                    s_hash_node = s_hash->search_by_key(s_hash, s_hash->function(s_hash, (void*)s_argv[s_arg_index], (size_t)strlen(s_argv[s_arg_index]))); /* first search */
                    if (s_hash_node != ((struct ts_mzapi_hash_node*)0))
                    {
                        do
                        {
                            s_temp = (struct ts_data*)s_hash_node->vector;
                            (void)fprintf(stdout, " line=%d, \"%s\"\n", s_temp->index, (char*)s_temp->ptr);
                            s_hash_node = s_hash->next_search(s_hash, s_hash_node);
                        } while (s_hash_node != ((struct ts_mzapi_hash_node*)0));
                    }
                    else (void)fprintf(stdout, "not found.\n");
                    (void)fprintf(stdout, "leave: search[%d] \"%s\"\n\n", s_arg_index, s_argv[s_arg_index]);
                }

                free_data(s_data);
            }
            else (void)fprintf(stdout, "no data\n");
            (void)mzapi_close_hash(s_hash);
        }
    }
    return(0);
}
#endif 

/* vim: set expandtab: */
/* End of source */
