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

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT
    QFile* file;
    QByteArray* byteArrayFile;
    QList<QByteArray> byteArrayLibrary;
    std::vector<char> charArrayFile;
    std::vector<char> charArrayCopmression;
    std::vector<char> charArrayDeCopmression;

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
