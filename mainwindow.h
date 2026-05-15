#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>

class QLineEdit;
class QPushButton;
class QLabel;
class GraphWidget;


class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget * parent = nullptr);

private:
    QLineEdit *functionInput;
    QPushButton *drawButton;
    QPushButton *zoomInButton;
    QPushButton *zoomOutButton;
    QLabel *statusLabel;
    GraphWidget *graphWidget;
};

#endif // MAINWINDOW_H