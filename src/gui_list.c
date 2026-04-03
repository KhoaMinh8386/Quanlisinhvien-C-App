#include <stdio.h>
#include "../include/gui.h"
#include "../include/management.h"
#include "../include/finance.h"

enum {
    COL_ID = 0,
    COL_NAME,
    COL_CREDITS,
    COL_GPA10,
    COL_GPA4,
    COL_TUITION,
    COL_SCHOLARSHIP,
    COL_PAYABLE,
    COL_STATUS,
    NUM_COLS
};

/* --- Refresh List Store with current student data --- */
void refresh_student_list(AppWidgets *widgets) {
    if (!widgets->store) return;
    
    gtk_list_store_clear(widgets->store);
    
    for (int i = 0; i < widgets->student_count; i++) {
        GtkTreeIter iter;
        char tuition_buf[50], scholar_buf[50], payable_buf[50];
        
        format_currency(widgets->students[i].tuition_base, tuition_buf);
        format_currency(widgets->students[i].scholarship, scholar_buf);
        format_currency(widgets->students[i].final_payable, payable_buf);
        
        ScholarshipStatus status = get_scholarship_status(&widgets->students[i]);
        const char *status_text = (status == SCHOLARSHIP_FULL) ? "🥇 Xuất sắc" : 
                                  (status == SCHOLARSHIP_NORMAL) ? "🥈 Khá" : "📚 Thường";
        
        gtk_list_store_append(widgets->store, &iter);
        gtk_list_store_set(widgets->store, &iter,
            COL_ID, widgets->students[i].id,
            COL_NAME, widgets->students[i].name,
            COL_CREDITS, widgets->students[i].credits,
            COL_GPA10, widgets->students[i].gpa_10,
            COL_GPA4, widgets->students[i].gpa_4,
            COL_TUITION, tuition_buf,
            COL_SCHOLARSHIP, scholar_buf,
            COL_PAYABLE, payable_buf,
            COL_STATUS, status_text,
            -1);
    }
    
    /* Update stats as well */
    update_stats(widgets);
}

/* --- Create student list with TreeView --- */
GtkWidget *create_list_tab(AppWidgets *widgets) {
    GtkWidget *vbox = gtk_box_new(GTK_ORIENTATION_VERTICAL, 10);
    gtk_container_set_border_width(GTK_CONTAINER(vbox), 20);
    
    /* Toolbar: Thêm, Sửa, Xóa */
    GtkWidget *toolbar = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 10);
    
    GtkWidget *btn_add = gtk_button_new_with_label("+ Thêm mới (Ctrl+N)");
    g_signal_connect_swapped(btn_add, "clicked", G_CALLBACK(show_student_form), widgets);
    
    GtkWidget *btn_edit = gtk_button_new_with_label("✏️ Chỉnh sửa");
    /* TODO: Connect edit signal */
    
    GtkWidget *btn_delete = gtk_button_new_with_label("🗑️ Xóa");
    g_signal_connect_swapped(btn_delete, "clicked", G_CALLBACK(show_confirm_delete), widgets);
    
    gtk_box_pack_start(GTK_BOX(toolbar), btn_add, FALSE, FALSE, 0);
    gtk_box_pack_start(GTK_BOX(toolbar), btn_edit, FALSE, FALSE, 0);
    gtk_box_pack_start(GTK_BOX(toolbar), btn_delete, FALSE, FALSE, 0);
    
    gtk_box_pack_start(GTK_BOX(vbox), toolbar, FALSE, FALSE, 0);
    
    /* Scrollable Area for TreeView */
    GtkWidget *scroll = gtk_scrolled_window_new(NULL, NULL);
    gtk_scrolled_window_set_policy(GTK_SCROLLED_WINDOW(scroll), GTK_POLICY_AUTOMATIC, GTK_POLICY_AUTOMATIC);
    gtk_box_pack_start(GTK_BOX(vbox), scroll, TRUE, TRUE, 0);
    
    /* TreeView setup */
    widgets->store = gtk_list_store_new(NUM_COLS, 
        G_TYPE_INT,    /* ID */
        G_TYPE_STRING, /* Name */
        G_TYPE_INT,    /* Credits */
        G_TYPE_FLOAT,  /* GPA 10 */
        G_TYPE_FLOAT,  /* GPA 4 */
        G_TYPE_STRING, /* Tuition */
        G_TYPE_STRING, /* Scholarship */
        G_TYPE_STRING, /* Payable */
        G_TYPE_STRING  /* Status */
    );
    
    widgets->treeview = gtk_tree_view_new_with_model(GTK_TREE_MODEL(widgets->store));
    g_object_unref(widgets->store);
    
    /* Columns configuration */
    const char *titles[] = {"ID", "Họ Tên", "Tỉn chỉ", "GPA 10", "GPA 4.0", "Học phí", "Học bổng", "Thực đóng", "Trạng thái"};
    GtkCellRenderer *renderer = gtk_cell_renderer_text_new();
    
    for (int i = 0; i < NUM_COLS; i++) {
        GtkTreeViewColumn *col = gtk_tree_view_column_new_with_attributes(titles[i], renderer, "text", i, NULL);
        gtk_tree_view_column_set_resizable(col, TRUE);
        gtk_tree_view_column_set_sort_column_id(col, i);
        gtk_tree_view_append_column(GTK_TREE_VIEW(widgets->treeview), col);
    }
    
    gtk_container_add(GTK_CONTAINER(scroll), widgets->treeview);
    
    /* Initial data load if any */
    refresh_student_list(widgets);
    
    return vbox;
}
