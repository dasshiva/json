#ifndef __JSON_H__
#define __JSON_H__

struct JSON;
typedef struct JSON JSON;

typedef struct ErrorInfo {
    const char* message;
    int line;
} ErrorInfo; 

JSON* ParseJSON(const char* file, ErrorInfo* err);
void  FreeJSON(JSON* json);

#endif
