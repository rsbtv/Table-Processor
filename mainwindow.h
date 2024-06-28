#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QMessageBox>
#include <QFileDialog> //библиотека диалоговых окон работы с файлами
#include <QCloseEvent> //для перехвата сигнала закрытия окна
#define MAX_MAS_SIZE 1000000//максимальный размер массива

QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();
    //void quickSort(void);

private slots:


    void on_pushButton_Sum_clicked();

    void on_pushButton_Random_clicked();

    void on_tableWidget_cellChanged(int row, int column);

    void on_pushButton_AddCol_clicked();

    void on_pushButton_DelCol_clicked();

    void on_spinBox_Col_valueChanged(int arg1);

    void on_pushButton_Min_clicked();

    void on_pushButton_Max_clicked();

    bool sorted();

    void quick_sort(int first, int last);

    void on_pushButton_Avg_clicked();

    void on_pushButton_Bubble_clicked();

    void on_pushButton_Quick_clicked();

    void on_pushButton_Monkey_clicked();

    void on_pushButton_Gnome_clicked();

    void on_pushButton_Hairbrush_clicked();

    void on_pushButton_Shaker_clicked();

    void on_pushButton_DelDouble_clicked();

    void line_poisk(int search);

    void Bin_poisk(int search);

    void on_pushButton_Search_clicked();

    void on_comboBox_Index_highlighted(int index);

    void on_pushButton_Fill_clicked();

    void on_pushButton_SaveTXT_clicked();

    void on_pushButton_OpenTXT_clicked();

    void checkTable();

    void openCheck();

    void BINopening();

    void TXTopening();

    void clearLabels();

    void on_pushButton_SaveBIN_clicked();

    void on_pushButton_OpenBIN_clicked();

private:
    Ui::MainWindow *ui;
    bool no_auto_change; //признак Не автоматического режима (ручной ввод)
    int * mas_table; //указатель на одномерный массив
//    int col_numbers[MAX_MAS_SIZE];
    bool mas_changed;
    bool mas_exist;
    bool cellChanged;
    bool fileSaved;
    bool fileOpened;
    bool isError;
    bool fileIsOk;
    int old_size_int;
    QString globalName;
//  QString back; // возможно понадобится
};
#endif // MAINWINDOW_H
