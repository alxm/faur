#include <a2x.h>

A_SETUP
{
	a2x_set("app.title", "a2x_sfx");
	a2x_set("app.version", "0.2");
	a2x_set("app.author", "Alex");
	a2x_set("app.quiet", "yes");
	a2x_set("video.window", "no");
}

A_MAIN
{
    if(a_argsNum != 4) {
        printf("Error: %s inputFile file.c file.h\n", a_args[0]);
        return;
    }

    char* const inputFile = a_args[1];
    char* const cFile = a_args[2];
    char* const hFile = a_args[3];

    char* const sfxName = a_str_extractName(inputFile);
    uint16_t* const sfxData = (uint16_t*)a_file_toBuffer(inputFile);
    const int length = a_file_size(inputFile) / sizeof(uint16_t);

    File* const hf = a_file_open(hFile, "w");
    File* const cf = a_file_open(cFile, "w");

    FILE* const h = a_file_handle(hf);
    FILE* const c = a_file_handle(cf);

    // header

    fprintf(h, "#pragma once\n\n");
    fprintf(h, "#include <stdint.h>\n\n");
    fprintf(h, "extern uint16_t sfx_%s[%d];\n", sfxName, length);

    // body

    fprintf(c, "#include \"%s\"\n\n", a_str_extractFile(hFile));
    fprintf(c, "uint16_t sfx_%s[%d] = {", sfxName, length);

    for(int i = 0; i < length; i++) {
        if(!(i % 8)) fprintf(c, "\n    ");
        fprintf(c, "0x%04x,", sfxData[i]);
    }

    fprintf(c, "\n};\n");

    a_file_close(hf);
    a_file_close(cf);
}
