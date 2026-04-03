# --- Makefile — Hệ Thống Quản Lý Sinh Viên Pro v2.0 ---
CC     = gcc
CFLAGS = -Wall -Wextra -std=c99 -Iinclude
LIBS   = -lcomctl32 -lgdi32 -lcomdlg32 -luser32 -mwindows

SRC = src/main_win32.c \
      src/academic.c   \
      src/management.c \
      src/dlg_student.c

TARGET = student_manager.exe

all: $(TARGET)

$(TARGET): $(SRC)
	$(CC) $(CFLAGS) -o $@ $^ $(LIBS)

run: all
	./$(TARGET)
