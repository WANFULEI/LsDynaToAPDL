#include "mainwindow.h"
#include "ui_mainwindow.h"


MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    ui->Convert->setDisabled(true);
    ui->label->setText("Dimension: 0 Mb");
    ui->Nodeinfo->setText("Total number node: 0");
    ui->ElemInfo->setText("Total number element shell: 0");

    // instanziate classes to work Lsdyna/APDL
    converter = new ConverterSintaX();
    node = new LsDynaSintax::Node();
    shell = new LsDynaSintax::ElementShell();
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::on_LoadFile_clicked()
{
    qDebug()<<"Open file dialog...";
    QString fileName = QFileDialog::getOpenFileName(this,tr("Open file"), "", tr("All Files (*.*)"));

    //instaziate class to retrive information file
    managefile = new ManageFile(fileName);
    ui->label->setText("Dimension: " + QString().setNum(managefile->getsize(),'d',2) + " Mb");
    if(fileName != "")
    {
        ui->lineEdit->setText(fileName);

        //clear if  already run
        node->Clear();
        shell->Clear();

        // Create a progress dialog.
        QProgressDialog dialog;
        dialog.setLabelText(QString("Reading file..."));

        // Create a QFutureWatcher and connect signals and slots.
        QFutureWatcher<void> futureWatcher;
        QObject::connect(&futureWatcher, SIGNAL(finished()), &dialog, SLOT(reset()));
        QObject::connect(&dialog, SIGNAL(canceled()), &futureWatcher, SLOT(cancel()));
        QObject::connect(&futureWatcher, SIGNAL(progressRangeChanged(int,int)), &dialog, SLOT(setRange(int,int)));
        QObject::connect(&futureWatcher, SIGNAL(progressValueChanged(int)), &dialog, SLOT(setValue(int)));

        // Start the computation.
        futureWatcher.setFuture(QtConcurrent::run(read, fileName, converter, node, shell));

        // Display the dialog and start the event loop.
        dialog.exec();

        futureWatcher.waitForFinished();

        // Query the future to check if was canceled.
        qDebug() << "Canceled?" << futureWatcher.future().isCanceled();
    }

    else
    {
        qDebug() <<"Missing input file, launch information message.";
        QMessageBox::information(this, tr("Info"), "The document has not been loaded.");
    }

    //active conversion
    ui->Convert->setEnabled(true);

    //update the output line edit
    managefile->setnewname();
    ui->lineEdit_2->setText(managefile->getnewname());

}

void MainWindow::on_Convert_clicked()
{
    // Create a progress dialog.
    QProgressDialog dialog;
    dialog.setLabelText(QString("Writing file..."));

    // Create a QFutureWatcher and connect signals and slots.
    QFutureWatcher<void> futureWatcher;
    QObject::connect(&futureWatcher, SIGNAL(finished()), &dialog, SLOT(reset()));
    QObject::connect(&dialog, SIGNAL(canceled()), &futureWatcher, SLOT(cancel()));
    QObject::connect(&futureWatcher, SIGNAL(progressRangeChanged(int,int)), &dialog, SLOT(setRange(int,int)));
    QObject::connect(&futureWatcher, SIGNAL(progressValueChanged(int)), &dialog, SLOT(setValue(int)));

    // Start the computation.
    futureWatcher.setFuture(QtConcurrent::run(APDLsintax::Writer, managefile->getnewname(), node->getNodeStructure(), shell->getElementStructure()));

    // Display the dialog and start the event loop.
    dialog.exec();

    futureWatcher.waitForFinished();

    // Query the future to check if was canceled.
    qDebug() << "Canceled?" << futureWatcher.future().isCanceled();
}

void MainWindow::on_Exit_released()
{
    //clear the heap
    delete converter;
    delete node;
    delete shell;
    delete managefile;

    qDebug()<<"closing app...";
    QApplication::quit();
}

void MainWindow::information()
{
    qDebug() <<"success!";
}

void MainWindow::open_about()
{

}

void MainWindow::on_actionInformazioni_triggered()
{
    about = new About(this);
    about->show();
}
