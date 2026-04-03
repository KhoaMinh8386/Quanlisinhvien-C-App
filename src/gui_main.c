#include <gtk/gtk.h>
#include "../include/gui.h"

/* --- Create Sidebar widget --- */
GtkWidget *create_sidebar(AppWidgets *widgets) {
    GtkWidget *sidebar = gtk_box_new(GTK_ORIENTATION_VERTICAL, 0);
    gtk_style_context_add_class(gtk_widget_get_style_context(sidebar), "bg_sidebar");
    gtk_widget_set_size_request(sidebar, 240, -1);

    /* Logo/Title */
    GtkWidget *logo_box = gtk_box_new(GTK_ORIENTATION_VERTICAL, 5);
    gtk_container_set_border_width(GTK_CONTAINER(logo_box), 30);
    
    GtkWidget *lbl_logo = gtk_label_new(NULL);
    gtk_label_set_markup(GTK_LABEL(lbl_logo), "<span size='x-large' weight='bold'>🎓 QUẢN LÝ</span>");
    GtkWidget *lbl_sub = gtk_label_new("Sinh Viên & Tài Chính");
    
    gtk_box_pack_start(GTK_BOX(logo_box), lbl_logo, FALSE, FALSE, 0);
    gtk_box_pack_start(GTK_BOX(logo_box), lbl_sub, FALSE, FALSE, 0);
    gtk_box_pack_start(GTK_BOX(sidebar), logo_box, FALSE, FALSE, 0);

    /* Sidebar Buttons */
    const char *btn_labels[] = {"👥 Danh Sách SV", "📊 Báo Cáo TC", "🔍 Tìm Kiếm", "💾 Lưu Dữ Liệu", "📂 Tải Dữ Liệu"};
    
    for (int i = 0; i < 5; i++) {
        GtkWidget *btn = gtk_button_new_with_label(btn_labels[i]);
        gtk_style_context_add_class(gtk_widget_get_style_context(btn), "sidebar-btn");
        gtk_box_pack_start(GTK_BOX(sidebar), btn, FALSE, FALSE, 10);
        
        /* Signal assignments */
        if (i == 0) g_signal_connect_swapped(btn, "clicked", G_CALLBACK(gtk_notebook_set_current_page), widgets->notebook);
        if (i == 1) g_signal_connect_swapped(btn, "clicked", G_CALLBACK(gtk_notebook_set_current_page), widgets->notebook + 1); /* +1 hack conceptually */
        if (i == 2) g_signal_connect_swapped(btn, "clicked", G_CALLBACK(show_search_dialog), widgets);
    }
    
    return sidebar;
}

/* --- Create Main Window Content --- */
void create_main_window(GtkApplication *app, AppWidgets *widgets) {
    widgets->window = gtk_application_window_new(app);
    gtk_window_set_title(GTK_WINDOW(widgets->window), "🎓 Hệ Thống Quản Lý Sinh Viên & Tài Chính");
    gtk_window_set_default_size(GTK_WINDOW(widgets->window), 1300, 800);
    gtk_window_set_position(GTK_WINDOW(widgets->window), GTK_WIN_POS_CENTER);

    /* Layout structure: HBox (sidebar | content) */
    GtkWidget *main_hbox = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 0);
    gtk_container_add(GTK_CONTAINER(widgets->window), main_hbox);

    /* Notebook for tabs */
    widgets->notebook = gtk_notebook_new();
    gtk_notebook_set_show_tabs(GTK_NOTEBOOK(widgets->notebook), FALSE); /* Hide tabs, switch via sidebar */
    
    /* GtkPaned to resize sidebar */
    GtkWidget *paned = gtk_paned_new(GTK_ORIENTATION_HORIZONTAL);
    gtk_box_pack_start(GTK_BOX(main_hbox), paned, TRUE, TRUE, 0);
    
    GtkWidget *sidebar = create_sidebar(widgets);
    gtk_paned_add1(GTK_PANED(paned), sidebar);
    
    /* Pages */
    GtkWidget *list_tab = create_list_tab(widgets);
    gtk_notebook_append_page(GTK_NOTEBOOK(widgets->notebook), list_tab, NULL);
    
    GtkWidget *report_tab = create_report_tab(widgets);
    gtk_notebook_append_page(GTK_NOTEBOOK(widgets->notebook), report_tab, NULL);

    gtk_paned_add2(GTK_PANED(paned), widgets->notebook);
    gtk_paned_set_position(GTK_PANED(paned), 240);

    /* Add shortcuts */
    /* TODO: Ctrl+N, Ctrl+S */
}
