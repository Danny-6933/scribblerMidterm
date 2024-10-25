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

    MouseEvent() : action(Press), time(0) {};
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
    bool isCapturing;
    quint64 initTime;
    QList<MouseEvent> events;
    QList<QGraphicsLineItem*> lines;
    QList<QGraphicsEllipseItem*> dots;
    QList<QList<MouseEvent>> eventsList;
    int tabCount;
    QList<QGraphicsItemGroup*> lineGroups;
    QList<QGraphicsItemGroup*> dotGroups;


    void addDot(const MouseEvent &e);
    void addLine(const MouseEvent &e);

    Q_OBJECT
public:
    Scribbler();
    void drawLines();
    void drawDots();
    void updateView();
    void clear();
    void loadFile(QList<QList<MouseEvent>> evl);

protected:
    void mouseMoveEvent(QMouseEvent *evt) override;
    void mousePressEvent(QMouseEvent *evt) override;
    void mouseReleaseEvent(QMouseEvent *evt) override;
public slots:
    void sendEventData();
    void startCapture();
    void sendAllData();
    void updateTabOpacity(int t);
signals:
    void emitEventData(QList<MouseEvent>& emittedEvents);
    void emitAllData(QList<QList<MouseEvent>> evs);
};

#endif // SCRIBBLER_H
