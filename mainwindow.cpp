#include "mainwindow.h"
#include "scribbler.h"

#include <QtWidgets>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent){

    scribbler = new Scribbler();

    setCentralWidget(scribbler);

    QHBoxLayout *mainLayout = new QHBoxLayout;

    tabWidget = new QTabWidget;
    mainLayout->addWidget(scribbler, 1);
    mainLayout->addWidget(tabWidget, 1);
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
    connect(saveFileAct, &QAction::triggered, scribbler, &Scribbler::sendAllData);
    saveFileAct->setShortcut(Qt::CTRL | Qt::Key_S);

    QMenu *fileMenu = new QMenu("File");
    fileMenu->addAction(resetScribbleAct);
    fileMenu->addAction(openFileAct);
    fileMenu->addAction(saveFileAct);
    menuBar()->addMenu(fileMenu);

    QAction *startCaptureAct = new QAction("Begin Capture");
    connect(startCaptureAct, &QAction::triggered, scribbler, &Scribbler::startCapture);
    startCaptureAct->setShortcut(Qt::CTRL | Qt::Key_B);

    QAction *endCaptureAct = new QAction("End Capture");
    connect(endCaptureAct, &QAction::triggered, scribbler, &Scribbler::sendEventData);
    endCaptureAct->setShortcut(Qt::CTRL | Qt::Key_E);

    connect(scribbler, &Scribbler::emitEventData, this, &MainWindow::addTab);
    connect(scribbler, &Scribbler::emitAllData, this, &MainWindow::saveFileSlot);


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

    connect(tabWidget, &QTabWidget::currentChanged, scribbler, &Scribbler::updateTabOpacity);

    QMenu *viewMenu = new QMenu("View");
    viewMenu->addAction(lineView);
    viewMenu->addAction(dotView);
    menuBar()->addMenu(viewMenu);


    QSettings settings("Very Real Company LLC", "Scribbler");
    lastDir = settings.value("lastDir", "").toString();
}

MainWindow::~MainWindow() {
    QSettings settings("Very Real Company LLC", "Scribbler");
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

    QFile inFile(fName);
    QDataStream in (&inFile);
    QList<QList<MouseEvent>> eventsList;

    if (!inFile.open(QIODevice::ReadOnly)) {
        QMessageBox::information(this, "Error", QString("Cannot open file \"%1\"").arg(fName));
    }

    in >> eventsList;
    // qDebug() << lastDir;

    scribbler->loadFile(eventsList);

    for (auto &eventList : eventsList) {
        addTab(eventList);
    }
}

void MainWindow::saveFileSlot(QList<QList<MouseEvent>> e) {
    QString fOutName = QFileDialog::getSaveFileName(this, "Save file to:", QStandardPaths::writableLocation(QStandardPaths::DesktopLocation), tr("Binary file (*.bin)"));
    // *.bin

    if (fOutName.isEmpty()) return;

    QFile outFile(fOutName);

    if (!outFile.open(QIODevice::WriteOnly | QIODevice::Truncate)) {
        QMessageBox::information(this, "Error", QString("Cannot write to file \"%1\"").arg(fOutName));
        return;
    }

    qDebug() << "saving file with" << e.size();

    QDataStream out(&outFile);

    out << e;

    outFile.close();

    QMessageBox::information(this, "File Update", "New File saved");
}

void MainWindow::addTab(QList<MouseEvent> e) {
    tabWidget->setHidden(false);

    int size = e.size();

    QTableWidget *eventInfoTable = new QTableWidget(size, 4);
    eventInfoTable->setHorizontalHeaderLabels(QStringList() << "Action type" << "Pos (x,y)" << "Time (MM:SS)" << "Speed (px/ms)");
    for (int i = 0; i < size; ++i) {

        QString action;
        if (e[i].action == 0) action = "Press";
        if (e[i].action == 1) action = "Move";
        if (e[i].action == 2) action = "Release";

        QTableWidgetItem *eventAction = new QTableWidgetItem();
        eventAction -> setData(Qt::DisplayRole, action);
        eventInfoTable->setItem(i,0,eventAction);


        QTableWidgetItem *eventPosition = new QTableWidgetItem();
        eventPosition -> setData(Qt::DisplayRole, QString("(%1, %2 )").arg(e[i].pos.x()).arg(e[i].pos.y()));
        eventInfoTable->setItem(i,1,eventPosition);


        quint64 timeMS = e[i].time;
        quint64 sec = timeMS/1000;
        int min = sec/60;

        QString readableTime = (min < 10 ? "0" : "") + QString::number(min) + ":" + (sec%60 < 10 ? "0" : "") + QString::number(sec%60);

        QTableWidgetItem *eventTime = new QTableWidgetItem();
        eventTime->setData(Qt::DisplayRole, readableTime);
        eventInfoTable->setItem(i, 2, eventTime);

        QString speed = "N/A";
        QString mph = "N/A";

        if (action == "Move") {
            MouseEvent m = e[i];
            MouseEvent p = e[i-1];
            // change in dist
            int Ddist = sqrt((m.pos.x()-p.pos.x()) * (m.pos.x()-p.pos.x()) + (m.pos.y()-p.pos.y())*(m.pos.x()-p.pos.x()));
            // change in time
            int Dtime = e[i].time - e[i-1].time;
            speed = QString::number(Ddist / Dtime);
            mph = QString::number((Ddist*12672000) / (Dtime*3600000));
        }

        QTableWidgetItem *eventSpeed = new QTableWidgetItem();
        eventSpeed->setData(Qt::DisplayRole, speed);
        eventInfoTable->setItem(i,3,eventSpeed);
        // eventSpeed->setToolTip(QString("Thats %1 mph!").arg(mph));
    }


    int tabIndex = tabWidget->addTab(eventInfoTable, QString("Capture %1").arg(tabWidget->count() + 1));
    tabWidget->setCurrentIndex(tabIndex);
}

void MainWindow::lineView() {
    scribbler->drawLines();
}

void MainWindow::dotView() {
    scribbler->drawDots();
}

