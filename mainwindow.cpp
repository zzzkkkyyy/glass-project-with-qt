#include "mainwindow.h"
#include "ui_mainwindow.h"

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow),
    mode(0), percent("%")
{
    ui->setupUi(this);
    image = new QImage;
    hardware = new Hardware(this);
    about = new About(this);

    ui->SizeLabel->setText(QString::number(ui->SizeSlider->value()) + percent);
    ui->SizeSlider->setDisabled(true);

    operatorgroup = new QButtonGroup(this);
    operatorgroup->addButton(ui->BenBian, 0);
    operatorgroup->addButton(ui->DaoBianBuLiang, 1);
    operatorgroup->addButton(ui->HuaHeng, 2);
    operatorgroup->addButton(ui->QueJiao, 3);
    operatorgroup->addButton(ui->BenJiao, 4);
    operatorgroup->addButton(ui->All, 5);
    ui->BenBian->setChecked(true);

    connect(ui->BenBian, SIGNAL(clicked()), this, SLOT(on_groupBox_clicked()));
    connect(ui->DaoBianBuLiang, SIGNAL(clicked()), this, SLOT(on_groupBox_clicked()));
    connect(ui->HuaHeng, SIGNAL(clicked()), this, SLOT(on_groupBox_clicked()));
    connect(ui->QueJiao, SIGNAL(clicked()), this, SLOT(on_groupBox_clicked()));
    connect(ui->BenJiao, SIGNAL(clicked()), this, SLOT(on_groupBox_clicked()));
    connect(ui->All, SIGNAL(clicked()), this, SLOT(on_groupBox_clicked()));
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::on_OpenButtom_clicked()
{
    ui->SizeSlider->setValue(ui->SizeSlider->maximum());
    QString tmp;
    QString path = QFileDialog::getOpenFileName(this, tr("Open File"), ".");
    if(!path.isEmpty())
    {
        QFile file(path);
        if (!file.open(QIODevice::ReadOnly | QIODevice::Text))
        {
            QMessageBox::warning(this, tr("Read File"), tr("Cannot open file:\n%1").arg(path));
            return;
        }
        ui->SizeSlider->setDisabled(false);
        openpath = path;
        image = new QImage;
        ProcessImage = new QImage;
        image->load(path);
        //ProcessImage->load(path);
        height = image->height();
        width = image->width();
        ui->SizeLabel->setText(QString::number(ui->SizeSlider->value()) + percent);
        *ProcessImage = *image;
        //image->scaled(ui->InputImage->width(), ui->InputImage->height());
        //delete scene, scene2;
        scene = new QGraphicsScene;
        scene2 = new QGraphicsScene;
        scene->addPixmap(QPixmap::fromImage(*image));
        scene2->addPixmap(QPixmap::fromImage(*ProcessImage));
        ui->InputImage->setScene(scene);
        ui->InputImage->show();
        ui->OutputImage->setScene(scene2);
        ui->OutputImage->show();
        //file.close();
    }
    else
    {
        QMessageBox::warning(this, tr("Path"), tr("You did not select any file."));
    }
}

void MainWindow::on_ProcessButtom_clicked()
{
    Process pro;
    qDebug() << "input size:" << image->height() << image->width();
    pro.init(image);
    //pro.initByPath(openpath);
    if (mode == 0)
    {
        pro.bengbian();
    }
    else if (mode == 1)
    {
        pro.daobian();
    }
    else if (mode == 2)
    {
        pro.huaheng();
    }
    else if (mode == 3)
    {
        pro.quejiao();
    }
    else if (mode == 4)
    {
        pro.bengjiao();
    }
    else if (mode == 5)
    {
        pro.all();
    }
    else
    {
        qDebug() << "no match mode";
    }
    *ProcessImage = pro.generate();
    //delete scene2;
	QPixmap pix = QPixmap::fromImage(*image);
	scene->clear();
	scene->addPixmap(pix);
	ui->InputImage->setScene(scene);
	ui->InputImage->show();
    scene2 = new QGraphicsScene;
    scene2->addPixmap(QPixmap::fromImage(*ProcessImage));
    ui->OutputImage->setScene(scene2);
    ui->OutputImage->show();
	ui->SizeSlider->setValue(100);
	ui->SizeLabel->setText(QString::number(ui->SizeSlider->value()) + percent);
}

void MainWindow::on_groupBox_clicked()
{
    mode = operatorgroup->checkedId();
}

void MainWindow::on_HardwareButtom_clicked()
{
    hardware->show();
}

void MainWindow::on_About_clicked()
{
    about->show();
}

void MainWindow::on_SaveButtom_clicked()
{
    QString path = QFileDialog::getSaveFileName(this, tr("Save File"), ".", tr("JPEG(*.jpg)"));
    if(!path.isEmpty())
    {
        QFile file(path);
        if (!file.open(QIODevice::WriteOnly | QIODevice::Text))
        {
            QMessageBox::warning(this, tr("Write File"), tr("Cannot open file:\n%1").arg(path));
            return;
        }
        ProcessImage->save(path);
        file.close();
    }
    else
    {
        QMessageBox::warning(this, tr("Path"), tr("You did not select any file."));
    }
}

void MainWindow::on_SizeSlider_valueChanged(int value)
{
    int new_height = height * value * 1.0 / ui->SizeSlider->maximum();
    int new_width = width * value * 1.0 / ui->SizeSlider->maximum();
    QPixmap pix = QPixmap::fromImage(*image);
    pix = pix.scaled(new_width, new_height);
    scene->clear();
    scene->addPixmap(pix);
    pix = QPixmap::fromImage(*ProcessImage);
    pix = pix.scaled(new_width, new_height);
    scene2->clear();
    scene2->addPixmap(pix);
    ui->InputImage->setScene(scene);
    ui->OutputImage->setScene(scene2);
    ui->InputImage->show();
    ui->OutputImage->show();
    ui->SizeLabel->setText(QString::number(ui->SizeSlider->value()) + percent);
}
