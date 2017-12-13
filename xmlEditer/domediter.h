#ifndef DOMEDITER_H
#define DOMEDITER_H

#include <QString>
#include <QWidget>
#include <QTreeWidget>

class QTreeWidgetItem;

class QDomDocument;

class QDomNode;

class ItemFlags;

class DomEditer : public QWidget
{
    Q_OBJECT

public:
    DomEditer(QWidget *parent);
    DomEditer(QWidget *parent, const QString path);
    ~DomEditer();

    bool readfile(const QString &fileName);

private slots:
    void addElement();
    void save();
    void deletEle();
    void replaceText();
    void deletAttribute();

private:
    void listfile();
    void maketree(QDomNode parent, QTreeWidgetItem *qgparent, QModelIndex index);
    void deletChildElement(QDomNode parent);

    QTreeWidget *TreeView;
    QDomDocument *Doc;
    QMap<QModelIndex, QDomNode> *Map;
    QList<QDomNode> *Node;
    QList<QModelIndex> *Index;
    QString *Path;
};

#endif // DOMEDITER_H
