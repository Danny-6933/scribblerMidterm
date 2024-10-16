#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
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
    void saveFileSlot(QList<MouseEvent> e);

    void startCapture();
    void endCapture();


    void lineView();
    void dotView();
    // void mouseMoved(QPoint pos, QColor col);
};
#endif // MAINWINDOW_H
