#ifndef TREE_VIEW_SRC_HPP
#define TREE_VIEW_SRC_HPP

#include <QFileSystemModel>
#include <QTreeView>

namespace Ui {
class window;
}

class tree_view : public QTreeView {
	Q_OBJECT

private:
	Ui::window *ui_;
	QFileSystemModel model_;

protected:
	void dragEnterEvent(QDragEnterEvent*) override;
	void dragLeaveEvent(QDragLeaveEvent*) override;
	void dragMoveEvent(QDragMoveEvent*) override;

public:
	explicit tree_view(QWidget *parent = nullptr);

	void set_ui(Ui::window*);
	const QFileSystemModel* model() const;

};

#endif
