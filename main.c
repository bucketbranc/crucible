#include <pdcurses.h>
#include <string.h>
#include <stdlib.h>
#include <locale.h>

// --- Color Pair Definitions ---
#define COLOR_PAIR_DEFAULT 1
#define COLOR_PAIR_TITLE 2
#define COLOR_PAIR_BUTTON_NORMAL 3
#define COLOR_PAIR_BUTTON_HIGHLIGHT 4
#define COLOR_PAIR_INPUT 5
#define COLOR_PAIR_BORDER 6

// --- Constants ---
#define MAX_USERS 100
#define MAX_DONATIONS 100
#define MAX_NAME_LENGTH 50
#define MAX_CONTROL_NUMBER_LENGTH 20

// --- Data Structures ---
typedef struct {
    char control_number[MAX_CONTROL_NUMBER_LENGTH];
    char name[MAX_NAME_LENGTH];
} User;

typedef struct {
    char user_control_number[MAX_CONTROL_NUMBER_LENGTH];
    int paper;
    int plastic;
    int aluminum;
} Donation;

// --- Global State ---
User users[MAX_USERS];
int user_count = 0;

Donation donations[MAX_DONATIONS];
int donation_count = 0;

char logged_in_user[MAX_CONTROL_NUMBER_LENGTH] = "";

// --- Asset File Paths ---
const char* LOGO_FILE = "assets/images/logo.txt";
const char* BANNER_FILE = "assets/images/banner.txt";
const char* COMO_RECICLAR_FILE = "assets/news/como_reciclar.txt";
const char* NOTICIAS_FILE = "assets/news/noticias_recientes.txt";
const char* CENTROS_FILE = "assets/news/centros_de_acopio.txt";

// --- Function Prototypes ---
void init_colors();
void draw_rounded_box(int y1, int x1, int y2, int x2);
char* read_asset_file(const char* filename);

// Component-like render functions
void render_navbar(const char* banner);
void render_footer();
void render_main_menu(int highlight);
void render_login_view();
void render_donation_form();
void render_user_list();
void render_donation_list();
void render_info_view(const char* title, const char* content_file);

// --- Main Application ---
int main() {
    // Initialization
    setlocale(LC_ALL, "");
    initscr();
    cbreak();
    noecho();
    curs_set(0);
    keypad(stdscr, TRUE);
    mousemask(ALL_MOUSE_EVENTS | REPORT_MOUSE_POSITION, NULL);
    start_color();
    init_colors();
    
    // Load assets
    char* banner_text = read_asset_file(BANNER_FILE);

    // Initial state
    int choice = -1;
    int highlight = 0;
    int main_menu_items = 6;
    bool running = true;
    int current_view = 0; // 0: Main Menu, 1: Login, 2: Donation Form, etc.

    while (running) {
        clear();
        bkgd(COLOR_PAIR(COLOR_PAIR_DEFAULT));
        
        render_navbar(banner_text);

        switch (current_view) {
            case 0: // Main Menu
                render_main_menu(highlight);
                break;
            case 1: // Login View
                render_login_view();
                current_view = 0; // Return to menu after
                break;
            case 2: // Register Donation
                if (strlen(logged_in_user) > 0) {
                    render_donation_form();
                } else {
                    mvprintw(LINES / 2, (COLS - 28) / 2, "Ininia sesion;");
                    getch();
                }
                current_view = 0;
                break;
            case 3: // List Users
                render_user_list();
                current_view = 0;
                break;
            case 4: // List Donations
                render_donation_list();
                current_view = 0;
                break;
            case 5: // Info: Como Reciclar
                //render_info_view("Como Reciclar", COMO_RECICLAR_FILE);
                int box_x = (COLS - 70) /2;
                mvprintw(8, (COLS - 20) / 2, "Hablale a la logisticulona");
                draw_rounded_box(8 - 1, ( (COLS - 70) /2 ) - 2, LINES - 4,  ( (COLS - 70) /2 ) + 72);

                attroff(COLOR_PAIR(COLOR_PAIR_TITLE) | A_BOLD);

                mvprintw(10, box_x, "Ser o no ser");

                current_view = 0;
                break;
            case 6: // Info: Noticias
                //render_info_view("Noticias Recientes", NOTICIAS_FILE);
                current_view = 0;
                break;
            case 7: // Info: Centros
                //render_info_view("Centros de Acopio", CENTROS_FILE);
                mvprintw(8, (COLS - 25) / 2, "Informacion sobre Reciclaje");
                current_view = 0;
                break;
        }
        
        render_footer();
        refresh();

        if (current_view == 0) { // Only handle menu navigation if in main menu
            choice = getch();
            switch (choice) {
                case KEY_UP:
                    highlight = (highlight == 0) ? main_menu_items - 1 : highlight - 1;
                    break;
                case KEY_DOWN:
                //Controlar las clases
                    highlight = (highlight == main_menu_items - 1) ? 0 : highlight + 1;
                    break;
                case 10: // Enter key
                    if (highlight == 0) { // Iniciar Sesion
                         current_view = 1;
                    } else if (highlight == 1) { // Registrar Donacion
                        current_view = 2;
                    } else if (highlight == 2) { // Listar Usuarios
                        current_view = 3;
                    } else if (highlight == 3) { // Listar Donaciones
                        current_view = 4;
                    } else if (highlight == 4) { // Informacion
                        // This will open a submenu
                        int info_highlight = 0;
                        int info_items = 3;
                        bool in_info_menu = true;
                        while(in_info_menu) {
                            clear();
                            render_navbar(banner_text);
                            mvprintw(8, (COLS - 25) / 2, "Informacion sobre Reciclaje");
                            const char *info_options[] = {"Como reciclar", "Noticias recientes", "Centros de residuos solidos"};
                            for(int i = 0; i < info_items; i++) {
                                if(i == info_highlight) attron(COLOR_PAIR(COLOR_PAIR_BUTTON_HIGHLIGHT));
                                mvprintw(10 + i * 2, (COLS - strlen(info_options[i])) / 2, info_options[i]);
                                if(i == info_highlight) attroff(COLOR_PAIR(COLOR_PAIR_BUTTON_HIGHLIGHT));
                            }
                            render_footer();
                            refresh();
                            int info_choice = getch();
                            switch(info_choice) {
                                case KEY_UP:
                                    info_highlight = (info_highlight == 0) ? info_items - 1 : info_highlight - 1;
                                    break;
                                case KEY_DOWN:
                                    info_highlight = (info_highlight == info_items - 1) ? 0 : info_highlight + 1;
                                    break;
                                case 10:
                                    if(info_highlight == 0) current_view = 5;
                                    if(info_highlight == 1) current_view = 6;
                                    if(info_highlight == 2) current_view = 7;
                                    in_info_menu = false;
                                    break;
                                case 'q':
                                case 27: // ESC
                                    in_info_menu = false;
                                    current_view = 0;
                                    break;
                            }
                        }
                    } else if (highlight == 5) { // Salir
                        running = false;
                    }
                    break;
                case 'q':
                case 27: // ESC key
                    running = false;
                    break;
            }
        }
    }

    free(banner_text);
    endwin();
    return 0;
}

// --- Colour ---
void init_colors() {
    init_pair(COLOR_PAIR_DEFAULT, COLOR_CYAN, COLOR_BLACK);
    init_pair(COLOR_PAIR_TITLE, COLOR_YELLOW, COLOR_BLACK);
    init_pair(COLOR_PAIR_BUTTON_NORMAL, COLOR_WHITE, COLOR_BLACK);
    init_pair(COLOR_PAIR_BUTTON_HIGHLIGHT, COLOR_BLACK, COLOR_CYAN);
    init_pair(COLOR_PAIR_INPUT, COLOR_BLACK, COLOR_WHITE);
    init_pair(COLOR_PAIR_BORDER, COLOR_WHITE, COLOR_BLACK);
}

// --- Drawind utilities ---
void draw_rounded_box(int y1, int x1, int y2, int x2) {
    attron(COLOR_PAIR(COLOR_PAIR_BORDER));
    mvaddch(y1, x1, ACS_ULCORNER);
    mvaddch(y1, x2, ACS_URCORNER);
    mvaddch(y2, x1, ACS_LLCORNER);
    mvaddch(y2, x2, ACS_LRCORNER);
    mvhline(y1, x1 + 1, ACS_HLINE, x2 - x1 - 1);
    mvhline(y2, x1 + 1, ACS_HLINE, x2 - x1 - 1);
    mvvline(y1 + 1, x1, ACS_VLINE, y2 - y1 - 1);
    mvvline(y1 + 1, x2, ACS_VLINE, y2 - y1 - 1);
    attroff(COLOR_PAIR(COLOR_PAIR_BORDER));
}

char* read_asset_file(const char* filename) {
    FILE *file = fopen(filename, "r");
    if (!file) return strdup("Error: Archivo no encontrado");
    fseek(file, 0, SEEK_END);
    long length = ftell(file);
    fseek(file, 0, SEEK_SET);
    char *buffer = malloc(length + 1);
    fread(buffer, 1, length, file);
    buffer[length] = '\0';
    fclose(file);
    return buffer;
}


// --- Render components  ---

void render_navbar(const char* banner) {
    char* logo_text = read_asset_file(LOGO_FILE);
    int start_y = 1;
    int start_x = 2;
    
    // Inprimir logo
    char* line = strtok(logo_text, "\n");
    while(line != NULL) {
        mvprintw(start_y++, start_x, "%s", line);
        line = strtok(NULL, "\n");
    }
    free(logo_text);

    // banner
    attron(COLOR_PAIR(COLOR_PAIR_TITLE) | A_BOLD);
    mvprintw(3, start_x + 25, banner);
    attroff(COLOR_PAIR(COLOR_PAIR_TITLE) | A_BOLD);

    // "Inicio"
    const char* inicio_btn = " Inicio ";
    int btn_x = COLS - strlen(inicio_btn) - 4;
    draw_rounded_box(2, btn_x, 4, btn_x + strlen(inicio_btn) + 1);
    mvprintw(3, btn_x + 1, inicio_btn);
    

    mvhline(6, 0, ACS_HLINE, COLS);
}

void render_footer() {
    const char* footer_text = "lil putos incorporated 2025. [scx]";
    int y = LINES - 2;
    int x = (COLS - strlen(footer_text)) / 2;
    mvhline(y - 1, 0, ACS_HLINE, COLS);
    mvprintw(y, x, footer_text);
}


void render_main_menu(int highlight) {
    char* menu_title = "Pinche menu mamalon";
    mvprintw(8, (COLS - strlen(menu_title)) / 2, menu_title);

    const char *menu_items[] = {
        "Iniciar Sesion",
        "Registrar Donacion",
        "Listar Usuarios",
        "Listar Donaciones",
        "Informacion sobre Reciclaje",
        "Salir"
    };
    int num_items = sizeof(menu_items) / sizeof(char*);

    for (int i = 0; i < num_items; ++i) {
        if (i == 0 && strlen(logged_in_user) > 0) continue;
        if (i == 1 && strlen(logged_in_user) == 0) continue;
        
        int y = 10 + i * 2;
        int x = (COLS - strlen(menu_items[i])) / 2;
        
        if (highlight == i) {
            attron(COLOR_PAIR(COLOR_PAIR_BUTTON_HIGHLIGHT));
        } else {
            attron(COLOR_PAIR(COLOR_PAIR_BUTTON_NORMAL));
        }
        mvprintw(y, x, " %s ", menu_items[i]);
        attroff(COLOR_PAIR(COLOR_PAIR_BUTTON_HIGHLIGHT));
        attroff(COLOR_PAIR(COLOR_PAIR_BUTTON_NORMAL));
    }
}

void render_login_view() {
    clear();
    render_navbar(read_asset_file(BANNER_FILE));
    
    int form_y = LINES / 2 - 3;
    int form_x = (COLS - 50) / 2;
    draw_rounded_box(form_y - 1, form_x - 2, form_y + 5, form_x + 52);

    mvprintw(form_y, form_x, "Inicio de Sesion");

    mvprintw(form_y + 2, form_x, "Numero de Control: ");
    attron(COLOR_PAIR(COLOR_PAIR_INPUT));
    for(int i=0; i < MAX_CONTROL_NUMBER_LENGTH; i++) mvprintw(form_y + 2, form_x + 20 + i, " ");
    attroff(COLOR_PAIR(COLOR_PAIR_INPUT));
    
    mvprintw(form_y + 4, form_x, "Nombre: ");
    attron(COLOR_PAIR(COLOR_PAIR_INPUT));
    for(int i=0; i < MAX_NAME_LENGTH; i++) mvprintw(form_y + 4, form_x + 20 + i, " ");
    attroff(COLOR_PAIR(COLOR_PAIR_INPUT));
    
    refresh();

    char control_num[MAX_CONTROL_NUMBER_LENGTH];
    char name[MAX_NAME_LENGTH];

    echo();
    curs_set(1);
    
    attron(COLOR_PAIR(COLOR_PAIR_INPUT));
    mvwgetnstr(stdscr, form_y + 2, form_x + 20, control_num, MAX_CONTROL_NUMBER_LENGTH - 1);
    mvwgetnstr(stdscr, form_y + 4, form_x + 20, name, MAX_NAME_LENGTH - 1);
    attroff(COLOR_PAIR(COLOR_PAIR_INPUT));

    noecho();
    curs_set(0);

    bool user_found = false;
    for (int i = 0; i < user_count; i++) {
        if (strcmp(users[i].control_number, control_num) == 0) {
            user_found = true;
            break;
        }
    }

    if (!user_found && user_count < MAX_USERS) {
        strcpy(users[user_count].control_number, control_num);
        strcpy(users[user_count].name, name);
        user_count++;
    }
    
    strcpy(logged_in_user, control_num);

    mvprintw(form_y + 7, form_x, "Hola, %s! Ayuda.", name);
    getch();
}


void render_donation_form() {
    clear();
    render_navbar(read_asset_file(BANNER_FILE));

    int form_y = LINES / 2 - 4;
    int form_x = (COLS - 40) / 2;
    draw_rounded_box(form_y - 1, form_x - 2, form_y + 8, form_x + 42);

    mvprintw(form_y, form_x, "Registrar Donacion");

    mvprintw(form_y + 2, form_x, "Papel (kg): ");
    mvprintw(form_y + 4, form_x, "Plastico (kg): ");
    mvprintw(form_y + 6, form_x, "Aluminio (kg): ");

    refresh();
    
    char paper_str[10], plastic_str[10], aluminum_str[10];

    echo();
    curs_set(1);
    attron(COLOR_PAIR(COLOR_PAIR_INPUT));
    mvwgetnstr(stdscr, form_y + 2, form_x + 15, paper_str, 9);
    mvwgetnstr(stdscr, form_y + 4, form_x + 15, plastic_str, 9);
    mvwgetnstr(stdscr, form_y + 6, form_x + 15, aluminum_str, 9);
    attroff(COLOR_PAIR(COLOR_PAIR_INPUT));
    noecho();
    curs_set(0);

    if (donation_count < MAX_DONATIONS) {
        strcpy(donations[donation_count].user_control_number, logged_in_user);
        donations[donation_count].paper = atoi(paper_str);
        donations[donation_count].plastic = atoi(plastic_str);
        donations[donation_count].aluminum = atoi(aluminum_str);
        donation_count++;
        mvprintw(form_y + 9, form_x, "Su donacion ha sido registrada, Presiona una tecla.");
    } else {
        mvprintw(form_y + 9, form_x, "DB Overflow");
    }

    getch();
}

void render_user_list() {
    clear();
    render_navbar(read_asset_file(BANNER_FILE));
    
    int list_y = 8;
    int list_x = (COLS - 60) / 2;
    draw_rounded_box(list_y - 1, list_x - 2, LINES - 4, list_x + 62);
    mvprintw(list_y, list_x, "Usuarios Registrados");
    mvprintw(list_y + 2, list_x, "No. Control         | Nombre");
    mvhline(list_y + 3, list_x, '-', 50);

    if (user_count == 0) {
        mvprintw(list_y + 4, list_x, "No hay usuarios registrados.");
    } else {
        for (int i = 0; i < user_count; i++) {
            mvprintw(list_y + 4 + i, list_x, "%-20s| %s", users[i].control_number, users[i].name);
        }
    }
    
    mvprintw(LINES - 5, list_x, "Presiona una tecla para volver.");
    getch();
}

void render_donation_list() {
    clear();
    render_navbar(read_asset_file(BANNER_FILE));

    int list_y = 8;
    int list_x = (COLS - 70) / 2;
    draw_rounded_box(list_y - 1, list_x - 2, LINES - 4, list_x + 72);
    mvprintw(list_y, list_x, "Lista de Donaciones");
    mvprintw(list_y + 2, list_x, "Usuario (No. Control) | Papel (kg) | Plastico (kg) | Aluminio (kg)");
    mvhline(list_y + 3, list_x, '-', 70);

    if (donation_count == 0) {
        mvprintw(list_y + 4, list_x, "No hay donaciones registradas.");
    } else {
        for (int i = 0; i < donation_count; i++) {
            mvprintw(list_y + 4 + i, list_x, "%-25s | %-10d | %-13d | %d",
                     donations[i].user_control_number,
                     donations[i].paper,
                     donations[i].plastic,
                     donations[i].aluminum);
        }
    }
    
    mvprintw(LINES - 5, list_x, "Presiona una tecla para volver.");
    getch();
}

void render_info_view(const char* title, const char* content_file) {
    clear();
    render_navbar(read_asset_file(BANNER_FILE));
    
    char* content = read_asset_file(content_file);
    int box_y = 8;
    int box_x = (COLS - 70) / 2;
    draw_rounded_box(box_y - 1, box_x - 2, LINES - 4, box_x + 72);
    
    attron(COLOR_PAIR(COLOR_PAIR_TITLE) | A_BOLD);
    mvprintw(box_y, box_x, title);
    attroff(COLOR_PAIR(COLOR_PAIR_TITLE) | A_BOLD);

    int content_y = box_y + 2;
    char* line = strtok(content, "\n");
    while(line != NULL && content_y < LINES - 5) {
        mvprintw(content_y++, box_x, line);
        line = strtok(NULL, "\n");
    }
    free(content);

    mvprintw(LINES - 5, box_x, "Presiona una tecla para volver.");
    getch();
}
