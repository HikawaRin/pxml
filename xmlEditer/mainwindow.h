#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>

class DomEditer;

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = 0);
    ~MainWindow();

private slots:
    void open();
    void save();

private:
    QAction *OpenAction;
    QAction *SaveAction;
    DomEditer *domediter;
};

#endif // MAINWINDOW_H
