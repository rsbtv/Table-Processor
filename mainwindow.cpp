#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <stdlib.h>
#include <stdio.h>
#include <iterator>
#include <QMessageBox>
#include <math.h> //подключаем библиотеки
using namespace std;
//bool isSorted = false;
//bool changed = false;


MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow) //конструктор
{
    ui->setupUi(this);
    no_auto_change = true; //ручной ввод
    clearLabels();
    ui->spinBox_Col->setMaximum(MAX_MAS_SIZE);//устанавливаем максимум для спинбокса
    ui->textEdit->setReadOnly(true);
    mas_table = nullptr;
    setWindowTitle("Новый лист — Таблицы"); //установка названия окна
    fileSaved = false;
    mas_exist = false;
    old_size_int = sizeof(int);
}

MainWindow::~MainWindow() //деструктор
{
    delete ui;
}


void MainWindow::clearLabels()
{
    ui->label_Max->clear();
    ui->label_Min->clear();
    ui->label_Avg->clear();
    ui->label_Sum->clear();
}
bool MainWindow::sorted() // проверка на возрастание всех элементов в массиве
{
    bool flag = true;
    int column = ui->tableWidget->columnCount();

    for (int j = 1; j < column; j++)
    {
        if (mas_table[j] < mas_table[j - 1])
            flag = false;
    }
    return flag;
}

void MainWindow::on_pushButton_Sum_clicked() //нажата кнопка суммы
{
    no_auto_change=false; //ручной ввод
    int column = ui->tableWidget->columnCount();//кол-во столбцов
    int sum=0; //изначальное значение суммы
    bool flag;
    bool isError=false;//ошибок нет
    bool flag_position = false;

    for (int j=0; j<column; j++) //проходим по ячейкам
        if (ui->tableWidget->item(0,j)!=nullptr)//проверяем существования ячейки
        {// ячейка существует
            sum+=ui->tableWidget->item(0,j)->text().toInt(&flag);//вычисление суммы
            if (!flag)//если в ячейке не int
            {
                if (!flag_position)
                {
                    ui->tableWidget->selectColumn(j);
                    ui->tableWidget->scrollToItem(ui->tableWidget->item(0,j));
                    QMessageBox::information(this, "Ошибка!", "В ячейке неверное значение", QMessageBox::Ok);
                }
                flag_position = true;
                //очищаем все значения при ошибке
                clearLabels();

                isError=true;//есть ошибка
            }
        }
        else
        {//не существует
            //создаём
            QTableWidgetItem * ti;//выделяем память
            ti = new QTableWidgetItem;
            ui->tableWidget->setItem(0,j,ti);
            ui->tableWidget->item(0,j)->setBackground(Qt::red);//окраска ячейки
            if (!flag_position)
            {
                ui->tableWidget->selectColumn(j);
                ui->tableWidget->scrollToItem(ui->tableWidget->item(0,j));
                QMessageBox::information(this, "Ошибка!", "В ячейке неверное значение", QMessageBox::Ok);
            }
            flag_position = true;

            //очищаем значения при ошибке
            clearLabels();

            isError=true; //есть ошибка
        }
    //выводим значение при отсутствии ошибок
    if (!isError)
    {
        ui->label_Sum->show(); //показываем лейбл суммы
        ui->label_Sum->setNum(sum); //выводим сумму в лейбл
    }

    no_auto_change=true; // не ручной ввод
}

void MainWindow::on_pushButton_Random_clicked() //заполнение случайными значениями
{
    no_auto_change=false;
    int colum=ui->tableWidget->columnCount(); //считывание кол-ва столбцов
    int val; //ввод переменных
    for (int j=0; j<colum;j++) //проходимся по ячейкам
    {
        if (ui->tableWidget->item(0,j)==nullptr) //не существует
        {
            QTableWidgetItem * ti; //создали указатель
            ti=new QTableWidgetItem; //выделили память
            ui->tableWidget->setItem (0,j,ti); //поместили ячейку в таблицу
        }
        ui->tableWidget->item(0,j)->setBackground(Qt::white); //окрашиваем фон ячейки
        val=rand()%201-100; //от -100 до 100
        ui->tableWidget->item(0,j)->setText(QString::number(val)); //устанавливаем значения в ячейке
    }
    no_auto_change=true; //не ручной ввод
}

void MainWindow::on_tableWidget_cellChanged(int row, int column) //изменили ячейку
{
    if (no_auto_change) //если ручной ввод
    {
        bool flag;
        ui->tableWidget->item(row,column)->text().toInt(&flag); //ссчитываение значейки с ячейки
        if  (flag)
            ui->tableWidget->item(row,column)->setBackground(Qt::white); //ячейка окрашивается в белый
        else
            ui->tableWidget->item(row,column)->setBackground(Qt::red); //ячейка окрашивается в красный
        clearLabels();
        ui->textEdit->clear();
        ui->comboBox_Index->clear();
    }
    else
        ui->tableWidget->item(row,column)->setBackground(Qt::white);
}

void MainWindow::on_pushButton_AddCol_clicked() //добавляем столбик
{
    ui->spinBox_Col->setValue(ui->spinBox_Col->value()+1);
    clearLabels();
}

void MainWindow::on_pushButton_DelCol_clicked() //удаляем столбик
{
    int col = ui->tableWidget->columnCount();
    if  (col>1)
    {
        ui->tableWidget->setColumnCount(col-1); //удаление в таблице
        ui->spinBox_Col->setValue(ui->spinBox_Col->value()-1); //удаление в спинбоксе
    }
    clearLabels();
}

void MainWindow::on_spinBox_Col_valueChanged(int arg1) //изменение в спинбоксе
{
    ui->tableWidget->setColumnCount(arg1);
    clearLabels();
}

void MainWindow::on_pushButton_Min_clicked() //поиск минимального значения
{
    no_auto_change=false;// ручной ввод
    int column = ui->tableWidget->columnCount(); //ссчитывание кол-ва столбцов
    int min = 0; //изначальное значение минимума
    bool isError = false; //присутствие/отсутствие ошибки
    bool flag_position = false;

    QTableWidgetItem *item = ui->tableWidget->item(0, 0);
    if(item == nullptr) //если ячейка пуста
    {
        item = new QTableWidgetItem();
        ui->tableWidget->setItem(0, 0, item); // установка пустого значения в ячейку
        ui->tableWidget->item(0,0)->setBackground(Qt::red); //окраска ячейки
        if (!flag_position)
        {
            ui->tableWidget->selectColumn(0);
            ui->tableWidget->scrollToItem(ui->tableWidget->item(0,0));
            QMessageBox::information(this, "Ошибка!", "В ячейке неверное значение", QMessageBox::Ok);
        }
        flag_position = true;

        clearLabels();
        isError = true;
    }

    //проверка значения
    bool flag = false;
    int val = item->text().toInt(&flag); //ссчитывание значения с ячейки
    if(!flag) //в ячейке не int
    {
        ui->tableWidget->item(0,0)->setBackground(Qt::red); //окраска ячейки
        if (!flag_position)
        {
            ui->tableWidget->selectColumn(0);
            ui->tableWidget->scrollToItem(ui->tableWidget->item(0,0));
            QMessageBox::information(this, "Ошибка!", "В ячейке неверное значение", QMessageBox::Ok);
        }
        flag_position = true;

        clearLabels();

        isError = true;
    }
    else
    {

        min = val;
    }

    for (int j=1; j<column; j++) //проходим по ячейкам
    {
        QTableWidgetItem *item = ui->tableWidget->item(0,j);
        if(item == nullptr) // проверка на пустоту ячейки
        {
            // если элемент пустой выдаем ошибку
            item = new QTableWidgetItem();
            ui->tableWidget->setItem(0,j, item); // установка пустого значения в ячейку
            ui->tableWidget->item(0,j)->setBackground(Qt::red);
            if (!flag_position)
            {
                ui->tableWidget->selectColumn(j);
                ui->tableWidget->scrollToItem(ui->tableWidget->item(0,j));
                QMessageBox::information(this, "Ошибка!", "В ячейке неверное значение", QMessageBox::Ok);
            }
            flag_position = true;
            clearLabels();
            isError = true;
        }

        //проверка на значение
        bool flag = false;
        int val = item->text().toInt(&flag); //переводим значение в int
        if(!flag)
        {// если есть ошибки
            ui->tableWidget->item(0,j)->setBackground(Qt::red); //окрашиваем ячейки
            if (!flag_position)
            {
                ui->tableWidget->selectColumn(j);
                ui->tableWidget->scrollToItem(ui->tableWidget->item(0,j));
                QMessageBox::information(this, "Ошибка!", "В ячейке неверное значение", QMessageBox::Ok);
            }
            flag_position = true;
            clearLabels();

            isError = true;
        }

        if(val < min)// сравниваем значения
        {
            min = val;
        }
    }

    if(!isError) //вывод минимального значения
    {
        ui->label_Min->show(); //показываем лейбл минимума
        ui->label_Min->setNum(min); //выводим минимум
    }
    no_auto_change=true; //не ручной ввод
}

void MainWindow::on_pushButton_Max_clicked() // поиск максимума
{
    no_auto_change=false;// не ручной вывод
    int column = ui->tableWidget->columnCount(); //получаем кол-во стобцов
    int max = 0;//изначальное значение максимума
    bool isError = false; //присутствие/отсутствие ошибки
    bool flag_position = false;

    QTableWidgetItem *item = ui->tableWidget->item(0, 0); //выделение памяти
    if(item == nullptr) //в ячейке пусто
    {
        // если элемент пустой выдаем ошибку
        item = new QTableWidgetItem();
        ui->tableWidget->setItem(0, 0, item); // установка пустого значения в ячейку
        ui->tableWidget->item(0,0)->setBackground(Qt::red); //красим ячейку
        if (!flag_position)
        {
            ui->tableWidget->selectColumn(0);
            ui->tableWidget->scrollToItem(ui->tableWidget->item(0,0));
            QMessageBox::information(this, "Ошибка!", "В ячейке неверное значение", QMessageBox::Ok);
        }
        flag_position = true;
        clearLabels();

        isError = true;
    }

    //проверка значения
    bool flag = false;
    int val = item->text().toInt(&flag); //перевод значения в ячейке
    if(!flag)
    {// если есть ошибки
        ui->tableWidget->item(0,0)->setBackground(Qt::red); //окрашиваем ячейку
        if (!flag_position)
        {
            ui->tableWidget->selectColumn(0);
            ui->tableWidget->scrollToItem(ui->tableWidget->item(0,0));
            QMessageBox::information(this, "Ошибка!", "В ячейке неверное значение", QMessageBox::Ok);
        }
        flag_position = true;
        clearLabels();

        isError = true;
    }
    else
    {

        max = val;
    }

    for (int j=1; j<column; j++) //проходим по ячейкам
    {
        QTableWidgetItem *item = ui->tableWidget->item(0,j); //выделение памяти
        if(item == nullptr) // проверка на пустоту ячейки
        {
            // если элемент пустой выдаем ошибку
            item = new QTableWidgetItem();
            ui->tableWidget->setItem(0,j, item); // установка пустого значения в ячейку
            ui->tableWidget->item(0,j)->setBackground(Qt::red); //окраска ячеек
            if (!flag_position)
            {
                ui->tableWidget->selectColumn(j);
                ui->tableWidget->scrollToItem(ui->tableWidget->item(0,j));
                QMessageBox::information(this, "Ошибка!", "В ячейке неверное значение", QMessageBox::Ok);
            }
            flag_position = true;
            clearLabels();
            isError = true;
        }

        //проверка на значение
        bool flag = false;
        int val = item->text().toInt(&flag); //перевод ячейки в int

        if(!flag) //в ячейке не int
        {
            ui->tableWidget->item(0,j)->setBackground(Qt::red); //окраска ячейки
            if (!flag_position)
            {
                ui->tableWidget->selectColumn(j);
                ui->tableWidget->scrollToItem(ui->tableWidget->item(0,j));
                QMessageBox::information(this, "Ошибка!", "В ячейке неверное значение", QMessageBox::Ok);
            }
            flag_position = true;
            clearLabels();


            isError = true;
        }

        if(val > max)// сравниваем значения
        {
            max = val;
        }
    }
    if(!isError) //если нет ошибок

    {
        ui->label_Max->show(); //показываем лейбл максимума
        ui->label_Max->setNum(max); //выводим значение максимума
    }
    no_auto_change=true;// не ручной ввод
}

void MainWindow::on_pushButton_Avg_clicked() //среднее арифметическое
{
    no_auto_change=false;//не ручной ввод
    int column = ui->tableWidget->columnCount();//получаем кол-во столбцов
    double avg = 0.0;//изначальное значение среднего
    double sum = 0.0;//изначальное значение суммы из ячеек
    bool isError = false;//отсутствие/присутствие ошибок
    bool flag_position = false;
    for (int j=0; j<column; j++ ) //проходим по значениям
    {
        QTableWidgetItem *item = ui->tableWidget->item(0,j);
        if (item == nullptr) //ячейка не существует
        {
            item = new QTableWidgetItem();
            ui->tableWidget->setItem(0,j, item);

            ui->tableWidget->item(0,j)->setBackground(Qt::red); //окрашиваем ячейки
            if (!flag_position)
            {
                ui->tableWidget->selectColumn(j);
                ui->tableWidget->scrollToItem(ui->tableWidget->item(0,j));
                QMessageBox::information(this, "Ошибка!", "В ячейке неверное значение", QMessageBox::Ok);
            }
            flag_position = true;
            clearLabels();

            isError = true;
        }
        bool flag = false;

        int val = item->text().toInt(&flag); //перевод значения ячейки в int
        if (!flag) // в ячейке не int
        {
            ui->tableWidget->item(0,j)->setBackground(Qt::red); //окраска ячейки
            if (!flag_position)
            {
                ui->tableWidget->selectColumn(j);
                ui->tableWidget->scrollToItem(ui->tableWidget->item(0,j));
                QMessageBox::information(this, "Ошибка!", "В ячейке неверное значение", QMessageBox::Ok);
            }
            flag_position = true;

            clearLabels();
            isError = true;//ошибка присутствует
        }
        sum += val; //рассчет суммы
    }
    if(!isError)//нет ошибок
    {
        avg = sum/column;//вычисление среднего значения
        ui->label_Avg->show(); //показываем лейбл среднего
        ui->label_Avg->setNum(avg); //выводим среднее значение


    }
    no_auto_change=true;// не ручной ввод
}

void MainWindow::on_pushButton_Bubble_clicked() //пузырьковая сортировка
{
    no_auto_change = false;

    int column = ui->tableWidget->columnCount();
    bool flag; // признак успешного преоб-я ячейки
    bool gl_flag = true; // признак успешного формирования массива
    bool flag_position = false; // признак ошибочности формирования чего-л.

    try
    {
        mas_table = new int[column];
    }
    catch (std::bad_alloc&)
    {
        QMessageBox::information(this,"Ошибка","Невозможно выделить память.",QMessageBox::Ok);
        gl_flag = false;
    }

    if (gl_flag) // если память выделили
    {
        for (int j = 0; j < column; j++)
        {
            if (ui->tableWidget->item(0,j) == nullptr) // если ячейка не существует
            {
                gl_flag=false; //как минимум одна ячейка не определена
                QTableWidgetItem * ti; //создали указатель
                ti = new QTableWidgetItem; //выделили память
                ui->tableWidget->setItem( 0,j, ti); //поместили ячейку в таблицу
                ui->tableWidget->item(0,j)->setBackground(Qt::red);
                if (!flag_position)
                {
                    ui->tableWidget->selectRow(j);
                    ui->tableWidget->scrollToItem(ui->tableWidget->item(0,j));
                }
                flag_position = true;
            }
            else
            {
                mas_table[j] = ui->tableWidget->item(0,j)->text().toInt(&flag);

                if (flag) // если конвертировалось в число
                {
                    ui->tableWidget->item(0,j)->setBackground(Qt::white);
                }
                else // если не конвертировалось в число
                {
                    if (!flag_position)
                    {
                        ui->tableWidget->selectColumn(j);
                        ui->tableWidget->scrollToItem(ui->tableWidget->item(0,j));
                    }

                    flag_position = true;
                    gl_flag=false; //как минимум одна ячейка содержит ошибочные данные
                    ui->tableWidget->item(0,j)->setBackground(Qt::red);
                }
            }
        }
    }

    if (gl_flag) // если массив сформирован
    {
        // работа с массивом

        if (column < 100000)
        {
            for (int i = 1; i < column; i++) // сортировка - пузырек
            {
                for (int j = i; j > 0 && mas_table[j - 1] > mas_table[j]; j--) // идем от текущего элемента в начало массива, пока предыдущие элементы больше
                {
                    swap(mas_table[j - 1],mas_table[j]);
                }
            }

            for (int j = 0; j < column; j++) // вывод полученного массива в таблицу
                ui->tableWidget->item(0,j)->setText(QString::number(mas_table[j]));
        }
        else
        {

            QMessageBox::information(this,"Ошибка","Слишком большое число элементов", QMessageBox::Ok);
        }
    }
    else // если что-то не так
    {
        QMessageBox::information(this,"Ошибка","Таблица содержит некорректные значения или незаполненные клетки", QMessageBox::Ok);
    }
    //очищаем, что есть
    delete [] mas_table;
    mas_table=nullptr;
    no_auto_change = true;



}

void MainWindow::quick_sort(int first, int last) // рекурсивная функция для быстрой сортировки
{
    int i = first, j = last, x = mas_table[(first + last) / 2]; // опорные элементы

    do{
        // разбиение массива на две части : все элементы из одной части не превосходят любого элемента из другой части
        while (mas_table[i] < x)
            i++;
        while (mas_table[j] > x)
            j--;

        if (i <= j)
        {
            if (mas_table[i] > mas_table[j])
            {
                swap(mas_table[i], mas_table[j]); // сортировка каждой части массива
            }
            i++;
            j--;
        }

    } while (i <= j);

    if (i < last)
        quick_sort(i, last);
    if (first < j)
        quick_sort(first, j);

}

void MainWindow::on_pushButton_Quick_clicked() //быстрая сортировка
{
    no_auto_change=false;//не ручной ввод
    int column = ui->tableWidget->columnCount();//получаем кол-во столбцов
    bool isError = false;//отсутствие/присутствие ошибок
    bool gl_flag = true; // признак успешного формирования массива
    bool flag_position = false;


    try
    {
        mas_table = new int[column];
    }
    catch (std::bad_alloc&)
    {
        QMessageBox::information(this,"Ошибка","Невозможно выделить память.",QMessageBox::Ok);
        gl_flag = false;
    }


    if (gl_flag)
    {
        for (int j=0; j<column; j++ ) //проходим по значениям
        {
            QTableWidgetItem *item = ui->tableWidget->item(0,j);
            if (item == nullptr) //ячейка не существует
            {
                item = new QTableWidgetItem();
                ui->tableWidget->setItem(0,j, item);

                ui->tableWidget->item(0,j)->setBackground(Qt::red); //окрашиваем ячейки
                if (!flag_position)
                {
                    ui->tableWidget->selectColumn(j);
                    ui->tableWidget->scrollToItem(ui->tableWidget->item(0,j));
                    QMessageBox::information(this, "Ошибка!", "В ячейке неверное значение", QMessageBox::Ok);
                }
                flag_position = true;
                clearLabels();

                isError = true;
            }
            bool flag = false;

            mas_table[j]=ui->tableWidget->item(0,j)->text().toInt(&flag);//перевод значения ячейки в int
            if (!flag) // в ячейке не int
            {
                ui->tableWidget->item(0,j)->setBackground(Qt::red); //окраска ячейки
                if (!flag_position)
                {
                    ui->tableWidget->selectColumn(j);
                    ui->tableWidget->scrollToItem(ui->tableWidget->item(0,j));
                    QMessageBox::information(this, "Ошибка!", "В ячейке неверное значение", QMessageBox::Ok);
                }
                flag_position = true;

                clearLabels();
                isError = true;//ошибка присутствует
            }
        }

    }
    if (gl_flag)
    {
        if(!isError)//нет ошибок
        {
            quick_sort(0, column-1); //используем написанную функцию
            for (int j = 0;j<column;j++)
            {
                ui->tableWidget->item(0,j)->setText(QString::number(mas_table[j])); //записываем значения в ячейки

            }
        }
    }
    //очищаем, что есть
    delete [] mas_table;
    mas_table=nullptr;
    no_auto_change=true;// не ручной ввод



}

void MainWindow::on_pushButton_Monkey_clicked() //обезьянья сортировка
{
    no_auto_change=false;//не ручной ввод
    int column = ui->tableWidget->columnCount();//получаем кол-во столбцов
    bool isError = false;//отсутствие/присутствие ошибок
    int temp = 0; //временная переменная
    bool sorted = false;
    bool flag_position = 0;
    bool gl_flag=true;

    try
    {
        mas_table = new int[column];
    }
    catch (std::bad_alloc&)
    {
        QMessageBox::information(this,"Ошибка","Невозможно выделить память.",QMessageBox::Ok);
        gl_flag = false;
    }

    if (gl_flag)
    {
        for (int j=0; j<column; j++ ) //проходим по значениям
        {
            QTableWidgetItem *item = ui->tableWidget->item(0,j);
            if (item == nullptr) //ячейка не существует
            {
                item = new QTableWidgetItem();
                ui->tableWidget->setItem(0,j, item);

                ui->tableWidget->item(0,j)->setBackground(Qt::red); //окрашиваем ячейки
                if (!flag_position)
                {
                    ui->tableWidget->selectColumn(j);
                    ui->tableWidget->scrollToItem(ui->tableWidget->item(0,j));
                    QMessageBox::information(this, "Ошибка!", "В ячейке неверное значение", QMessageBox::Ok);
                }
                flag_position = true;
                clearLabels();
                isError = true;
            }
            bool flag = false;
            mas_table[j]=ui->tableWidget->item(0,j)->text().toInt(&flag);//перевод значения ячейки в int
            if (!flag) // в ячейке не int
            {
                ui->tableWidget->item(0,j)->setBackground(Qt::red); //окраска ячейки
                if (!flag_position)
                {
                    ui->tableWidget->selectColumn(j);
                    ui->tableWidget->scrollToItem(ui->tableWidget->item(0,j));
                    QMessageBox::information(this, "Ошибка!", "В ячейке неверное значение", QMessageBox::Ok);
                }
                flag_position = true;

                clearLabels();
                isError = true;//ошибка присутствует
            }

        }
    }
    if (gl_flag)
    {
        if(!isError)//нет ошибок
        {
            if (column<=10) //будем использовать не более 10 значений, т.к. обезьянья сортировка очень неэффективная
            {
                while(!sorted)
                {
                    int i=rand()%column;
                    int j=rand()%column;
                    temp=mas_table[i];
                    mas_table[i]=mas_table[j];
                    mas_table[j]=temp;
                    ui->tableWidget->item(0,j)->setText(QString::number(mas_table[j]));
                    ui->tableWidget->item(0,i)->setText(QString::number(mas_table[i]));
                    sorted=true;
                    for (int l=0;l<column-1;l++)
                    {
                        if (mas_table[l]>mas_table[l+1])
                        {
                            sorted=false;
                        }
                    }
                }
            }
            else
            {
                QMessageBox::information(this, "Ошибка!", "Слишком много ячеек, \n рекомендуемое кол-во: 10", QMessageBox::Ok);
                clearLabels();
            }


        }
    }
    //очищаем, что есть
    delete [] mas_table;
    mas_table=nullptr;
    no_auto_change=true;// не ручной ввод



}

void MainWindow::on_pushButton_Gnome_clicked() //гномья сортировка
{
    no_auto_change=false;//не ручной ввод
    int column = ui->tableWidget->columnCount();//получаем кол-во столбцов
    bool isError = false;//отсутствие/присутствие ошибок
    int temp = 0;
    bool flag_position = false;
    bool gl_flag=true;

    try
    {
        mas_table = new int[column];
    }
    catch (std::bad_alloc&)
    {
        QMessageBox::information(this,"Ошибка","Невозможно выделить память.",QMessageBox::Ok);
        gl_flag = false;
    }

    if (gl_flag)
    {
        for (int j=0; j<column; j++ ) //проходим по значениям
        {
            QTableWidgetItem *item = ui->tableWidget->item(0,j);
            if (item == nullptr) //ячейка не существует
            {
                item = new QTableWidgetItem();
                ui->tableWidget->setItem(0,j, item);

                ui->tableWidget->item(0,j)->setBackground(Qt::red); //окрашиваем ячейки
                if (!flag_position)
                {
                    ui->tableWidget->selectColumn(j);
                    ui->tableWidget->scrollToItem(ui->tableWidget->item(0,j));
                    QMessageBox::information(this, "Ошибка!", "В ячейке неверное значение", QMessageBox::Ok);
                }
                flag_position = true;
                clearLabels();

                isError = true;
            }
            bool flag = false;

            mas_table[j]=ui->tableWidget->item(0,j)->text().toInt(&flag);//перевод значения ячейки в int
            if (!flag) // в ячейке не int
            {
                ui->tableWidget->item(0,j)->setBackground(Qt::red); //окраска ячейки

                if (!flag_position)
                {
                    ui->tableWidget->selectColumn(j);
                    ui->tableWidget->scrollToItem(ui->tableWidget->item(0,j));
                    QMessageBox::information(this, "Ошибка!", "В ячейке неверное значение", QMessageBox::Ok);
                }
                flag_position = true;

                clearLabels();
                isError = true;//ошибка присутствует
            }
        }
    }
    if (gl_flag)
    {
        if(!isError)//нет ошибок
        {
            int j=1;
            int k = 2; //переменная чтобы избежать Segmentation Fault
            while (j<column)
            {
                if (mas_table[j-1]<=mas_table[j])
                {
                    j=k;
                    k+=1;
                }
                else
                {
                    temp=mas_table[j];
                    mas_table[j]=mas_table[j-1];
                    mas_table[j-1]=temp;
                    ui->tableWidget->item(0,j)->setText(QString::number(mas_table[j]));
                    ui->tableWidget->item(0,j-1)->setText(QString::number(mas_table[j-1]));
                    j--;
                    if (j==0)
                    {
                        j=k;
                        k+=1;

                    }
                }
            }
        }
    }
    //очищаем, что есть
    delete [] mas_table;
    mas_table=nullptr;
    no_auto_change=true;// не ручной ввод


}

void MainWindow::on_pushButton_Hairbrush_clicked() //сортировка расческой
{
    no_auto_change=false;//не ручной ввод
    int column = ui->tableWidget->columnCount();//получаем кол-во столбцов
    bool isError = false;//отсутствие/присутствие ошибок
    int temp = 0;
    int n = column;
    double s=1.247;
    bool flag_position = false;
    bool gl_flag=true;

    try
    {
        mas_table = new int[column];
    }
    catch (std::bad_alloc&)
    {
        QMessageBox::information(this,"Ошибка","Невозможно выделить память.",QMessageBox::Ok);
        gl_flag = false;
    }

    if (gl_flag)
    {
        for (int j=0; j<column; j++ ) //проходим по значениям
        {
            QTableWidgetItem *item = ui->tableWidget->item(0,j);
            if (item == nullptr) //ячейка не существует
            {
                item = new QTableWidgetItem();
                ui->tableWidget->setItem(0,j, item);

                ui->tableWidget->item(0,j)->setBackground(Qt::red); //окрашиваем ячейки

                if (!flag_position)
                {
                    ui->tableWidget->selectColumn(j);
                    ui->tableWidget->scrollToItem(ui->tableWidget->item(0,j));
                    QMessageBox::information(this, "Ошибка!", "В ячейке неверное значение", QMessageBox::Ok);
                }
                flag_position = true;
                clearLabels();

                isError = true;
            }
            bool flag = false;

            mas_table[j]=ui->tableWidget->item(0,j)->text().toInt(&flag);//перевод значения ячейки в int
            if (!flag) // в ячейке не int
            {
                ui->tableWidget->item(0,j)->setBackground(Qt::red); //окраска ячейки

                if (!flag_position)
                {
                    ui->tableWidget->selectColumn(j);
                    ui->tableWidget->scrollToItem(ui->tableWidget->item(0,j));
                    QMessageBox::information(this, "Ошибка!", "В ячейке неверное значение", QMessageBox::Ok);
                }
                flag_position = true;

                clearLabels();
                isError = true;//ошибка присутствует
            }
        }

    }
    if (gl_flag)
    {
        if(!isError)//нет ошибок
        {
            for (int j=0;j<column;j++) //собственно алгоритм
            {
                n/=s;
                for (int j=0;j<column-n;j++)
                {
                    if (mas_table[j]>mas_table[j+n])
                    {
                        temp=mas_table[j];
                        mas_table[j]=mas_table[j+n];
                        mas_table[j+n]=temp;
                        ui->tableWidget->item(0,j)->setText(QString::number(mas_table[j]));
                        ui->tableWidget->item(0,j+n)->setText(QString::number(mas_table[j+n]));
                    }
                }
            }
        }
    }
    //очищаем, что есть
    delete [] mas_table;
    mas_table=nullptr;
    no_auto_change=true;// не ручной ввод



}

void MainWindow::on_pushButton_Shaker_clicked() //шейкерная сортировка
{
    no_auto_change=false;//не ручной ввод
    int column = ui->tableWidget->columnCount();//получаем кол-во столбцов
    bool isError = false;//отсутствие/присутствие ошибок
    //int temp = 0;
    int left, right;
    left = 1;
    right = column-1;
    bool flag_position = false;
    bool gl_flag=true;

    try
    {
        mas_table = new int[column];
    }
    catch (std::bad_alloc&)
    {
        QMessageBox::information(this,"Ошибка","Невозможно выделить память.",QMessageBox::Ok);
        gl_flag = false;
    }

    if (gl_flag)
    {
        for (int j=0; j<column; j++ ) //проходим по значениям
        {
            QTableWidgetItem *item = ui->tableWidget->item(0,j);
            if (item == nullptr) //ячейка не существует
            {
                item = new QTableWidgetItem();
                ui->tableWidget->setItem(0,j, item);

                ui->tableWidget->item(0,j)->setBackground(Qt::red); //окрашиваем ячейки
                if (!flag_position)
                {
                    ui->tableWidget->selectColumn(j);
                    ui->tableWidget->scrollToItem(ui->tableWidget->item(0,j));
                    QMessageBox::information(this, "Ошибка!", "В ячейке неверное значение", QMessageBox::Ok);
                }
                flag_position = true;
                clearLabels();

                isError = true;
            }
            bool flag = false;

            mas_table[j]=ui->tableWidget->item(0,j)->text().toInt(&flag);//перевод значения ячейки в int
            if (!flag) // в ячейке не int
            {
                ui->tableWidget->item(0,j)->setBackground(Qt::red); //окраска ячейки

                if (!flag_position)
                {
                    ui->tableWidget->selectColumn(j);
                    ui->tableWidget->scrollToItem(ui->tableWidget->item(0,j));
                    QMessageBox::information(this, "Ошибка!", "В ячейке неверное значение", QMessageBox::Ok);
                }
                flag_position = true;

                clearLabels();
                isError = true;//ошибка присутствует
            }
        }
    }
    if (gl_flag)
    {
        if(!isError)//нет ошибок
        {
            while (left <= right)
            {
                for (int j = right; j >= left; j--)
                {
                    if (mas_table[j-1] > mas_table[j])
                    {
                        swap(mas_table[j-1], mas_table[j]);
                        ui->tableWidget->item(0,j)->setText(QString::number(mas_table[j]));
                        ui->tableWidget->item(0,j-1)->setText(QString::number(mas_table[j-1]));
                    }
                }
                left++;
                for (int j = left; j <= right; j++)
                {
                    if (mas_table[j-1] > mas_table[j])
                    {
                        swap(mas_table[j-1], mas_table[j]);
                        ui->tableWidget->item(0,j)->setText(QString::number(mas_table[j]));
                        ui->tableWidget->item(0,j-1)->setText(QString::number(mas_table[j-1]));
                    }
                }
                right--;

            }
        }
    }
    //очищаем, что есть
    delete [] mas_table;
    mas_table=nullptr;
    no_auto_change=true;// не ручной ввод


}

void MainWindow::on_pushButton_DelDouble_clicked() //удаление дубликатов
{
    ui->textEdit->clear();
    ui->comboBox_Index->clear();

    no_auto_change=false;//не ручной ввод
    int column = ui->tableWidget->columnCount();//получаем кол-во столбцов
    bool isError = false;//отсутствие/присутствие ошибок
    //int *mas_table= new int[column];
    //    int n=0;
    bool flag_position = false;
    //int spinbox_val = ui->spinBox_Col->value();
    bool gl_flag=true;

    try
    {
        mas_table = new int[column];
    }
    catch (std::bad_alloc&)
    {
        QMessageBox::information(this,"Ошибка","Невозможно выделить память.",QMessageBox::Ok);
        gl_flag = false;
    }

    if (gl_flag)
    {
        for (int j=0; j<column; j++ ) //проходим по значениям
        {
            QTableWidgetItem *item = ui->tableWidget->item(0,j);
            if (item == nullptr) //ячейка не существует
            {
                item = new QTableWidgetItem();
                ui->tableWidget->setItem(0,j, item);

                ui->tableWidget->item(0,j)->setBackground(Qt::red); //окрашиваем ячейки
                if (!flag_position)
                {
                    ui->tableWidget->selectColumn(j);
                    ui->tableWidget->scrollToItem(ui->tableWidget->item(0,j));
                    QMessageBox::information(this, "Ошибка!", "В ячейке неверное значение", QMessageBox::Ok);
                }
                flag_position = true;
                clearLabels();

                isError = true;
            }
            bool flag = false;
            //переменная для позиционирования ошибки
            mas_table[j]=ui->tableWidget->item(0,j)->text().toInt(&flag);//перевод значения ячейки в int
            if (!flag) // в ячейке не int
            {
                ui->tableWidget->item(0,j)->setBackground(Qt::red); //окраска ячейки
                if (!flag_position)
                {
                    ui->tableWidget->selectRow(j);
                    ui->tableWidget->scrollToItem(ui->tableWidget->item(0,j));
                    QMessageBox::information(this, "Ошибка!", "В ячейке неверное значение", QMessageBox::Ok);
                }
                flag_position = true;

                clearLabels();
                isError = true;//ошибка присутствует
            }
        }

    }
    if (gl_flag)
    {
        if(!isError)//нет ошибок
        {
            // работа с массивом
            quick_sort(0, column - 1); // вызов рекурсии для старта быстрой сортировки

            int masSize = ui->spinBox_Col->value();

            int  j = 0;
            for(int x = 0; x < masSize; x++)
                if(0 == count(mas_table, mas_table+j, mas_table[x]))
                    mas_table[j++] = mas_table[x];

            ui->spinBox_Col->setValue(j);

            for (int jj = 0; jj < j; jj++) // вывод полученного массива в таблицу
                ui->tableWidget->item(0,jj)->setText(QString::number(mas_table[jj]));
        }
    }
    //очищаем, что есть
    delete [] mas_table;
    mas_table=nullptr;
    no_auto_change=true;// не ручной ввод


}

void MainWindow::line_poisk(int search) // линейный поиск
{
    //ui->comboBox_search->clear();

    int index = 0;
    int column = ui->tableWidget->columnCount();
    int kol = 0;

    for (int i = 0; i < column; i++)
    {
        if (mas_table[i] == search)
        {
            ui->comboBox_Index->insertItem(index, QString::number(i + 1));

            if (kol > 600000)
            {
                break;
            }

            index++;
            kol++;
        }
    }

    if (kol == 0)
    {
        ui->textEdit->setVisible(false);
        ui->comboBox_Index->setVisible(false);
        ui->textEdit->setVisible(true);
        ui->textEdit->setText("Не найдено");
    }
    if (kol > 0 && kol<=1000000)
    {
        ui->textEdit->setVisible(true);
        ui->comboBox_Index->setVisible(true);
        ui->textEdit->setVisible(true);
        ui->comboBox_Index->setCurrentIndex(0);
        ui->textEdit->setText("Количество найденных элементов: " + QString::number(kol));
    }
    if (kol > 1000000)
    {
        ui->textEdit->setVisible(true);
        ui->comboBox_Index->setVisible(true);
        QMessageBox::information(this, "Ошибка", "Найдено слишком много элементов", QMessageBox::Ok);
    }
}

void MainWindow::Bin_poisk(int search) // бинарный поиск
{
    ui->comboBox_Index->clear();
    ui->textEdit->setVisible(true);
    ui->comboBox_Index->setVisible(true);

    int index = 0;
    int column = ui->spinBox_Col->value();
    int kol = 0;

    int average_index = 0, // переменная для хранения индекса среднего элемента массива
            first_index   = 0, // индекс первого элемента в массиве
            last_index    = column -1; // индекс последнего элемента в массиве

    while (first_index < last_index)
    {
        average_index = first_index + (last_index - first_index) / 2; // меняем индекс среднего значения

        //search <= mas_table[average_index] ? last_index = average_index : first_index = average_index + 1;    // найден ключевой элемент или нет

        if (search <= mas_table[average_index]) // найден ключевой элемент или нет
        {
            last_index = average_index;
        }
        else
        {
            first_index = average_index + 1;
        }
    }

    if (mas_table[last_index] == search)
    {
        while (mas_table[last_index] == search && last_index < column)
        {
            ui->comboBox_Index->insertItem(index, QString::number(last_index + 1));
            last_index++;
            index++;
            kol++;

            if (kol > 1000000)
            {
                break;
            }
        }
    }

    if (kol == 0)
    {
        ui->textEdit->setVisible(false);
        ui->comboBox_Index->setVisible(false);
        ui->textEdit->setVisible(true);
        ui->textEdit->setText("Не найдено");
    }
    if (kol > 0)
    {
        ui->textEdit->setVisible(true);
        ui->comboBox_Index->setVisible(true);
        ui->textEdit->setVisible(true);
        ui->comboBox_Index->setCurrentIndex(0);
        ui->textEdit->setText("Количество найденных элементов: " + QString::number(kol));
    }
    if (kol > 1000000)
    {
        QMessageBox::information(this, "Ошибка!", "Найдено слишком много элементов", QMessageBox::Ok);
    }

}

void MainWindow::on_pushButton_Search_clicked() //поиск
{


    no_auto_change = false;

    int column = ui->tableWidget->columnCount();
    bool flag; // признак успешного преоб-я ячейки
    bool gl_flag = true; // признак успешного формирования массива
    bool flag_position = false; // признак ошибочности формирования чего-л.

    //    delete [] mas_table;
    //    mas_table=nullptr;
    //обработка исключений
    try
    {
        mas_table = new int[column];
    }
    catch (std::bad_alloc&)
    {
        QMessageBox::information(this,"Ошибка","Невозможно выделить память.",QMessageBox::Ok);
        gl_flag = false;
    }


    if (gl_flag) // если память выделили
    {
        for (int j = 0; j < column; j++)
        {
            if (ui->tableWidget->item(0,j) == nullptr) // если ячейка не существует
            {
                gl_flag=false; //как минимум одна ячейка не определена
                QTableWidgetItem * ti; //создали указатель
                ti = new QTableWidgetItem; //выделили память
                ui->tableWidget->setItem(0,j, ti); //поместили ячейку в таблицу
                ui->tableWidget->item(0,j)->setBackground(Qt::red);

                if (!flag_position) //позиционирование ошибки
                {
                    ui->tableWidget->selectColumn(j);
                    ui->tableWidget->scrollToItem(ui->tableWidget->item(0,j));
                    QMessageBox::information(this, "Ошибка!", "В ячейке неверное значение", QMessageBox::Ok);
                }
                flag_position = true;
            }
            else
            {
                mas_table[j] = ui->tableWidget->item(0,j)->text().toInt(&flag);

                if (flag) // если конвертировалось в число
                {
                    ui->tableWidget->item(0,j)->setBackground(Qt::white);
                }
                else // если не конвертировалось в число
                {
                    if (!flag_position)
                    {
                        ui->tableWidget->selectRow(j);
                        ui->tableWidget->scrollToItem(ui->tableWidget->item(0,j));
                    }

                    flag_position = true;
                    gl_flag=false; //как минимум одна ячейка содержит ошибочные данные
                    ui->tableWidget->item(0,j)->setBackground(Qt::red);
                }
            }
        }
    }

    if (gl_flag) // если массив сформирован
    {
        // работа с массивом
        int search = ui->lineEdit_Search->text().toInt(&flag);
        if (flag)
        {
            if (sorted()) // если массив отсортирован, то используем бинарный поиск
            {
                Bin_poisk(search);

            }
            else // если массив не отсортирован, то используем линейный поиск
            {
                line_poisk(search);
            }
        }
        else //введено не число
        {
            QMessageBox::information(this, "Ошибка!", "В поисковую строку введено не число", QMessageBox::Ok);
        }
    }
    else
    {
        QMessageBox::information(this, "Ошибка!", "В ячейке неверное значение", QMessageBox::Ok);
    }
    //очищаем, что есть
    delete [] mas_table;
    mas_table=nullptr;
    no_auto_change = true;


}

void MainWindow::on_comboBox_Index_highlighted(int index) //указывает на ячейку с найденным значением
{
    index=ui->comboBox_Index->itemText(index).toInt()-1; // ссчитывание индекса

    ui->tableWidget->selectColumn(index); //выделение столбца
    ui->tableWidget->scrollToItem(ui->tableWidget->item(0,index)); //позиционирование ошибки
}

void MainWindow::on_pushButton_Fill_clicked() // функция заполнения ячеек введенным значением
{
    no_auto_change = false; //ручной ввод
    int column = ui->tableWidget->columnCount(); //кол-вол столбцов
    bool flag = true;
    int zn = ui->lineEdit_Search->text().toInt(&flag); //проверка на int

    if (flag) // в ячейке int
    {
        for (int j = 0; j < column; j++) //проходим по значениям
        {
            if (ui->tableWidget->item(0,j) != nullptr) // проверка сущ-я ячейки
            {
                ui->tableWidget->item(0,j)->setText(QString::number(zn));
                ui->tableWidget->item(0,j)->setBackground(Qt::white);

            }
            else // ячейка не существует
            {

                // cоздание
                QTableWidgetItem * ti; // создание указателя
                ti = new QTableWidgetItem; // выделение памяти
                ui->tableWidget->setItem(0,j, ti); // поместили ячейку в таблицу
                ui->tableWidget->item(0,j)->setText(QString::number(zn));
                ui->tableWidget->item(0,j)->setBackground(Qt::white);

            }
        }
    }
    else
    {
        QMessageBox::information(this,"Ошибка","В строку введено не число", QMessageBox::Ok);
    }

    no_auto_change = true;
}

void MainWindow::on_pushButton_SaveTXT_clicked() //сохранение TXT
{
    checkTable();
    if (!isError)
    {
        QString fileName;
        fileName = QFileDialog::getSaveFileName(this, "Сохранить в формате TXT", "C:\\Users\\student\\Documents", "Text files (*.txt)");

        if (fileName.isEmpty())
        {
            QMessageBox::information(this, "Ошибка", "Файл не выбран", QMessageBox::Ok);
            fileSaved = false;
        }
        else
        {
            QFile file;
            file.setFileName(fileName);
            file.open(QIODevice::WriteOnly);

            QString str;
            int size = ui->spinBox_Col->value();
            str.setNum(size);
            str.append("\n"); //добавляем перевод строки

            file.write(str.toLocal8Bit());

            for (int j=0; j<size; j++)
            {
                if (ui->tableWidget->item(0,j)==nullptr)
                {
                    QTableWidgetItem * ti;
                    ti = new QTableWidgetItem;
                    ui->tableWidget->setItem(0,j,ti);
                }
                str = ui->tableWidget->item(0,j)->text();
                str.append("\n"); //добавляем перевод строки

                file.write(str.toLocal8Bit());
                fileSaved = true;
                QFileInfo fileInfo(fileName);
                QString base = fileInfo.baseName();
                setWindowTitle(base + " — Таблицы");
                globalName = fileName;
            }
            file.close();
        }
    }
}




void MainWindow::checkTable() //проверка таблицы на наличие ошибок
{
    int column = ui->tableWidget->columnCount();
    bool gl_flag = true; // признак успешного формирования массива
    bool flag_position = false; // признак ошибочности формирования чего-л.
    isError = false;

    try
    {
        mas_table = new int[column];
    }
    catch (std::bad_alloc&)
    {
        QMessageBox::information(this,"Ошибка","Невозможно выделить память.",QMessageBox::Ok);
        gl_flag = false;
    }

    if (gl_flag) // если память выделили
    {
        for (int j=0; j<column; j++ ) //проходим по значениям
        {
            QTableWidgetItem *item = ui->tableWidget->item(0,j);
            if (item == nullptr) //ячейка не существует
            {
                item = new QTableWidgetItem();
                ui->tableWidget->setItem(0,j, item);

                ui->tableWidget->item(0,j)->setBackground(Qt::red); //окрашиваем ячейки
                if (!flag_position)
                {
                    ui->tableWidget->selectColumn(j);
                    ui->tableWidget->scrollToItem(ui->tableWidget->item(0,j));
                    QMessageBox::information(this, "Ошибка!", "Неверное значение или пустая ячейка", QMessageBox::Ok);
                    isError = true;
                }
                flag_position = true;
                //очищаем значения при ошибке
            }
            bool flag = false;
            //переменная для позиционирования ошибки
            mas_table[j]=ui->tableWidget->item(0,j)->text().toInt(&flag);//перевод значения ячейки в int
            if (!flag) // в ячейке не int
            {
                ui->tableWidget->item(0,j)->setBackground(Qt::red); //окраска ячейки
                if (!flag_position)
                {
                    ui->tableWidget->selectRow(j);
                    ui->tableWidget->scrollToItem(ui->tableWidget->item(0,j));
                    QMessageBox::information(this, "Ошибка!", "Неверное значение или пустая ячейка", QMessageBox::Ok);
                    isError = true;
                }
                flag_position = true;
            }
            else
            {
                ui->tableWidget->item(0,j)->setBackground(Qt::white);
                //                isError = false;
            }
        }
    }

    else // если что-то не так
    {
        QMessageBox::information(this,"Ошибка","Таблица содержит некорректные значения или незаполненные клетки", QMessageBox::Ok);
    }
    //очищаем, что есть
    delete [] mas_table;
    mas_table=nullptr;
    no_auto_change = true;
}


void MainWindow::on_pushButton_OpenTXT_clicked() //открытие TXT
{
    checkTable();
    if (!isError)
    {
        int answer = QMessageBox::question(this, "Таблицы", "Сохранить данный файл перед открытием нового?", QMessageBox::Yes, QMessageBox::No);
        if (answer == QMessageBox::Yes)
        {

            on_pushButton_SaveTXT_clicked();
            if (fileSaved)
            {
                TXTopening();
            }
        }
        else if (QMessageBox::No)
        {
            TXTopening();
        }
    }
    else
    {
        int answer_2 = QMessageBox::question(this, "Ошибки в таблице", "Открыть новый файл? Данные не сохранятся", QMessageBox::Yes | QMessageBox::No);
        if (answer_2 == QMessageBox::Yes)
        {
            TXTopening();
        }
    }
}


void MainWindow::openCheck() //проверка файла перед открытием
{
    QFile file;
    file.setFileName(globalName);
    file.open(QIODevice::ReadOnly);

    QByteArray ba;
    QString str;
    int size;
    int count = 0;
    int data = 0;
    bool flag;
    ba=file.readLine();
    str.clear();
    str.append(ba);
    str.remove("\n");
    size = str.toInt();
    while (!file.atEnd())
    {
        ba=file.readLine();
        str.clear();
        str.append(ba);
        str.remove("\n");
        data = str.toInt(&flag);
        if (flag)
        {
            count++;
        }
    }
    if (count == size && size!=0)
    {
        fileIsOk=true;
    }
    else
    {
        fileIsOk=false;
    }

}





void MainWindow::on_pushButton_SaveBIN_clicked() //сохраняем бинарник
{
    checkTable();
    if (!isError)
    {
        QString fileName;
        fileName = QFileDialog::getSaveFileName(this, "Сохранить в формате BIN", "C:\\Users\\student\\Documents", "Binary files (*.bin)");

        if (fileName.isEmpty())
        {
            QMessageBox::information(this, "Ошибка", "Файл не выбран", QMessageBox::Ok);
            fileSaved = false;
        }
        else
        {
            QFile file;
            file.setFileName(fileName);
            file.open(QIODevice::WriteOnly);

            char *mas;
            int size_int = sizeof(int);
            mas = new char[size_int];

            int size = ui->spinBox_Col->value();
            memcpy(mas,&size,size_t(size_int));
            file.write(mas, size_int);

            for (int j=0; j<size; j++)
            {
                if (ui->tableWidget->item(0,j)==nullptr)
                {
                    QTableWidgetItem * ti;
                    ti = new QTableWidgetItem;
                    ui->tableWidget->setItem(0,j,ti);
                }
                int tmp = ui->tableWidget->item(0,j)->text().toInt();
                memcpy(mas,&tmp,size_t(size_int));
                file.write(mas,size_int);

            }
            fileSaved = true;
            QFileInfo fileInfo(fileName);
            QString base = fileInfo.baseName();
            setWindowTitle(base + " — Таблицы");
            globalName = fileName;
            file.close();
            delete [] mas;
        }
    }

}

void MainWindow::on_pushButton_OpenBIN_clicked() //нажата кнопка открыть bin
{
    checkTable();
    if (!isError)
    {
        int answer = QMessageBox::question(this, "Таблицы", "Сохранить данный файл перед открытием нового?", QMessageBox::Yes | QMessageBox::No | QMessageBox::Cancel);
        if (answer == QMessageBox::Yes)
        {
            on_pushButton_SaveBIN_clicked();
            if (fileSaved)
            {
                BINopening();
            }
        }
        else if (answer == QMessageBox::No)
        {
            BINopening();
        }
    }
    else
    {
        int answer_2 = QMessageBox::question(this, "Ошибки в таблице", "Открыть новый файл? Данные не сохранятся", QMessageBox::Yes | QMessageBox::No);
        if (answer_2 == QMessageBox::Yes)
        {
            BINopening();
        }
    }
}

void MainWindow::BINopening() //открываем бинарный файл
{
    no_auto_change = false;
    QString filename;
    filename = QFileDialog::getOpenFileName(this, "Открыть в формате BIN", "C:\\Users\\student\\Documents", "Binary files (*.bin)");

    if (filename.isEmpty())
    {
        QMessageBox::information(this, "Ошибка", "Файл не выбран.", QMessageBox::Ok);
    }
    else
    {
        QFile file;
        file.setFileName(filename);
        file.open(QIODevice::ReadOnly);

        char * mas;
        int size_int = sizeof(int);
        mas = new char[size_int];
        int size, tmp;
        file.read(mas,size_int);
        memcpy(&size,mas,size_t(size_int));

        if (size <= 0 || size > MAX_MAS_SIZE || size == NULL) //если размер меньше/равен нулю или превышает максимальные размеры массива
        {
            QMessageBox::information(this, "Ошибка", "Неверный формат", QMessageBox::Ok);
            file.close();
            delete [] mas;
            return;
        }
        else
        {
            int bin_size = size*size_int;
            int bin_size_2 = file.size()-size_int;
            if (bin_size != bin_size_2) //сравниваем заявленный размер с фактическим
            {
                QMessageBox::information(this, "Ошибка", "Неверный формат", QMessageBox::Ok);
                file.close();
                delete [] mas; //освобождаем после работы
                return;
            }
            else
            {
                //освобождаем
                delete [] mas_table;
                mas_table=nullptr;

                no_auto_change = false;

                int column = size;
                bool gl_flag = true; // признак успешного формирования массива

                try
                {
                    mas_table = new int[column];
                }
                catch (std::bad_alloc&)
                {
                    QMessageBox::information(this,"Ошибка","Нельзя выделить память",QMessageBox::Ok);
                    file.close();
                    delete [] mas; //освобождаем после  работы
                    gl_flag = false;
                }

                if (gl_flag) // если память выделили
                {

                    for (int j = 0; j < (file.size() / size_int) - 1; j++)
                    {
                        if (file.atEnd())
                        {
                            QMessageBox::information(this, "Ошибка", "Неверный формат", QMessageBox::Ok);
                            file.close();
                            delete [] mas; //освобождаем после  работы
                            return;
                        }

                        file.read(mas,size_int);

                        memcpy(&tmp,mas,size_t(size_int));
                        mas_table[j] = tmp;
                    }
                    ui->spinBox_Col->setValue(size);

                    for (int j=0; j < size; j++)
                    {
                        if (ui->tableWidget->item(0,j)==nullptr) //выделяем память
                        {
                            QTableWidgetItem * ti;
                            ti = new QTableWidgetItem;
                            ui->tableWidget->setItem(0,j,ti);
                        }

                        ui->tableWidget->item(0,j)->setText(QString::number(mas_table[j]));
                        ui->tableWidget->item(0,0)->setBackground(Qt::white);
                    }
                    globalName = filename;
                    QFileInfo fileInfo(globalName);
                    QString base = fileInfo.baseName();
                    setWindowTitle(base + " — Таблицы");
                    file.close();
                }
            }
        }
        file.close();
        delete [] mas; //освобождаем после  работы
    }
    no_auto_change = true;
}

void MainWindow::TXTopening() //открытие текстовика
{
    QString fileName;
    fileName = QFileDialog::getOpenFileName(this,"Открыть в формате TXT","C:\\Users\\student\\Documents","Text files (*.txt)");
    if (fileName.isEmpty())
    {
        QMessageBox::information(this,"Ошибка","Файл не выбран",QMessageBox::Ok);
    }
    else
    {
        globalName = fileName;
        openCheck();

        if (fileIsOk)
        {
            QFile file;
            file.setFileName(fileName);
            file.open(QIODevice::ReadOnly);
            int size;
            QString str;
            QByteArray ba;
            bool flag;

            ba = file.readLine();

            str.clear();
            str.append(ba);
            str.remove("\n");

            size = str.toInt(&flag);

            if (!flag)
            {
                QMessageBox::information(this,"Ошибка","Неверный формат",QMessageBox::Ok);
            }
            else
            {
                ui->spinBox_Col->setValue(size);
                for (int j=0; j<size; j++)
                {
                    if (ui->tableWidget->item(0,j) == nullptr)
                    {
                        QTableWidgetItem * ti;
                        ti = new QTableWidgetItem;
                        ui->tableWidget->setItem(0,j,ti);
                    }
                    ba = file.readLine();

                    str.clear();
                    str.append(ba);
                    str.remove("\n");
                    ui->tableWidget->item(0,j)->setText(str);

                }
            }
            globalName = fileName;
            QFileInfo fileInfo(globalName);
            QString base = fileInfo.baseName();
            setWindowTitle(base + " — Таблицы");
            file.close();

        }
        else
        {
            QMessageBox::information(this, "Ошибка", "Неверный формат", QMessageBox::Ok);
        }
    }
}
