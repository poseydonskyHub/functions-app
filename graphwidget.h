#ifndef GRAPHWIDGET_H
#define GRAPHWIDGET_H

#include <QPoint>
#include <QString>
#include <QWidget>
#include <QVector>

class QPainter;
class QMouseEvent;

class GraphWidget : public QWidget
{
    Q_OBJECT

public:
    explicit GraphWidget(QWidget *parent = nullptr);

    bool setFunction(const QString &expression);
    void zoomIn();
    void zoomOut();

protected:
    void paintEvent(QPaintEvent *event) override;
    void mousePressEvent(QMouseEvent *event) override;
    void mouseMoveEvent(QMouseEvent *event) override;
    void mouseReleaseEvent(QMouseEvent *event) override;

private:
    struct HolePoint
    {
        double x;
        double y;
    };

    void drawGrid(QPainter &painter);
    void drawAxes(QPainter &painter);
    void drawFunction(QPainter &painter);
    void drawHolePoints(QPainter &painter);

    QString normalizeExpression(const QString &expression) const;
    bool isFunctionValid(const QString &expression) const;

    bool evaluateExpression(const QString &expression, double xValue, double &result) const;
    bool parseExpression(const QString &text, int &pos, double xValue, double &result) const;
    bool parseTerm(const QString &text, int &pos, double xValue, double &result) const;
    bool parsePower(const QString &text, int &pos, double xValue, double &result) const;
    bool parseUnary(const QString &text, int &pos, double xValue, double &result) const;
    bool parsePrimary(const QString &text, int &pos, double xValue, double &result) const;
    bool parseNumber(const QString &text, int &pos, double &result) const;

    bool isImplicitMultiplicationNeeded(QChar left, QChar right) const;
    bool isValueEnding(QChar ch) const;
    bool isValueStarting(QChar ch) const;

    void updateHolePoints();
    bool findHoleAt(double x, double &y) const;
    bool containsHoleNear(double x) const;

    double scale;
    double offsetX;
    double offsetY;

    bool dragging;
    QPoint lastMousePos;

    QString functionExpression;
    QVector<HolePoint> holePoints;
};

#endif // GRAPHWIDGET_H