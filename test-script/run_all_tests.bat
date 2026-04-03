@echo off
setlocal
echo ----------------------------------------
echo BẮT ĐẦU CHẠY KIỂM THỬ TỰ ĐỘNG (CI/CD)
echo ----------------------------------------

set CFLAGS=-Wall -Wextra -std=c99 -I../student_manager_gui/include
set ENCODING=-finput-charset=UTF-8 -fexec-charset=UTF-8

echo [1/8] Compiling dependencies...
gcc %CFLAGS% -c ../student_manager_gui/src/academic.c -o academic.o %ENCODING%
if not exist academic.o (
    echo Compilation failed for academic.c!
    exit /b 1
)

echo [2/8] Compiling and running test_gpa...
gcc %CFLAGS% test_gpa.c academic.o -o test_gpa.exe %ENCODING%
test_gpa.exe

echo [3/8] Compiling and running test_scholarship...
gcc %CFLAGS% test_scholarship.c academic.o -o test_scholarship.exe %ENCODING%
test_scholarship.exe

echo [4/8] Compiling and running test_tuition...
gcc %CFLAGS% test_tuition.c academic.o -o test_tuition.exe %ENCODING%
test_tuition.exe

echo [5/8] Compiling and running test_input_validation...
gcc %CFLAGS% test_input_validation.c -o test_input_validation.exe %ENCODING%
test_input_validation.exe

echo [6/8] Compiling and running test_integration...
gcc %CFLAGS% test_integration.c academic.o -o test_integration.exe %ENCODING%
test_integration.exe

echo [7/8] Compiling and running test_boundary...
gcc %CFLAGS% test_boundary.c academic.o -o test_boundary.exe %ENCODING%
test_boundary.exe

echo [8/8] Compiling and running test_regression...
gcc %CFLAGS% test_regression.c -o test_regression.exe %ENCODING%
test_regression.exe

echo ----------------------------------------
echo HOÀN TẤT TẤT CẢ CÁC TEST CASE!
echo ----------------------------------------
endlocal
pause
