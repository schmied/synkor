#ifndef BASE_VIEW_HPP
#define BASE_VIEW_HPP

#include <QAbstractItemView>
#include <QFileSystemModel>
#include <QLineEdit>
#include <QPushButton>
#include <QStatusBar>

class window;
class list_view;
class tree_view;

static const auto FILTER_BASE_TREE = QDir::AllDirs | QDir::NoDotAndDotDot;
static const auto FILTER_BASE_LIST = QDir::AllEntries | QDir::NoDotAndDotDot;
static const auto FILTER_ADD_HIDDEN = QDir::Hidden | QDir::System;

class item_view {

private:
	window *window_;
	list_view *list_;
	tree_view *tree_;
	QLineEdit *head_;
	QFileSystemModel list_model_;
	QFileSystemModel tree_model_;

public:
	item_view(window*, tree_view*, list_view*, QLineEdit*);

	window* main_window();
	list_view* list();
	tree_view* tree();
	QLineEdit* head();
	QStatusBar* status_bar();
	QFileSystemModel* list_model();
	QFileSystemModel* tree_model();

	void mousePressEventTree(QMouseEvent*);
	void mouseDoubleClickEventList(QMouseEvent*);

	void toggleFilter();

	void dropEvent(QDropEvent*, QAbstractItemView*);
	static void dragEnterEvent(QDragEnterEvent*);
	static void dragLeaveEvent(QDragLeaveEvent*, QAbstractItemView*);
	void dragMoveEvent(QDragMoveEvent*, QAbstractItemView*, QFileSystemModel*, QStatusBar*);

};

#endif
