
#include <QMouseEvent>

#include "list_view.hpp"

#include "item_view.hpp"

list_view::list_view(QWidget *parent) : QListView(parent) {
	setContextMenuPolicy(Qt::DefaultContextMenu);
}

void list_view::set_item_view(item_view *item_view) {
	item_view_ = item_view;
}

void list_view::dragEnterEvent(QDragEnterEvent *event) {
	item_view::dragEnterEvent(event);
}

void list_view::dragLeaveEvent(QDragLeaveEvent *event) {
	item_view::dragLeaveEvent(event, this);
}

void list_view::dragMoveEvent(QDragMoveEvent *event) {
	item_view::dragMoveEvent(event, this, item_view_->list_model(), item_view_->status_bar());
}

void list_view::dropEvent(QDropEvent *event) {
	item_view::dropEvent(event, this);
}

void list_view::mousePressEvent(QMouseEvent *event) {
	if (event->button() & Qt::MouseButton::LeftButton) {
		QListView::mousePressEvent(event);
		return;
	}
	if (event->button() & Qt::MouseButton::RightButton) {
		event->accept();
	}
}

void list_view::mouseDoubleClickEvent(QMouseEvent *event) {
	item_view_->mouseDoubleClickEventList(event);
}
