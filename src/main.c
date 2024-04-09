#include <stdio.h>
#include <stdint.h>
#include <string.h>
#include <malloc.h>
#include <stdlib.h>
#include "utf8.h"
#include "render.h"
#include "font.h"
#include <time.h>


#if (TEST_UTF8 > 0u)
    #include "microhei32.h"
    font_t *select_fnt = &wqy_microhei32;
#elif (CONFIG_FONT_FIXED_WIDTH_HEIGHT > 0u)
    #include "monaco.h"
    font_t *select_fnt = &monaco32;
#else
    #include "freesans32.h"
    font_t *select_fnt = &freesans32;
#endif

int main (int argc , char *argv[]) {

#if 0
    easyfont_render_cxt_t *pctx = render_init(1768, 48, 3);
    if (!pctx) {
        printf("pctx 1 is NULL\n");
        exit(-1);
    }
    time_t t = time(NULL);
    struct tm tm = *localtime(&t);

    char text_test[64];
    // char text_test[128] = "!\"#&\'()*+,-./'0123456789:;<=>?@[\\]^_`{|}~ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz";
    sprintf(text_test, "%d-%02d-%02d %02d:%02d:%02d", tm.tm_year + 1900, tm.tm_mon + 1, tm.tm_mday, tm.tm_hour, tm.tm_min, tm.tm_sec);
    printf("now: %s\n", text_test);

    render_draw_string(pctx, 0, 0, select_fnt, text_test);
#else
    int width = 529;
    // int width = 512;
    int height = 300;
    easyfont_render_cxt_t *pctx = render_init(width, height, 3);
    if (!pctx) {
        printf("pctx 1 is NULL\n");
        exit(-1);
    }
#endif
    render_save_bitmap(pctx, "test2.bmp");
    render_deinit(pctx);
    exit(0);
}
