#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QDebug>
#include <QMainWindow>
#include <QFile>
#include <QByteArray>
#include <QFileDialog>
#include <cmath>
#include <ctime>
#include <vector>
#include "clzwcompressor.h"
#include "clzwdecompressor.h"

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT
    QFile* file;
    QByteArray* byteArrayFile;
    std::vector<uchar> charArrayFile;

    char libraryByteRate = 1;
    int librarySize;

public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();

private slots:
    void on_actionOpen_triggered();

    void on_actionCompress_triggered();

    void on_actionDeCompress_triggered();

    void on_doubleSpinBox_LibraryByteRate_valueChanged(double arg1);

private:
    Ui::MainWindow *ui;
};

#endif // MAINWINDOW_H
