#include "scribbler.h"
#include <QtWidgets>

MouseEvent::MouseEvent(int _action, QPointF _pos, quint64 _time)
    :action(_action), pos(_pos), time(_time) { }

QDataStream &operator<<(QDataStream &out, const MouseEvent &evt) {
    return out << evt.action << evt.pos << evt.time;
}

QDataStream &operator>>(QDataStream &in, MouseEvent &evt) {
    return in >> evt.action >> evt.pos >> evt.time;
}


Scribbler::Scribbler()
    :lineWidth(4.0), drawingLines(true), isCapturing(false), lines(), dots() {

    setScene(&scene);
    setSceneRect(QRectF(0.0, 0.0, 800.0, 600.0));
    setMinimumSize(QSize(400, 400));
    setRenderHint(QPainter::Antialiasing, true);
    setBackgroundBrush(Qt::white);

    scene.addRect(sceneRect());

    initTime = 2 << 30;
    qDebug() << initTime;
    tabCount = 1;

}

void Scribbler::mousePressEvent(QMouseEvent *evt) {
    QGraphicsView::mousePressEvent(evt);

    QPointF p = mapToScene(evt->pos());

    if (initTime > evt->timestamp()) {
        initTime = evt->timestamp();
    }

    MouseEvent curEvent = MouseEvent(MouseEvent::Press, p, evt->timestamp() - initTime);
    events << curEvent;
    addDot(curEvent);
    lastPoint = curEvent.pos;

    // qDebug() << evt->timestamp();

}

void Scribbler::mouseMoveEvent(QMouseEvent *evt) {
    QGraphicsView::mouseMoveEvent(evt);

    QPointF p = mapToScene(evt->pos());

    MouseEvent curEvent = MouseEvent(MouseEvent::Move, p, evt->timestamp() - initTime);

    addDot(curEvent);

    addLine(curEvent);

    lastPoint = p;
    events << curEvent;
}

void Scribbler::mouseReleaseEvent(QMouseEvent *evt) {
    QGraphicsView::mouseReleaseEvent(evt);

    QPointF p = mapToScene(evt->pos());
    isDrawing = false;

    events << MouseEvent(MouseEvent::Release, p, evt->timestamp() - initTime);
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
    eventsList.clear();
    scene.clear();
    lines.clear();
    dots.clear();
    dotGroups.clear();
    lineGroups.clear();
    initTime = 2 << 30;
    drawingLines = true;
}

void Scribbler::loadFile(QList<QList<MouseEvent>> evl) {
    clear();

    tabCount = 0;
    for (const auto &eventList : evl) {
        tabCount++;

        dotGroups.append(scene.createItemGroup({}));
        lineGroups.append(scene.createItemGroup({}));

        for (const auto &e : eventList) {
            addDot(e);
            if (e.action == MouseEvent::Move) {
                addLine(e);
            }
            lastPoint = e.pos;
        }
    }
}
void Scribbler::sendEventData() {
    if (isCapturing) {
        emit emitEventData(events);
        eventsList.append(events);
        // qDebug() << eventsList.size();
        events.clear();
        ++tabCount;
        isCapturing = false;
    } else {
        QMessageBox::information(this, "Error", QString("No capture started, cannot end anything!"));
    }

}

void Scribbler::startCapture() {
    isCapturing = true;
    // eventsList.append(events);
    events.clear();
}

void Scribbler::sendAllData() {
    emit emitAllData(eventsList);
}

void Scribbler::addDot(const MouseEvent &e) {
    isDrawing = true;
    QGraphicsEllipseItem *dot = new QGraphicsEllipseItem(QRectF(e.pos - QPointF(0.5 * lineWidth, 0.5 * lineWidth), QSizeF(lineWidth, lineWidth)));
    dot->setBrush(Qt::black);
    scene.addItem(dot);

    if (dotGroups.size() < tabCount) {
        dotGroups.append(scene.createItemGroup({}));
    }
    dotGroups[tabCount - 1]->addToGroup(dot);

    dots.append(dot);
}

void Scribbler::addLine(const MouseEvent &e) {
    QGraphicsLineItem *line = new QGraphicsLineItem(QLineF(lastPoint, e.pos));
    line->setPen(QPen(Qt::black, lineWidth, Qt::SolidLine, Qt::FlatCap));
    scene.addItem(line);

    if (lineGroups.size() < tabCount) {
        lineGroups.append(scene.createItemGroup({}));
    }
    lineGroups[tabCount - 1]->addToGroup(line);
    // qDebug() << "t:" << tabCount;

    lines.append(line);
    if (!drawingLines) {
        line->setVisible(false);
    }
    lastPoint = e.pos;
}

void Scribbler::updateTabOpacity(int t) {
    for (int i = 0; i < dotGroups.size(); ++i) {
        if (i == t) {
            dotGroups[i]->setOpacity(1.0);
            lineGroups[i]->setOpacity(1.0);
        } else {
            dotGroups[i]->setOpacity(0.1);
            lineGroups[i]->setOpacity(0.3);
        }
    }
}

