#ifndef HARDWARE_H
#define HARDWARE_H

#include <QDialog>
#include <QImage>

namespace Ui {
class Hardware;
}

class Hardware : public QDialog
{
    Q_OBJECT

public:
    explicit Hardware(QWidget *parent = 0);
    ~Hardware();

private:
    Ui::Hardware *ui;
    QImage *image;
};

#endif // HARDWARE_H
