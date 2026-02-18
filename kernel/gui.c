/*
 * OpenOS - GUI Implementation
 */

#include "gui.h"
#include "console.h"
#include "string.h"

/* Forward declarations */
void* kmalloc(size_t size);
void kfree(void* ptr);

/* Global GUI state */
static gui_state_t gui;

/* Simulated framebuffer (in kernel memory) */
static uint32_t framebuffer_data[GUI_WIDTH * GUI_HEIGHT];

/* Initialize GUI subsystem */
void gui_init(void) {
    if (gui.initialized) return;
    
    console_write("GUI: Initializing windowing system...\n");
    
    gui.framebuffer = framebuffer_data;
    gui.width = GUI_WIDTH;
    gui.height = GUI_HEIGHT;
    gui.window_list = NULL;
    gui.next_window_id = 1;
    gui.initialized = 1;
    
    /* Clear screen to black */
    gui_clear_screen(COLOR_BLACK);
    
    console_write("GUI: 800x600x32 framebuffer initialized\n");
}

/* Draw a single pixel */
void gui_draw_pixel(int x, int y, uint32_t color) {
    if (x < 0 || x >= gui.width || y < 0 || y >= gui.height) return;
    gui.framebuffer[y * gui.width + x] = color;
}

/* Draw a rectangle outline */
void gui_draw_rect(rect_t* rect, uint32_t color) {
    if (!rect) return;
    
    /* Top and bottom edges */
    for (int x = rect->x; x < rect->x + rect->width; x++) {
        gui_draw_pixel(x, rect->y, color);
        gui_draw_pixel(x, rect->y + rect->height - 1, color);
    }
    
    /* Left and right edges */
    for (int y = rect->y; y < rect->y + rect->height; y++) {
        gui_draw_pixel(rect->x, y, color);
        gui_draw_pixel(rect->x + rect->width - 1, y, color);
    }
}

/* Draw a filled rectangle */
void gui_draw_filled_rect(rect_t* rect, uint32_t color) {
    if (!rect) return;
    
    for (int y = rect->y; y < rect->y + rect->height; y++) {
        for (int x = rect->x; x < rect->x + rect->width; x++) {
            gui_draw_pixel(x, y, color);
        }
    }
}

/* Draw a line (simple DDA algorithm) */
void gui_draw_line(int x1, int y1, int x2, int y2, uint32_t color) {
    int dx = x2 - x1;
    int dy = y2 - y1;
    
    /* Calculate absolute values */
    int abs_dx = dx < 0 ? -dx : dx;
    int abs_dy = dy < 0 ? -dy : dy;
    int steps = abs_dx > abs_dy ? abs_dx : abs_dy;
    
    float x_inc = (float)dx / (float)steps;
    float y_inc = (float)dy / (float)steps;
    
    float x = x1;
    float y = y1;
    
    for (int i = 0; i <= steps; i++) {
        gui_draw_pixel((int)x, (int)y, color);
        x += x_inc;
        y += y_inc;
    }
}

/* Draw text (simple 8x8 bitmap font simulation) */
void gui_draw_text(int x, int y, const char* text, uint32_t color) {
    if (!text) return;
    
    int offset_x = x;
    while (*text) {
        /* Simple character rendering - just draw a box for now */
        for (int dy = 0; dy < 8; dy++) {
            for (int dx = 0; dx < 8; dx++) {
                if ((dx == 0 || dx == 7 || dy == 0 || dy == 7)) {
                    gui_draw_pixel(offset_x + dx, y + dy, color);
                }
            }
        }
        offset_x += 9;
        text++;
    }
}

/* Create a new window */
window_t* gui_create_window(int x, int y, int width, int height, const char* title) {
    window_t* window = (window_t*)kmalloc(sizeof(window_t));
    if (!window) return NULL;
    
    window->id = gui.next_window_id++;
    window->rect.x = x;
    window->rect.y = y;
    window->rect.width = width;
    window->rect.height = height;
    window->bg_color = COLOR_LIGHTGRAY;
    window->state = WINDOW_VISIBLE;
    
    if (title) {
        strncpy(window->title, title, 63);
        window->title[63] = '\0';
    } else {
        window->title[0] = '\0';
    }
    
    /* Add to window list */
    window->next = gui.window_list;
    gui.window_list = window;
    
    return window;
}

/* Destroy a window */
void gui_destroy_window(window_t* window) {
    if (!window) return;
    
    /* Remove from window list */
    window_t** current = &gui.window_list;
    while (*current) {
        if (*current == window) {
            *current = window->next;
            break;
        }
        current = &(*current)->next;
    }
    
    kfree(window);
}

/* Show window */
void gui_show_window(window_t* window) {
    if (window) {
        window->state = WINDOW_VISIBLE;
    }
}

/* Hide window */
void gui_hide_window(window_t* window) {
    if (window) {
        window->state = WINDOW_HIDDEN;
    }
}

/* Render a single window */
void gui_render_window(window_t* window) {
    if (!window || window->state != WINDOW_VISIBLE) return;
    
    /* Draw window background */
    gui_draw_filled_rect(&window->rect, window->bg_color);
    
    /* Draw window border */
    gui_draw_rect(&window->rect, COLOR_BLACK);
    
    /* Draw title bar */
    rect_t title_bar = {
        window->rect.x,
        window->rect.y,
        window->rect.width,
        20
    };
    gui_draw_filled_rect(&title_bar, COLOR_BLUE);
    
    /* Draw title text */
    if (window->title[0]) {
        gui_draw_text(window->rect.x + 5, window->rect.y + 6, window->title, COLOR_WHITE);
    }
}

/* Render all windows */
void gui_render_all(void) {
    window_t* window = gui.window_list;
    while (window) {
        gui_render_window(window);
        window = window->next;
    }
}

/* Clear screen */
void gui_clear_screen(uint32_t color) {
    for (int i = 0; i < gui.width * gui.height; i++) {
        gui.framebuffer[i] = color;
    }
}

/* Placeholder memory allocation functions (to be replaced with real heap) */
void* kmalloc(size_t size) {
    static uint8_t heap[65536];
    static size_t heap_pos = 0;
    
    if (heap_pos + size > sizeof(heap)) return NULL;
    void* ptr = &heap[heap_pos];
    heap_pos += size;
    return ptr;
}

void kfree(void* ptr) {
    /* Simple allocator doesn't support free */
    (void)ptr;
}
