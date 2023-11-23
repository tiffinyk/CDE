#ifndef SRC_VIEW_WIDGET_H
#define SRC_VIEW_WIDGET_H

#include <string>
#include <QtCore>
#include <QtGui>
#include <QtWidgets>

#include "ui/GraphicItem.h"
#include "ui/Netlist.h"

//Q_DECLARE_METATYPE(COS::Point);

//namespace FPGAViewer {
	using std::string;
	//using namespace ARCH;
    //using namespace COS;
	//using namespace GraphicItemLib;

	class MainView : public QGraphicsView
	{
		Q_OBJECT
	public:
		MainView(QWidget *parent = 0) : QGraphicsView(parent) { init_view(); }
		MainView(QGraphicsScene *scene, QWidget *parent = 0) : QGraphicsView(scene, parent), _one2one_matrix(matrix()) { init_view(); }
		//~MainView();

		void    full_scale()       { setMatrix(_reset_matrix); }
		void    init_scale();

	signals:
		void    viewport_changed(QRectF);
        //void    cluster_moved(Point, Point);
        //void	net_pulled(const string&, Point);
		void	deselect_all();

		//void	tile_double_clicked(const string&);
		void	tile_double_clicked(ADGNode*);

	public slots:
        //void    zoom_in(QGraphicsItem *net = 0);
        //void    zoom_out(QGraphicsItem *net = 0);
        void    zoom_in();
        void    zoom_out();

	protected:
		void    mousePressEvent(QMouseEvent *event);
		void    mouseMoveEvent(QMouseEvent *event);
		void    mouseReleaseEvent(QMouseEvent *event);
		void	dragEnterEvent(QDragEnterEvent *event);
		//void	dropEvent(QDropEvent *event);
		bool    viewportEvent(QEvent *event);

		void	mouseDoubleClickEvent(QMouseEvent * event);

	private:
		void	init_view();

		bool      _rubber_started;
		QPoint    _rubber_started_point;

		QGraphicsItem    *_last_rubber_rect;
		//QGraphicsItem    *_now_rubber_rect;

		QRectF    _zoom_rect;
		QMatrix   _reset_matrix;

		QMatrix   _one2one_matrix;

	};

	class ViewWidget : public QWidget
	{
		Q_OBJECT

//		typedef map<string, Route*>	route_map;

	public:

		ViewWidget(QWidget *parent = 0);

		~ViewWidget();
		void		free_design();
		void		free_modify();

		//void		load_arch(const string& file_name);
		//void		load_design(const string& file_name);
		//void		load_netlist(const string& file_name);
        void    	draw_chip(ADG* );
        void    	draw_netlist(Netlist* );
        void    	remove_netlist(Netlist* );
        Netlist*	get_netlist()	{ return *drawed_netlists.begin(); }

		//void		draw_placement(bool draw = true);
		void		draw_route(bool draw = true);
		
		//void		save_placement(QString save_file_path);
		//void		save_rt_placement(QString save_file_path);
		//void		bit_generate(QString cil_file_path, QString netlist_file_path = "");
		//void		hide_unselected_line(bool hide);
		//void		hide_unhighlighted_line(bool hide);

		// graphics view related
		void		update_view() const { _view->viewport()->update(); }
		void		full_scale()  const { _view->full_scale(); }
        //void		zoom_in(QString item = "");
		//void		zoom_out(QString item = "");
        void		zoom_in()	{ _view->zoom_in(); }
        void		zoom_out()	{ _view->zoom_out();}

		//void		select_route_by_name(const string& name, bool select);
		//void		select_cluster_by_pos(const Point &pos, bool select);
		void		clear();

		//QStringList	creat_route_list() const { return _design->creat_route_list(); }
		//bool		arch_is_ready()    const { return _arch->is_ready(); }
		bool		arch_is_ready()    const { return  arch_ready; }
		//bool		design_modified()  const { return _design->is_modified(); }
		//const QString& design_path()   const { return _design->file_path(); }

		bool route_visible = false;

	public slots:
//		void		addSignalTap();
	signals:
		void		design_is_modified(bool modified);
		void		design_route_is_modified(bool modified);
		void		draw_viewport(QRectF);
		void		set_current_net(int);
		void		emit_draw(bool);
		void		show_sta_info(QString);
		void		free_design_data();
		void		list_highlighted_net(QStringList);

		void		file_is_loading();
		void		file_is_loaded();
		void		add_progress(const string& output);
		void		set_status(const string& status);
		void		process_started();
		void		process_finished();

		void		deselect_all_nets();

		//void		update_tile(const string& tileName);
		void		update_tile(ADGNode*);

	private slots:
//		void		select_changed(string, string);
//		void		cluster_moved(Point, Point);
//		void		net_moved(const std::string &, Point);

		void		disp_stdout();
		void		disp_stderr();

//		void		highlight_cluster(const Pins&, const Pins&, bool);
//		void		highlight_route(route_map, bool);

		//void		deselect_all() { _design->deselect_all(); }

		void		drag_rt_list(QListWidgetItem*, bool);

		void		route_finish_handler();
		
	private:

		friend class UserDesign;
//		void		add_net(Route* net);
//		void		addPadModel(Point clusterPos);
//		void		addNetModel(const string& net_name);

	private:
		enum MyRoles {
			NetlistRole = Qt::UserRole + 1
		};

		//ADG* _adg = NULL;
		//std::unique_ptr<Arch> _arch;
		//UserDesign* _design;

		QGraphicsScene*		_scene;
		MainView*			_view;

		QProcess			_rt_process;
		QProcess			_bg_process;

		QSortFilterProxyModel*	_netProxy;
		QStandardItemModel*		_netModel;

		QSortFilterProxyModel*	_padProxy;
		QStandardItemModel*		_padModel;

        std::map<int, CGRATile*> tiles;
        std::map<int, CGRANet*> nets;
        std::set<Netlist*> drawed_netlists;
        bool arch_ready = false;
	};


//}

#endif // SRC_VIEW_WIDGET_H
