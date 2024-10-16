#include "mainwindow.h"
#include "scribbler.h"

#include <QtWidgets>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent){

    scribbler = new Scribbler();

    setCentralWidget(scribbler);

    QHBoxLayout *mainLayout = new QHBoxLayout;

    tabWidget = new QTabWidget;
    mainLayout->addWidget(scribbler);
    mainLayout->addWidget(tabWidget);
    tabWidget->setHidden(true);

    QWidget *centralWidget = new QWidget(this);
    centralWidget->setLayout(mainLayout);
    setCentralWidget(centralWidget);

    QAction *resetScribbleAct = new QAction("Reset Scribble");
    connect(resetScribbleAct, &QAction::triggered, this, &MainWindow::resetScribble);
    resetScribbleAct->setShortcut(Qt::CTRL | Qt::Key_R);

    QAction *openFileAct = new QAction("Open file");
    connect(openFileAct, &QAction::triggered, this, &MainWindow::openFileSlot);
    openFileAct->setShortcut(Qt::CTRL | Qt::Key_O);


    QAction *saveFileAct = new QAction("Save file");
    // connect(saveFileAct, &QAction::triggered, this, &MainWindow::saveFileSlot);
    saveFileAct->setShortcut(Qt::CTRL | Qt::Key_S);

    QMenu *fileMenu = new QMenu("File");
    fileMenu->addAction(resetScribbleAct);
    fileMenu->addAction(openFileAct);
    fileMenu->addAction(saveFileAct);
    menuBar()->addMenu(fileMenu);

    QAction *startCaptureAct = new QAction("Begin Capture");
    connect(startCaptureAct, &QAction::triggered, this, &MainWindow::startCapture);
    startCaptureAct->setShortcut(Qt::CTRL | Qt::Key_B);

    QAction *endCaptureAct = new QAction("End Capture");
    connect(endCaptureAct, &QAction::triggered, scribbler, &Scribbler::sendEventData);
    endCaptureAct->setShortcut(Qt::CTRL | Qt::Key_E);

    connect(scribbler, &Scribbler::emitEventData, this, &MainWindow::saveFileSlot);


    QMenu *captureMenu = new QMenu("Capture");
    captureMenu->addAction(startCaptureAct);
    captureMenu->addAction(endCaptureAct);
    menuBar()->addMenu(captureMenu);


    QAction *lineView = new QAction("Line View");
    connect(lineView, &QAction::triggered, this, &MainWindow::lineView);
    lineView->setShortcut(Qt::SHIFT | Qt::Key_L);

    QAction *dotView = new QAction("Dot View");
    connect(dotView, &QAction::triggered, this, &MainWindow::dotView);
    dotView->setShortcut(Qt::SHIFT | Qt::Key_D);

    QMenu *viewMenu = new QMenu("View");
    viewMenu->addAction(lineView);
    viewMenu->addAction(dotView);
    menuBar()->addMenu(viewMenu);


    QSettings settings("Very Real Company LLC", "Scribbler");
    lastDir = settings.value("lastDir", "").toString();
}

MainWindow::~MainWindow() {
    QSettings settings("Very Real Company LLC", "Graphics1");
    settings.setValue("lastDir", lastDir);
}

void MainWindow::resetScribble() {
    scribbler->clear();
    tabWidget->setHidden(true);
    tabWidget->clear();
    qDebug() << "resetting";
}

void MainWindow::openFileSlot() {
    qDebug() << "clickéd";

    QString fName = QFileDialog::getOpenFileName(this, "Select file", lastDir);
    if (fName.isEmpty()) return;

    lastDir = QFileInfo(fName).absolutePath();
    // qDebug() << lastDir;


    QImage image(fName);
    if (image.isNull()) return;


    scribbler->loadFile();
}


void MainWindow::saveFileSlot(QList<MouseEvent> e) {
    QString fOutName = QFileDialog::getSaveFileName(this, "Save file to:", QStandardPaths::writableLocation(QStandardPaths::DesktopLocation), tr("Binary file (*.bin)"));
    // *.bin

    if (fOutName.isEmpty()) return;

    QFile outFile(fOutName);

    if (!outFile.open(QIODevice::WriteOnly | QIODevice::Truncate)) {
        QMessageBox::information(this, "Error", QString("Cannot write to file \"%1\"").arg(fOutName));
        return;
    }

    QDataStream out(&outFile);

    out << e;

    outFile.close();

    // message to user
    QMessageBox::information(this, "File Update", "New File saved");
}

void MainWindow::startCapture() {
    tabWidget->setHidden(false);
}

void MainWindow::endCapture() {
    // emit sendEventData();
    tabWidget->setHidden(false);
}


void MainWindow::lineView() {
    scribbler->drawLines();
}

void MainWindow::dotView() {
    scribbler->drawDots();
}

