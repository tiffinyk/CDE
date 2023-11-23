#include <numeric>
#include "ui/ViewWidget.h"

namespace {
    const double TILE_SPACE_H = 25.;
    const double TILE_SPACE_V = 20.;
    const double SCENE_MARGIN = 30.;
    const double TILE_SPACE = 100.;

    const double BIRD_WIDTH = 190.;
    const double BIRD_HEIGHT = 130.;

    /*const double GPE_SPACE = 60.;
    const double GIB_SPACE = 50.;
    const double IOB_SPACE = 20.;
    const double LSU_SPACE = 50.;
    const double NODE_Y = 30.;*/
}

	bool MainView::viewportEvent(QEvent *event)
	{
		bool return_val = QGraphicsView::viewportEvent(event);
		QRectF draw_rect(mapToScene(rect()).boundingRect());
		double w_time = scene()->sceneRect().width() / BIRD_WIDTH;
		double h_time = scene()->sceneRect().height() / BIRD_HEIGHT;
		double top(draw_rect.top() / h_time);
		double left(draw_rect.left() / w_time);
		double height(draw_rect.height() / h_time);
		double width(draw_rect.width() / w_time);

		// draw bird-view rect
		emit viewport_changed(QRectF(left, top, width, height));
		return return_val;
	}

	void MainView::mousePressEvent(QMouseEvent *event)
	{
		if (event->button() == Qt::RightButton) {
			if (!_rubber_started) { 
				_zoom_rect.setTopLeft(event->pos());
				_rubber_started_point = event->pos();
			}
			_zoom_rect.setBottomRight(event->pos());
			_rubber_started = true;
			setCursor(Qt::CrossCursor);
		}
		else
			QGraphicsView::mousePressEvent(event);
	}

	// for show of zoom rect
	void MainView::mouseMoveEvent(QMouseEvent *event)
	{
		if (!_rubber_started) return;

		QGraphicsScene *scene = this->scene();
		if (_last_rubber_rect) {
			scene->removeItem(_last_rubber_rect);
			delete _last_rubber_rect;
			_last_rubber_rect = 0;
		}
		QPointF top_left = mapToScene(_rubber_started_point);
		QPointF bot_right = mapToScene(event->pos());
		_last_rubber_rect = scene->addRect(QRectF(top_left, bot_right), QPen(Qt::yellow));
		this->viewport()->update();
	}

	void MainView::mouseReleaseEvent(QMouseEvent *event)
	{
		if (event->button() == Qt::RightButton && _rubber_started)
		{
			_zoom_rect.setBottomRight(event->pos());
			unsetCursor();
			// handle zoom selection
			_zoom_rect = _zoom_rect.normalized();
			if (_zoom_rect.height() < 5. || _zoom_rect.width() < 5.) return;
			QPointF center_pos((_zoom_rect.left() + _zoom_rect.right()) / 2.0,
				(_zoom_rect.top() + _zoom_rect.bottom())/ 2.0);

			centerOn(mapToScene(center_pos.toPoint()));
			double zoom_scale = qMin(height() / _zoom_rect.height(), width() / _zoom_rect.width());
			scale(zoom_scale, zoom_scale);
			// end zoom
		}
		else
		{
			QGraphicsView::mouseReleaseEvent(event);
			/*if (event->button() == Qt::LeftButton && 
				(Cluster::MoveStarted || Route::NetMoveStarted) )
			{
				QPointF new_pos(mapToScene(event->pos()));
				if (Cluster *MoveTo = dynamic_cast<Cluster*>(scene()->itemAt(new_pos, QTransform())))
				{
					Cluster *MoveFrom = Cluster::MoveFrom;
					if (MoveFrom &&
						MoveTo != MoveFrom &&
						!MoveTo->is_used() &&
						MoveTo->same_type(MoveFrom))
					{
						MoveTo->set_movable(true);
						MoveTo->set_used(true);
						MoveFrom->set_used(false);
						MoveFrom->set_movable(false);
						emit cluster_moved(MoveFrom->logic_pos(), MoveTo->logic_pos());
						viewport()->update();
					}
					Route* NetFrom = Route::NetFrom;
					if (NetFrom && 
						!MoveTo->is_used() &&
						(MoveTo->cluster_type() == "IOB") &&
						!(MoveTo->pad()).empty())
					{
						MoveTo->set_movable(true);
						MoveTo->set_used(true);
						string net = NetFrom->name();
						emit net_pulled(NetFrom->name(), MoveTo->logic_pos());
						viewport()->update();
					}
					else if ((MoveTo->cluster_type() == "IOB") && 
							 (MoveTo->pad()).empty())
					{
						QMessageBox msgBox;
						msgBox.setText("This cluster is unreachable from outside the package.");
						msgBox.exec();
					}
				}
			}
			else*/
				emit deselect_all();

			/*Cluster::MoveStarted = false;
			Cluster::MoveFrom    = 0;

			Route::NetMoveStarted = false;
			Route::NetFrom = 0;*/
		}

		QGraphicsScene *scene = this->scene();
		if (_last_rubber_rect != 0)
		{	// delete the zoom rect when mouse released
			scene->removeItem(_last_rubber_rect);
			delete _last_rubber_rect;
			_last_rubber_rect = 0;
		}

		_rubber_started = false;
	}

	void MainView::dragEnterEvent(QDragEnterEvent *event)
	{
		if (event->mimeData()->hasFormat("text/plain"))
			event->acceptProposedAction();
	}

	/*void MainView::dropEvent(QDropEvent *event)
	{
		QPointF new_pos(mapToScene(event->pos()));
		if (Cluster *MoveTo = dynamic_cast<Cluster*>(scene()->itemAt(new_pos, QTransform())))
		{
			Route* NetFrom = Route::NetFrom;
			if (NetFrom && 
				!MoveTo->is_used() &&
				(MoveTo->cluster_type() == "IOB") &&
				!(MoveTo->pad()).empty())
			{
				MoveTo->set_movable(true);
				MoveTo->set_used(true);
				string net = NetFrom->name();
				emit net_pulled(NetFrom->name(), MoveTo->logic_pos());
				viewport()->update();
			}
			else if ((MoveTo->cluster_type() == "IOB") && 
					 (MoveTo->pad()).empty())
			{
				QMessageBox msgBox;
				msgBox.setText("This cluster is unreachable from outside the package.");
				msgBox.exec();
			}
			Route::NetMoveStarted = false;
			Route::NetFrom = 0;
		}
	}*/

	void MainView::mouseDoubleClickEvent(QMouseEvent *event)
	{
		if (QGraphicsItem *item = itemAt(event->pos())) {
			if (item->type() == CGRATile::Type)
			{
				emit tile_double_clicked(qgraphicsitem_cast<CGRATile*>(item)->node);
			}
			/*switch (item->type()) {
				case (CGRATile::Type):
					emit tile_double_clicked(qgraphicsitem_cast<Tile *>(item)->name());
					emit tile_double_clicked(qgraphicsitem_cast<CGRATile*>(item)->node->id());
					break;
			}*/
		}
	}

	void MainView::init_scale() 
	{
		QRectF scene_rect(scene()->sceneRect());
		
		/* I use the width() & height() instead of sizeHint() before,
		 * the problem is that the width() are not valid since the GUI 
		 * hasn't appear when using command line model
		 * This can be resolved
		 * by using sizeHint().width() & sizeHint().height()
		 */
		double view_width = sizeHint().width();
		double view_height = sizeHint().height();
		double full_scale = qMin(view_width / scene_rect.width(), view_height / scene_rect.height());

		setMatrix(_one2one_matrix);       // reset to 1:1 view-scene
		scale(full_scale, full_scale);
		_reset_matrix = matrix();
	}

	void MainView::init_view()
	{
		setViewportUpdateMode(QGraphicsView::FullViewportUpdate);
		setOptimizationFlag(QGraphicsView::DontSavePainterState);
		_rubber_started = false;
		_last_rubber_rect = 0;
	}

    void MainView::zoom_in()
    {
        scale(2, 2);
        QList<QGraphicsItem *> items = scene()->selectedItems();
        if (items.count() == 1) {
            QGraphicsItem *item = items.takeFirst();
            centerOn(item->scenePos() + item->boundingRect().center());
            return;
        }
    }

    void MainView::zoom_out()
    {
        scale(0.5, 0.5);
        QList<QGraphicsItem *> items = scene()->selectedItems();
        if (items.count() == 1) {
            QGraphicsItem *item = items.takeFirst();
            centerOn(item->scenePos() + item->boundingRect().center());
            return;
        }
    }

    /*void MainView::zoom_in(QGraphicsItem *net)
	{
		scale(2, 2);
		QList<QGraphicsItem *> items = scene()->selectedItems();
		if (items.count() == 1) {
			QGraphicsItem *item = items.takeFirst();
			centerOn(item->scenePos() + item->boundingRect().center());
			return;
		}
		if (net)
			centerOn(net->scenePos() + net->boundingRect().center());
	}

	void MainView::zoom_out(QGraphicsItem *net)
	{
		scale(0.5, 0.5);
		QList<QGraphicsItem *> items = scene()->selectedItems();
		if (items.count() == 1) {
			QGraphicsItem *item = items.takeFirst();
			centerOn(item->scenePos() + item->boundingRect().center());
			return;
		}
		if (net)
			centerOn(net->scenePos() + net->boundingRect().center());
    }*/

	ViewWidget::ViewWidget(QWidget *parent /* = 0 */)
		: QWidget(parent), /*_arch(new Arch()), _design(new UserDesign(this, _arch.get())),*/
		  _scene(new QGraphicsScene(this)), _view(new MainView(_scene, this))
	{
		//_scene->setBackgroundBrush(QColor(82, 82, 82)); // dark gray
		_scene->setBackgroundBrush(Qt::black); // black background
		QVBoxLayout* layout= new QVBoxLayout(this); // memory deallocated automatically
		layout->addWidget(_view);
		setLayout(layout);
		setAcceptDrops(true);

		connect(_view, SIGNAL(viewport_changed(QRectF)), this, SIGNAL(draw_viewport(QRectF)));
		//connect(_view, SIGNAL(cluster_moved(Point, Point)), this, SLOT(cluster_moved(Point, Point)));
		//connect(_view, SIGNAL(net_pulled(const string&, Point)), this, SLOT(net_moved(const std::string &, Point)));
		//connect(_view, SIGNAL(deselect_all()), this, SLOT(deselect_all()));
		//connect(_view, SIGNAL(tile_double_clicked(const string&)), this, SIGNAL(update_tile(const string&)));
		connect(_view, SIGNAL(tile_double_clicked(ADGNode*)), this, SIGNAL(update_tile(ADGNode*)));

		connect(&_rt_process, SIGNAL(readyReadStandardOutput()), this, SLOT(disp_stdout()));
		connect(&_rt_process, SIGNAL(readyReadStandardError()), this, SLOT(disp_stderr()));
		connect(&_bg_process, SIGNAL(readyReadStandardOutput()), this, SLOT(disp_stdout()));
		connect(&_bg_process, SIGNAL(readyReadStandardError()), this, SLOT(disp_stderr()));

		connect(&_rt_process, SIGNAL(started()), this, SIGNAL(process_started()));
		connect(&_rt_process, SIGNAL(finished(int, QProcess::ExitStatus)), this, SLOT(route_finish_handler()));
	}

	ViewWidget::~ViewWidget()
	{
		free_design();
		//_arch->clear();
		/*if (_adgir != NULL)
		{
			_adgir->clear();
		}*/
	}

	void ViewWidget::free_design()
	{
		/*_design->clear();
		emit free_design_data();
		emit design_is_modified(false);
		emit design_route_is_modified(false);*/
	}

	void ViewWidget::free_modify()
	{
		/*_design->clear_modify();
		emit design_is_modified(false);
		emit design_route_is_modified(false);*/
	}

    // ===================================================
    //   Graphics functions
    // ===================================================

    void ViewWidget::draw_chip(ADG* _adg)
    {
        QGraphicsRectItem* chip = new QGraphicsRectItem;

        ADGNode* p_node;
        ADGLink* p_link;
        CGRATile* first = NULL;

        int scale_x = 0, scale_y = 0, pos_x, pos_y;

        for (auto& n : _adg->nodes())
        {
            CGRATile* _tile = new CGRATile;
            tiles[n.first] = _tile;
            p_node = n.second;
            if (first == NULL) { first = _tile; }

            pos_x = p_node->x();
            pos_y = p_node->y();

            _tile->setPos((pos_x + 1) * TILE_SPACE, (pos_y + 1) * TILE_SPACE);

            scale_x = pos_x > scale_x ? pos_x : scale_x;
            scale_y = pos_y > scale_y ? pos_y : scale_y;

            //Draw ADG nodes
            _tile->setTile(p_node, p_node->type());

            _scene->addItem(_tile);
        }

        //Draw the CGRA chip
        chip->setPos(0.0, 0.0);
        chip->setPen(QPen(Qt::white, 0));
        chip->setRect(0.0, 0.0, (scale_x + 2) * TILE_SPACE, (scale_y + 2) * TILE_SPACE);
        _scene->addItem(chip);
        chip->stackBefore(first);   //Put the chip boundary under other tiles.

        _view->setSceneRect(QRectF(0.0, 0.0, (scale_x + 2) * TILE_SPACE, (scale_y + 2) * TILE_SPACE));

        //Draw ADG nets
        //double xSrc, ySrc, xDst, yDst;
        std::pair<QPointF, QPointF> s, d, p;
        int src, srcPort, dst, dstPort;

        for (auto& l : _adg->links())
        {
            p_link = l.second;

            src = p_link->srcId();
            srcPort = p_link->srcPortIdx();
            dst = p_link->dstId();
            dstPort = p_link->dstPortIdx();

            if (src != 0 && dst != 0)//The src or dst cannot be the CGRA chip
            {
                s = tiles[src]->output(srcPort);
                d = tiles[dst]->input(dstPort);
                //p.setX((s.x() + d.x()) / 2);
                //p.setY((s.y() + d.y()) / 2);
                CGRANet* _net = new CGRANet;
                _net->setPos(s.first);
                _net->setNet(p_link, srcPort, s, dstPort, d);
                _scene->addItem(_net);
                _net->stackBefore(first);   //Put nets under the tile layer.
                nets[l.first] = _net;
            }
        }

        showMaximized();
        _view->init_scale();
        arch_ready = true;
		emit file_is_loaded();
    }

    void ViewWidget::draw_netlist(Netlist* _netlist)
    {
		if(drawed_netlists.size() == 1)
		{
			for(auto n : (*drawed_netlists.begin())->nodes())
			{
				tiles[n.first]->setUser("");
			}
		}
    	for(int e : _netlist->nets())
    	{
    		nets[e]->addUsed();
    	}
    	for(auto n : _netlist->nodes())
    	{
    		tiles[n.first]->addUsed();
    		if(drawed_netlists.size() == 0)	{ tiles[n.first]->setUser(n.second.second); }
    	}
		for (auto s : _netlist->switches())
		{
			tiles[s]->addUsed();
		}
		drawed_netlists.emplace(_netlist);
		update_view();
		emit file_is_loaded();
    }

    void ViewWidget::remove_netlist(Netlist* _netlist)
    {
    	for(int e : _netlist->nets())
    	{
    		nets[e]->delUsed();
    	}
    	for(auto n : _netlist->nodes())
    	{
    		tiles[n.first]->delUsed();
    		if(drawed_netlists.size() == 1)	{ tiles[n.first]->setUser(""); }
    	}
		for (auto s : _netlist->switches())
		{
			tiles[s]->delUsed();
		}
		if(drawed_netlists.size() > 0)	{ drawed_netlists.erase(_netlist); }
		if(drawed_netlists.size() == 1)
		{
			for(auto n : (*drawed_netlists.begin())->nodes())
			{
				tiles[n.first]->setUser(n.second.second);
			}
		}
		update_view();
		emit file_is_loaded();
    }

    void ViewWidget::clear()
    {
    	tiles.clear();
    	nets.clear();
    	drawed_netlists.clear();
    	_scene->clear();
    }


	void ViewWidget::draw_route(bool draw/* = true*/)
	{
		//_design->show_route(draw);
	}

	void ViewWidget::disp_stdout()
	{
		QString info(_rt_process.readAllStandardOutput() + 
					_bg_process.readAllStandardOutput());
		emit add_progress(info.toStdString());
	}

	void ViewWidget::disp_stderr()
	{
		QString info(_rt_process.readAllStandardError() + 
					_bg_process.readAllStandardError());
		emit add_progress(info.toStdString());
	}

	void ViewWidget::drag_rt_list(QListWidgetItem* item, bool begin)
	{
		/*Route::NetMoveStarted = begin;
		Route::NetFrom = item ? _design->get_route_by_name(item->text().toStdString()) : nullptr;*/
	}

	void ViewWidget::route_finish_handler()
	{
		/*QMessageBox msgBox;
		msgBox.setText("Generate Bit Stream?");
		msgBox.setStandardButtons(QMessageBox::Yes | QMessageBox::No);
		msgBox.setDefaultButton(QMessageBox::Yes);
		int ret = msgBox.exec();
		if (ret == QMessageBox::No) return;

		QString cil_file_path = _arch->file_path();
		cil_file_path.replace("_arch.xml", "_cil.xml");

		QFile file_finder;
		if (!file_finder.exists(cil_file_path)) {
			cil_file_path = QFileDialog::getOpenFileName(this,
			"Choose the CIL file", ".", "XML files (*.xml)");
		}

		if (!cil_file_path.isEmpty())
			bit_generate(cil_file_path, _design->save_path());

		emit process_finished();*/
	}

