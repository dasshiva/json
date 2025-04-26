#include <stdio.h>
#include <stdlib.h>
#include "json.h"

int main(int argc, const char** argv) {
	if (argc != 2) {
		printf("usage: %s file.json\n", argv[0]);
		return 1;
	}

	ErrorInfo err = {0};
	JSON* json = ParseJSON(argv[1], &err);
	if (!json) {
		if (err.line != 0) 
			printf("ERROR:%s:%d %s\n", argv[1], err.line, err.message);
		else
			printf("Our of memory!\n");

		return 1;
	}

	printf("Successfully parsed\n");
	return 0;

}
