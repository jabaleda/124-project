//Members: Baleda, Garcia, Villanueva
//Description: GUI for Project

#include <gtk/gtk.h>

static GtkWidget *widget;
static GtkWidget *dow_stk;
static GtkWidget *og_vertical;
static GtkWidget *og_layout;
static GtkWidget *choose_view;
static GtkWidget *file_read;
static GListStore *lex_store;
static GListStore *st_store;
static GtkWidget *terminal_view;

static char file_pathname[1000];

#define LEXEME_ITEM_TYPE lexeme_item_get_type()
#define SYMBOL_ITEM_TYPE symbol_item_get_type()

#ifndef ALL_C
#define ALL_C
#include "all.c"
#endif

typedef struct _LexemeItem {
    GObject parent_instance;
    gchar *lexeme;
    gchar *classification;
} LexemeItem;

typedef struct _LexemeItemClass {
    GObjectClass parent_class;
} LexemeItemClass;

G_DEFINE_TYPE(LexemeItem, lexeme_item, G_TYPE_OBJECT);

static void lexeme_item_class_init(LexemeItemClass *lex_class) {}
static void lexeme_item_init(LexemeItem *self) {}

typedef struct _SymbolItem {
    GObject parent_instance;
    gchar *modifier;
    gchar *value;
} SymbolItem;

typedef struct _SymbolItemClass {
    GObjectClass parent_class;
} SymbolItemClass;

G_DEFINE_TYPE(SymbolItem, symbol_item, G_TYPE_OBJECT);

static void symbol_item_class_init(SymbolItemClass *sym) {}
static void symbol_item_init(SymbolItem *self) {}

//load and display a file
static void load_file(const char *file_name){
    gchar *content = NULL;
    gsize length = 0;

    GtkTextBuffer *buff = gtk_text_view_get_buffer(GTK_TEXT_VIEW(file_read));
    if (!buff){
        return;
    }

    if(g_file_get_contents(file_name, &content, &length, NULL)){
        gtk_text_buffer_set_text(buff, content, length);
        g_free(content);
    } else {
        gtk_text_buffer_set_text(buff, "Unable to read file.", -1);
    }
}

//when open button is clicked (handles file choosing)
static void file_chosen(GtkButton *button, gpointer data){
    GtkFileChooser *chose = GTK_FILE_CHOOSER(data);
    if(!GTK_IS_FILE_CHOOSER(chose)){
        return;
    }
    GFile *file = gtk_file_chooser_get_file(chose);
    if (file){
        char *file_path = g_file_get_path(file);

        //only locate .lol files
        if(g_str_has_suffix(file_path, ".lol")){
            g_print("File chosen: %s\n", file_path);
            gtk_text_view_set_editable(GTK_TEXT_VIEW(file_read), TRUE);
            load_file(file_path);

            strcpy(file_pathname, file_path);

        } else {
            //check if file_read is NULL
            if(GTK_IS_TEXT_VIEW(file_read)){
                g_print("Hello!");
                gtk_text_view_set_editable(GTK_TEXT_VIEW(file_read), FALSE);
                GtkTextBuffer *buff = gtk_text_view_get_buffer(GTK_TEXT_VIEW(file_read));
                gtk_text_buffer_set_text(buff, "Only LOLCODE files are allowed.", -1);
            }
        }
        g_free(file_path);
        g_object_unref(file);
        
    }

    gtk_stack_set_visible_child_name(GTK_STACK(dow_stk), "main");
}

//When cancel button is clicked (returns back to main window)
static void cancel_file(GtkButton *button, gpointer data) {
    gtk_stack_set_visible_child_name(GTK_STACK(dow_stk), "main");
}

// Sets up a label for each cell
static void on_setup_lexeme(GtkListItemFactory *factory, GtkListItem *item, gpointer data) {
    GtkWidget *label = gtk_label_new("");
    gtk_list_item_set_child(item, label);
}

static void on_bind_lexeme(GtkListItemFactory *factory, GtkListItem *item, gpointer data) {
    GtkWidget *label = gtk_list_item_get_child(item);
    LexemeItem *lex = (LexemeItem *)gtk_list_item_get_item(item);
    gtk_label_set_text(GTK_LABEL(label), lex->lexeme);
}

static void on_setup_class(GtkListItemFactory *factory, GtkListItem *item, gpointer data) {
    GtkWidget *label = gtk_label_new("");
    gtk_list_item_set_child(item, label);
}

static void on_bind_class(GtkListItemFactory *factory, GtkListItem *item, gpointer data) {
    GtkWidget *label = gtk_list_item_get_child(item);
    LexemeItem *lex = (LexemeItem *)gtk_list_item_get_item(item);
    gtk_label_set_text(GTK_LABEL(label), lex->classification);
}

// Modifier column
static void on_setup_symbol_modifier(GtkListItemFactory *factory, GtkListItem *item, gpointer data) {
    GtkWidget *label = gtk_label_new("");
    gtk_list_item_set_child(item, label);
}

static void on_bind_symbol_modifier(GtkListItemFactory *factory, GtkListItem *item, gpointer data) {
    GtkWidget *label = gtk_list_item_get_child(item);
    SymbolItem *sym = (SymbolItem *) gtk_list_item_get_item(item);
    gtk_label_set_text(GTK_LABEL(label), sym->modifier);
}

// Value column
static void on_setup_symbol_value(GtkListItemFactory *factory, GtkListItem *item, gpointer data) {
    GtkWidget *label = gtk_label_new("");
    gtk_list_item_set_child(item, label);
}

static void on_bind_symbol_value(GtkListItemFactory *factory, GtkListItem *item, gpointer data) {
    GtkWidget *label = gtk_list_item_get_child(item);
    SymbolItem *sym = (SymbolItem *) gtk_list_item_get_item(item);
    gtk_label_set_text(GTK_LABEL(label), sym->value);
}

static void click_exec(GtkButton *button, gpointer data){
    GtkTextBuffer *buff = gtk_text_view_get_buffer(GTK_TEXT_VIEW(terminal_view));
    gtk_text_buffer_insert_at_cursor(buff, "Executing...\n", -1);
    // gtk_text_buffer_insert_at_cursor(buff, file_pathname, -1);
    execute_code(file_pathname, terminal_view, buff);
}

//open file chooser menu
static void open_file(GtkButton *button, gpointer data){
    GtkWidget *choose_box = gtk_box_new(GTK_ORIENTATION_VERTICAL, 5);

    GtkWidget *chose = gtk_file_chooser_widget_new(GTK_FILE_CHOOSER_ACTION_OPEN);
    gtk_widget_set_vexpand(chose, TRUE);
    gtk_widget_set_hexpand(chose, TRUE);

    GtkFileFilter *file_filter = gtk_file_filter_new();
    gtk_file_filter_add_pattern(file_filter, "*.lol");
    gtk_file_chooser_add_filter(GTK_FILE_CHOOSER(chose), file_filter);
    gtk_box_append(GTK_BOX(choose_box), chose);

    //set of buttons
    GtkWidget *button_box = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 5);
    GtkWidget *open_btn = gtk_button_new_with_label("Open");
    GtkWidget *cancel_btn = gtk_button_new_with_label("Cancel");
    gtk_box_append(GTK_BOX(button_box), open_btn);
    gtk_box_append(GTK_BOX(button_box), cancel_btn);
    gtk_box_append(GTK_BOX(choose_box), button_box);

    //call file_chosen for open 
    g_signal_connect(open_btn, "clicked", G_CALLBACK(file_chosen), chose);
    //call cancel_file for cancel
    g_signal_connect(cancel_btn, "clicked", G_CALLBACK(cancel_file), NULL);

    if(choose_view){
        gtk_stack_remove(GTK_STACK(dow_stk), choose_view);
    }
    choose_view = choose_box;
    gtk_stack_add_named(GTK_STACK(dow_stk), choose_view, "Choose a File");
    gtk_stack_set_visible_child_name(GTK_STACK(dow_stk), "Choose a File");
}

//main gtk window
static void activate(GtkApplication *app, gpointer data){
    //new window
    widget = gtk_application_window_new(app);
    gtk_window_set_title(GTK_WINDOW(widget), "LOLCODE");
    gtk_window_set_default_size(GTK_WINDOW(widget), 800, 600);

    dow_stk = gtk_stack_new();
    gtk_window_set_child(GTK_WINDOW(widget), dow_stk);

    //vertical box
    og_vertical = gtk_box_new(GTK_ORIENTATION_VERTICAL, 10);
    //horizontal box inside of original vertical box
    og_layout = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 10);

    //text editor
    GtkWidget *vertical_left = gtk_box_new(GTK_ORIENTATION_VERTICAL, 5);
    gtk_box_append(GTK_BOX(og_layout), vertical_left);

    //browse file button
    GtkWidget *open_button = gtk_button_new_with_label("Browse File");
    gtk_widget_set_size_request(open_button, 100, 30);
    gtk_box_append(GTK_BOX(vertical_left), open_button);
    g_signal_connect(open_button, "clicked", G_CALLBACK(open_file), NULL);

    file_read = gtk_text_view_new();

    //scrollable box
    GtkWidget *scrolling = gtk_scrolled_window_new();
    gtk_scrolled_window_set_child(GTK_SCROLLED_WINDOW(scrolling), file_read);
    gtk_widget_set_vexpand(scrolling, TRUE);
    gtk_widget_set_hexpand(scrolling, TRUE);
    gtk_box_append(GTK_BOX(vertical_left), scrolling);

    //lexeme list
    GtkWidget *vertical_middle = gtk_box_new(GTK_ORIENTATION_VERTICAL, 5);
    gtk_box_append(GTK_BOX(og_layout), vertical_middle);

    //label of lexemes
    GtkWidget *process_label = gtk_label_new("Lexemes");
    gtk_box_append(GTK_BOX(vertical_middle), process_label);

    lex_store = g_list_store_new(LEXEME_ITEM_TYPE);
    GtkSelectionModel *select = GTK_SELECTION_MODEL(gtk_multi_selection_new(G_LIST_MODEL(lex_store)));

    GtkWidget *view = gtk_column_view_new(select);
    gtk_widget_set_vexpand(view, TRUE);
    gtk_widget_set_hexpand(view, TRUE);

    //Lexeme
    GtkListItemFactory *lex_factory = gtk_signal_list_item_factory_new();
    g_signal_connect(lex_factory, "setup", G_CALLBACK(on_setup_lexeme), NULL);
    g_signal_connect(lex_factory, "bind",  G_CALLBACK(on_bind_lexeme),  NULL);
    GtkColumnViewColumn *col_lexeme = gtk_column_view_column_new("Lexeme", lex_factory);
    gtk_column_view_append_column(GTK_COLUMN_VIEW(view), col_lexeme);
    gtk_column_view_column_set_expand(col_lexeme, TRUE);

    //Classification
    GtkListItemFactory *class_factory = gtk_signal_list_item_factory_new();
    g_signal_connect(class_factory, "setup", G_CALLBACK(on_setup_class), NULL);
    g_signal_connect(class_factory, "bind",  G_CALLBACK(on_bind_class),  NULL);
    GtkColumnViewColumn *col_class = gtk_column_view_column_new("Classification", class_factory);
    gtk_column_view_append_column(GTK_COLUMN_VIEW(view), col_class);
    gtk_column_view_column_set_expand(col_class, TRUE);

    //scrollable box
    GtkWidget *lex_scroll = gtk_scrolled_window_new();
    gtk_scrolled_window_set_child(GTK_SCROLLED_WINDOW(lex_scroll), view);
    gtk_widget_set_vexpand(lex_scroll, TRUE);
    gtk_widget_set_hexpand(lex_scroll, TRUE);
    gtk_box_append(GTK_BOX(vertical_middle), lex_scroll);

    //symbol table
    GtkWidget *vertical_right = gtk_box_new(GTK_ORIENTATION_VERTICAL, 5);
    gtk_box_append(GTK_BOX(og_layout), vertical_right);

    GtkWidget *stable_label = gtk_label_new("Symbol Table");
    gtk_box_append(GTK_BOX(vertical_right), stable_label);

    st_store = g_list_store_new(SYMBOL_ITEM_TYPE);
    GtkSelectionModel *select2 = GTK_SELECTION_MODEL(gtk_multi_selection_new(G_LIST_MODEL(st_store)));

    GtkWidget *view2 = gtk_column_view_new(select2);
    gtk_widget_set_vexpand(view2, TRUE);
    gtk_widget_set_hexpand(view2, TRUE);

    //Modifier
    GtkListItemFactory *modifier_factory = gtk_signal_list_item_factory_new();
    g_signal_connect(modifier_factory, "setup", G_CALLBACK(on_setup_symbol_modifier), NULL);
    g_signal_connect(modifier_factory, "bind", G_CALLBACK(on_bind_symbol_modifier), NULL);

    GtkColumnViewColumn *col_modifier = gtk_column_view_column_new("Modifier", modifier_factory);
    gtk_column_view_column_set_expand(col_modifier, TRUE); // fill space
    gtk_column_view_append_column(GTK_COLUMN_VIEW(view2), col_modifier);

    //Value
    GtkListItemFactory *value_factory = gtk_signal_list_item_factory_new();
    g_signal_connect(value_factory, "setup", G_CALLBACK(on_setup_symbol_value), NULL);
    g_signal_connect(value_factory, "bind", G_CALLBACK(on_bind_symbol_value), NULL);

    GtkColumnViewColumn *col_value = gtk_column_view_column_new("Value", value_factory);
    gtk_column_view_column_set_expand(col_value, TRUE);
    gtk_column_view_append_column(GTK_COLUMN_VIEW(view2), col_value);

    //scrollable box
    GtkWidget *symbol_scroll = gtk_scrolled_window_new();
    gtk_scrolled_window_set_child(GTK_SCROLLED_WINDOW(symbol_scroll), view2);
    gtk_widget_set_vexpand(symbol_scroll, TRUE);
    gtk_widget_set_hexpand(symbol_scroll, TRUE);
    gtk_box_append(GTK_BOX(vertical_right), symbol_scroll);


    //terminal
    GtkWidget *bottom_box = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 10);
    gtk_widget_set_hexpand(bottom_box, TRUE);
    gtk_widget_set_vexpand(bottom_box, TRUE);

    //label of console
    GtkWidget *con_label = gtk_label_new("Console:");
    gtk_box_append(GTK_BOX(bottom_box), con_label);

    //execute button
    GtkWidget *execute = gtk_button_new_with_label("Execute");
    gtk_widget_set_size_request(execute, 100, 30);
    gtk_box_append(GTK_BOX(bottom_box), execute);

    //assess view of console
    terminal_view = gtk_text_view_new();
    gtk_text_view_set_editable(GTK_TEXT_VIEW(terminal_view), TRUE);
    gtk_text_view_set_wrap_mode(GTK_TEXT_VIEW(terminal_view), GTK_WRAP_WORD_CHAR);
    gtk_widget_set_hexpand(terminal_view, TRUE);
    gtk_widget_set_vexpand(terminal_view, TRUE);

    //scrollable box
    GtkWidget *term_scroll = gtk_scrolled_window_new();
    gtk_scrolled_window_set_child(GTK_SCROLLED_WINDOW(term_scroll), terminal_view);
    gtk_widget_set_hexpand(term_scroll, TRUE);
    gtk_widget_set_vexpand(term_scroll, TRUE);

    gtk_box_append(GTK_BOX(bottom_box), term_scroll);

    //call click_exec to handle button
    g_signal_connect(execute, "clicked", G_CALLBACK(click_exec), NULL);

    gtk_box_append(GTK_BOX(og_vertical), og_layout);
    gtk_box_append(GTK_BOX(og_vertical), bottom_box);

    gtk_stack_add_named(GTK_STACK(dow_stk), og_vertical, "main");
    gtk_stack_set_visible_child_name(GTK_STACK(dow_stk), "main");

    gtk_window_present(GTK_WINDOW(widget));
}

//main function
int main(int argc, char **argv){
    GtkApplication *app;
    int status;

#ifndef G_APPLICATION_DEFAULT_FLAGS
    #define G_APPLICATION_DEFAULT_FLAGS G_APPLICATION_FLAGS_NONE
#endif

    app = gtk_application_new("com.example.gtkapp", G_APPLICATION_DEFAULT_FLAGS);
    g_signal_connect(app, "activate", G_CALLBACK(activate), NULL);
    status = g_application_run(G_APPLICATION(app), argc, argv);
    g_object_unref(app);
    return status;
}