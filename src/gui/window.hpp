#ifndef WINDOW_HPP
#define WINDOW_HPP

#include <QMainWindow>

#include "item_view.hpp"

namespace Ui {
class window;
}

class window : public QMainWindow {
	Q_OBJECT

private:
	Ui::window *ui_;
	item_view *rl_item_left_;
	item_view *rl_item_right_;

	QAction *action_context_copy;
	QAction *action_context_cut;
	QAction *action_context_paste;
	QAction *action_drop_copy;
	QAction *action_drop_move;

private slots:
	void on_rl_item_left_head_filter_clicked();
	void on_rl_item_right_head_filter_clicked();
	void context_copy_action();
	void context_cut_action();
	void context_paste_action();
	void drop_copy_action();
	void drop_move_action();

protected:

public:
	explicit window(QWidget *parent = nullptr);
	~window();

	Ui::window *ui();

	QMenu *context_menu_;
	QMenu *drop_menu_;

	QString action_dir_dst;
	QList<QString> action_items_src;

};

#endif
