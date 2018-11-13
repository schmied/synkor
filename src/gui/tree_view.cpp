
#include <QMenu>
#include <QMouseEvent>

#include "tree_view.hpp"
#include "window.hpp"

tree_view::tree_view(QWidget *parent) : QTreeView(parent) {
//	setColumnHidden(1, true);
//	setColumnHidden(2, true);
//	setColumnHidden(3, true);

//	setContextMenuPolicy(Qt::CustomContextMenu);
	setContextMenuPolicy(Qt::DefaultContextMenu);
}

void tree_view::set_item_view(item_view *item_view) {
	item_view_ = item_view;
}

void tree_view::dragEnterEvent(QDragEnterEvent *event) {
	item_view::dragEnterEvent(event);
}

void tree_view::dragLeaveEvent(QDragLeaveEvent *event) {
	item_view::dragLeaveEvent(event, this);
}

void tree_view::dragMoveEvent(QDragMoveEvent *event) {
	item_view_->dragMoveEvent(event, this, item_view_->tree_model(), item_view_->status_bar());
}

void tree_view::dropEvent(QDropEvent *event) {
	item_view_->dropEvent(event, this);
}

void tree_view::mousePressEvent(QMouseEvent *event) {
	item_view_->mousePressEventTree(event);
}

void tree_view::mouseDoubleClickEvent(QMouseEvent *event) {
	item_view_->mousePressEventTree(event);
}

void tree_view::contextMenuEvent(QContextMenuEvent *event) {
	const auto index = item_view_->tree()->indexAt(event->pos());
	const auto path = item_view_->tree_model()->filePath(index);
	const auto main_window = item_view_->main_window();
	main_window->action_data1 = path;
	main_window->context_menu_->popup(viewport()->mapToGlobal(event->pos()));
	event->accept();
}
