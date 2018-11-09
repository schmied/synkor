#ifndef WINDOW_HPP
#define WINDOW_HPP

#include <QFileSystemModel>
#include <QMainWindow>

namespace Ui {
class window;
}

class window : public QMainWindow {
	Q_OBJECT

private:
	Ui::window *ui_;
	QFileSystemModel model_src_list_;

private slots:
	void on_tree_view_src_clicked(const QModelIndex &index);
	void on_list_view_src_doubleClicked(const QModelIndex &index);
	void on_tree_view_src_doubleClicked(const QModelIndex &index);

protected:

public:
	explicit window(QWidget *parent = nullptr);
	~window();

};

#endif
