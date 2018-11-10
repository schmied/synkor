
#include <QMouseEvent>

#include "tree_view.hpp"

#include "window.hpp"

#include "ui_window.h"

tree_view::tree_view(QWidget *parent) : QTreeView(parent) {
/*
	setColumnHidden(2, true);
	setColumnHidden(3, true);

	setContextMenuPolicy(Qt::DefaultContextMenu);
//	setContextMenuPolicy(Qt::CustomContextMenu);
*/
}

void tree_view::set_base_view(base_view *base_view) {
	base_view_ = base_view;
}

void tree_view::dragEnterEvent(QDragEnterEvent *event) {
	base_view::dragEnterEvent(event);
}

void tree_view::dragLeaveEvent(QDragLeaveEvent *event) {
	base_view::dragLeaveEvent(event, this);
}

void tree_view::dragMoveEvent(QDragMoveEvent *event) {
	base_view::dragMoveEvent(event, this, base_view_->tree_model(), base_view_->status_bar());
}

void tree_view::dropEvent(QDropEvent *event) {
	base_view::dropEvent(event, this);
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
	base_view_->mousePressEventTree(event);
}

void tree_view::mouseDoubleClickEvent(QMouseEvent *event) {
	base_view_->mousePressEventTree(event);
}
