#ifndef __JSON_H__
#define __JSON_H__

struct JSON;
typedef struct JSON JSON;

enum Status {
    SUCCESS,
    FILE_NULL,
    FILE_ACCESS_ERROR,
	FILE_READ_ERROR,
};

typedef struct ErrorInfo {
    const char* message;
    int line;
    int type;
} ErrorInfo; 

JSON* ParseJSON(const char* file, ErrorInfo* err);
void  FreeJSON(JSON* json);

#endif
