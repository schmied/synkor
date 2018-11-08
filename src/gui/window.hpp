#ifndef WINDOW_HPP
#define WINDOW_HPP

#include <QMainWindow>

#include <QFileSystemModel>
#include <QTreeView>

namespace Ui {
class window;
}

class window : public QMainWindow
{
	Q_OBJECT

public:
	explicit window(QWidget *parent = nullptr);
	~window();

private slots:
	void on_tree_view_src_clicked(const QModelIndex &index);

	void on_list_view_src_doubleClicked(const QModelIndex &index);

	void on_tree_view_src_doubleClicked(const QModelIndex &index);

private:
	Ui::window *ui;
	QFileSystemModel model_src_tree;
	QFileSystemModel model_src_list;
	QFileSystemModel model_dst;

};

#endif // WINDOW_HPP
