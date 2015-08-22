#include <QtCore/QList>
#include <QtCore/QString>
#include <QtCore/QMap>
#include <QtCore/QFile>



QList<int> getList()
{
    return QList<int>();
}

void detach1()
{
    getList().first(); // Warning
}

void detach2()
{
    getList().at(0); // OK
}

void lvalue()
{
    QStringList s;
    s.first(); // OK
}

QStringList test_string() { return {}; }
QStringList& test_string_ref() { static QStringList s; return s; }
QStringList * test_string_ptr() { return {}; }
const QStringList test_const_string() { return {}; }
const QStringList & test_const_string_ref() { static QStringList s; return s; }
const QStringList * test_const_string_ptr() { return {}; }

void qstrings()
{
    QString s;
    s.toLatin1().data(); // OK, list isn't shared
    test_string().first(); // Warning
    test_const_string().first(); // OK
    test_const_string_ref().first(); // OK
    test_const_string_ptr()->first(); // OK

    test_string().first(); // Warning
    test_string_ref().first(); // Warning
    test_string_ptr()->first(); // Warning
}


void maps()
{
    QMap<int, QStringList> map;
    map.value(0).first(); // OK, value() returns const T
    map[0].removeAll("asd"); // OK
    map.values().first(); // OK, QMap::values() isn't shared
}

void more()
{
    QFile::encodeName("foo").data();
}

void foo(QStringList *list)
{
    auto it = list->begin();
}
