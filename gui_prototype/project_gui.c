//Members: Baleda, Garcia, Villanueva
//Description: GUI for Project

#include <gtk/gtk.h>

GtkTextBuffer *buffer;

//load and display a file
static void load_file(const char *file_name, gpointer data){
    gchar *content = NULL;
    gsize length = 0;

    if(g_file_get_contents(file_name, &content, &length, NULL)){
        gtk_text_buffer_set_text(buffer, content, length);
        g_free(content);
    } else {
        gtk_text_buffer_set_text(buffer, "Unable to read file.", -1);
    }
}

//function caller for open_file
static void file_open(GObject *source, GAsyncResult *res, gpointer data){
    GtkFileDialog *new_dialog = GTK_FILE_DIALOG(source);
    GFile *file = gtk_file_dialog_open_finish(new_dialog, res, NULL);
        if (file != NULL){
            char *file_path = g_file_get_path(file);
            load_file(file_path, data);
            g_free(file_path);
            g_object_unref(file);
        }
    
}

//when open file is clicked
static void open_file(GtkButton *button, gpointer data){
    GtkWindow *opened = GTK_WINDOW(data);
    GtkFileDialog *dialog = gtk_file_dialog_new();

    gtk_file_dialog_open(dialog, opened, NULL, file_open, opened);

}

//main gtk window
static void activate(GtkApplication *app, gpointer data){
    GtkWidget *widget = gtk_application_window_new(app);
    gtk_window_set_title(GTK_WINDOW(widget), "LOLCODE");
    gtk_window_set_default_size(GTK_WINDOW(widget), 800, 600);

    GtkWidget *vertical_layout = gtk_box_new(GTK_ORIENTATION_VERTICAL, 5);
    gtk_window_set_child(GTK_WINDOW(widget), vertical_layout);

    GtkWidget *open_button = gtk_button_new_with_label("Browse File");
    gtk_box_append(GTK_BOX(vertical_layout), open_button);

    GtkWidget *file_read = gtk_text_view_new();
    buffer = gtk_text_view_get_buffer(GTK_TEXT_VIEW(file_read));
    gtk_text_view_set_editable(GTK_TEXT_VIEW(file_read), TRUE);

    GtkWidget *scrolling = gtk_scrolled_window_new();
    gtk_scrolled_window_set_child(GTK_SCROLLED_WINDOW(scrolling), file_read);
    gtk_box_append(GTK_BOX(vertical_layout), scrolling);

    g_signal_connect(open_button, "clicked", G_CALLBACK(open_file), widget);

    gtk_widget_show(widget);
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