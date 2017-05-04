#include "mainwindow.h"
#include "ui_mainwindow.h"

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);
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
            for(int i = 0; i < byteArrayFile->count(); i++)
            {
                charArrayFile.push_back(byteArrayFile->at(i));
                ui->progressBar_dataByte->setValue((double)i / byteArrayFile->count() * 100.0);
            }
            ui->progressBar_dataByte->setValue(100.0);
            ui->label_FileSize_value->setText(QString::number(byteArrayFile->count()));
        }
    }
}

void MainWindow::on_actionCompress_triggered()
{
    if(charArrayFile.size() > 0)
    {
        byteArrayLibrary.clear();
        byteArrayLibrary.append(QByteArray(libraryByteRate, 0));
        charArrayCopmression.clear();

        QByteArray wordsFind;
        char word = 0;
        int curFileByte = 0;
        int curLibraryAddr = 0;
        int libraryAddr = 0;
        QList<char> ByteLibraryAddr;
        bool hasAddr = false;

        clock_t time = clock();

        do
        {
            wordsFind.clear();
            word = 0;
            curLibraryAddr = 1;
            libraryAddr = 0;
            hasAddr = false;
            do
            {
                word = charArrayFile.at(curFileByte);
                wordsFind.append(charArrayFile.at(curFileByte));
                curLibraryAddr = byteArrayLibrary.indexOf(wordsFind, curLibraryAddr);
                if(curLibraryAddr > 0)
                {
                    libraryAddr = curLibraryAddr;
                    if(curFileByte == (int)charArrayFile.size() - 1)
                    {
                        wordsFind.clear();
                        word = 0;
                        hasAddr = false;
                    }
                    else
                    {
                        curFileByte++;
                        hasAddr = true;
                    }
                }
                else
                {
                    hasAddr = false;
                }
            }
            while(hasAddr);

            ByteLibraryAddr.clear();
            for(char b = libraryByteRate - 1; b >= 0; b--)
            {
                ByteLibraryAddr.append((libraryAddr >> (b * 8)) & 0xFF);
            }

            for(char i = 0; i < ByteLibraryAddr.count(); i++)
                charArrayCopmression.push_back(ByteLibraryAddr.at(i));
            charArrayCopmression.push_back(word);
            if(byteArrayLibrary.count() < librarySize)
                byteArrayLibrary.append(wordsFind);

            ui->progressBar_dataByte->setValue((double)curFileByte / charArrayFile.size() * 100.0);
            ui->progressBar_dataLibrary->setValue((double)byteArrayLibrary.count() / librarySize * 100.0);
            curFileByte++;
        }
        while(curFileByte < (int)charArrayFile.size());

        ui->label_ResultSize_value->setText(QString::number(charArrayCopmression.size()));
        time = clock() - time;
        ui->label_Time_value->setText(QString::number(time));

        QString fileName = QFileDialog::getSaveFileName(this, tr("Save File"), "", "");
        if(!fileName.isEmpty())
        {
            file->setFileName(fileName);
            if(file->open(QIODevice::WriteOnly))
            {
                ui->progressBar_dataByte->setValue(0.0);
                delete byteArrayFile;
                byteArrayFile = new QByteArray();
                for(std::vector<char>::iterator i = charArrayCopmression.begin(); i != charArrayCopmression.end(); i++)
                {
                    byteArrayFile->append(*i);
                }
                file->write(*byteArrayFile);
                ui->progressBar_dataByte->setValue(100.0);
                file->close();
            }
        }
    }
}

void MainWindow::on_actionDeCompress_triggered()
{
    if(charArrayFile.size() > 0)
    {
        charArrayDeCopmression.clear();
        byteArrayLibrary.clear();
        byteArrayLibrary.append(QByteArray(libraryByteRate, 0));
        QByteArray findAddr;
        QByteArray newWord;
        clock_t time = clock();

        int i = 0;
        do
        {
            findAddr.clear();
            newWord.clear();
            for(int b = 0; b < libraryByteRate; b++)
            {
                findAddr.append(charArrayFile.at(i + b));
            }
            int addrInt = 0;
            for(int i = 0; i < libraryByteRate; i++)
            {
                addrInt += (uchar)findAddr.at(libraryByteRate - 1 - i) * (pow(256, i));
            }
            if(addrInt != 0)
                newWord.append(byteArrayLibrary.at(addrInt));
            if(!((i + libraryByteRate) == ((int)charArrayFile.size() - 1) && (charArrayFile.at(i + libraryByteRate) == 0)))
                newWord.append(charArrayFile.at(i + libraryByteRate));

            if(byteArrayLibrary.count() < librarySize)
            {
                byteArrayLibrary.append(newWord);
            }

            for(int j = 0; j < newWord.count(); j++)
                charArrayDeCopmression.push_back((uchar)newWord.at(j));
            i+= (libraryByteRate + 1);
            ui->progressBar_dataByte->setValue((double)i / charArrayFile.size() * 100.0);
        }
        while(i < (int)charArrayFile.size());

        ui->progressBar_dataByte->setValue(100.0);
        ui->label_ResultSize_value->setText(QString::number(charArrayDeCopmression.size()));
        time = clock() - time;
        ui->label_Time_value->setText(QString::number(time));

        QString fileName = QFileDialog::getSaveFileName(this, tr("Save File"), "", "");
        if(!fileName.isEmpty())
        {
            file->setFileName(fileName);
            if(file->open(QIODevice::WriteOnly))
            {
                ui->progressBar_dataByte->setValue(0.0);
                byteArrayFile->clear();
                for(std::vector<char>::iterator i = charArrayDeCopmression.begin(); i != charArrayDeCopmression.end(); i++)
                {
                    byteArrayFile->append(*i);
                }
                file->write(*byteArrayFile);
                ui->progressBar_dataByte->setValue(100.0);
                file->close();
            }
        }
    }
}

void MainWindow::on_doubleSpinBox_LibraryByteRate_valueChanged(double arg1)
{
    libraryByteRate = (char)arg1;
    librarySize = pow(256, libraryByteRate);
    ui->label_LibraryLength_value->setText(QString::number(librarySize));
}
