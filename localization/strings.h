#include "en-US/strings.h"
#include "es-ES/strings.h"
#include "gl-ES/strings.h"

struct {
	wchar_t *code;
	struct strings *strings;
} languages[] = {
	{L"en-US", &en_US},
	{L"es-ES", &es_ES},
	{L"gl-ES", &gl_ES},
};

int num_languages = 3;
