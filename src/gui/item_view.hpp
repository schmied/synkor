#ifndef BASE_VIEW_HPP
#define BASE_VIEW_HPP

#include <QAbstractItemView>
#include <QFileSystemModel>
#include <QLineEdit>
#include <QStatusBar>


class list_view;
class tree_view;

class item_view {

private:
	list_view *list_;
	tree_view *tree_;
	QLineEdit *head_;
	QStatusBar *status_bar_;
	QFileSystemModel list_model_;
	QFileSystemModel tree_model_;


public:
	item_view(tree_view*, list_view*, QLineEdit*, QStatusBar*);

	list_view* list();
	tree_view* tree();
	QLineEdit* head();
	QStatusBar* status_bar();
	QFileSystemModel* list_model();
	QFileSystemModel* tree_model();

	void mousePressEventTree(QMouseEvent*);
	void mouseDoubleClickEventList(QMouseEvent*);

	static void dragEnterEvent(QDragEnterEvent*);
	static void dragLeaveEvent(QDragLeaveEvent*, QAbstractItemView*);
	static void dragMoveEvent(QDragMoveEvent*, QAbstractItemView*, QFileSystemModel*, QStatusBar*);
	static void dropEvent(QDropEvent*, QAbstractItemView*);

};

#endif
