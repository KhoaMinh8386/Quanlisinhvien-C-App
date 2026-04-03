#ifndef GUI_H
#define GUI_H

#include <gtk/gtk.h>
#include "student.h"

/* --- Globals/Structs for UI elements --- */
typedef struct {
    GtkWidget *window;
    GtkWidget *notebook;
    GtkWidget *treeview;
    GtkListStore *store;
    GtkWidget *statusbar;
    
    Student students[MAX_STUDENTS];
    int student_count;
    
    /* Stats for report tab */
    GtkWidget *lbl_total_students;
    GtkWidget *lbl_total_revenue;
    GtkWidget *lbl_avg_gpa;
    GtkWidget *lbl_scholarships;
} AppWidgets;

/* --- Main Window functions --- */
void create_main_window(GtkApplication *app, AppWidgets *widgets);
GtkWidget *create_sidebar(AppWidgets *widgets);
GtkWidget *create_notebook(AppWidgets *widgets);

/* --- List functions --- */
GtkWidget *create_list_tab(AppWidgets *widgets);
void refresh_student_list(AppWidgets *widgets);

/* --- Form/Student functions --- */
void show_student_form(AppWidgets *widgets, int student_id); /* student_id = -1 for new */

/* --- Report functions --- */
GtkWidget *create_report_tab(AppWidgets *widgets);
void update_stats(AppWidgets *widgets);

/* --- Dialog functions --- */
void show_search_dialog(AppWidgets *widgets);
void show_confirm_delete(AppWidgets *widgets);
void show_message(GtkWindow *parent, GtkMessageType type, const char *title, const char *msg);

#endif /* GUI_H */
