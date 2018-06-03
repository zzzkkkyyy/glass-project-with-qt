#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QString>
#include <QFileDialog>
#include <QImage>
#include <QMessageBox>
#include <QButtonGroup>
#include <QString>
#include <QGraphicsScene>
#include <string>
#include <QDebug>
#include "hardware.h"
#include "about.h"
#include "process.h"

using std::string;

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();

private slots:
    void on_OpenButtom_clicked();

    void on_ProcessButtom_clicked();

    void on_groupBox_clicked();

    void on_HardwareButtom_clicked();

    void on_About_clicked();

    void on_SaveButtom_clicked();

    void on_SizeSlider_valueChanged(int value);

private:
    Ui::MainWindow *ui;
    QImage *image, *ProcessImage;
    int mode;
    QButtonGroup *operatorgroup;
    Hardware *hardware;
    About *about;
    QGraphicsScene *scene, *scene2;
    QString openpath;
    int height, width;
    QString percent;
};

#endif // MAINWINDOW_H
