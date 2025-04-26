#include "json.h"
#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>

static const char* e2s[] = {
    "Success"
    "File argument to ParseJSON() is NULL",
    "Cannot access the JSON file",
    NULL
};

// Implements RFC 8259
typedef struct PrimitiveNode {
    union {
        char*   string;  // All strings
        int64_t number;  // Any number representable in 64 bits
        double  decimal; // A floating point number
        int     boolean; // Can be either 0 or 1
    };
    int type;
} PrimitiveNode;

struct Node;
typedef struct ArrayNode {
    uint64_t len;
    struct Node**   elems;
} ArrayNode;


typedef struct KeyValuePair {
    char* key;
    struct Node* value;
} KeyValuePair;

typedef struct ObjectNode {
    uint64_t       len;
    KeyValuePair** fields; 
} ObjectNode;

typedef struct ContainerNode {
    union {
        ArrayNode* array;
        ObjectNode* object;
    };
    int type;
} ContainerNode;

typedef struct Node {
    union {
        PrimitiveNode* pnode;
        ContainerNode* cnode;
    };
    int type;
} Node;

typedef struct JSON {
    const char*  name;   // Filename from where JSON has been parsed from
    FILE*  handle; // Handle from which file is read
    Node** nodes;  // Data in the JSON file
} JSON;

JSON* ParseJSON(const char* file, ErrorInfo* err) {
    JSON* json = NULL;
    if (!err) {
        err = malloc(sizeof(ErrorInfo));
        if (!err)
            return NULL;
    }

    err->message = NULL;
    err->line = 1;
    err->type = SUCCESS;

    if (!file) {
        err->type = FILE_NULL;
        err->message = e2s[FILE_NULL];
        return json;
    }

    FILE* handle = fopen(file, "r");
    if (!handle) {
        err->type = FILE_ACCESS_ERROR;
        err->message = e2s[FILE_ACCESS_ERROR];
        return json;
    }

    json = malloc(sizeof(JSON));
    json->name = file;
    json->handle = handle;
    json->nodes = NULL;

    return json;
}
