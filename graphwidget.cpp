#include "graphwidget.h"

#include <QFontMetrics>
#include <QMouseEvent>
#include <QPainter>
#include <QPen>
#include <QtGlobal>
#include <cmath>

GraphWidget::GraphWidget(QWidget *parent)
    : QWidget(parent),
    scale(40.0),
    offsetX(0.0),
    offsetY(0.0),
    dragging(false),
    functionExpression("x")
{
    setMinimumSize(700, 500);
    setAutoFillBackground(true);
    updateHolePoints();
}

bool GraphWidget::setFunction(const QString &expression)
{
    QString normalized = normalizeExpression(expression);

    if (!isFunctionValid(normalized)) {
        return false;
    }

    functionExpression = normalized;
    updateHolePoints();
    update();
    return true;
}

void GraphWidget::zoomIn()
{
    scale += 10.0;

    if (scale > 200.0) {
        scale = 200.0;
    }

    updateHolePoints();
    update();
}

void GraphWidget::zoomOut()
{
    scale -= 10.0;

    if (scale < 10.0) {
        scale = 10.0;
    }

    updateHolePoints();
    update();
}

void GraphWidget::paintEvent(QPaintEvent *event)
{
    Q_UNUSED(event);

    QPainter painter(this);
    painter.fillRect(rect(), Qt::white);
    painter.setRenderHint(QPainter::Antialiasing, true);

    drawGrid(painter);
    drawAxes(painter);
    drawFunction(painter);
    drawHolePoints(painter);
}

void GraphWidget::mousePressEvent(QMouseEvent *event)
{
    if (event->button() == Qt::LeftButton) {
        dragging = true;
        lastMousePos = event->pos();
    }
}

void GraphWidget::mouseMoveEvent(QMouseEvent *event)
{
    if (dragging) {
        QPoint delta = event->pos() - lastMousePos;
        offsetX += delta.x();
        offsetY += delta.y();
        lastMousePos = event->pos();
        updateHolePoints();
        update();
    }
}

void GraphWidget::mouseReleaseEvent(QMouseEvent *event)
{
    if (event->button() == Qt::LeftButton) {
        dragging = false;
    }
}

void GraphWidget::drawGrid(QPainter &painter)
{
    const int w = width();
    const int h = height();
    const int step = static_cast<int>(scale);

    const int centerX = static_cast<int>(w / 2.0 + offsetX);
    const int centerY = static_cast<int>(h / 2.0 + offsetY);

    painter.setPen(QPen(QColor(220, 220, 220), 1));

    int startX = centerX % step;
    if (startX < 0) {
        startX += step;
    }

    for (int x = startX; x < w; x += step) {
        painter.drawLine(x, 0, x, h);
    }

    int startY = centerY % step;
    if (startY < 0) {
        startY += step;
    }

    for (int y = startY; y < h; y += step) {
        painter.drawLine(0, y, w, y);
    }
}

void GraphWidget::drawAxes(QPainter &painter)
{
    const int w = width();
    const int h = height();
    const int tickSize = 5;

    const int centerX = static_cast<int>(w / 2.0 + offsetX);
    const int centerY = static_cast<int>(h / 2.0 + offsetY);

    painter.setPen(QPen(Qt::black, 2));
    painter.drawLine(0, centerY, w, centerY);
    painter.drawLine(centerX, 0, centerX, h);

    painter.drawText(w - 20, centerY - 8, "X");
    painter.drawText(centerX + 8, 20, "Y");

    QFontMetrics fm = painter.fontMetrics();

    int maxRight = static_cast<int>(std::ceil((w - centerX) / scale));
    int maxLeft = static_cast<int>(std::ceil(centerX / scale));
    int maxUp = static_cast<int>(std::ceil(centerY / scale));
    int maxDown = static_cast<int>(std::ceil((h - centerY) / scale));

    for (int i = -maxLeft; i <= maxRight; ++i) {
        int x = static_cast<int>(centerX + i * scale);
        painter.drawLine(x, centerY - tickSize, x, centerY + tickSize);

        if (i != 0) {
            QString text = QString::number(i);
            int textWidth = fm.horizontalAdvance(text);
            painter.drawText(x - textWidth / 2, centerY + 20, text);
        }
    }

    for (int i = -maxDown; i <= maxUp; ++i) {
        int y = static_cast<int>(centerY - i * scale);
        painter.drawLine(centerX - tickSize, y, centerX + tickSize, y);

        if (i != 0) {
            QString text = QString::number(i);
            painter.drawText(centerX + 10, y + fm.height() / 4, text);
        }
    }
}

void GraphWidget::drawFunction(QPainter &painter)
{
    const int w = width();
    const int h = height();

    const double centerX = w / 2.0 + offsetX;
    const double centerY = h / 2.0 + offsetY;

    painter.setPen(QPen(Qt::blue, 2));

    bool hasPrevPoint = false;
    QPoint prevPoint;

    for (int px = 0; px < w; ++px) {
        double x = (px - centerX) / scale;

        if (containsHoleNear(x)) {
            hasPrevPoint = false;
            continue;
        }

        double y = 0.0;

        if (!evaluateExpression(functionExpression, x, y) || !std::isfinite(y)) {
            hasPrevPoint = false;
            continue;
        }

        int py = static_cast<int>(std::round(centerY - y * scale));
        QPoint currentPoint(px, py);

        if (py < -100000 || py > 100000) {
            hasPrevPoint = false;
            continue;
        }

        if (hasPrevPoint) {
            if (std::abs(currentPoint.y() - prevPoint.y()) < h * 2) {
                painter.drawLine(prevPoint, currentPoint);
            }
        }

        prevPoint = currentPoint;
        hasPrevPoint = true;
    }
}

void GraphWidget::drawHolePoints(QPainter &painter)
{
    const double centerX = width() / 2.0 + offsetX;
    const double centerY = height() / 2.0 + offsetY;

    painter.setPen(QPen(Qt::red, 2));
    painter.setBrush(Qt::white);

    const int radius = 5;

    for (const HolePoint &hole : holePoints) {
        int px = static_cast<int>(std::round(centerX + hole.x * scale));
        int py = static_cast<int>(std::round(centerY - hole.y * scale));

        if (px < -20 || px > width() + 20 || py < -20 || py > height() + 20) {
            continue;
        }

        painter.drawEllipse(QPoint(px, py), radius, radius);
    }
}

QString GraphWidget::normalizeExpression(const QString &expression) const
{
    QString text = expression;
    text.remove(' ');
    text = text.toLower();

    if (text.startsWith("y=")) {
        text = text.mid(2);
    }

    QString result;

    for (int i = 0; i < text.length(); ++i) {
        QChar current = text[i];

        if (!result.isEmpty()) {
            QChar previous = result.back();

            if (isImplicitMultiplicationNeeded(previous, current)) {
                result += '*';
            }
        }

        result += current;
    }

    return result;
}

bool GraphWidget::isFunctionValid(const QString &expression) const
{
    if (expression.isEmpty()) {
        return false;
    }

    double result = 0.0;

    if (evaluateExpression(expression, 1.0, result)) {
        return true;
    }

    if (evaluateExpression(expression, 2.0, result)) {
        return true;
    }

    if (evaluateExpression(expression, -1.5, result)) {
        return true;
    }

    return false;
}

bool GraphWidget::evaluateExpression(const QString &expression, double xValue, double &result) const
{
    int pos = 0;

    if (!parseExpression(expression, pos, xValue, result)) {
        return false;
    }

    return pos == expression.length();
}

bool GraphWidget::parseExpression(const QString &text, int &pos, double xValue, double &result) const
{
    if (!parseTerm(text, pos, xValue, result)) {
        return false;
    }

    while (pos < text.length()) {
        QChar op = text[pos];

        if (op != '+' && op != '-') {
            break;
        }

        ++pos;

        double right = 0.0;
        if (!parseTerm(text, pos, xValue, right)) {
            return false;
        }

        if (op == '+') {
            result += right;
        } else {
            result -= right;
        }
    }

    return true;
}

bool GraphWidget::parseTerm(const QString &text, int &pos, double xValue, double &result) const
{
    if (!parsePower(text, pos, xValue, result)) {
        return false;
    }

    while (pos < text.length()) {
        QChar op = text[pos];

        if (op != '*' && op != '/') {
            break;
        }

        ++pos;

        double right = 0.0;
        if (!parsePower(text, pos, xValue, right)) {
            return false;
        }

        if (op == '*') {
            result *= right;
        } else {
            if (std::abs(right) < 1e-12) {
                return false;
            }
            result /= right;
        }
    }

    return true;
}

bool GraphWidget::parsePower(const QString &text, int &pos, double xValue, double &result) const
{
    if (!parseUnary(text, pos, xValue, result)) {
        return false;
    }

    while (pos < text.length() && text[pos] == '^') {
        ++pos;

        double exponent = 0.0;
        if (!parseUnary(text, pos, xValue, exponent)) {
            return false;
        }

        result = std::pow(result, exponent);

        if (!std::isfinite(result)) {
            return false;
        }
    }

    return true;
}

bool GraphWidget::parseUnary(const QString &text, int &pos, double xValue, double &result) const
{
    if (pos < text.length() && text[pos] == '+') {
        ++pos;
        return parseUnary(text, pos, xValue, result);
    }

    if (pos < text.length() && text[pos] == '-') {
        ++pos;

        if (!parseUnary(text, pos, xValue, result)) {
            return false;
        }

        result = -result;
        return true;
    }

    return parsePrimary(text, pos, xValue, result);
}

bool GraphWidget::parsePrimary(const QString &text, int &pos, double xValue, double &result) const
{
    if (pos >= text.length()) {
        return false;
    }

    if (text[pos] == 'x') {
        result = xValue;
        ++pos;
        return true;
    }

    if (text[pos] == '(') {
        ++pos;

        if (!parseExpression(text, pos, xValue, result)) {
            return false;
        }

        if (pos >= text.length() || text[pos] != ')') {
            return false;
        }

        ++pos;
        return true;
    }

    if (text[pos] == '|') {
        ++pos;

        if (!parseExpression(text, pos, xValue, result)) {
            return false;
        }

        if (pos >= text.length() || text[pos] != '|') {
            return false;
        }

        ++pos;
        result = std::abs(result);
        return true;
    }

    return parseNumber(text, pos, result);
}

bool GraphWidget::parseNumber(const QString &text, int &pos, double &result) const
{
    int start = pos;
    bool hasDigit = false;
    bool hasDot = false;

    while (pos < text.length()) {
        QChar ch = text[pos];

        if (ch.isDigit()) {
            hasDigit = true;
            ++pos;
        } else if (ch == '.' && !hasDot) {
            hasDot = true;
            ++pos;
        } else {
            break;
        }
    }

    if (!hasDigit) {
        return false;
    }

    bool ok = false;
    result = text.mid(start, pos - start).toDouble(&ok);
    return ok;
}

bool GraphWidget::isImplicitMultiplicationNeeded(QChar left, QChar right) const
{
    return isValueEnding(left) && isValueStarting(right);
}

bool GraphWidget::isValueEnding(QChar ch) const
{
    return ch.isDigit() || ch == 'x' || ch == ')';
}

bool GraphWidget::isValueStarting(QChar ch) const
{
    return ch.isDigit() || ch == 'x' || ch == '(' || ch == '|';
}

void GraphWidget::updateHolePoints()
{
    holePoints.clear();

    const double centerX = width() / 2.0 + offsetX;
    const int w = width();

    for (int px = 1; px < w - 1; ++px) {
        double x = (px - centerX) / scale;

        if (containsHoleNear(x)) {
            continue;
        }

        double holeY = 0.0;
        if (findHoleAt(x, holeY)) {
            holePoints.append({x, holeY});
        }
    }
}

bool GraphWidget::findHoleAt(double x, double &y) const
{
    double exact = 0.0;
    if (evaluateExpression(functionExpression, x, exact) && std::isfinite(exact)) {
        return false;
    }

    const double delta = 1e-4;
    double left = 0.0;
    double right = 0.0;

    bool leftOk = evaluateExpression(functionExpression, x - delta, left) && std::isfinite(left);
    bool rightOk = evaluateExpression(functionExpression, x + delta, right) && std::isfinite(right);

    if (!leftOk || !rightOk) {
        return false;
    }

    if (std::abs(left - right) > 0.05) {
        return false;
    }

    if (std::abs(left) > 1e5 || std::abs(right) > 1e5) {
        return false;
    }

    y = (left + right) / 2.0;
    return true;
}

bool GraphWidget::containsHoleNear(double x) const
{
    const double eps = 0.5 / scale;

    for (const HolePoint &hole : holePoints) {
        if (std::abs(hole.x - x) < eps) {
            return true;
        }
    }

    return false;
}