#include <gtk/gtk.h>
#include <stdlib.h>
#include <string.h>
#include "../include/gui.h"
#include "../include/management.h"
#include "../include/academic.h"
#include "../include/finance.h"

/* --- Callback: Calculate GPA and results realtime --- */
static void on_input_changed(GtkEditable *editable, gpointer user_data) {
    /* TODO: Implement real-time update of GPA and payable amount in the dialog labels */
}

/* --- Show Student Form Dialog --- */
void show_student_form(AppWidgets *widgets, int student_id) {
    GtkWidget *dialog = gtk_dialog_new_with_buttons(
        (student_id == -1) ? "Thêm Sinh Viên Mới" : "Chỉnh Sửa Sinh Viên",
        GTK_WINDOW(widgets->window),
        GTK_DIALOG_MODAL | GTK_DIALOG_DESTROY_WITH_PARENT,
        "Hủy", GTK_RESPONSE_CANCEL,
        "Lưu", GTK_RESPONSE_ACCEPT,
        NULL
    );
    
    gtk_window_set_default_size(GTK_WINDOW(dialog), 600, 400);

    GtkWidget *content_area = gtk_dialog_get_content_area(GTK_DIALOG(dialog));
    gtk_container_set_border_width(GTK_CONTAINER(content_area), 20);

    /* Grid Layout for Form */
    GtkWidget *grid = gtk_grid_new();
    gtk_grid_set_column_spacing(GTK_GRID(grid), 20);
    gtk_grid_set_row_spacing(GTK_GRID(grid), 15);
    gtk_container_add(GTK_CONTAINER(content_area), grid);

    /* --- Personal Information Columns --- */
    GtkWidget *lbl_id = gtk_label_new("Mã sinh viên:");
    GtkWidget *ent_id = gtk_entry_new();
    gtk_grid_attach(GTK_GRID(grid), lbl_id, 0, 0, 1, 1);
    gtk_grid_attach(GTK_GRID(grid), ent_id, 1, 0, 1, 1);

    GtkWidget *lbl_name = gtk_label_new("Họ và tên:");
    GtkWidget *ent_name = gtk_entry_new();
    gtk_grid_attach(GTK_GRID(grid), lbl_name, 0, 1, 1, 1);
    gtk_grid_attach(GTK_GRID(grid), ent_name, 1, 1, 1, 1);

    GtkWidget *lbl_credits = gtk_label_new("Số tín chỉ (0-30):");
    GtkWidget *spin_credits = gtk_spin_button_new_with_range(0, 30, 1);
    gtk_grid_attach(GTK_GRID(grid), lbl_credits, 0, 2, 1, 1);
    gtk_grid_attach(GTK_GRID(grid), spin_credits, 1, 2, 1, 1);

    /* --- Academic Scores Columns --- */
    GtkWidget *lbl_attendance = gtk_label_new("Điểm chuyên cần:");
    GtkWidget *scale_attendance = gtk_scale_new_with_range(GTK_ORIENTATION_HORIZONTAL, 0, 10, 0.1);
    gtk_grid_attach(GTK_GRID(grid), lbl_attendance, 2, 0, 1, 1);
    gtk_grid_attach(GTK_GRID(grid), scale_attendance, 3, 0, 1, 1);

    GtkWidget *lbl_midterm = gtk_label_new("Điểm giữa kỳ:");
    GtkWidget *scale_midterm = gtk_scale_new_with_range(GTK_ORIENTATION_HORIZONTAL, 0, 10, 0.1);
    gtk_grid_attach(GTK_GRID(grid), lbl_midterm, 2, 1, 1, 1);
    gtk_grid_attach(GTK_GRID(grid), scale_midterm, 3, 1, 1, 1);

    GtkWidget *lbl_final = gtk_label_new("Điểm cuối kỳ:");
    GtkWidget *scale_final = gtk_scale_new_with_range(GTK_ORIENTATION_HORIZONTAL, 0, 10, 0.1);
    gtk_grid_attach(GTK_GRID(grid), lbl_final, 2, 2, 1, 1);
    gtk_grid_attach(GTK_GRID(grid), scale_final, 3, 2, 1, 1);

    /* --- Status / Summary card in dialog --- */
    GtkWidget *summary_box = gtk_box_new(GTK_ORIENTATION_VERTICAL, 5);
    gtk_grid_attach(GTK_GRID(grid), summary_box, 0, 4, 4, 1);
    
    GtkWidget *lbl_summary_title = gtk_label_new("KẾT QUẢ DỰ TÍNH:");
    gtk_box_pack_start(GTK_BOX(summary_box), lbl_summary_title, FALSE, FALSE, 0);

    /* Fill Data if editing */
    if (student_id != -1) {
        int idx = find_student_by_id(widgets->students, widgets->student_count, student_id);
        if (idx != -1) {
            char id_str[20];
            sprintf(id_str, "%d", widgets->students[idx].id);
            gtk_entry_set_text(GTK_ENTRY(ent_id), id_str);
            gtk_entry_set_text(GTK_ENTRY(ent_name), widgets->students[idx].name);
            gtk_spin_button_set_value(GTK_SPIN_BUTTON(spin_credits), widgets->students[idx].credits);
            gtk_range_set_value(GTK_RANGE(scale_attendance), widgets->students[idx].score_attendance);
            gtk_range_set_value(GTK_RANGE(scale_midterm), widgets->students[idx].score_midterm);
            gtk_range_set_value(GTK_RANGE(scale_final), widgets->students[idx].score_final);
        }
    }

    gtk_widget_show_all(dialog);

    /* --- Handle Response --- */
    if (gtk_dialog_run(GTK_DIALOG(dialog)) == GTK_RESPONSE_ACCEPT) {
        Student s;
        s.id = atoi(gtk_entry_get_text(GTK_ENTRY(ent_id)));
        strncpy(s.name, gtk_entry_get_text(GTK_ENTRY(ent_name)), 50);
        s.credits = (int)gtk_spin_button_get_value(GTK_SPIN_BUTTON(spin_credits));
        s.score_attendance = (float)gtk_range_get_value(GTK_RANGE(scale_attendance));
        s.score_midterm = (float)gtk_range_get_value(GTK_RANGE(scale_midterm));
        s.score_final = (float)gtk_range_get_value(GTK_RANGE(scale_final));
        
        /* Cập nhật logic & dữ liệu */
        if (student_id == -1) { /* Thêm mới */
            if (add_student(widgets->students, &widgets->student_count, s)) {
                show_message(GTK_WINDOW(widgets->window), GTK_MESSAGE_INFO, "Thành công", "Đã thêm sinh viên mới.");
                refresh_student_list(widgets);
            } else {
                show_message(GTK_WINDOW(widgets->window), GTK_MESSAGE_ERROR, "Lỗi", "ID đã tồn tại hoặc danh sách đầy.");
            }
        } else { /* Sửa */
            if (edit_student(widgets->students, widgets->student_count, student_id, s)) {
                show_message(GTK_WINDOW(widgets->window), GTK_MESSAGE_INFO, "Thành công", "Đã cập nhật thông tin.");
                refresh_student_list(widgets);
            }
        }
    }

    gtk_widget_destroy(dialog);
}
