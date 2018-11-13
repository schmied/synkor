
#include <QMenu>
#include <QMouseEvent>

#include "list_view.hpp"
#include "window.hpp"

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
	item_view_->dragMoveEvent(event, this, item_view_->list_model(), item_view_->status_bar());
}

void list_view::dropEvent(QDropEvent *event) {
	item_view_->dropEvent(event, this);
}

void list_view::mousePressEvent(QMouseEvent *event) {
//	if (!(event->button() & Qt::MouseButton::LeftButton)) {
//		event->ignore();
//		return;
//	}
	QListView::mousePressEvent(event);
	event->accept();;
}

void list_view::mouseDoubleClickEvent(QMouseEvent *event) {
	item_view_->mouseDoubleClickEventList(event);
}

void list_view::contextMenuEvent(QContextMenuEvent *event) {
	const auto index = item_view_->list()->indexAt(event->pos());
	const auto path = item_view_->tree_model()->filePath(index);
	const auto main_window = item_view_->main_window();
	main_window->action_data1 = path;
	main_window->context_menu_->popup(viewport()->mapToGlobal(event->pos()));
	event->accept();
}
