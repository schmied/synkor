
#include <QMouseEvent>

#include "list_view.hpp"

#include "base_view.hpp"

list_view::list_view(QWidget *parent) : QListView(parent) {
	setContextMenuPolicy(Qt::DefaultContextMenu);
}

void list_view::set_base_view(base_view *base_view) {
	base_view_ = base_view;
}

void list_view::dragEnterEvent(QDragEnterEvent *event) {
	base_view::dragEnterEvent(event);
}

void list_view::dragLeaveEvent(QDragLeaveEvent *event) {
	base_view::dragLeaveEvent(event, this);
}

void list_view::dragMoveEvent(QDragMoveEvent *event) {
	base_view::dragMoveEvent(event, this, base_view_->list_model(), base_view_->status_bar());
}

void list_view::dropEvent(QDropEvent *event) {
	base_view::dropEvent(event, this);
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
	base_view_->mouseDoubleClickEventList(event);
}
