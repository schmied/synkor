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

private slots:

	void on_rl_item_left_head_filter_clicked();

	void on_rl_item_right_head_filter_clicked();

protected:

public:
	explicit window(QWidget *parent = nullptr);
	~window();

};

#endif
