#include "ui/SubArchWidget.h"


	const qreal SubArchView::MARGIN = 20.;
	const qreal SubArchView::V_SPACE = 5.;
	const int C_SPACE = 120;

// SubArchView

	SubArchView::SubArchView(QGraphicsScene* scene, QWidget* parent)
		: QGraphicsView(scene, parent)
	{
		_showHierachyAction = new QAction(tr("Show inner structure..."), this);
		
		_cmenu = new QMenu(this);
		_cmenu->addAction(_showHierachyAction);
		_showHierachyAction->setEnabled(false);

		connect(_showHierachyAction,	SIGNAL(triggered(bool)),
			this,	SIGNAL(showInnerStructTriggered(bool)));
	}

	void SubArchView::contextMenuEvent(QContextMenuEvent* event)
	{
		/*QGraphicsItem* item = scene()->itemAt(mapToScene(event->pos()), QTransform());
		if (!item) return;

		if ((65536 < item->type()) && (item->type() < 65546)) {
			Primitive* centerPrimitive = qgraphicsitem_cast<Primitive *>(item);

			if (centerPrimitive->primitive()->down_module()->num_instances()) {
				_centerPrimitive = centerPrimitive;
				_showHierachyAction->setEnabled(true);
			} else {
				_showHierachyAction->setEnabled(false);
			}
			_cmenu->exec(event->globalPos());
		}
		event->accept();*/
	}

	void SubArchView::mouseDoubleClickEvent(QMouseEvent *event)
	{
		QGraphicsItem* item = scene()->itemAt(mapToScene(event->pos()), QTransform());
		if (!item) return;
		/*
		switch (item->type()) {
			case Primitive::Type:
			case GSB::Type:
			case SliceBlock::Type:
			case ModulePin::Type:
				emit instanceDoubleClicked(item);
		}*/
		event->accept();
	}

	void SubArchView::mousePressEvent(QMouseEvent *event)
	{
		scene()->update(sceneRect());

		QGraphicsItem* item = scene()->itemAt(mapToScene(event->pos()), QTransform());
		if (!item) return;

		/*switch (item->type()) {
			case PrimitiveNet::Type:	
				emit netClicked(qgraphicsitem_cast<PrimitiveNet *>(item)->net());
				break;
			case Primitive::Type:
			case GSB::Type:
			case SliceBlock::Type:
				_centerPrimitive = qgraphicsitem_cast<Primitive *>(item);
		}*/
		
	}
	
	void SubArchView::wheelEvent(QWheelEvent *event)
	{
		setTransformationAnchor(QGraphicsView::AnchorUnderMouse);
		if (event->delta() > 0) 
			scale(2., 2.);
		else 
			scale(0.5, 0.5);
		event->accept();
		update();
	}

// SubArchWidget

	SubArchWidget::SubArchWidget(QWidget *parent)
		: QWidget(parent)
	{
		_scene = new QGraphicsScene(this);
		_scene->setBackgroundBrush(Qt::black);
		_view	= new SubArchView(_scene, this);
		_view->ensureVisible(QRectF(0, 0, 0, 0));
		_view->show();
		QSizePolicy policy = _view->sizePolicy();
		policy.setHorizontalPolicy(QSizePolicy::Expanding);
		_view->setSizePolicy(policy);

		_searchNetText = new QLineEdit();
		_searchNetText->setPlaceholderText(QString("Look for Net..."));

		_infoText = new QTextEdit;
		_infoText->setFont(QFont("Consolas"));
		_infoText->setTextInteractionFlags(Qt::TextSelectableByMouse);

		_netProxy = new QSortFilterProxyModel;
		_netModel = new QStandardItemModel;
		_netProxy->setSourceModel(_netModel);
		_netModel->setHeaderData(0, Qt::Horizontal, "Nets");

		_netList = new QTreeView;
		_netList->setModel(_netProxy);
		_netList->setAlternatingRowColors(true);

		_instProxy = new QSortFilterProxyModel;
		_instModel = new QStandardItemModel;
		_instProxy->setSourceModel(_instModel);
		_instModel->setHeaderData(0, Qt::Horizontal, "Instances");
		_instModel->setHeaderData(1, Qt::Horizontal, "Pins");

		_instList = new QTreeView;
		_instList->setModel(_instProxy);
		_instList->setAlternatingRowColors(true);

		QTabWidget* tabWidget = new QTabWidget;
		tabWidget->addTab(_instList, "Instance Information");
		tabWidget->addTab(_netList, "Nets Information");

		QSplitter* leftSplitter= new QSplitter(Qt::Vertical);
		leftSplitter->addWidget(tabWidget);
		leftSplitter->addWidget(_infoText);

		QVBoxLayout* vlayout = new QVBoxLayout;
		vlayout->addWidget(_searchNetText);
		vlayout->addWidget(leftSplitter);

		QWidget* leftWidget = new QWidget;
		leftWidget->setLayout(vlayout);
		leftWidget->resize(_netList->sizeHint().width(), tabWidget->sizeHint().height());
		policy = leftWidget->sizePolicy();
		policy.setHorizontalPolicy(QSizePolicy::Maximum);
		leftWidget->setSizePolicy(policy);

		QSplitter* splitter= new QSplitter;
		splitter->addWidget(leftWidget);
		splitter->addWidget(_view);

		QVBoxLayout* layout = new QVBoxLayout;
		layout->addWidget(splitter);
		setLayout(layout);

		/*connect(_view,	SIGNAL(netClicked(COS::Net *)),
			this,	SLOT(updateNet(COS::Net *)));
		connect(_view,	SIGNAL(showInnerStructTriggered(bool)),
			this,	SLOT(resetInstance(bool)));
		connect(_view,	SIGNAL(instanceDoubleClicked(QGraphicsItem *)),
			this,	SLOT(updateInstance(QGraphicsItem *)));*/

/*		connect(_searchNetText,	SIGNAL(textChanged(QString)), 
			this,	SLOT(filter(QString)));
		connect(_netList->selectionModel(),		SIGNAL(currentChanged(const QModelIndex&, const QModelIndex&)),
			this,	SLOT(onNetCurrentChanged(const QModelIndex&, const QModelIndex&)));
		connect(_instList->selectionModel(),	SIGNAL(currentChanged(const QModelIndex&, const QModelIndex&)),
			this,	SLOT(onInstCurrentChanged(const QModelIndex&, const QModelIndex&)));*/
		
		resize(maximumSize());
	}
/*
	void SubArchWidget::updateModule(const Instance* instance)
	{
		cleanAll();

		Module* mod = instance->down_module();
		setWindowTitle(QString("%1 (%2)")
			.arg(QString::fromStdString(instance->name()))
			.arg(QString::fromStdString(mod->name())));

		qreal vOffset = SubArchView::MARGIN;

		for (Net* net: mod->nets()) {
			PrimitiveNet* pnet = new PrimitiveNet(net);

			_scene->addItem(pnet);

			QStandardItem* netItem = new QStandardItem(QString::fromStdString(net->name()));
			netItem->setData(QVariant::fromValue((void*)net), NetlistRole);
			_netModel->appendRow(netItem);

			_netPnetMap[net] = pnet;
		}

		for (Instance* inst: mod->instances()) {
			Primitive* prim = GFactory::makePrimitive(inst);

			if ("GRM" == inst->down_module()->type() || "GSB" == inst->down_module()->type()) {
				prim->setPos(SubArchView::MARGIN + 3 * Primitive::MIN_WIDTH, 
					SubArchView::MARGIN);
			} else {
				prim->setPos(SubArchView::MARGIN, vOffset);
				vOffset += prim->boundingRect().height() * 2;
			}
			_scene->addItem(prim);

			QStandardItem* instItem = new QStandardItem(QString::fromStdString(inst->name()));
			instItem->setData(QVariant::fromValue((void*)inst), NetlistRole);
			_instModel->appendRow(instItem);

			for (Pin* pin: inst->pins()) {
				QStandardItem* pinItem = new QStandardItem(QString::fromStdString(pin->name()));
				pinItem->setData(QVariant::fromValue((void*)pin), NetlistRole);
				instItem->appendRow(pinItem);
			}

			_objPrimMap[inst] = prim;
		}

		for (map<Object*, QGraphicsItem *>::const_iterator iter = _objPrimMap.begin();
			iter != _objPrimMap.end(); ++iter) {
			Primitive* prim = dynamic_cast<Primitive*>(iter->second);
			if (prim->primitive()->down_module()->type() == "GRM" || prim->primitive()->down_module()->type() == "GSB") continue;
			manualArrangeTileNets(prim);
		}

		show();
		fitView();
	}*/

	void SubArchWidget::updateModule(ADGNode* n)
	{
		cleanAll();
		setWindowTitle(QString::fromStdString(n->type()) + ":" + QString::number(n->id()));
		ADG* adg = n->subADG();
		ADGNode* p_node;
		ADGLink* p_link;
		double x = 0.0, y = 0.0;
		int num = 0;
		std::string t;
		std::map<int, CGRACluster*> clusters;
		for (auto& n : adg->nodes())
		{
			p_node = n.second;
			t = p_node->type();
			if (t != "This")
			{
				if (num % 4 == 0)
				{
					x = C_SPACE;
					y += C_SPACE;
				}
				else
				{
					x += C_SPACE;
				}
				CGRACluster* c = new CGRACluster;
				c->setPos(x, y);
				c->setCluster(p_node, t);
				_scene->addItem(c);
				clusters[n.first] = c;
				num++;
			}
		}

		int srcId, srcPort, dstId, dstPort;
		QPointF src, dst;
		double iy, oy;
		double ix = 20.0, ox = 5 * C_SPACE - 20.0;
		for (auto& l : adg->links())
		{
			p_link = l.second;
			srcId = p_link->srcId();
			srcPort = p_link->srcPortIdx();
			dstId = p_link->dstId();
			dstPort = p_link->dstPortIdx();
			CGRANet* _net = new CGRANet;
			CGRAPin* ipin, * opin;
			std::map<int, CGRAPin*> ipins, opins;
			if (srcId != 0 && dstId != 0)	//Inner nets
			{
				src = clusters[srcId]->output(srcPort);
				dst = clusters[dstId]->input(dstPort);
			}
			else if (srcId == 0 && dstId != 0)	//Input nets
			{
				iy = srcPort / 5 * C_SPACE + srcPort % 5 * 10 + C_SPACE * 5 / 4 + 10;
				src = QPointF(ix, iy);
				dst = clusters[dstId]->input(dstPort);
				if (ipins[srcPort] == NULL)
				{
					ipin = new CGRAPin;
					ipins[srcPort] = ipin;
					ipin->setPos(src);
					ipin->setPin(srcPort, 0, "I");
					_scene->addItem(ipin);
				}
			}
			else if (srcId != 0 && dstId == 0)	//Output nets
			{
				oy = dstPort / 6 * C_SPACE + dstPort % 6 * 10 + C_SPACE * 5 / 4 + 5;
				src = clusters[srcId]->output(srcPort);
				dst = QPointF(ox, oy);
				if (opins[dstPort] == NULL)
				{
					opin = new CGRAPin;
					opins[dstPort] = opin;
					opin->setPos(dst);
					opin->setPin(dstPort, 0, "O");
					_scene->addItem(opin);
				}
			}
			else//Bypass nets
			{
				iy = srcPort / 5 * C_SPACE + srcPort % 5 * 10 + C_SPACE * 5 / 4 + 10;
				oy = dstPort / 6 * C_SPACE + dstPort % 6 * 10 + C_SPACE * 5 / 4 + 10;
				src = QPointF(ix, iy);
				dst = QPointF(ox, oy);
				if (ipins[srcPort] == NULL)
				{
					ipin = new CGRAPin;
					ipins[srcPort] = ipin;
					ipin->setPos(src);
					ipin->setPin(srcPort, 0, "I");
					_scene->addItem(ipin);
				}
				if (opins[dstPort] == NULL)
				{
					opin = new CGRAPin;
					opins[dstPort] = opin;
					opin->setPos(dst);
					opin->setPin(dstPort, 0, "O");
					_scene->addItem(opin);
				}
			}
			_net->setPos(0.0, 0.0);
			_net->setNet(p_link, srcPort, src, dstPort, dst);
			_scene->addItem(_net);
		}

		showMaximized();
		fitView();
	}

	void SubArchWidget::filter(QString pattern)
	{
		_netProxy->setFilterWildcard(pattern);
		_instProxy->setFilterWildcard(pattern);
	}

	void SubArchWidget::cleanAll()
	{
        clearItems();
		clearModels();
	}

    void SubArchWidget::clearItems()
	{
		_scene->clear();

		_levelItemsMap.clear();
    }
	
	void SubArchWidget::clearModels()
	{
		_netModel->clear();
		_instModel->clear();
	}

	void SubArchWidget::fitView()
	{
		_view->setSceneRect(_view->scene()->itemsBoundingRect().adjusted(-100, -100, 100, 100));
		_view->fitInView(_view->sceneRect(), Qt::KeepAspectRatio);	
	}

