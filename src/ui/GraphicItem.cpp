#include "ui/GraphicItem.h"
#include <algorithm>
#include <vector>


CGRATile::CGRATile(ADGNode* a, std::string t)
{
    setFlags(QGraphicsItem::ItemIsSelectable);

    node = a;
    setType(t);
}

void CGRATile::setTile(ADGNode* a, std::string t)
{
    node = a;
    setType(t);
}

void CGRATile::setType(std::string t)
{
    itype = t;

    int inum = node->numInputs();
    int onum = node->numOutputs();
    int i, o, n, s;
    double x1, y1, x2, y2;

    if (t == "GIB")
    {
        height = GIB_SPACE;
        width = GIB_SPACE;

        for (i = 0; i < inum; i++)
        {
            s = i % 4;
            n = i / 4;
            switch (s)
            {
            case 1:
            {
                x1 = width / 4 + (n + 1.5) * PORT_SPACE;
                y1 = height / 2 - (n + 0.5) * PORT_SPACE;
                x2 = width / 4 + (n + 1) * PORT_SPACE;
                y2 = height / 2 - (n + 1) * PORT_SPACE;
                break;
            }
            case 2:
            {
                x1 = width / 2 - (n + 0.5) * PORT_SPACE;
                y1 = -height / 4 - (n + 1.5) * PORT_SPACE;
                x2 = width / 2 - (n + 1) * PORT_SPACE;
                y2 = -height / 4 - (n + 1) * PORT_SPACE;
                break;
            }
            case 3:
            {
                x1 = -width / 4 - (n + 1.5) * PORT_SPACE;
                y1 = -height / 2 + (n + 0.5) * PORT_SPACE;
                x2 = -width / 4 - (n + 1) * PORT_SPACE;
                y2 = -height / 2 + (n + 1) * PORT_SPACE;
                break;
            }
            default:
            {
                x1 = -width / 2 + (n + 0.5) * PORT_SPACE;
                y1 = height / 4 + (n + 1.5) * PORT_SPACE;
                x2 = -width / 2 + (n + 1) * PORT_SPACE;
                y2 = height / 4 + (n + 1) * PORT_SPACE;
            }
            }
            ipins[i] = std::make_pair(QPointF(x1, y1), QPointF(x2, y2));
            inputs[i] = std::make_pair(mapToScene(QPointF(x1, y1)), mapToScene(QPointF(x2, y2)));
        }

        for (o = 0; o < onum; o++)
        {
            s = o % 4;
            n = o / 4;
            switch (s)
            {
            case 1:
            {
                x1 = -width / 4 - (n + 1) * PORT_SPACE;
                y1 = -height / 2 + n * PORT_SPACE;
                x2 = -width / 4 - (n + 0.5) * PORT_SPACE;
                y2 = -height / 2 + (n + 0.5) * PORT_SPACE;
                break;
            }
            case 2:
            {
                x1 = -width / 2 + n * PORT_SPACE;
                y1 = height / 4 + (n + 1) * PORT_SPACE;
                x2 = -width / 2 + (n + 0.5) * PORT_SPACE;
                y2 = height / 4 + (n + 0.5) * PORT_SPACE;
                break;
            }
            case 3:
            {
                x1 = width / 4 + (n + 1) * PORT_SPACE;
                y1 = height / 2 - n * PORT_SPACE;
                x2 = width / 4 + (n + 0.5) * PORT_SPACE;
                y2 = height / 2 - (n + 0.5) * PORT_SPACE;
                break;
            }
            default:
            {
                x1 = width / 2 - n * PORT_SPACE;
                y1 = -height / 4 - (n + 1) * PORT_SPACE;
                x2 = width / 2 - (n + 0.5) * PORT_SPACE;
                y2 = -height / 4 - (n + 0.5) * PORT_SPACE;
            }
            }
            opins[o] = std::make_pair(QPointF(x1, y1), QPointF(x2, y2));
            outputs[o] = std::make_pair(mapToScene(QPointF(x1, y1)), mapToScene(QPointF(x2, y2)));
        }
    }
    else
    {
        if (t == "GPE")
        {
            height = GPE_SPACE;
            width = GPE_SPACE;
        }
        else if (t == "IB" || t == "OB" || t == "IOB")
        {
            height = IOB_SPACE;
            width = IOB_SPACE;
        }
        else if (t == "LSU")
        {
            height = LSU_SPACE;
            width = LSU_SPACE;
        }

        for (i = 0; i < inum; i++)
        {
            s = i % 4;
            n = i / 4;
            switch (s)
            {
            case 1:
            {
                x1 = width / 2 + PORT_SPACE / 2;
                y1 = -height / 2 + (2 * n + 1) * PORT_SPACE;
                x2 = width / 2;
                y2 = -height / 2 + (2 * n + 1) * PORT_SPACE;
                break;
            }
            case 2:
            {
                x1 = width / 2 - (2 * n + 1) * PORT_SPACE;
                y1 = height / 2 + PORT_SPACE / 2;
                x2 = width / 2 - (2 * n + 1) * PORT_SPACE;
                y2 = height / 2;
                break;
            }
            case 3:
            {
                x1 = -width / 2 - PORT_SPACE / 2;
                y1 = height / 2 - (2 * n + 1) * PORT_SPACE;
                x2 = -width / 2;
                y2 = height / 2 - (2 * n + 1) * PORT_SPACE;
                break;
            }
            default:
            {
                x1 = -width / 2 + (2 * n + 1) * PORT_SPACE;
                y1 = -height / 2 - PORT_SPACE / 2;
                x2 = -width / 2 + (2 * n + 1) * PORT_SPACE;
                y2 = -height / 2;
            }
            }
            ipins[i] = std::make_pair(QPointF(x1, y1), QPointF(x2, y2));
            inputs[i] = std::make_pair(mapToScene(QPointF(x1, y1)), mapToScene(QPointF(x2, y2)));
        }
        for (o = 0; o < onum; o++)
        {
            s = o % 4;
            n = o / 4;
            switch (s)
            {
            case 1:
            {
                x1 = -width / 2 - PORT_SPACE / 2;
                y1 = height / 2 - (2 * n + 2) * PORT_SPACE;
                x2 = -width / 2;
                y2 = height / 2 - (2 * n + 2) * PORT_SPACE;
                break;
            }
            case 2:
            {
                x1 = -width / 2 + (2 * n + 2) * PORT_SPACE;
                y1 = -height / 2 - PORT_SPACE / 2;
                x2 = -width / 2 + (2 * n + 2) * PORT_SPACE;
                y2 = -height / 2;
                break;
            }
            case 3:
            {
                x1 = width / 2 + PORT_SPACE / 2;
                y1 = -height / 2 + (2 * n + 2) * PORT_SPACE;
                x2 = width / 2;
                y2 = -height / 2 + (2 * n + 2) * PORT_SPACE;
                break;
            }
            default:
            {
                x1 = width / 2 - (2 * n + 2) * PORT_SPACE;
                y1 = height / 2 + PORT_SPACE / 2;
                x2 = width / 2 - (2 * n + 2) * PORT_SPACE;
                y2 = height / 2;
            }
            }
            opins[o] = std::make_pair(QPointF(x1, y1), QPointF(x2, y2));
            outputs[o] = std::make_pair(mapToScene(QPointF(x1, y1)), mapToScene(QPointF(x2, y2)));
        }
    }
}
/*
void CGRATile::mousePressEvent(QGraphicsSceneMouseEvent* event)
{
    if (event->button() == Qt::LeftButton)
    {
        selected = !selected;
    }
}*/

QRectF CGRATile::boundingRect() const
{
    return QRectF(-width / 2, -height / 2, width, height);
}

void CGRATile::paint(QPainter* painter, const QStyleOptionGraphicsItem* option, QWidget* widget)
{
    Q_UNUSED(widget);

    //Draw tiles
    double scaleFactor = painter->transform().m11();
    QPen pen;
    if (option->state & QStyle::State_Selected)
    {
        pen.setWidthF(3.0 / scaleFactor);
    }
    else
    {
        pen.setWidth(0);
    }
    if (used == 0)
    {
        pen.setColor(Qt::white);
    }
    else
    {
        pen.setColor(Qt::green);
    }
    QRectF rect(-width / 2, -height / 2, width, height);

    if (itype == "GIB")
    {
        //pen.setColor(Qt::yellow);
        painter->setPen(pen);        
        QPolygonF polygon;
        polygon << QPointF(-width / 4, -height / 2) << QPointF(width / 4, -height / 2) << QPointF(width / 2, -height / 4) << QPointF(width / 2, height / 4) << QPointF(width / 4, height / 2) << QPointF(-width / 4, height / 2) << QPointF(-width / 2, height / 4) << QPointF(-width / 2, -height / 4);
        painter->drawPolygon(polygon);
        polygon.clear();
        polygon.squeeze();
    }
    else
    {
        if (itype == "GPE")
        {
            //pen.setColor(Qt::green);
            painter->setPen(pen);
        }
        else if (itype == "IB" || itype == "OB" || itype == "IOB")
        {
            //pen.setColor(Qt::white);
            painter->setPen(pen);
        }
        else if (itype == "LSU")
        {
            //pen.setColor(Qt::white);
            painter->setPen(pen);
        }
        painter->drawRect(rect);
    }

    //Print tile name
    QString name = QString::fromStdString(itype) + ":" + QString::number(node->id());
    QFont font;
    font.setPixelSize(10);
    painter->setFont(font);
    if(user == "")
    {
        painter->drawText(rect, Qt::AlignHCenter | Qt::AlignVCenter, name);
    }
    else
    {
        painter->drawText(QRectF(-width / 2, -height / 2, width, height / 2), Qt::AlignHCenter | Qt::AlignBottom, name);
        painter->drawText(QRectF(-width / 2, 0.0, width, height / 2), Qt::AlignHCenter | Qt::AlignTop, QString::fromStdString(user));
    }

    //Draw pins
    int inum = node->numInputs();
    int onum = node->numOutputs();
    int n;
    QPointF top, middle, array1, array2;

    pen.setWidthF(3.0 / scaleFactor);
    painter->setPen(pen);

    for (n = 0; n < inum; n++)
    {
        top = ipins[n].second;
        middle = (ipins[n].first + ipins[n].second) / 2;
        array1.setX((top.x() + middle.x() - top.y() + middle.y()) / 2);
        array1.setY((top.x() - middle.x() + top.y() + middle.y()) / 2);
        array2.setX((top.x() + middle.x() + top.y() - middle.y()) / 2);
        array2.setY((middle.x() - top.x() + top.y() + middle.y()) / 2);

        painter->drawLine(ipins[n].first, ipins[n].second);
        painter->drawLine(top, array1);
        painter->drawLine(top, array2);
    }
    
    for (n = 0; n < onum; n++)
    {
        top = opins[n].first;
        middle = (opins[n].first + opins[n].second) / 2;
        array1.setX((top.x() + middle.x() - top.y() + middle.y()) / 2);
        array1.setY((top.x() - middle.x() + top.y() + middle.y()) / 2);
        array2.setX((top.x() + middle.x() + top.y() - middle.y()) / 2);
        array2.setY((middle.x() - top.x() + top.y() + middle.y()) / 2);

        painter->drawLine(opins[n].first, opins[n].second);
        painter->drawLine(top, array1);
        painter->drawLine(top, array2);
    }
}

CGRACluster::CGRACluster(ADGNode* a, std::string t)
{
    node = a;
    setType(t);
}

void CGRACluster::setCluster(ADGNode* a, std::string t)
{
    node = a;
    setType(t);
}

void CGRACluster::setType(std::string t)
{
    itype = t;
    if (t == "Muxn" || t == "ALU" || t == "SRAM" || t == "DelayPipe")
    {
        width = MUXN_X;
        height = MUXN_Y;
    }
    else
    {
        width = RF_X;
        height = RF_Y;
    }
    Ports();
}

void CGRACluster::Ports()
{
    int pin = 0, i = 0, o = 0;
    int n, s;
    double offset = 0.0;
    QPointF point;
    while (offset < width - PIN_S)
    {
        s = pin % 4;
        n = pin / 4;
        offset = PIN_S * (n + 1);
        switch (s)
        {
        case 1:
        {
            point = QPointF(-width / 2, -height / 2 + offset);
            break;
        }
        case 2:
        {
            point = QPointF(-width / 2 + offset, height / 2);
            break;
        }
        case 3:
        {
            point = QPointF(width / 2, height / 2 - offset);
            break;
        }
        default:
        {
            point = QPointF(width / 2 - offset, -height / 2);
        }
        }
        if (n % 2 == 0)
        {
            inputs[i++] = mapToScene(point);
        }
        else
        {
            outputs[o++] = mapToScene(point);
        }
        pin++;
    }
}

QRectF CGRACluster::boundingRect() const
{
    return QRectF(-width / 2, -height / 2, width, height);
}

void CGRACluster::paint(QPainter* painter, const QStyleOptionGraphicsItem* option, QWidget* widget)
{
    Q_UNUSED(option);
    Q_UNUSED(widget);

    QFont font;
    font.setPixelSize(10);
    QString name = QString::fromStdString(itype) + ":" + QString::number(node->id());
    QRectF rect(-width / 2, -height / 2, width, height);

    painter->setPen(QPen(Qt::green, 0));
    painter->drawRect(rect);

    painter->setFont(font);
    painter->drawText(rect, Qt::AlignHCenter | Qt::AlignVCenter, name);
}

CGRANet::CGRANet(ADGLink* e, int s, std::pair<QPointF,QPointF> src, int d, std::pair<QPointF,QPointF> dst)
{
    //setFlags(QGraphicsItem::ItemIsSelectable);
    link = e;
    sid = s / 4;
    did = d / 4;
    src0 = QPointF(0.0, 0.0);
    src1 = src.second - src.first;
    dst0 = dst.first - src.first;
    dst1 = dst.second - src.first;
}

void CGRANet::setNet(ADGLink* e, int s, std::pair<QPointF, QPointF> src, int d, std::pair<QPointF, QPointF> dst)
{
    link = e;
    sid = s / 4;
    did = d / 4;
    src0 = QPointF(0.0, 0.0);
    src1 = src.second - src.first;
    dst0 = dst.first - src.first;
    dst1 = dst.second - src.first;
}

void CGRANet::setNet(ADGLink* e, int s, QPointF src, int d, QPointF dst)
{
    link = e;
    itype = "intraconnect";
    sid = s;
    did = d;
    src0 = src;
    dst0 = dst;
}

/*
void CGRANet::mousePressEvent(QGraphicsSceneMouseEvent* event)
{
    if (event->button() == Qt::LeftButton)
    {
        selected = !selected;
    }
}*/

QRectF CGRANet::boundingRect() const
{
    qreal x1 = src0.x();
    qreal x2 = dst0.x();
    qreal y1 = src0.y();
    qreal y2 = dst0.y();

    return QRectF(std::min(x1, x2), std::min(y1, y2), std::fabs(x2 - x1), std::fabs(y2 - y1));
    //return QRectF(0.0, 0.0, 0.0, 0.0);
}

void CGRANet::paint(QPainter* painter, const QStyleOptionGraphicsItem* option, QWidget* widget)
{
    Q_UNUSED(option);
    Q_UNUSED(widget);

    double scaleFactor = painter->transform().m11();
    /*if (option->state & QStyle::State_Selected) { painter->setPen(QPen(Qt::white, 2.0 / scaleFactor)); }
    if (used) { painter->setPen(QPen(Qt::red, 0)); }
    else { painter->setPen(QPen(Qt::white, 0)); }*/
    painter->setPen(Pen());

    QPainterPath path = Path();
    painter->drawPath(path);
    //path.clear();
}

QPen CGRANet::Pen()
{
    QPen pen;
    pen.setWidth(0);
    if(used > 0)
    {
        int r = link->id()%10 * 25;
        int g = link->id()%7 * 40;
        int b = link->id()%9 * 30;
        pen.setColor(QColor(r, g, b));
    }
    else
    {
        pen.setColor(Qt::white);
    }
    return pen;
}

QPainterPath CGRANet::Path()
{
    QPainterPath path;
    path.moveTo(src0);
    QPointF p1, p2, p3;
    int ns, nd;
    double ax, ay, dx, dy;

    if(itype == "interconnect")
    {
        ns = sid % 4;
        nd = did % 4;

        QPointF vs = src0 - src1;
        QPointF vd = dst0 - dst1;
        double asx = fabs(vs.x());
        double asy = fabs(vs.y());
        double adx = fabs(vd.x());
        double ady = fabs(vd.y());

        p1 = (asx > 1.0 && asy > 1.0) ? (6 * vs) : (src0 + (ns + 1) * vs);
        p3 = (adx > 1.0 && ady > 1.0) ? (dst0 + 5.5 * vd) : (dst0 + (nd + 1.5) * vd);
        path.lineTo(p1);

        ax = std::fabs(p3.x() - p1.x()) > 60.0 ? 60.0 : std::fabs(p3.x() - p1.x());
        ay = std::fabs(p3.y() - p1.y()) > 60.0 ? 60.0 : std::fabs(p3.y() - p1.y());
        dx = (p3.x() - p1.x()) > 0.0 ? ax : -ax;
        dy = (p3.y() - p1.y()) > 0.0 ? ay : -ay;
        if (asx > 1.0 && asy < 1.0)
        {
            p1 += QPointF(0.0, dy);
            path.lineTo(p1);
        }
        else if (asx < 1.0 && asy > 1.0)
        {
            p1 += QPointF(dx, 0.0);
            path.lineTo(p1);
        }

        if (adx > 1.0 && ady < 1.0)
        {
            p2 = p3 - QPointF(0.0, dy);
            path.lineTo(p1.x(), p2.y());
            path.lineTo(p2);
        }
        else if (adx < 1.0 && ady > 1.0)
        {
            p2 = p3 - QPointF(dx, 0.0);
            path.lineTo(p1.x(), p2.y());
            path.lineTo(p2);
        }
        else
        {
            path.lineTo(p1.x(), p3.y());
        }
        path.lineTo(p3);
        path.lineTo(dst0);
    }

    if (itype == "intraconnect")
    {
        ns = sid % 4;
        nd = did % 4;
        double as = sid % 4 * 10 + 6.5;
        double ad = did % 4 * 10 + 8;
        if (src0.x() < CLUSTER_SPACE && dst0.x() > TILE_SPACE - CLUSTER_SPACE)    //Bypass nets
        {
            path.lineTo(dst0.x(), src0.y());
        }
        else if (src0.x() < CLUSTER_SPACE)//Input nets
        {
            switch (nd)
            {
            case 1:
            {
                p2 = dst0 - QPointF(ad, 0.0);
                path.lineTo(p2.x(), src0.y());
                break;
            }
            case 2:
            {
                p2 = dst0 + QPointF(0.0, ad);
                p1 = p2 - QPointF(CLUSTER_SPACE, 0.0);
                path.lineTo(p1.x(), src0.y());
                path.lineTo(p1);
                break;
            }
            case 3:
            {
                p2 = dst0 + QPointF(ad, 0.0);
                path.lineTo(p2.x(), src0.y());
                break;
            }
            default:
            {
                p2 = dst0 - QPointF(0.0, ad);
                p1 = p2 - QPointF(CLUSTER_SPACE, 0.0);
                path.lineTo(p1.x(), src0.y());
                path.lineTo(p1);
            }
            }
            path.lineTo(p2);
        }
        else if (dst0.x() > TILE_SPACE - CLUSTER_SPACE)//Output nets
        {
            switch (ns)
            {
            case 1:
            {
                p1 = src0 - QPointF(as, 0.0);
                path.lineTo(p1);
                break;
            }
            case 2:
            {
                p1 = src0 + QPointF(0.0, as);
                path.lineTo(p1);
                p1 += QPointF(CLUSTER_SPACE, 0.0);
                path.lineTo(p1);
                break;
            }
            case 3:
            {
                p1 = src0 + QPointF(as, 0.0);
                path.lineTo(p1);
                break;
            }
            default:
            {
                p1 = src0 - QPointF(0.0, as);
                path.lineTo(p1);
                p1 += QPointF(CLUSTER_SPACE, 0.0);
                path.lineTo(p1);
            }
            }
            path.lineTo(p1.x(), dst0.y());
        }
        else//Intra nets
        {
            switch (ns)
            {
            case 1:
            {
                p1 = src0 - QPointF(as, 0.0);
                break;
            }
            case 2:
            {
                p1 = src0 + QPointF(0.0, as);
                break;
            }
            case 3:
            {
                p1 = src0 + QPointF(as, 0.0);
                break;
            }
            default:
            {
                p1 = src0 - QPointF(0.0, as);
            }
            }
            switch (nd)
            {
            case 1:
            {
                p3 = dst0 - QPointF(ad, 0.0);
                break;
            }
            case 2:
            {
                p3 = dst0 + QPointF(0.0, ad);
                break;
            }
            case 3:
            {
                p3 = dst0 + QPointF(ad, 0.0);
                break;
            }
            default:
            {
                p3 = dst0 - QPointF(0.0, ad);
            }
            }
            path.lineTo(p1);
            ax = std::fabs(p3.x() - p1.x()) > CLUSTER_SPACE ? CLUSTER_SPACE : std::fabs(p3.x() - p1.x());
            ay = std::fabs(p3.y() - p1.y()) > CLUSTER_SPACE ? CLUSTER_SPACE : std::fabs(p3.y() - p1.y());
            dx = (p3.x() - p1.x()) > 0.0 ? ax : -ax;
            dy = (p3.y() - p1.y()) > 0.0 ? ay : -ay;
            if (ns == 1 || ns == 3)
            {
                p1 += QPointF(0.0, dy);
                path.lineTo(p1);
            }
            else
            {
                p1 += QPointF(dx, 0.0);
                path.lineTo(p1);
            }
            if (nd == 1 || nd == 3)
            {
                p2 = p3 - QPointF(0.0, dy);
                path.lineTo(p1.x(), p2.y());
                path.lineTo(p2);
            }
            else
            {
                p2 = p3 - QPointF(dx, 0.0);
                path.lineTo(p1.x(), p2.y());
                path.lineTo(p2);
            }
            path.lineTo(p3);
        }
        path.lineTo(dst0);
    }

    return path;
}

QRectF CGRAPin::boundingRect() const
{
    if (level == 0)
    {
        if (itype == "input")
        {
            return QRectF(-L0_X - L0_Y, -L0_Y / 2, L0_X + L0_Y, L0_Y);
        }
        else
        {
            return QRectF(0.0, -L0_Y / 2, L0_X + L0_Y, L0_Y);
        }
    }
    else
    {
        return QRectF(0.0, 0.0, 0.0, 0.0);
    }
}

void CGRAPin::paint(QPainter* painter, const QStyleOptionGraphicsItem* option, QWidget* widget)
{
    QFont font;
    QRectF rect;
    QString name = QString::fromStdString(itype) + ":" + QString::number(id);
    if (level == 0)
    {
        painter->setPen(QPen(Qt::green, 0));
        QPolygonF polygon;
        if (itype == "I")
        {
            polygon << QPointF(-L0_X - L0_Y, -L0_Y / 2) << QPointF(-L0_Y, -L0_Y / 2) << QPointF(0.0, 0.0) << QPointF(-L0_Y, L0_Y / 2) << QPointF(-L0_X - L0_Y, L0_Y / 2);
            rect = QRectF(-L0_X - L0_Y, -L0_Y / 2, L0_X, L0_Y);
        }
        else
        {
            polygon << QPointF(0.0, 0.0) << QPointF(L0_Y, -L0_Y / 2) << QPointF(L0_X + L0_Y, -L0_Y / 2) << QPointF(L0_X + L0_Y, L0_Y / 2) << QPointF(L0_Y, L0_Y / 2);
            rect = QRectF(L0_Y, -L0_Y / 2, L0_X, L0_Y);
        }
        painter->drawPolygon(polygon);
        font.setPixelSize(5);
        painter->setFont(font);
        painter->drawText(rect, Qt::AlignHCenter | Qt::AlignVCenter, name);
        polygon.clear();
        polygon.squeeze();
    }
}
