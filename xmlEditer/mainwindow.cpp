#include <QAction>
#include <QMenuBar>
#include <QMessageBox>
#include <QStatusBar>
#include <QToolBar>
#include <QDir>
#include <QInputDialog>
#include <QFileDialog>

#include "mainwindow.h"
#include "domediter.h"

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
{
    setWindowTitle(tr("XML DOM Reader"));

    OpenAction = new QAction(tr("&Open..."), this);
    OpenAction->setShortcut(QKeySequence::Open);
    OpenAction->setStatusTip(tr("Open an existing file"));
    connect(OpenAction, &QAction::triggered
            , this, &MainWindow::open);

    SaveAction = new QAction(tr("&Save..."), this);
    SaveAction->setShortcut(QKeySequence::SaveAs);
    SaveAction->setStatusTip(tr("Save file"));
    connect(SaveAction, &QAction::triggered
            , this, &MainWindow::saveAs);

    QMenu *file = menuBar()->addMenu(tr("&File"));
    file->addAction(OpenAction);
    file->addAction(SaveAction);

    resize(600, 600);

    statusBar();
}

MainWindow::~MainWindow()
{

}

void MainWindow::open()
{
    QString path = QFileDialog::getOpenFileName(this
                                                , tr("Open XML file")
                                                , "D:/"
                                                , tr("XML File (*.xml)")
                                                );
    if(path.isEmpty())
    {
        QMessageBox::information(this,
                                tr("Wrong"),
                                tr("Please input right path"));
        return;
    }else
    {
        domediter = new DomEditer(this, path);
        setCentralWidget(domediter);
    }
}

void MainWindow::saveAs()
{

}
