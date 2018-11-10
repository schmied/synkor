#ifndef LIST_VIEW_HPP
#define LIST_VIEW_HPP

#include <QListView>

class base_view;

class list_view : public QListView {
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
	explicit list_view(QWidget *parent = nullptr);

	void set_base_view(base_view*);

};

#endif
