#include "mainwindow.h"
#include <QApplication>
#include <QTreeView>
#include <QFileSystemModel>
#include <QLineEdit>
#include <QVBoxLayout>
#include <QDir>
#include <QSortFilterProxyModel>
#include <QWidget>

class CustomFileSystemModel : public QFileSystemModel
{
public:
    CustomFileSystemModel(QObject *parent = nullptr)
        : QFileSystemModel(parent)
    {
    }

    bool filterAcceptsRow(int sourceRow, const QModelIndex &sourceParent) const
    {
        QModelIndex index = this->index(sourceRow, 0, sourceParent);
        QString filePath = this->data(index, QFileSystemModel::FilePathRole).toString();
        QString fileName = this->data(index, QFileSystemModel::FileNameRole).toString();

        // Получаем фильтр из QLineEdit
        QString filter = QCoreApplication::instance()->findChild<QLineEdit*>("filterLineEdit")->text();

        // Фильтруем по полному пути
        if (!filter.isEmpty() && !filePath.contains(filter, Qt::CaseInsensitive)) {
            return false;
        }

        return true;
    }
};

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);

    // Создаем главное окно
    QWidget window;
    QVBoxLayout layout(&window);

    // Получаем домашнюю директорию текущего пользователя
    QString homeDir = QDir::homePath();

    // Создаем модель файловой системы
    CustomFileSystemModel model;
    model.setRootPath(homeDir);
    model.setFilter(QDir::Dirs | QDir::Files | QDir::Hidden);

    QModelIndex index = model.index(homeDir);

    // Создаем представление дерева файловой системы
    QTreeView treeView;
    treeView.setModel(&model);
    treeView.expand(index);
    treeView.scrollTo(index); // разворачиваем домашнюю директорию
    treeView.setCurrentIndex(index);
    treeView.resizeColumnToContents(0);
    treeView.resizeColumnToContents(3);
    treeView.setColumnWidth(0, 500);
    treeView.hideColumn(1); // Скрываем колонку с размером файлов
    treeView.hideColumn(2); // Скрываем колонку с типом файлов

    // Создаем QLineEdit виджет для фильтрации
    QLineEdit filterLineEdit;
    filterLineEdit.setPlaceholderText("Filter...");
    QObject::connect(&filterLineEdit, &QLineEdit::textChanged, [&model](const QString &text) {
        model.setNameFilters(QStringList() << "*" + text + "*");
        model.setNameFilterDisables(false);
    });

    // Добавляем виджеты в макет
    layout.addWidget(&filterLineEdit);
    layout.addWidget(&treeView);

    window.showMaximized();

    model.sort(0, Qt::SortOrder::AscendingOrder);

    return a.exec();
}
