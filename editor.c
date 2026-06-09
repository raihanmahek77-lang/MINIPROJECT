#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>

#define CANVAS_WIDTH 80
#define CANVAS_HEIGHT 24
#define MAX_SHAPES 100

typedef enum {
    SHAPE_LINE = 1,
    SHAPE_RECTANGLE,
    SHAPE_CIRCLE,
    SHAPE_TRIANGLE
} ShapeType;

typedef struct {
    int id;
    ShapeType type;
    int x1, y1;
    int x2, y2;
    int x3, y3;
    int radius;
    int color;     // 1=Red, 2=Green, 3=Yellow, 4=Blue, 5=Magenta, 6=Cyan, 7=White
    int is_filled; // 1=Filled, 0=Outline
    int active;
} Shape;

Shape shapes[MAX_SHAPES];
int next_id = 1;
char canvas[CANVAS_HEIGHT][CANVAS_WIDTH];
int canvas_color[CANVAS_HEIGHT][CANVAS_WIDTH];

// Initialize canvas
void init_canvas() {
    for (int y = 0; y < CANVAS_HEIGHT; y++) {
        for (int x = 0; x < CANVAS_WIDTH; x++) {
            canvas[y][x] = '_';
            canvas_color[y][x] = 0; // Default color
        }
    }
}

// Plot a point safely
void plot(int x, int y, int color) {
    if (x >= 0 && x < CANVAS_WIDTH && y >= 0 && y < CANVAS_HEIGHT) {
        canvas[y][x] = '*';
        canvas_color[y][x] = color;
    }
}

// Bresenham's Line Algorithm
void draw_line_algo(int x1, int y1, int x2, int y2, int color) {
    int dx = abs(x2 - x1);
    int dy = abs(y2 - y1);
    int sx = (x1 < x2) ? 1 : -1;
    int sy = (y1 < y2) ? 1 : -1;
    int err = dx - dy;

    while (1) {
        plot(x1, y1, color);
        if (x1 == x2 && y1 == y2) break;
        int e2 = 2 * err;
        if (e2 > -dy) {
            err -= dy;
            x1 += sx;
        }
        if (e2 < dx) {
            err += dx;
            y1 += sy;
        }
    }
}

// Horizontal line for filling
void draw_hline(int x1, int x2, int y, int color) {
    if (x1 > x2) { int t = x1; x1 = x2; x2 = t; }
    for (int x = x1; x <= x2; x++) {
        plot(x, y, color);
    }
}

// Rectangle Algorithm
void draw_rectangle_algo(int x1, int y1, int x2, int y2, int color, int is_filled) {
    if (x1 > x2) { int t = x1; x1 = x2; x2 = t; }
    if (y1 > y2) { int t = y1; y1 = y2; y2 = t; }

    if (is_filled) {
        for (int y = y1; y <= y2; y++) {
            draw_hline(x1, x2, y, color);
        }
    } else {
        draw_line_algo(x1, y1, x2, y1, color); // Top
        draw_line_algo(x2, y1, x2, y2, color); // Right
        draw_line_algo(x2, y2, x1, y2, color); // Bottom
        draw_line_algo(x1, y2, x1, y1, color); // Left
    }
}

// Midpoint Circle Algorithm
void draw_circle_algo(int xc, int yc, int r, int color, int is_filled) {
    int x = 0, y = r;
    int d = 3 - 2 * r;

    while (y >= x) {
        if (is_filled) {
            draw_hline(xc - x, xc + x, yc + y, color);
            draw_hline(xc - x, xc + x, yc - y, color);
            draw_hline(xc - y, xc + y, yc + x, color);
            draw_hline(xc - y, xc + y, yc - x, color);
        } else {
            plot(xc + x, yc + y, color);
            plot(xc - x, yc + y, color);
            plot(xc + x, yc - y, color);
            plot(xc - x, yc - y, color);
            plot(xc + y, yc + x, color);
            plot(xc - y, yc + x, color);
            plot(xc + y, yc - x, color);
            plot(xc - y, yc - x, color);
        }

        x++;
        if (d > 0) {
            y--;
            d = d + 4 * (x - y) + 10;
        } else {
            d = d + 4 * x + 6;
        }
    }
}

// Barycentric helper
int sign(int p1x, int p1y, int p2x, int p2y, int p3x, int p3y) {
    return (p1x - p3x) * (p2y - p3y) - (p2x - p3x) * (p1y - p3y);
}

// Triangle Algorithm
void draw_triangle_algo(int x1, int y1, int x2, int y2, int x3, int y3, int color, int is_filled) {
    if (is_filled) {
        int minX = x1, maxX = x1;
        if (x2 < minX) minX = x2; if (x3 < minX) minX = x3;
        if (x2 > maxX) maxX = x2; if (x3 > maxX) maxX = x3;
        
        int minY = y1, maxY = y1;
        if (y2 < minY) minY = y2; if (y3 < minY) minY = y3;
        if (y2 > maxY) maxY = y2; if (y3 > maxY) maxY = y3;

        for (int y = minY; y <= maxY; y++) {
            for (int x = minX; x <= maxX; x++) {
                int d1 = sign(x, y, x1, y1, x2, y2);
                int d2 = sign(x, y, x2, y2, x3, y3);
                int d3 = sign(x, y, x3, y3, x1, y1);

                int has_neg = (d1 < 0) || (d2 < 0) || (d3 < 0);
                int has_pos = (d1 > 0) || (d2 > 0) || (d3 > 0);

                if (!(has_neg && has_pos)) {
                    plot(x, y, color);
                }
            }
        }
    } else {
        draw_line_algo(x1, y1, x2, y2, color);
        draw_line_algo(x2, y2, x3, y3, color);
        draw_line_algo(x3, y3, x1, y1, color);
    }
}

void render_shapes() {
    init_canvas();
    for (int i = 0; i < MAX_SHAPES; i++) {
        if (!shapes[i].active) continue;

        Shape s = shapes[i];
        if (s.type == SHAPE_LINE) {
            draw_line_algo(s.x1, s.y1, s.x2, s.y2, s.color);
        } else if (s.type == SHAPE_RECTANGLE) {
            draw_rectangle_algo(s.x1, s.y1, s.x2, s.y2, s.color, s.is_filled);
        } else if (s.type == SHAPE_CIRCLE) {
            draw_circle_algo(s.x1, s.y1, s.radius, s.color, s.is_filled);
        } else if (s.type == SHAPE_TRIANGLE) {
            draw_triangle_algo(s.x1, s.y1, s.x2, s.y2, s.x3, s.y3, s.color, s.is_filled);
        }
    }
}

void display_canvas() {
    render_shapes();
    printf("\n=== Canvas ===\n");
    for (int y = 0; y < CANVAS_HEIGHT; y++) {
        for (int x = 0; x < CANVAS_WIDTH; x++) {
            int c = canvas_color[y][x];
            char ch = canvas[y][x];
            if (c > 0 && c <= 7) {
                // ANSI colors: 31=Red, 32=Green, 33=Yellow, 34=Blue, 35=Magenta, 36=Cyan, 37=White
                printf("\033[0;%dm%c\033[0m", 30 + c, ch);
            } else {
                putchar(ch);
            }
        }
        putchar('\n');
    }
    printf("==============\n");
}

int find_free_slot() {
    for (int i = 0; i < MAX_SHAPES; i++) {
        if (!shapes[i].active) return i;
    }
    return -1;
}

int find_shape_index(int id) {
    for (int i = 0; i < MAX_SHAPES; i++) {
        if (shapes[i].active && shapes[i].id == id) return i;
    }
    return -1;
}

void add_shape() {
    int slot = find_free_slot();
    if (slot == -1) {
        printf("Error: Max shapes reached.\n");
        return;
    }

    int type;
    printf("Select Shape Type:\n");
    printf("1. Line\n2. Rectangle\n3. Circle\n4. Triangle\nChoice: ");
    scanf("%d", &type);

    Shape s;
    s.id = next_id++;
    s.active = 1;
    s.type = (ShapeType)type;

    if (type == SHAPE_LINE) {
        s.x1 = 10; s.y1 = 5; s.x2 = 30; s.y2 = 15;
    } else if (type == SHAPE_RECTANGLE) {
        s.x1 = 5; s.y1 = 5; s.x2 = 25; s.y2 = 15;
    } else if (type == SHAPE_CIRCLE) {
        s.x1 = 40; s.y1 = 12; s.radius = 8;
    } else if (type == SHAPE_TRIANGLE) {
        s.x1 = 60; s.y1 = 5; s.x2 = 70; s.y2 = 18; s.x3 = 50; s.y3 = 18;
    } else {
        printf("Invalid choice.\n");
        return;
    }

    printf("Color: 1-Red, 2-Green, 3-Yellow, 4-Blue, 5-Magenta, 6-Cyan, 7-White\nChoice: ");
    scanf("%d", &s.color);

    if (type != SHAPE_LINE) {
        printf("Fill the shape? (1 for Yes, 0 for No): ");
        scanf("%d", &s.is_filled);
    } else {
        s.is_filled = 0; // Lines can't be filled
    }

    shapes[slot] = s;
    printf("Shape added with ID: %d at default positions.\n", s.id);
}

void delete_shape() {
    int id;
    printf("Enter ID of shape to delete: ");
    scanf("%d", &id);

    int idx = find_shape_index(id);
    if (idx != -1) {
        shapes[idx].active = 0;
        printf("Shape %d deleted.\n", id);
    } else {
        printf("Shape ID not found.\n");
    }
}

void modify_shape() {
    int id;
    printf("Enter ID of shape to modify (will be shifted right and down by 2): ");
    scanf("%d", &id);

    int idx = find_shape_index(id);
    if (idx != -1) {
        shapes[idx].x1 += 2;
        shapes[idx].y1 += 2;
        if (shapes[idx].type == SHAPE_LINE || shapes[idx].type == SHAPE_RECTANGLE) {
            shapes[idx].x2 += 2;
            shapes[idx].y2 += 2;
        } else if (shapes[idx].type == SHAPE_TRIANGLE) {
            shapes[idx].x2 += 2;
            shapes[idx].y2 += 2;
            shapes[idx].x3 += 2;
            shapes[idx].y3 += 2;
        }
        // Circle's radius stays the same, center just shifted
        printf("Shape %d modified (shifted right and down by 2 units).\n", id);
    } else {
        printf("Shape ID not found.\n");
    }
}

void list_shapes() {
    printf("--- Active Shapes ---\n");
    int count = 0;
    for (int i = 0; i < MAX_SHAPES; i++) {
        if (shapes[i].active) {
            count++;
            Shape s = shapes[i];
            printf("ID: %d | Type: ", s.id);
            if (s.type == SHAPE_LINE) printf("Line (%d,%d to %d,%d)", s.x1, s.y1, s.x2, s.y2);
            else if (s.type == SHAPE_RECTANGLE) printf("Rectangle (%d,%d to %d,%d)", s.x1, s.y1, s.x2, s.y2);
            else if (s.type == SHAPE_CIRCLE) printf("Circle (center %d,%d, radius %d)", s.x1, s.y1, s.radius);
            else if (s.type == SHAPE_TRIANGLE) printf("Triangle (%d,%d, %d,%d, %d,%d)", s.x1, s.y1, s.x2, s.y2, s.x3, s.y3);
            
            printf(" | Color: %d | Filled: %d\n", s.color, s.is_filled);
        }
    }
    if (count == 0) printf("No active shapes.\n");
    printf("---------------------\n");
}

int main() {
    // initialize shapes array
    for (int i = 0; i < MAX_SHAPES; i++) shapes[i].active = 0;

    int choice;
    do {
        display_canvas(); // Always display the canvas
        printf("\n--- 2D Graphics Editor ---\n");
        printf("1. Add Shape\n");
        printf("2. Delete Shape\n");
        printf("3. Modify Shape\n");
        printf("4. List Shapes\n");
        printf("5. Exit\n");
        printf("Enter choice: ");
        if (scanf("%d", &choice) != 1) {
            // handle invalid input
            while(getchar() != '\n'); 
            continue;
        }

        switch (choice) {
            case 1: add_shape(); break;
            case 2: delete_shape(); break;
            case 3: modify_shape(); break;
            case 4: list_shapes(); break;
            case 5: printf("Exiting...\n"); break;
            default: printf("Invalid choice.\n");
        }
    } while (choice != 5);

    return 0;
}
