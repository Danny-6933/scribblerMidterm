#include "scribbler.h"
#include <QtWidgets>

MouseEvent::MouseEvent(int _action, QPointF _pos, quint64 _time)
    :action(_action), pos(_pos), time(_time) { }

QDataStream &operator<<(QDataStream &out, const MouseEvent &evt) {
    return out << evt.action << evt.pos << evt.time;
}

QDataStream &operator>>(QDataStream &in, MouseEvent &evt) {
    return in << evt.action << evt.pos << evt.time;
}


Scribbler::Scribbler()
    :lineWidth(4.0), drawingLines(true), lines(), dots() {

    setScene(&scene);
    setSceneRect(QRectF(0.0, 0.0, 800.0, 600.0));
    setMinimumSize(QSize(400, 400));
    setRenderHint(QPainter::Antialiasing, true);
    setBackgroundBrush(Qt::white);

    scene.addRect(sceneRect());

}

void Scribbler::mousePressEvent(QMouseEvent *evt) {
    QGraphicsView::mousePressEvent(evt);

    isDrawing = true;
    QPointF p = mapToScene(evt->pos());
    lastPoint = p;

    QGraphicsEllipseItem *curDot = new QGraphicsEllipseItem(QRectF(p - QPointF(0.4*lineWidth, 0.4*lineWidth), QSizeF(lineWidth, lineWidth)));
    curDot->setBrush(Qt::black);
    scene.addItem(curDot);

    dots.append(curDot);

    events << MouseEvent(MouseEvent::Press, p, evt->timestamp());
}

void Scribbler::mouseMoveEvent(QMouseEvent *evt) {
    QGraphicsView::mouseMoveEvent(evt);

    QPointF p = mapToScene(evt->pos());

    QGraphicsEllipseItem *curDot = new QGraphicsEllipseItem(QRectF(p - QPointF(0.4*lineWidth, 0.4*lineWidth), QSizeF(lineWidth, lineWidth)));
    curDot->setBrush(Qt::black);
    scene.addItem(curDot);
    dots.append(curDot);

    QGraphicsLineItem *curLine = new QGraphicsLineItem(QLineF(lastPoint, p));
    curLine->setPen(QPen(Qt::black, lineWidth, Qt::SolidLine, Qt::FlatCap));
    if (!drawingLines) {
        curLine->setVisible(false);
    }
    scene.addItem(curLine);
    lines.append(curLine);

    lastPoint = p;
    events << MouseEvent(MouseEvent::Move, p, evt->timestamp());
}

void Scribbler::mouseReleaseEvent(QMouseEvent *evt) {
    QGraphicsView::mouseReleaseEvent(evt);

    QPointF p = mapToScene(evt->pos());
    isDrawing = false;

    events << MouseEvent(MouseEvent::Release, p, evt->timestamp());
}

void Scribbler::drawLines() {
    drawingLines = true;
    updateView();
}

void Scribbler::drawDots() {
    drawingLines = false;
    updateView();
}

void Scribbler::updateView() {
    for (auto &line : lines) {
        line->setVisible(drawingLines);
    }
    for (auto &dot : dots) {
        dot->setVisible(true);
    }
}

void Scribbler::clear() {
    events.clear();
    scene.clear();
    lines.clear();
    dots.clear();
    drawingLines = true;
}


void Scribbler::loadFile() {
    qDebug() << "loading File";
}

void Scribbler::sendEventData() {
    emit emitEventData(events);
}
