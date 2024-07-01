#include <gtk/gtk.h>
#include <math.h>
#include <stdlib.h>
#include <time.h>
//gcc artillery_game.c -o artillery_game `pkg-config --cflags --libs gtk+-3.0` -lm
#define GRID_SIZE 16
#define CELL_SIZE 40
#define WINDOW_SIZE (GRID_SIZE * CELL_SIZE)
#define CANNON_X (GRID_SIZE / 2)
#define CANNON_Y (GRID_SIZE - 1)
#define BALL_SIZE 10

typedef struct {
    GtkWidget *angle_entry;
    GtkWidget *charge_entry;
    GtkWidget *drawing_area;
    int cannon_angle;
    int ball_x;
    int ball_y;
    int target_x;
    int target_y;
    int firing;
    int charge;
} AppData;
int xx;
int yy;
void draw_grid(cairo_t *cr);
void place_cannon(cairo_t *cr, int angle);
void place_target(cairo_t *cr, int x, int y);
void rotate_cannon(GtkButton *button, AppData *data);
void fire_cannon(GtkButton *button, AppData *data);
gboolean on_draw_event(GtkWidget *widget, cairo_t *cr, AppData *data);
gboolean animate_ball(gpointer user_data);

int main(int argc, char *argv[]) {
    gtk_init(&argc, &argv);

    AppData data;
    data.cannon_angle = 90;
    data.firing = 0;

    GtkWidget *window = gtk_window_new(GTK_WINDOW_TOPLEVEL);
    gtk_window_set_title(GTK_WINDOW(window), "Artillery Game");
    gtk_window_set_default_size(GTK_WINDOW(window), WINDOW_SIZE, WINDOW_SIZE + 100);

    g_signal_connect(window, "destroy", G_CALLBACK(gtk_main_quit), NULL);

    GtkWidget *grid = gtk_grid_new();
    gtk_container_add(GTK_CONTAINER(window), grid);

    GtkWidget *angle_label = gtk_label_new("Angle (0-180):");
    data.angle_entry = gtk_entry_new();
    GtkWidget *angle_button = gtk_button_new_with_label("Rotate");

    GtkWidget *charge_label = gtk_label_new("Charge:");
    data.charge_entry = gtk_entry_new();
    GtkWidget *charge_button = gtk_button_new_with_label("Fire");

    data.drawing_area = gtk_drawing_area_new();
    gtk_widget_set_size_request(data.drawing_area, WINDOW_SIZE, WINDOW_SIZE);

    gtk_grid_attach(GTK_GRID(grid), angle_label, 0, 0, 1, 1);
    gtk_grid_attach(GTK_GRID(grid), data.angle_entry, 1, 0, 1, 1);
    gtk_grid_attach(GTK_GRID(grid), angle_button, 2, 0, 1, 1);

    gtk_grid_attach(GTK_GRID(grid), charge_label, 3, 0, 1, 1);
    gtk_grid_attach(GTK_GRID(grid), data.charge_entry, 4, 0, 1, 1);
    gtk_grid_attach(GTK_GRID(grid), charge_button, 5, 0, 1, 1);

    gtk_grid_attach(GTK_GRID(grid), data.drawing_area, 0, 1, 6, 1);

    g_signal_connect(angle_button, "clicked", G_CALLBACK(rotate_cannon), &data);
    g_signal_connect(charge_button, "clicked", G_CALLBACK(fire_cannon), &data);
    g_signal_connect(data.drawing_area, "draw", G_CALLBACK(on_draw_event), &data);

    gtk_widget_show_all(window);

    srand(time(NULL));
    data.target_x = rand() % GRID_SIZE;
    data.target_y = 0;

    gtk_main();

    return 0;
}

void draw_grid(cairo_t *cr) {
    for (int i = 0; i <= GRID_SIZE; ++i) {
        cairo_move_to(cr, i * CELL_SIZE, 0);
        cairo_line_to(cr, i * CELL_SIZE, WINDOW_SIZE);
        cairo_move_to(cr, 0, i * CELL_SIZE);
        cairo_line_to(cr, WINDOW_SIZE, i * CELL_SIZE);
    }
    cairo_stroke(cr);
}

void place_cannon(cairo_t *cr, int angle) {
    double angle_rad = angle * M_PI / 180.0;
    int x1 = CANNON_X * CELL_SIZE + CELL_SIZE / 2;
    int y1 = CANNON_Y * CELL_SIZE + CELL_SIZE / 2;
    int x2 = (int)(x1 + 20 * cos(angle_rad));
    int y2 = (int)(y1 - 20 * sin(angle_rad));

    cairo_set_line_width(cr, 5);
    cairo_move_to(cr, x1, y1);
    cairo_line_to(cr, x2, y2);
    cairo_stroke(cr);
}

void place_target(cairo_t *cr, int x, int y) {
    int x1 = x * CELL_SIZE;
    int y1 = y * CELL_SIZE;
    int x2 = x1 + CELL_SIZE;
    int y2 = y1 + CELL_SIZE;

    cairo_rectangle(cr, x1, y1, x2 - x1, y2 - y1);
    cairo_set_source_rgb(cr, 1, 0, 0);
    cairo_fill(cr);
}

void rotate_cannon(GtkButton *button, AppData *data) {
    const gchar *text = gtk_entry_get_text(GTK_ENTRY(data->angle_entry));
    int angle = atoi(text);

    if (angle < 0) angle = 0;
    if (angle > 180) angle = 180;

    data->cannon_angle = angle;
    gtk_widget_queue_draw(data->drawing_area);
}

void fire_cannon(GtkButton *button, AppData *data) {
    const gchar *text = gtk_entry_get_text(GTK_ENTRY(data->charge_entry));
    int charge = atoi(text);

    if (charge > 0) {
        data->charge = charge;
        data->firing = 1;
        data->ball_x = CANNON_X * CELL_SIZE + CELL_SIZE / 2;
        data->ball_y = CANNON_Y * CELL_SIZE + CELL_SIZE / 2;
        g_timeout_add(100, animate_ball, data);
    }
}

gboolean on_draw_event(GtkWidget *widget, cairo_t *cr, AppData *data) {
    draw_grid(cr);
    place_cannon(cr, data->cannon_angle);
    place_target(cr, data->target_x, data->target_y);

    if (data->firing) {
        printf("*%d,%d\n", data->ball_x, data->ball_y);
        cairo_arc(cr, data->ball_x, data->ball_y, BALL_SIZE / 2, 0, 2 * M_PI);
        cairo_set_source_rgb(cr, 0, 0, 1);
        cairo_fill(cr);
    }

    return FALSE;
}

gboolean animate_ball(gpointer user_data) {
    AppData *data = (AppData *)user_data;

    double angle_rad = data->cannon_angle * M_PI / 180.0;
    double dx = cos(angle_rad);
    double dy = -sin(angle_rad);
    data->ball_x=CANNON_X *  CELL_SIZE + CELL_SIZE / 2;
    data->ball_y=CANNON_Y *  CELL_SIZE + CELL_SIZE / 2;
    int distance = 0;

    while (distance < data->charge && data->ball_x >= 0 && data->ball_x < WINDOW_SIZE && data->ball_y >= 0 && data->ball_y < WINDOW_SIZE) {
        data->ball_x = (int)(data->ball_x + dx * CELL_SIZE);
        data->ball_y = (int)(data->ball_y + dy * CELL_SIZE);
        distance++;
        xx=data->ball_x;
        yy=data->ball_y;
        gtk_widget_queue_draw(data->drawing_area);
        g_usleep(100000);
    }

    while (data->ball_y < WINDOW_SIZE - CELL_SIZE) {
        data->ball_y += CELL_SIZE;
        printf("%d,%d\n", data->ball_x, data->ball_y);
        xx=data->ball_x;
        yy=data->ball_y;
        gtk_widget_queue_draw(data->drawing_area);
        g_usleep(100000);
    }

    if ((data->ball_x / CELL_SIZE) == data->target_x && (data->ball_y / CELL_SIZE) == data->target_y) {
        data->target_x = rand() % GRID_SIZE;
        data->target_y = 0;
    }

    data->firing = 0;
    gtk_widget_queue_draw(data->drawing_area);

    return G_SOURCE_REMOVE;
}

