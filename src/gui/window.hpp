#ifndef WINDOW_HPP
#define WINDOW_HPP

#include <QMainWindow>

#include "base_view.hpp"

namespace Ui {
class window;
}

class window : public QMainWindow {
	Q_OBJECT

private:
	Ui::window *ui_;
	base_view *base_view_;

private slots:

protected:

public:
	explicit window(QWidget *parent = nullptr);
	~window();

};

#endif
