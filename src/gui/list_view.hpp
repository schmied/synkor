#ifndef LIST_VIEW_HPP
#define LIST_VIEW_HPP

#include <QListView>

class item_view;

class list_view : public QListView {
	Q_OBJECT

private:
	item_view *item_view_;

protected:
	void dragEnterEvent(QDragEnterEvent*) override;
	void dragLeaveEvent(QDragLeaveEvent*) override;
	void dragMoveEvent(QDragMoveEvent*) override;
	void dropEvent(QDropEvent*) override;
	void mousePressEvent(QMouseEvent*) override;
	void mouseDoubleClickEvent(QMouseEvent*) override;
	void contextMenuEvent(QContextMenuEvent*) override;

public:
	explicit list_view(QWidget *parent = nullptr);

	void set_item_view(item_view*);

};

#endif
