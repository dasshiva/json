#include <stdio.h>
#include <stdint.h>

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
    Node**   elems;
} ArrayNode;


typedef struct KeyValuePair {
    char* key;
    Node* value;
} KeyValuePair;

typedef struct ContainerNode {
    uint64_t       len;
    KeyValuePair** fields; 
} ContainerNode;

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
};

typedef struct JSON {
    char*  name; // Filename from where JSON has been parsed from
    Node** nodes; // Data in the JSON file
} JSON;
