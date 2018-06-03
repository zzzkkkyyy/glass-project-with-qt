#include "hardware.h"
#include "ui_hardware.h"

Hardware::Hardware(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::Hardware)
{
    ui->setupUi(this);

    image = new QImage;
    image->load(QString(":/hardware/Hardware.jpg"));
    QGraphicsScene *scene = new QGraphicsScene;
    scene->addPixmap(QPixmap::fromImage(*image));
    ui->HardwareImage->setScene(scene);
    ui->HardwareImage->show();
}

Hardware::~Hardware()
{
    delete ui;
}
