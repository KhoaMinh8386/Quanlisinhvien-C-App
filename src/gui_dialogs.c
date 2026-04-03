#include <gtk/gtk.h>
#include "../include/gui.h"

/* --- Show Generic Message Dialog --- */
void show_message(GtkWindow *parent, GtkMessageType type, const char *title, const char *msg) {
    GtkWidget *dialog = gtk_message_dialog_new(parent, GTK_DIALOG_MODAL, type, GTK_BUTTONS_OK, "%s", msg);
    gtk_window_set_title(GTK_WINDOW(dialog), title);
    gtk_dialog_run(GTK_DIALOG(dialog));
    gtk_widget_destroy(dialog);
}

/* --- Show Confirmation for Deletion --- */
void show_confirm_delete(AppWidgets *widgets) {
    GtkTreeSelection *selection = gtk_tree_view_get_selection(GTK_TREE_VIEW(widgets->treeview));
    GtkTreeIter iter;
    GtkTreeModel *model;
    
    if (gtk_tree_selection_get_selected(selection, &model, &iter)) {
        int id;
        gtk_tree_model_get(model, &iter, 0, &id, -1);
        
        GtkWidget *dialog = gtk_message_dialog_new(GTK_WINDOW(widgets->window), GTK_DIALOG_MODAL,
                            GTK_MESSAGE_QUESTION, GTK_BUTTONS_YES_NO,
                            "Bạn có chắc chắn muốn xóa sinh viên ID %d không?", id);
        gtk_window_set_title(GTK_WINDOW(dialog), "Xác nhận xóa");
        
        if (gtk_dialog_run(GTK_DIALOG(dialog)) == GTK_RESPONSE_YES) {
            /* Logic xóa */
            /* delete_student(widgets->students, &widgets->student_count, id); */
            /* refresh_student_list(widgets); */
            show_message(GTK_WINDOW(widgets->window), GTK_MESSAGE_INFO, "Thông báo", "Đã xóa sinh viên.");
        }
        
        gtk_widget_destroy(dialog);
    } else {
        show_message(GTK_WINDOW(widgets->window), GTK_MESSAGE_WARNING, "Cảnh báo", "Vui lòng chọn 1 sinh viên trong danh sách.");
    }
}

/* --- Show Search Dialog --- */
void show_search_dialog(AppWidgets *widgets) {
    GtkWidget *dialog = gtk_dialog_new_with_buttons("Tìm Kiếm Sinh Viên",
                        GTK_WINDOW(widgets->window),
                        GTK_DIALOG_MODAL | GTK_DIALOG_DESTROY_WITH_PARENT,
                        "Đóng", GTK_RESPONSE_CLOSE,
                        NULL);
                        
    GtkWidget *content_area = gtk_dialog_get_content_area(GTK_DIALOG(dialog));
    GtkWidget *hbox = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 10);
    gtk_container_set_border_width(GTK_CONTAINER(hbox), 20);
    
    GtkWidget *entry = gtk_entry_new();
    gtk_entry_set_placeholder_text(GTK_ENTRY(entry), "Nhập tên hoặc ID...");
    GtkWidget *btn = gtk_button_new_with_label("Tìm 🔍");
    
    gtk_box_pack_start(GTK_BOX(hbox), entry, TRUE, TRUE, 0);
    gtk_box_pack_start(GTK_BOX(hbox), btn, FALSE, FALSE, 0);
    gtk_container_add(GTK_CONTAINER(content_area), hbox);
    
    gtk_widget_show_all(dialog);
    gtk_dialog_run(GTK_DIALOG(dialog));
    gtk_widget_destroy(dialog);
}
