#ifndef SCRIBBLER_H
#define SCRIBBLER_H

#include <QGraphicsView>
#include <QGraphicsLineItem>
#include <QGraphicsEllipseItem>

class MouseEvent {
public:
    enum {
        Press,
        Move,
        Release
    };

    int action;
    QPointF pos;
    quint64 time;

    MouseEvent(int _action, QPointF _pos, quint64 _time);

    // friend QTextStream &operator<<(QTextStream &out, const MouseEvent &evt);
    friend QDataStream &operator<<(QDataStream &out, const MouseEvent &evt);
    friend QDataStream &operator>>(QDataStream &in, MouseEvent &evt);
};



class Scribbler : public QGraphicsView
{
    QGraphicsScene scene;
    double lineWidth;
    QPointF lastPoint;
    bool isDrawing;
    bool drawingLines;
    QList<MouseEvent> events;
    QList<QGraphicsLineItem> lines;
    QList<QGraphicsEllipseItem> dots;


    Q_OBJECT
public:
    Scribbler();
    void drawLines();
    void drawDots();
    void clear();
    void loadFile();

protected:
    void mouseMoveEvent(QMouseEvent *evt) override;
    void mousePressEvent(QMouseEvent *evt) override;
    void mouseReleaseEvent(QMouseEvent *evt) override;
public slots:
    void sendEventData();
signals:
    void emitEventData(QList<MouseEvent> *emittedEvents);
};

#endif // SCRIBBLER_H
