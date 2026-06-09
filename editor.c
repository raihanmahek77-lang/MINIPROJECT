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
    int active; // 1 if in use, 0 if deleted
} Shape;

Shape shapes[MAX_SHAPES];
int next_id = 1;
char canvas[CANVAS_HEIGHT][CANVAS_WIDTH];

// Initialize canvas with '_'
void init_canvas() {
    for (int y = 0; y < CANVAS_HEIGHT; y++) {
        for (int x = 0; x < CANVAS_WIDTH; x++) {
            canvas[y][x] = '_';
        }
    }
}

// Plot a point safely
void plot(int x, int y) {
    if (x >= 0 && x < CANVAS_WIDTH && y >= 0 && y < CANVAS_HEIGHT) {
        canvas[y][x] = '*';
    }
}

// Bresenham's Line Algorithm
void draw_line_algo(int x1, int y1, int x2, int y2) {
    int dx = abs(x2 - x1);
    int dy = abs(y2 - y1);
    int sx = (x1 < x2) ? 1 : -1;
    int sy = (y1 < y2) ? 1 : -1;
    int err = dx - dy;

    while (1) {
        plot(x1, y1);
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

// Midpoint Circle Algorithm
void draw_circle_algo(int xc, int yc, int r) {
    int x = 0, y = r;
    int d = 3 - 2 * r;
    plot(xc + x, yc + y);
    plot(xc - x, yc + y);
    plot(xc + x, yc - y);
    plot(xc - x, yc - y);
    plot(xc + y, yc + x);
    plot(xc - y, yc + x);
    plot(xc + y, yc - x);
    plot(xc - y, yc - x);

    while (y >= x) {
        x++;
        if (d > 0) {
            y--;
            d = d + 4 * (x - y) + 10;
        } else {
            d = d + 4 * x + 6;
        }
        plot(xc + x, yc + y);
        plot(xc - x, yc + y);
        plot(xc + x, yc - y);
        plot(xc - x, yc - y);
        plot(xc + y, yc + x);
        plot(xc - y, yc + x);
        plot(xc + y, yc - x);
        plot(xc - y, yc - x);
    }
}

void render_shapes() {
    init_canvas();
    for (int i = 0; i < MAX_SHAPES; i++) {
        if (!shapes[i].active) continue;

        Shape s = shapes[i];
        if (s.type == SHAPE_LINE) {
            draw_line_algo(s.x1, s.y1, s.x2, s.y2);
        } else if (s.type == SHAPE_RECTANGLE) {
            draw_line_algo(s.x1, s.y1, s.x2, s.y1); // Top
            draw_line_algo(s.x2, s.y1, s.x2, s.y2); // Right
            draw_line_algo(s.x2, s.y2, s.x1, s.y2); // Bottom
            draw_line_algo(s.x1, s.y2, s.x1, s.y1); // Left
        } else if (s.type == SHAPE_CIRCLE) {
            draw_circle_algo(s.x1, s.y1, s.radius);
        } else if (s.type == SHAPE_TRIANGLE) {
            draw_line_algo(s.x1, s.y1, s.x2, s.y2);
            draw_line_algo(s.x2, s.y2, s.x3, s.y3);
            draw_line_algo(s.x3, s.y3, s.x1, s.y1);
        }
    }
}

void display_canvas() {
    render_shapes();
    printf("\n=== Canvas ===\n");
    for (int y = 0; y < CANVAS_HEIGHT; y++) {
        for (int x = 0; x < CANVAS_WIDTH; x++) {
            putchar(canvas[y][x]);
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
    printf("Enter ID of shape to modify: ");
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
            if (s.type == SHAPE_LINE) printf("Line (%d,%d to %d,%d)\n", s.x1, s.y1, s.x2, s.y2);
            else if (s.type == SHAPE_RECTANGLE) printf("Rectangle (%d,%d to %d,%d)\n", s.x1, s.y1, s.x2, s.y2);
            else if (s.type == SHAPE_CIRCLE) printf("Circle (center %d,%d, radius %d)\n", s.x1, s.y1, s.radius);
            else if (s.type == SHAPE_TRIANGLE) printf("Triangle (%d,%d, %d,%d, %d,%d)\n", s.x1, s.y1, s.x2, s.y2, s.x3, s.y3);
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
