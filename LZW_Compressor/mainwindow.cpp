#include "mainwindow.h"
#include "ui_mainwindow.h"

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    this->setFixedSize(QSize(480, 294));
    setWindowFlags(Qt::Window | Qt::WindowMinimizeButtonHint | Qt::WindowCloseButtonHint);
    file = new QFile();
    byteArrayFile = new QByteArray();

    libraryByteRate = 1;
    librarySize = pow(256, libraryByteRate);
    ui->label_LibraryLength_value->setText(QString::number(librarySize));
}

MainWindow::~MainWindow()
{
    delete file;
    delete byteArrayFile;
    delete ui;
}

void MainWindow::on_actionOpen_triggered()
{
    QString fileName = QFileDialog::getOpenFileName(this, tr("Open File"), "", "");
    if(!fileName.isEmpty())
    {
        file->setFileName(fileName);
        if(file->open(QIODevice::ReadOnly))
        {
            delete byteArrayFile;
            ui->groupBox->setTitle(fileName);
            byteArrayFile = new QByteArray(file->readAll());
            file->close();
            charArrayFile.clear();
            char* begin = reinterpret_cast<char*>(byteArrayFile->data());
            char* end = begin + byteArrayFile->length();
            charArrayFile.assign(begin, end);
            ui->progressBar_dataByte->setValue(100.0);
            ui->label_FileSize_value->setText(QString::number(byteArrayFile->count()));
        }
    }
}

void MainWindow::on_actionCompress_triggered()
{
    CLZWCompressor c;
    std::vector<uchar> out;
    time_t t = clock();

    connect(&c, SIGNAL(passed_process(int)), ui->progressBar_dataByte, SLOT(setValue(int)));
    connect(&c, SIGNAL(library_capacity_changed(int)), ui->progressBar_dataLibrary, SLOT(setValue(int)));
    c.set_library_address_length((int)ui->doubleSpinBox_LibraryByteRate->value());
    c.compress(&charArrayFile, &out);

    ui->label_Time_value->setText(QString::number(clock() - t));

    QString fileName = QFileDialog::getSaveFileName(this, tr("Save File"), "", "");
    if(!fileName.isEmpty())
    {
        file->setFileName(fileName);
        if(file->open(QIODevice::WriteOnly))
        {
            byteArrayFile = new QByteArray();
            byteArrayFile->setRawData(reinterpret_cast<const char*>(out.data()), out.size());
            file->write(*byteArrayFile);
            file->close();
        }
    }
}

void MainWindow::on_actionDeCompress_triggered()
{
    CLZWDecompressor d;
    std::vector<uchar> out;
    time_t t = clock();

    connect(&d, SIGNAL(passed_process(int)), ui->progressBar_dataByte, SLOT(setValue(int)));
    connect(&d, SIGNAL(library_capacity_changed(int)), ui->progressBar_dataLibrary, SLOT(setValue(int)));
    d.set_library_address_length((int)ui->doubleSpinBox_LibraryByteRate->value());
    d.decompress(&charArrayFile, &out);

    ui->label_Time_value->setText(QString::number(clock() - t));

    QString fileName = QFileDialog::getSaveFileName(this, tr("Save File"), "", "");
    if(!fileName.isEmpty())
    {
        file->setFileName(fileName);
        if(file->open(QIODevice::WriteOnly))
        {
            ui->progressBar_dataByte->setValue(0.0);
            byteArrayFile->clear();
            byteArrayFile->setRawData(reinterpret_cast<const char*>(out.data()), out.size());
            file->write(*byteArrayFile);
            ui->progressBar_dataByte->setValue(100.0);
            file->close();
        }
    }
}

void MainWindow::on_doubleSpinBox_LibraryByteRate_valueChanged(double arg1)
{
    libraryByteRate = (char)arg1;
    librarySize = pow(256, libraryByteRate);
    ui->label_LibraryLength_value->setText(QString::number(librarySize));
}
