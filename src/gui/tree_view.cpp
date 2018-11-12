
#include <QMouseEvent>

#include "tree_view.hpp"

#include "window.hpp"

#include "ui_window.h"

tree_view::tree_view(QWidget *parent) : QTreeView(parent) {
//	setColumnHidden(1, true);
//	setColumnHidden(2, true);
//	setColumnHidden(3, true);

/*
	setContextMenuPolicy(Qt::DefaultContextMenu);
//	setContextMenuPolicy(Qt::CustomContextMenu);
*/
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
	item_view::dragMoveEvent(event, this, item_view_->tree_model(), item_view_->status_bar());
}

void tree_view::dropEvent(QDropEvent *event) {
	item_view::dropEvent(event, this);
}

/*
void tree_view::contextMenuEvent(QContextMenuEvent *event) {
//	ui_->label_src->setText("MENU");
	QMenu *menu=new QMenu(this);
	menu->addAction(new QAction("MENU", this));
//	menu->popup(viewport()->mapToGlobal(event->pos()));
	menu->popup(event->pos());
	event->accept();
}
*/

void tree_view::mousePressEvent(QMouseEvent *event) {
	item_view_->mousePressEventTree(event);
}

void tree_view::mouseDoubleClickEvent(QMouseEvent *event) {
	item_view_->mousePressEventTree(event);
}
