#include <gtk/gtk.h>
#include <stdio.h>
#include "../include/gui.h"
#include "../include/management.h"

static void activate(GtkApplication *app, gpointer user_data) {
    AppWidgets *widgets = (AppWidgets *)user_data;
    
    /* Load Binary Data */
    load_from_file(widgets->students, &widgets->student_count);
    
    /* Create GUI */
    create_main_window(app, widgets);
    
    /* Global CSS Provider */
    GtkCssProvider *css_provider = gtk_css_provider_new();
    gtk_css_provider_load_from_path(css_provider, "resources/style.css", NULL);
    gtk_style_context_add_provider_for_screen(gdk_screen_get_default(),
                                            GTK_STYLE_PROVIDER(css_provider),
                                            GTK_STYLE_PROVIDER_PRIORITY_USER);
    
    gtk_widget_show_all(widgets->window);
}

int main(int argc, char **argv) {
    AppWidgets *widgets = g_new0(AppWidgets, 1);
    
    GtkApplication *app = gtk_application_new("com.student.manager", G_APPLICATION_FLAGS_NONE);
    g_signal_connect(app, "activate", G_CALLBACK(activate), widgets);
    
    int status = g_application_run(G_APPLICATION(app), argc, argv);
    
    /* Cleanup */
    /* save_to_file(widgets->students, widgets->student_count); */
    g_object_unref(app);
    g_free(widgets);
    
    return status;
}
