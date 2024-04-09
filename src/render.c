#include <string.h>
#include "render.h"
#include "utf8.h"

void save_bitmap(easyfont_render_cxt_t *pctx, const char* filename) {
    FILE *fp = fopen(filename, "wb");
    bitmap *pbitmap  = (bitmap*)calloc(1, sizeof(bitmap));
    strcpy(pbitmap->fileheader.signature, "BM"); // fi.bfType    = 0x4D42;
    pbitmap->fileheader.filesize = pctx->height * pctx->width * pctx->bbp + sizeof(bitmap);
    pbitmap->fileheader.fileoffset_to_pixelarray = sizeof(bitmap);
    pbitmap->bitmapinfoheader.dibheadersize = sizeof(bitmapinfoheader);
    pbitmap->bitmapinfoheader.width = pctx->width;
    pbitmap->bitmapinfoheader.height = -pctx->height;
    pbitmap->bitmapinfoheader.planes = _planes;
    pbitmap->bitmapinfoheader.bitsperpixel = pctx->bbp * 8;
    pbitmap->bitmapinfoheader.compression = _compression;
    pbitmap->bitmapinfoheader.imagesize = pctx->height * pctx->width * pctx->bbp;
    pbitmap->bitmapinfoheader.ypixelpermeter = _ypixelpermeter;
    pbitmap->bitmapinfoheader.xpixelpermeter = _xpixelpermeter;
    pbitmap->bitmapinfoheader.numcolorspallette = 0;
    fwrite (pbitmap, 1, sizeof(bitmap), fp);

    fwrite(pctx->pixelbuffer, 1, pctx->height * pctx->width * pctx->bbp, fp);
    free(pbitmap);
    fclose(fp);
}
#include <math.h>
void save_bitmap_1bit(easyfont_render_cxt_t *pctx, const char* filename) {
    FILE *fp = fopen(filename, "wb");

    // int prow = (pctx->width + 31) / 32 * sizeof(uint32_t);
    // int prow = pctx->width % 8 ? (pctx->width + 31) / 32 * 4 : pctx->width / 8;
    // 8 - (pctx->width % 8)
    int mod = 32 - (pctx->width % 32);
    int prow = pctx->width % 8 ? (pctx->width + mod) / 32 * 4 : pctx->width / 8;
    double dd = (pctx->width + 31) / 32;
    // int prow = (dd) * sizeof(uint32_t);
    uint32_t size  = prow * pctx->height;
    uint8_t *pixelbuffer = (uint8_t*)calloc(1, size);
    int pitch = -prow;

    // double g = pctx->width / 8;

    float g = ceil(pctx->width / 8);
    printf("pctx->width: %d, g: %.6f %d\n", pctx->width, g, pctx->width % 8);
    printf("prow %d; prow * 8 %d\n", prow, prow * 8);
    printf("dd: %f; mod: %d; s: %d\n", dd, mod, pctx->width + mod);

    memset(pixelbuffer, 0, size);

    for (int x = 0; x < pctx->height; x++) {
        for (int y = 0; y < prow; y++) {
            if ((x == 0) | (x == pctx->height - 1)) {
                pixelbuffer[x * prow + y] = 0xFF;
            } else {
                if ((y == 0) ) {
                    pixelbuffer[x * prow + y] = 0x80;
                // } else if (y == prow - 1) {
                } else if (y == (pctx->width / 8) - 1) {
                    // printf("y:          %d\n", y);
                    pixelbuffer[x * prow + y] = 0x01;
                }
            }
        }
    }

    uint32_t isize = sizeof(bmp_info_t) + 2 * sizeof(uint32_t);

    bmp_file_t fi = {0};
    fi.bfType    = 0x4D42;
    fi.bfOffBits = sizeof(fi) + isize;
    fi.bfSize    = isize + size;
    fwrite(&fi, 1, sizeof(fi), fp);

    bmp_info_t im = {0};
    im.biSize          = sizeof(im);
    im.biPlanes        = 1;
    im.biXPelsPerMeter = im.biYPelsPerMeter = 0; // 2834;
    im.biWidth         = pctx->width;
    im.biHeight        = (pitch < 0) ? pctx->height : -pctx->height;
    im.biBitCount      = 1;
    im.biSizeImage     = size;
    // im.biClrUsed       = 2;
    // im.biClrImportant  = 0;
    fwrite(&im, 1, sizeof(im), fp);

    uint32_t rgb[] = { 0x0, 0x00ff00 };
    // uint32_t rgb[] = { 0x0, ~0x0 };
    fwrite(&rgb[0], 1, sizeof(rgb), fp);
    fwrite(pixelbuffer, 1, size, fp);
    fflush(fp);
    free(pixelbuffer);
    fclose(fp);
}

void render_save_bitmap(easyfont_render_cxt_t *pctx, const char* filename) {
    save_bitmap_1bit(pctx, filename);
}

easyfont_render_cxt_t *render_init(uint32_t width, uint32_t height, uint8_t bbp) {
    easyfont_render_cxt_t *pctx = (easyfont_render_cxt_t*)calloc(1, sizeof(easyfont_render_cxt_t));
    pctx->width = width;
    pctx->height = height;
    pctx->bbp = bbp;
    // pctx->pixelbuffer = (uint8_t*)malloc(height * width * bbp); // BYTES PER PIXEL
    pctx->pixelbuffer = (uint8_t*)calloc(1, height * width * bbp); // BYTES PER PIXEL

#if (CONFIG_BPP == 1u)
    pctx->brush_color[0] = BLACK_COLOR;
    pctx->brush_color[1] = WHITE_COLOR;
#elif (CONFIG_BPP == 2u)
    pctx->brush_color[0] = BLACK_COLOR;
    pctx->brush_color[1] = DARK_GREY_COLOR;
    pctx->brush_color[2] = LIGHT_GREY_COLOR;
    pctx->brush_color[3] = WHITE_COLOR;
#elif (CONFIG_BPP == 4u)
    pctx->brush_color[0] = BLACK_COLOR;
    pctx->brush_color[1] = BLACK_COLOR;
    pctx->brush_color[2] = BLACK_COLOR;
    pctx->brush_color[3] = BLACK_COLOR;
    pctx->brush_color[4] = BLACK_COLOR;
    pctx->brush_color[5] = DARK_GREY_COLOR;
    pctx->brush_color[6] = DARK_GREY_COLOR;
    pctx->brush_color[7] = DARK_GREY_COLOR;
    pctx->brush_color[8] = LIGHT_GREY_COLOR;
    pctx->brush_color[9] = LIGHT_GREY_COLOR;
    pctx->brush_color[10] = LIGHT_GREY_COLOR;
    pctx->brush_color[11] = LIGHT_GREY_COLOR;
    pctx->brush_color[12] = WHITE_COLOR;
    pctx->brush_color[13] = WHITE_COLOR;
    pctx->brush_color[14] = WHITE_COLOR;
    pctx->brush_color[15] = WHITE_COLOR;
#endif
    render_set_back_color(pctx, DEFALT_BG_COLOR);
    render_set_brush_color(pctx, DEFALT_BRUSH_COLOR);

    render_clear_screen(pctx);
    return pctx;
}

void render_deinit(easyfont_render_cxt_t *ctx) {
    free(ctx->pixelbuffer);
    free(ctx);
}

void render_clear_screen(easyfont_render_cxt_t *ctx) {
    render_fill_rect(ctx, 0, 0, ctx->width, ctx->height, DEFALT_BG_COLOR);
}

void render_draw_pixel(easyfont_render_cxt_t *ctx, uint16_t x, uint16_t y, color_t color) {
    if (!ctx->pixelbuffer || (ctx->pixelbuffer == (void*)-1)) {
        printf("!pixelbuffer ||  (pixelbuffer == (void*)-1)\n");
        return;
    }

    if (x >= ctx->width || y >= ctx->height) {
        printf("x(%d) >= LCD_WIDTH || y(%d) >= LCD_HEIGHT\n", x, y);
        return;
    }

    // uint32_t location = x * (_bitsperpixel / 8) + y * ctx->width * ctx->bbp; // BYTES_PER_PIXEL
    uint32_t location = x * ctx->bbp + y * ctx->width * ctx->bbp; // BYTES_PER_PIXEL

    uint8_t r = RGB565_TO_R8(color);
    uint8_t g = RGB565_TO_G8(color);
    uint8_t b = RGB565_TO_B8(color);

    *(ctx->pixelbuffer + location) = b;        // blue
    *(ctx->pixelbuffer + location + 1) = g;    // green
    *(ctx->pixelbuffer + location + 2) = r;    // red
    // VALGRIND TELL WEAK
    // *(ctx->pixelbuffer + location + 3) = 0;    // alpha
}

void render_fill_rect(
    easyfont_render_cxt_t *ctx, uint16_t x, uint16_t y, uint16_t width, uint16_t height, color_t color) {
    render_set_bound(ctx, x, y, x + width - 1, y + height - 1);

    uint32_t area = width * height;
    while (area--) {
        render_write_gram(ctx, color);
    }
}

void render_set_bound(
    easyfont_render_cxt_t *ctx, uint16_t startx, uint16_t starty, uint16_t endx, uint16_t endy)
{
    ctx->screen_bond.x0 = startx;
    ctx->screen_bond.y0 = starty;
    ctx->screen_bond.x1 = endx;
    ctx->screen_bond.y1 = endy;

    ctx->cur.x = startx;
    ctx->cur.y = starty;
}

void render_write_gram(easyfont_render_cxt_t *ctx, color_t color) {
    render_draw_pixel(ctx, ctx->cur.x, ctx->cur.y, color);

    ++ctx->cur.x;
    if (ctx->cur.x > ctx->screen_bond.x1) {
        ctx->cur.x = ctx->screen_bond.x0;
        ++ctx->cur.y;
    }

    if (ctx->cur.y > ctx->screen_bond.y1) {
        ctx->cur.x = ctx->screen_bond.x0;
        ctx->cur.y = ctx->screen_bond.y0;
    }
}

#if (CONFIG_BPP == 2u || CONFIG_BPP == 4u)
static void aa_brush_color(easyfont_render_cxt_t *ctx) {
    uint8_t lr;
    uint8_t lg;
    uint8_t lb;

#if (CONFIG_BPP == 2)
    uint8_t idxmax = 2;
#elif(CONFIG_BPP == 4)
    uint8_t idxmax = 14;
#endif

    for (int i = 1; i <= idxmax; i++) {
#if (CONFIG_BPP == 2)
        lr = (((uint16_t)RGB565_TO_R8(ctx->brush_color[3])) * i + ((uint16_t)RGB565_TO_R8(ctx->brush_color[0])) * (3 - i)) / 3;
        lg = (((uint16_t)RGB565_TO_G8(ctx->brush_color[3])) * i + ((uint16_t)RGB565_TO_G8(ctx->brush_color[0])) * (3 - i)) / 3;
        lb = (((uint16_t)RGB565_TO_B8(ctx->brush_color[3])) * i + ((uint16_t)RGB565_TO_B8(ctx->brush_color[0])) * (3 - i)) / 3;
#elif(CONFIG_BPP == 4)
        lr = (((uint16_t)RGB565_TO_R8(ctx->brush_color[15])) * i + ((uint16_t)RGB565_TO_R8(ctx->brush_color[0])) * (1 5 -i)) / 15;
        lg = (((uint16_t)RGB565_TO_G8(ctx->brush_color[15])) * i + ((uint16_t)RGB565_TO_G8(ctx->brush_color[0])) * (1 5 -i)) / 15;
        lb = (((uint16_t)RGB565_TO_B8(ctx->brush_color[15])) * i + ((uint16_t)RGB565_TO_B8(ctx->brush_color[0])) * (1 5 -i)) / 15;
#endif
        ctx->brush_color[i] = RGB888_TO_RGB565(lr, lg, lb);
    }
}
#endif

#if (CONFIG_FONT_MARGIN == 0u && CONFIG_FONT_ENC == 0u)
// encoding method: raw
static void font_render_engine_raw(
    easyfont_render_cxt_t *ctx, const font_t *fnt, const font_symbol_t *sym)
{
    uint16_t i = 0;
    uint8_t j = 0;

#if (CONFIG_FONT_FIXED_WIDTH_HEIGHT > 0u)
    uint16_t area = fnt->width * fnt->height;
#else
    uint16_t area = sym->width * sym->height;
#endif

    const uint8_t *bmp = (const uint8_t*)(fnt->bmp_base + sym->bmp_index);

    while (area--) {
#if (CONFIG_BPP == 1u)
        uint8_t color_pixel = (bmp[i] >> j) & 0x01;
        render_write_gram(ctx, brush_color[color_pixel]);

        if (j == 7) { ++i; j = 0; } else { ++j; }
#elif (CONFIG_BPP == 2u)
        uint8_t color_pixel = (bmp[i] >> j) & 0x03;
        render_write_gram(ctx, brush_color[color_pixel]);

        if (j == 6) { ++i; j = 0; } else { j += 2; }
#elif (CONFIG_BPP == 4u)
        uint8_t color_pixel = (bmp[i] >> j) & 0x0F;
        render_write_gram(ctx, brush_color[color_pixel]);

        if (j == 4) { ++i; j = 0; } else { j += 4; }
#endif
    }
}
#endif

#if (CONFIG_FONT_ENC == 1u)
// encoding method: rawbb
static void font_render_engine_rawbb(
    easyfont_render_cxt_t *ctx, const font_t *fnt, const font_symbol_t *sym)
{
#if (CONFIG_FONT_FIXED_WIDTH_HEIGHT > 0u)
    uint8_t font_width = fnt->width;
    uint8_t font_height = fnt->height;
#else
    uint8_t font_width = sym->width;
    uint8_t font_height = sym->height;
#endif

    uint8_t top = sym->margin_top;
    uint8_t bottom = font_height - sym->margin_bottom-1;
    uint8_t left = sym->margin_left;
    uint8_t right = font_width - sym->margin_right-1;

    uint16_t bi = sym->bmp_index;

    uint8_t h, w;
    uint8_t i=0;

    for (h = 0; h < font_height; h++) {
        for (w = 0; w < font_width; w++) {
            if (w < left || w > right || h < top || h > bottom) {
                // debug: render_write_gram(ctx, BLUE_COLOR);
                render_write_gram(ctx, ctx->brush_color[0]);
            } else {
#if (CONFIG_BPP == 1u)
                uint8_t color_pixel = (fnt->bmp_base[bi] >> i) & 0x01;
                render_write_gram(ctx, ctx->brush_color[color_pixel]);

                if (i == 7) { i = 0; ++bi; } else { ++i; }
#elif (CONFIG_BPP == 2u)
                uint8_t color_pixel = (fnt->bmp_base[bi] >> i) & 0x03;
                render_write_gram(ctx, ctx->brush_color[color_pixel]);

                if (i == 6) { i = 0; ++bi; } else { i += 2; }
#elif (CONFIG_BPP == 4u)
                uint8_t color_pixel = (fnt->bmp_base[bi] >> i) & 0x0F;
                render_write_gram(ctx, ctx->brush_color[color_pixel]);

                if (i == 4) { i = 0; ++bi; } else { i += 4; }
#endif
            }
        }
    }
}
#endif

#if (CONFIG_FONT_ENC == 3u)

typedef enum {
    RLE_STATE_SINGLE = 0,
    RLE_STATE_REPEATE,
    RLE_STATE_COUNTER,
} rle_state_t;

static uint32_t rle_rdp;
static const uint8_t * rle_in;
static uint8_t rle_bpp;
static uint8_t rle_prev_v;
static uint8_t rle_cnt;
static rle_state_t rle_state;

static inline void rle_init(const uint8_t * in,  uint8_t bpp);
static inline uint8_t rle_next(void);

static void font_render_engine_lvgl(
    easyfont_render_cxt_t *ctx, const font_t *fnt, const font_symbol_t *sym)
{
#if (CONFIG_FONT_FIXED_WIDTH_HEIGHT > 0u)
    uint8_t font_width = fnt->width;
    uint8_t font_height = fnt->height;
#else
    uint8_t font_width = sym->width;
    uint8_t font_height = sym->height;
#endif

    uint8_t top = sym->margin_top;
    uint8_t bottom = font_height - sym->margin_bottom-1;
    uint8_t left = sym->margin_left;
    uint8_t right = font_width - sym->margin_right-1;

    uint16_t bi = sym->bmp_index;

    rle_init(&fnt->bmp_base[bi], CONFIG_BPP);

    for (int h = 0; h < font_height; h++) {
        for (int w = 0; w < font_width; w++) {
            if (w < left || w > right || h < top || h > bottom) {
                // debug: render_write_gram(ctx, BLUE_COLOR);
                render_write_gram(ctx, ctx->brush_color[0]);
            } else {
#if (CONFIG_BPP == 1u)
                uint8_t b = rle_next() & 0x01;
                render_write_gram(ctx, ctx->brush_color[b]);
#elif (CONFIG_BPP == 2u)
                uint8_t b = rle_next() & 0x03;
                render_write_gram(ctx, ctx->brush_color[b]);
#elif (CONFIG_BPP == 4u)
                uint8_t b = rle_next() & 0x0F;
                render_write_gram(ctx, ctx->brush_color[b]);
#endif
            }
        }
    }
}

/**
 * Read bits from an input buffer. The read can cross byte boundary.
 * @param in the input buffer to read from.
 * @param bit_pos index of the first bit to read.
 * @param len number of bits to read (must be <= 8).
 * @return the read bits
 */
static inline uint8_t get_bits(const uint8_t * in, uint32_t bit_pos, uint8_t len)
{
    uint8_t bit_mask;
    switch(len) {
        case 1:
            bit_mask = 0x1;
            break;
        case 2:
            bit_mask = 0x3;
            break;
        case 3:
            bit_mask = 0x7;
            break;
        case 4:
            bit_mask = 0xF;
            break;
        case 8:
            bit_mask = 0xFF;
            break;
        default:
            bit_mask = (uint16_t)((uint16_t) 1 << len) - 1;
    }

    uint32_t byte_pos = bit_pos >> 3;
    bit_pos = bit_pos & 0x7;

    if (bit_pos + len >= 8) {
        uint16_t in16 = (in[byte_pos] << 8) + in[byte_pos + 1];
        return (in16 >> (16 - bit_pos - len)) & bit_mask;
    } else {
        return (in[byte_pos] >> (8 - bit_pos - len)) & bit_mask;
    }
}

static inline void rle_init(const uint8_t * in,  uint8_t bpp) {
    rle_in = in;
    rle_bpp = bpp;
    rle_state = RLE_STATE_SINGLE;
    rle_rdp = 0;
    rle_prev_v = 0;
    rle_cnt = 0;
}

static inline uint8_t rle_next(void) {
    uint8_t v = 0;
    uint8_t ret = 0;

    if (rle_state == RLE_STATE_SINGLE) {
        ret = get_bits(rle_in, rle_rdp, rle_bpp);
        if (rle_rdp != 0 && rle_prev_v == ret) {
            rle_cnt = 0;
            rle_state = RLE_STATE_REPEATE;
        }

        rle_prev_v = ret;
        rle_rdp += rle_bpp;
    } else if (rle_state == RLE_STATE_REPEATE) {
        v = get_bits(rle_in, rle_rdp, 1);
        rle_cnt++;
        rle_rdp += 1;
        if (v == 1) {
            ret = rle_prev_v;
            if (rle_cnt == 11) {
                rle_cnt = get_bits(rle_in, rle_rdp, 6);
                rle_rdp += 6;
                if (rle_cnt != 0) {
                    rle_state = RLE_STATE_COUNTER;
                } else {
                    ret = get_bits(rle_in, rle_rdp, rle_bpp);
                    rle_prev_v = ret;
                    rle_rdp += rle_bpp;
                    rle_state = RLE_STATE_SINGLE;
                }
            }
        } else {
            ret = get_bits(rle_in, rle_rdp, rle_bpp);
            rle_prev_v = ret;
            rle_rdp += rle_bpp;
            rle_state = RLE_STATE_SINGLE;
        }

    } else if (rle_state == RLE_STATE_COUNTER) {
        ret = rle_prev_v;
        rle_cnt--;
        if (rle_cnt == 0) {
            ret = get_bits(rle_in, rle_rdp, rle_bpp);
            rle_prev_v = ret;
            rle_rdp += rle_bpp;
            rle_state = RLE_STATE_SINGLE;
        }
    }

    return ret;
}
#endif

//=========================================================================

void draw_char(
    easyfont_render_cxt_t *ctx, uint16_t x, uint16_t y, const font_t *fnt, utf8_t c)
{
    font_symbol_t sym;
    if (!fnt->lookup(c, &sym)) { return; }

#if (CONFIG_FONT_FIXED_WIDTH_HEIGHT > 0u)
    uint8_t font_width = fnt->width;
    uint8_t font_height = fnt->height;
#else
    uint8_t font_width = sym.width;
    uint8_t font_height = sym.height;
#endif

    render_set_bound(ctx, x, y, x + font_width - 1, y + font_height - 1);

#if (CONFIG_FONT_ENC == 0u)
    font_render_engine_raw(ctx, fnt, &sym);
#elif(CONFIG_FONT_ENC == 1u)
    font_render_engine_rawbb(ctx, fnt, &sym);
#elif(CONFIG_FONT_ENC == 2u)
    font_render_engine_u8g2(ctx, fnt, &sym);
#elif(CONFIG_FONT_ENC == 3u)
    font_render_engine_lvgl(ctx, fnt, &sym);
#else
    #error "Unsupported ENCODING_METHOD"
#endif

    render_set_bound(ctx, 0, 0, ctx->width - 1, ctx->height - 1);
}

//=========================================================================


void render_draw_string(easyfont_render_cxt_t *ctx, uint16_t x, uint16_t y, const font_t *fnt, const char *s)
{
    uint16_t orgx = x;

    utf8_t c;
    while ((c = utf8_getchar(s)) != '\0') {
        if (c == '\r') {
            // no operation
        } else if (c == '\n') {
#if (CONFIG_FONT_FIXED_WIDTH_HEIGHT > 0u)
            y += (fnt->height + ROW_CLERANCE_SIZE);
#else
            y += (fnt->default_height + ROW_CLERANCE_SIZE);
#endif
            x = orgx;
        } else if (c == ' ') {
#if (CONFIG_FONT_FIXED_WIDTH_HEIGHT > 0u)
            x += fnt->width;
#else
            x += fnt->default_width;
#endif
        } else {

            draw_char(ctx, x, y, fnt, c);
#if (CONFIG_FONT_FIXED_WIDTH_HEIGHT > 0u)
            x += fnt->width;
#else
            font_symbol_t sym;
            if(!fnt->lookup(c, &sym)) {
                x += fnt->default_width;
            } else {
                x += sym.width;
            }
#endif
        }
        s += utf8_charlen(c);
    }
}

//=========================================================================

void render_set_back_color(easyfont_render_cxt_t *ctx, color_t color) {
    ctx->brush_color[0] = color;

#if (CONFIG_BPP== 2u || CONFIG_BPP == 4u)
    aa_brush_color(ctx);
#endif
}

//=========================================================================

void render_set_brush_color(easyfont_render_cxt_t *ctx, color_t color) {
    ctx->brush_color[(1 << CONFIG_BPP) - 1] = color;

#if (CONFIG_BPP== 2u || CONFIG_BPP == 4u)
    aa_brush_color(ctx);
#endif
}
