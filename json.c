#include "json.h"
#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>

static const char* e2s[] = {
    "Success",
    "File argument to ParseJSON() is NULL",
    "Cannot access the JSON file",
	"Failed to read from file, possible file corruption",
	"Token not expected",
    NULL
};

#define FILE_EOF (-1)

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
    const char*  name; // Filename from where JSON has been parsed from
    FILE*  handle;     // Handle from which file is read
	uint32_t offset;   // Offset into the file (in bytes)
	uint32_t line;     // Line number in the file
    Node** nodes;      // Data in the JSON file
	char   PbBuf[16];  // Buffer to putback tokens to
	int    PbLen;      // Currently consumed length of PbBuf 
} JSON;


static int Next(JSON* json) {
	int ch = 0;
	if (json->PbLen != 0) {
		ch = json->PbBuf[json->PbLen - 1];
		json->PbLen--;
		return ch;
	}

	if (fread(&ch, 1, 1, json->handle) != 1) {
		if (feof(json->handle))
			return FILE_EOF;
		else
			return FILE_READ_ERROR;
	}

	if (ch == '\n')
		json->line++;

	json->offset++;
	return ch;
}

static inline void PutBack(JSON* json, int ch) {
	if (ch == FILE_EOF)
		return;

	if (json->PbLen > 16) {
		printf("Internal Error: PutBack buffer length exceeded\n");
		return;
	}

	json->offset--;
	json->PbBuf[json->PbLen - 1] = ch;
	json->PbLen += 1;
}

/*ws = *(
 %x20 / ; Space
 %x09 / ; Horizontal tab
 %x0A / ; Line feed or New line
 %x0D ) */
int IsSpace(int ch) {
    switch (ch) {
        case 0x20: case 0x09: case 0x0A: case 0x0D: return 1;
        default: return 0;
	}
}

static void SkipSpace(JSON* json) {
	while (1) {
		int ch = Next(json);
        // We could have used isspace here but not all systems have a
        // implementation of isspace() that is suitable for our purposes
		if (!IsSpace(ch)) {
			PutBack(json, ch);
			break;
		}
	}
}
/*
 * value = false / null / true / object / array / number / string
 */
static int ParseValue(JSON* json) {
	// EOF can only be the first character in the stream after a space
	int ch = Next(json);
	if (ch == FILE_EOF)
		return ch;

	return UNK_TOKEN;
}

JSON* ParseJSON(const char* file, ErrorInfo* err) {
    JSON* json = NULL;
    if (!err) 
        return NULL;

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

	// json ::= ws value ws
	while (1) {
		SkipSpace(json);
		int status = ParseValue(json);
		if (status > 0) {
			err->line = json->line;
			err->type = status;
			err->message = e2s[err->type];
			free(json);
			return NULL;
		}

		if (status == FILE_EOF)
			break;
		SkipSpace(json);
	}
    return json;
}
