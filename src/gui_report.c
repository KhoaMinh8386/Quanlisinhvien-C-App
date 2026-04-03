#include <gtk/gtk.h>
#include <stdio.h>
#include <string.h>
#include "../include/gui.h"
#include "../include/management.h"
#include "../include/finance.h"

/* --- Create Status/Report Tab --- */
GtkWidget *create_report_tab(AppWidgets *widgets) {
    GtkWidget *vbox = gtk_box_new(GTK_ORIENTATION_VERTICAL, 20);
    gtk_container_set_border_width(GTK_CONTAINER(vbox), 20);
    
    /* Title header */
    GtkWidget *lbl_header = gtk_label_new(NULL);
    gtk_label_set_markup(GTK_LABEL(lbl_header), "<span size='xx-large' weight='bold'>📊 Báo Cáo Tài Chính</span>");
    gtk_box_pack_start(GTK_BOX(vbox), lbl_header, FALSE, FALSE, 10);
    
    /* Stats Layout - 2 columns Grid */
    GtkWidget *grid = gtk_grid_new();
    gtk_grid_set_column_spacing(GTK_GRID(grid), 20);
    gtk_grid_set_row_spacing(GTK_GRID(grid), 20);
    gtk_grid_set_column_homogeneous(GTK_GRID(grid), TRUE);
    gtk_box_pack_start(GTK_BOX(vbox), grid, FALSE, FALSE, 0);

    /* Card: Total Students */
    GtkWidget *card1 = gtk_box_new(GTK_ORIENTATION_VERTICAL, 5);
    gtk_style_context_add_class(gtk_widget_get_style_context(card1), "stat-card");
    gtk_grid_attach(GTK_GRID(grid), card1, 0, 0, 1, 1);
    
    GtkWidget *lbl_label1 = gtk_label_new("👥 TỔNG SINH VIÊN");
    gtk_style_context_add_class(gtk_widget_get_style_context(lbl_label1), "stat-label");
    widgets->lbl_total_students = gtk_label_new("0");
    gtk_style_context_add_class(gtk_widget_get_style_context(widgets->lbl_total_students), "stat-value");
    gtk_box_pack_start(GTK_BOX(card1), lbl_label1, FALSE, FALSE, 0);
    gtk_box_pack_start(GTK_BOX(card1), widgets->lbl_total_students, FALSE, FALSE, 0);

    /* Card: Total Revenue */
    GtkWidget *card2 = gtk_box_new(GTK_ORIENTATION_VERTICAL, 5);
    gtk_style_context_add_class(gtk_widget_get_style_context(card2), "stat-card");
    gtk_grid_attach(GTK_GRID(grid), card2, 1, 0, 1, 1);
    
    GtkWidget *lbl_label2 = gtk_label_new("💰 TỔNG THU HỌC PHÍ");
    gtk_style_context_add_class(gtk_widget_get_style_context(lbl_label2), "stat-label");
    widgets->lbl_total_revenue = gtk_label_new("0 VNĐ");
    gtk_style_context_add_class(gtk_widget_get_style_context(widgets->lbl_total_revenue), "stat-value");
    gtk_box_pack_start(GTK_BOX(card2), lbl_label2, FALSE, FALSE, 0);
    gtk_box_pack_start(GTK_BOX(card2), widgets->lbl_total_revenue, FALSE, FALSE, 0);

    /* Card: Average GPA */
    GtkWidget *card3 = gtk_box_new(GTK_ORIENTATION_VERTICAL, 5);
    gtk_style_context_add_class(gtk_widget_get_style_context(card3), "stat-card");
    gtk_grid_attach(GTK_GRID(grid), card3, 0, 1, 1, 1);
    
    GtkWidget *lbl_label3 = gtk_label_new("🎓 GPA TRUNG BÌNH");
    gtk_style_context_add_class(gtk_widget_get_style_context(lbl_label3), "stat-label");
    widgets->lbl_avg_gpa = gtk_label_new("0.0");
    gtk_style_context_add_class(gtk_widget_get_style_context(widgets->lbl_avg_gpa), "stat-value");
    gtk_box_pack_start(GTK_BOX(card3), lbl_label3, FALSE, FALSE, 0);
    gtk_box_pack_start(GTK_BOX(card3), widgets->lbl_avg_gpa, FALSE, FALSE, 0);

    /* Card: Scholarships */
    GtkWidget *card4 = gtk_box_new(GTK_ORIENTATION_VERTICAL, 5);
    gtk_style_context_add_class(gtk_widget_get_style_context(card4), "stat-card");
    gtk_grid_attach(GTK_GRID(grid), card4, 1, 1, 1, 1);
    
    GtkWidget *lbl_label4 = gtk_label_new("🏆 SINH VIÊN NHẬN HỌC BỔNG");
    gtk_style_context_add_class(gtk_widget_get_style_context(lbl_label4), "stat-label");
    widgets->lbl_scholarships = gtk_label_new("0 SV");
    gtk_style_context_add_class(gtk_widget_get_style_context(widgets->lbl_scholarships), "stat-value");
    gtk_box_pack_start(GTK_BOX(card4), lbl_label4, FALSE, FALSE, 0);
    gtk_box_pack_start(GTK_BOX(card4), widgets->lbl_scholarships, FALSE, FALSE, 0);

    /* Export button */
    GtkWidget *btn_export = gtk_button_new_with_label("📥 Xuất Báo Cáo CSV");
    gtk_style_context_add_class(gtk_widget_get_style_context(btn_export), "btn-primary");
    gtk_box_pack_end(GTK_BOX(vbox), btn_export, FALSE, FALSE, 10);

    update_stats(widgets);
    
    return vbox;
}

/* --- Update Statistics Labels --- */
void update_stats(AppWidgets *widgets) {
    if (widgets->student_count == 0) {
        gtk_label_set_text(GTK_LABEL(widgets->lbl_total_students), "0");
        gtk_label_set_text(GTK_LABEL(widgets->lbl_total_revenue), "0 VNĐ");
        gtk_label_set_text(GTK_LABEL(widgets->lbl_avg_gpa), "0.0");
        gtk_label_set_text(GTK_LABEL(widgets->lbl_scholarships), "0 SV");
        return;
    }
    
    long total_rev = 0;
    float total_gpa = 0;
    int scholars = 0;
    
    for (int i = 0; i < widgets->student_count; i++) {
        total_rev += widgets->students[i].final_payable;
        total_gpa += widgets->students[i].gpa_4;
        if (get_scholarship_status(&widgets->students[i]) != SCHOLARSHIP_NONE) scholars++;
    }
    
    char buf[100], rev_buf[100];
    sprintf(buf, "%d", widgets->student_count);
    gtk_label_set_text(GTK_LABEL(widgets->lbl_total_students), buf);
    
    format_currency(total_rev, rev_buf);
    gtk_label_set_text(GTK_LABEL(widgets->lbl_total_revenue), rev_buf);
    
    sprintf(buf, "%.2f", total_gpa / widgets->student_count);
    gtk_label_set_text(GTK_LABEL(widgets->lbl_avg_gpa), buf);
    
    sprintf(buf, "%d SV", scholars);
    gtk_label_set_text(GTK_LABEL(widgets->lbl_scholarships), buf);
}
