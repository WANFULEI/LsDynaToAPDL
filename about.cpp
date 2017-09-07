#include "about.h"
#include "ui_about.h"
#include <QLabel>
#include <QImage>

About::About(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::About)
{
    //define window property
    ui->setupUi(this);
    this->setWindowTitle("");

    //define plain text object and content
    ui->plainTextEdit->setReadOnly(true);
    ui->plainTextEdit->appendPlainText(read(":/Resources/Resources/License/License.txt"));

    //set image property
    QPixmap imageObject(":/Resources/Resources/Icon/generic.png");
    imageObject.scaled(120,120);

    //define visualization
    ui->label->setPixmap(imageObject);
}

About::~About()
{
    delete ui;
}

QString read(QString pfileName)
{
    QFile fileInput(pfileName);
    if(!fileInput.open(QFile::ReadOnly | QFile::Text))
    {
        qDebug() << "failed to poen file";
        return NULL;
    }

    QTextStream text(&fileInput);
    QString all = text.readAll();

    qDebug() << all;

    fileInput.close();
    return all;
}
