//Members: Baleda, Garcia, Villanueva
//Description: GUI for Project

#include <gtk/gtk.h>

//static GtkTextBuffer *buffer;
static GtkWidget *widget;
static GtkWidget *dow_stk;
static GtkWidget *og_layout;
static GtkWidget *choose_view;
static GtkWidget *file_read;

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

static void file_chosen(GtkButton *button, gpointer data){
    GtkFileChooser *chose = GTK_FILE_CHOOSER(data);
    if(!GTK_IS_FILE_CHOOSER(chose)){
        return;
    }
    GFile *file = gtk_file_chooser_get_file(chose);
    if (file){
        char *file_path = g_file_get_path(file);

        if(g_str_has_suffix(file_path, ".lol")){
            g_print("File chosen: %s\n", file_path);
            load_file(file_path);
        } else {
            if(GTK_IS_TEXT_VIEW(file_read)){
                g_print("Hello!");
                GtkTextBuffer *buff = gtk_text_view_get_buffer(GTK_TEXT_VIEW(file_read));
                gtk_text_buffer_set_text(buff, "Only LOLCODE files are allowed.", -1);
            }
        }
        g_free(file_path);
        g_object_unref(file);
        
    }

    gtk_stack_set_visible_child(GTK_STACK(dow_stk), "main");
}

static void cancel_file(GtkButton *button, gpointer data) {
    gtk_stack_set_visible_child(GTK_STACK(dow_stk), "main");
}

static void open_file(GtkButton *button, gpointer data){
    GtkWidget *choose_box = gtk_box_new(GTK_ORIENTATION_VERTICAL, 5);

    GtkWidget *chose = gtk_file_chooser_widget_new(GTK_FILE_CHOOSER_ACTION_OPEN);
    gtk_widget_set_vexpand(chose, TRUE);
    gtk_widget_set_hexpand(chose, TRUE);

    GtkFileFilter *file_filter = gtk_file_filter_new();
    gtk_file_filter_add_pattern(file_filter, "*.lol");
    gtk_file_chooser_add_filter(GTK_FILE_CHOOSER(chose), file_filter);
    gtk_box_append(GTK_BOX(choose_box), chose);

    GtkWidget *button_box = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 5);
    GtkWidget *open_btn = gtk_button_new_with_label("Open");
    GtkWidget *cancel_btn = gtk_button_new_with_label("Cancel");
    gtk_box_append(GTK_BOX(button_box), open_btn);
    gtk_box_append(GTK_BOX(button_box), cancel_btn);
    gtk_box_append(GTK_BOX(choose_box), button_box);

    g_signal_connect(open_btn, "clicked", G_CALLBACK(file_chosen), chose);
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
    widget = gtk_application_window_new(app);
    gtk_window_set_title(GTK_WINDOW(widget), "LOLCODE");
    gtk_window_set_default_size(GTK_WINDOW(widget), 800, 600);

    dow_stk = gtk_stack_new();
    gtk_window_set_child(GTK_WINDOW(widget), dow_stk);

    og_layout = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 10);

    GtkWidget *vertical_left = gtk_box_new(GTK_ORIENTATION_VERTICAL, 5);
    gtk_box_append(GTK_BOX(og_layout), vertical_left);

    GtkWidget *open_button = gtk_button_new_with_label("Browse File");
    gtk_widget_set_size_request(open_button, 100, 30);
    gtk_box_append(GTK_BOX(vertical_left), open_button);
    g_signal_connect(open_button, "clicked", G_CALLBACK(open_file), NULL);

    file_read = gtk_text_view_new();
    gtk_text_view_set_editable(GTK_TEXT_VIEW(file_read), TRUE);

    GtkWidget *scrolling = gtk_scrolled_window_new();
    gtk_scrolled_window_set_child(GTK_SCROLLED_WINDOW(scrolling), file_read);
    gtk_widget_set_vexpand(scrolling, TRUE);
    //gtk_widget_set_hexpand(scrolling, TRUE);
    gtk_box_append(GTK_BOX(vertical_left), scrolling);

    GtkWidget *vertical_right = gtk_box_new(GTK_ORIENTATION_VERTICAL, 5);
    gtk_box_append(GTK_BOX(og_layout), vertical_right);

    GtkWidget *process_label = gtk_label_new("Process");
    gtk_box_append(GTK_BOX(vertical_right), process_label);

    GtkWidget *view = gtk_text_view_new();
    gtk_text_view_set_editable(GTK_TEXT_VIEW(view), FALSE);
    gtk_widget_set_vexpand(view, TRUE);
    gtk_widget_set_hexpand(view, TRUE);
    gtk_box_append(GTK_BOX(vertical_right), view);

    gtk_stack_add_named(GTK_STACK(dow_stk), og_layout, "main");
    gtk_stack_set_visible_child_name(GTK_STACK(dow_stk), "main");

    gtk_window_present(GTK_WINDOW(widget));
}

int main(int argc, char **argv){
    GtkApplication *app;
    int status;

    app = gtk_application_new("com.example.gtkapp", G_APPLICATION_DEFAULT_FLAGS);
    g_signal_connect(app, "activate", G_CALLBACK(activate), NULL);
    status = g_application_run(G_APPLICATION(app), argc, argv);
    g_object_unref(app);
    return status;
}