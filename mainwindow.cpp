#include "mainwindow.h"
#include "graphwidget.h"

#include <QWidget>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QLineEdit>
#include <QPushButton>
#include <QLabel>
#include <QIcon>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
{
    setWindowTitle("functions");
    resize(900, 650);

    setWindowFlags(Qt::Window);

    setWindowIcon(QIcon(":/icons/app_icon.png"));

    QWidget *central = new QWidget(this);
    setCentralWidget(central);

    QVBoxLayout *mainLayout = new QVBoxLayout(central);
    QHBoxLayout *topLayout = new QHBoxLayout();

    QLabel *label = new QLabel("Function:", this);

    functionInput = new QLineEdit(this);
    functionInput->setPlaceholderText("Examples: y = 2x, y = x^2");
    functionInput->setText("y = x");

    drawButton = new QPushButton("built", this);
    zoomInButton = new QPushButton("+", this);
    zoomOutButton = new QPushButton("-", this);


    graphWidget = new GraphWidget(this);

    topLayout->addWidget(label);
    topLayout->addWidget(functionInput);
    topLayout->addWidget(drawButton);
    topLayout->addWidget(zoomInButton);
    topLayout->addWidget(zoomOutButton);

    mainLayout->addLayout(topLayout);
    mainLayout->addWidget(graphWidget);

    connect(drawButton, &QPushButton::clicked, this, [this]() {
        const QString input = functionInput->text().trimmed();

        graphWidget -> setFunction(input);
    });

    connect(zoomInButton, &QPushButton::clicked, graphWidget, &GraphWidget::zoomIn);
    connect(zoomOutButton, &QPushButton::clicked, graphWidget, &GraphWidget::zoomOut);
}

