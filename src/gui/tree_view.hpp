#ifndef TREE_VIEW_HPP
#define TREE_VIEW_HPP

#include <QTreeView>

class base_view;

class tree_view : public QTreeView {
	Q_OBJECT

private:
	base_view *base_view_;

protected:
	void dragEnterEvent(QDragEnterEvent*) override;
	void dragLeaveEvent(QDragLeaveEvent*) override;
	void dragMoveEvent(QDragMoveEvent*) override;
	void dropEvent(QDropEvent*) override;
	void mousePressEvent(QMouseEvent*) override;
	void mouseDoubleClickEvent(QMouseEvent*) override;
//	void contextMenuEvent(QContextMenuEvent*) override;
//	void customContextMenuRequested(const QPoint&);

public:
	explicit tree_view(QWidget *parent = nullptr);

	void set_base_view(base_view*);

};

#endif
