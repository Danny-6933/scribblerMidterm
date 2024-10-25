#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QtWidgets>
#include <QGraphicsScene>
#include "scribbler.h"

class MainWindow : public QMainWindow
{
    Q_OBJECT

    QString lastDir;
    // QGraphicsScene scene;
    Scribbler *scribbler;
    QTabWidget *tabWidget;

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

public slots:
    void resetScribble();
    void openFileSlot();
    void saveFileSlot(QList<QList<MouseEvent>> e);
    void addTab(QList<MouseEvent> e);

   //  void startCapture();
    // void endCapture();


    void lineView();
    void dotView();
    // void mouseMoved(QPoint pos, QColor col);
};
#endif // MAINWINDOW_H
