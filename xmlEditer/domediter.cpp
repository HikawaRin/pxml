#include "domediter.h"

#include <QAbstractItemView>
#include <QAction>
#include <QInputDialog>
#include <QPushButton>
#include <QTreeWidget>
#include <QStringList>
#include <QFile>
#include <QtXml>
#include <QMap>
#include <QMessageBox>
#include <QList>
#include <QBoxLayout>
#include <QStandardItemModel>

DomEditer::DomEditer(QWidget *parent)
    :QWidget(parent)
{

}

DomEditer::DomEditer(QWidget *parent, const QString path)
    :QWidget(parent)
{
    TreeView = new QTreeWidget(this);
    Map = new QMap<QModelIndex, QDomNode>();
    Node = new QList<QDomNode>();
    Index = new QList<QModelIndex>();
    Path = new QString(path);
    Doc = new QDomDocument();

    QStringList headers;
    headers << "Elementes";
    TreeView->setHeaderLabels(headers);

    if(!DomEditer::readfile(path))
    {
        return;
    }

    QPushButton *SavechangeButton = new QPushButton(tr("Save change"), this);
    QPushButton *addElementButton = new QPushButton(tr("add Element"), this);
    QPushButton *deletElementButton = new QPushButton(tr("delete Element"), this);
    QPushButton *replaceTextButton = new QPushButton(tr("replace Text"), this);
    QHBoxLayout *Buttonlayout = new QHBoxLayout;
    Buttonlayout->addWidget(SavechangeButton);
    Buttonlayout->addWidget(addElementButton);
    Buttonlayout->addWidget(deletElementButton);
    Buttonlayout->addWidget(replaceTextButton);

    QVBoxLayout *layout = new QVBoxLayout;
    layout->addWidget(TreeView);
    layout->addLayout(Buttonlayout);
    setLayout(layout);

    connect(SavechangeButton, &QPushButton::clicked
            , this, &DomEditer::save);
    connect(addElementButton, &QPushButton::clicked
            , this, &DomEditer::addElement);
    connect(deletElementButton, &QPushButton::clicked
            , this, &DomEditer::deletEle);
    connect(replaceTextButton, &QPushButton::clicked
            , this, &DomEditer::replaceText);
}

DomEditer::~DomEditer()
{

}

bool DomEditer::readfile(const QString &fileName)
{
    QFile file(fileName);
    if (!file.open(QFile::ReadOnly | QFile::Text))
    {
        QMessageBox::critical(this,
                              tr("Error"),
                              tr("Cannot read file %1").arg(fileName));
        return false;
    }

    QString errorStr;
    int errorLine;
    int errorColumn;

    if (!Doc->setContent(file.readAll(), false, &errorStr, &errorLine,&errorColumn))
    {
        QMessageBox::critical(this,
                              tr("Error"),
                              tr("Parse error at line %1, column %2: %3").arg(errorLine).arg(errorColumn).arg(errorStr));
        return false;
    }
    file.close();
    Doc->normalize();

    DomEditer::listfile();

    return true;
}

void DomEditer::maketree(QDomNode parent, QTreeWidgetItem *qgparent, QModelIndex pindex)
{
    int row = 0;
    QDomNode sib = parent;
    while (!sib.isNull())
    {
        QTreeWidgetItem *qparent = new QTreeWidgetItem(qgparent, QStringList(sib.toElement().tagName()));
        QModelIndex index = TreeView->model()->index(row, 0, pindex);

        Node->push_back(sib);
        Index->push_back(index);
        Map->insert(Index->last(), Node->last());

        QDomNode child = sib.firstChild();
        if(child.isElement())
        {
            DomEditer::maketree(child, qparent, index);
        }else if(child.isText())
        {
            new QTreeWidgetItem(qparent, QStringList(child.toText().data()));
            QModelIndex cindex = TreeView->model()->index(row, 0, index);

            Node->push_back(child);
            Index->push_back(cindex);
            Map->insert(Index->last(), Node->last());
        }

        sib = sib.nextSibling();
        if(!sib.isNull())
        {
            row += 1;
        }
    }
}

void DomEditer::listfile()
{
    QDomElement root = Doc->documentElement();
    QStringList sroot;
    sroot << root.tagName();
    QTreeWidgetItem *troot = new QTreeWidgetItem(TreeView, sroot);

    QModelIndex index = TreeView->model()->index(0, 0);

    Node->push_back(root);
    Index->push_back(index);
    Map->insert(Index->last(), Node->last());

    QDomNode child = root.firstChild();
    if (!child.isNull())
    {
        DomEditer::maketree(child, troot, index);
    }
}

void DomEditer::addElement()
{
    QItemSelectionModel *selectionmodel = TreeView->selectionModel();
    QModelIndex index = selectionmodel->currentIndex();
    if(!index.isValid())
    {
        return;
    }else
    {
        QDomNode node = Map->value(index);
        if(!node.isElement())
        {
            QMessageBox::critical(this,
                                  tr("Error"),
                                  tr("Can't add here"));
            return;
        }else
        {
            QString elementName = QInputDialog::getText(this,
                                                        tr("Add element"),
                                                        tr("Directory name"));
            if(!elementName.isEmpty())
            {
                QDomElement ele = Doc->createElement(elementName);
                QDomText nullText = Doc->createTextNode(" ");
                ele.appendChild(nullText);
                node.appendChild(ele);

                QTreeWidgetItem *troot = TreeView->currentItem();
                QStringList s, n;
                s << elementName;
                n << QString(" ");
                QTreeWidgetItem *newroot = new QTreeWidgetItem(troot, s);
                new QTreeWidgetItem(newroot, n);

                int row = 0;
                while(TreeView->model()->hasIndex(row, 0, index))
                {
                    row += 1;
                }
                QModelIndex eindex = TreeView->model()->index((row - 1), 0, index);

                Node->push_back(QDomNode(ele));
                Index->push_back(eindex);
                Map->insert(Index->last(), Node->last());

                QModelIndex tindex = TreeView->model()->index(0, 0, eindex);
                Node->push_back(QDomNode(nullText));
                Index->push_back(tindex);
                Map->insert(Index->last(), Node->last());
            }
        }
    }
}

void DomEditer::save()
{
    QFile file(*Path);
    if( !file.open( QIODevice::WriteOnly | QIODevice::Text ) )
    {
        QMessageBox::critical(this,
                              tr("Error"),
                              tr("Can't Save"));
        return;
    }
    QTextStream out(&file);
    Doc->normalize();
    Doc->save(out, 4);
    file.close();
    QMessageBox::information(this,
                            tr("Save"),
                            tr("Save successed"));
}

void DomEditer::deletEle()
{
    QItemSelectionModel *selectionmodel = TreeView->selectionModel();
    QModelIndex index = selectionmodel->currentIndex();
    QTreeWidgetItem *item = TreeView->currentItem();

    if(!index.isValid())
    {
        return;
    }else
    {
        QDomNode dnode = Map->value(index);
        if(!dnode.isElement())
        {
            return;
        }else
        {
            QDomNode child = dnode.firstChild();
            if(!child.isNull())
            {
                DomEditer::deletChildElement(child);
            }

            QDomNode parent = dnode.parentNode();
            parent.removeChild(dnode);
            if(parent.firstChild().isNull())
            {
                QDomText t = Doc->createTextNode(" ");
                parent.appendChild(t);
            }

            delete item;
        }
    }
}

void DomEditer::deletChildElement(QDomNode parent)
{
    QDomNode sib = parent;

    while(!sib.isNull())
    {
        QDomNode child = sib.firstChild();
        if(!child.isNull())
        {
            DomEditer::deletChildElement(child);
        }

        QModelIndex sindex = Map->key(sib);
        Map->remove(sindex);
        Index->removeOne(sindex);

        sib = sib.nextSibling();
    }
}

void DomEditer::replaceText()
{
    QItemSelectionModel *selectionmodel = TreeView->selectionModel();
    QModelIndex index = selectionmodel->currentIndex();
    QTreeWidgetItem *item = TreeView->currentItem();

    if(!index.isValid())
    {
        return;
    }else
    {
        QString stext = QInputDialog::getText(this,
                                                    tr("Replace text"),
                                                    tr("text"));
        QDomNode node = Map->value(index);
        if(node.toText().isNull())
        {
            QMessageBox::information(this,
                                    tr("Write error"),
                                    tr("This isn't a text node"));
        }else
        {
            QDomText ntext = Doc->createTextNode(stext);
            QDomNode parent = node.parentNode();
            parent.replaceChild(ntext, node);

            Map->remove(index);
            Index->removeOne(index);
            Index->push_back(index);
            Node->push_back(ntext);
            Map->insert(Index->last(), Node->last());

            QTreeWidgetItem *iparent = item->parent();
            QStringList s;
            s << stext;
            new QTreeWidgetItem(iparent, s);
            delete item;
        }
    }
}
