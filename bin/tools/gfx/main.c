#include <a2x.h>

A_SETUP
{
	a_settings_set("app.title", "a2x_gfx");
	a_settings_set("app.version", "0.2");
	a_settings_set("app.author", "Alex");
	a_settings_set("app.quiet", "yes");
	a_settings_set("video.window", "no");
}

A_MAIN
{
    if(a_main_numArgs() != 4) {
        printf("Error: %s inputFile file.c file.h\n", a_main_getArg(0));
        return;
    }

    char* const inputFile = a_main_getArg(1);
    char* const cFile = a_main_getArg(2);
    char* const hFile = a_main_getArg(3);

    const int length = a_file_size(inputFile);
    char* const gfxName = a_str_extractName(inputFile);
    uint8_t* const gfxData = a_file_toBuffer(inputFile);

    AFile* const hf = a_file_open(hFile, "w");
    AFile* const cf = a_file_open(cFile, "w");

    FILE* const h = a_file_handle(hf);
    FILE* const c = a_file_handle(cf);

    // header

    fprintf(h, "#pragma once\n\n");
    fprintf(h, "#include <stdint.h>\n\n");
    fprintf(h, "extern uint8_t gfx_%s[];\n", gfxName);

    // body

    fprintf(c, "#include \"%s\"\n\n", a_str_extractFile(hFile));
    fprintf(c, "uint8_t gfx_%s[] = {", gfxName);

    for(int i = 0; i < length; i++) {
        if(!(i % 8)) fprintf(c, "\n    ");
        fprintf(c, "0x%02x,", gfxData[i]);
    }

    fprintf(c, "\n};\n");

    a_file_close(hf);
    a_file_close(cf);
}
