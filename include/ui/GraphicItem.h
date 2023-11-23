#ifndef __GRAPHICITEM_H__
#define __GRAPHICITEM_H__

#include <QtGui>
#include <QtCore>
#include <QtWidgets>
#include <map>
#include "adg/adg.h"

//using namespace COS;

const int TileT = 1;
const int ClusterT = 2;
const int NetT = 3;
const int PinT = 4;

/*class CGRATile : public QGraphicsRectItem
{
public:
    ADGNode* node;
//        QRectF rect()         const { return _rect; }
//        void set_rect(const QRectF& rect) { _rect = rect; }
    private:
//        QRectF _rect;
    };*/

class CGRATile : public QGraphicsItem
{
public:
    enum { Type = UserType + TileT };
    int type() const { return Type; }

    CGRATile() { setFlags(QGraphicsItem::ItemIsSelectable); }
    CGRATile(ADGNode* , std::string );
    ADGNode* node;
    void setTile(ADGNode* , std::string );
    void setType(std::string );
    std::string TileType() { return itype; }
    std::pair<QPointF, QPointF> input(int i) { return inputs[i]; }
    std::pair<QPointF, QPointF> output(int o) { return outputs[o]; }

    void setUsed(int u) { used = u; }
    void addUsed() { used++; }
    void delUsed() { if(used > 0) used--; }
    int isUsed() { return used; }
    void setUser(std::string u) { user = u; }
    std::string getUser() { return user; }

protected:
    QRectF boundingRect() const;
    void paint(QPainter* painter, const QStyleOptionGraphicsItem* option, QWidget* widget);
    //void mousePressEvent(QGraphicsSceneMouseEvent* event);

private:
    bool selected = false;
    int used = 0;
    std::string user = "";
    qreal width;
    qreal height;
    std::string itype;
    std::map<int, std::pair<QPointF, QPointF>> inputs;
    std::map<int, std::pair<QPointF, QPointF>> outputs;
    std::map<int, std::pair<QPointF, QPointF>> ipins;
    std::map<int, std::pair<QPointF, QPointF>> opins;

    const double GPE_SPACE = 60.;
    const double GIB_SPACE = 60.;
    const double IOB_SPACE = 30.;
    const double LSU_SPACE = 50.;
    const double PORT_SPACE = 5.;
    const double NODE_Y = 30.;
};

class CGRACluster : public QGraphicsItem
{
public:
    enum { Type = UserType + ClusterT };
    int type() const { return Type; }

    CGRACluster() { setFlags(QGraphicsItem::ItemIsSelectable); }
    CGRACluster(ADGNode*, std::string);
    ADGNode* node = NULL;
    void setCluster(ADGNode*, std::string);
    void setType(std::string );
    std::string ClusterType() { return itype; }
    QPointF input(int i) { return inputs[i]; }
    QPointF output(int o) { return outputs[o]; }

    void setUsed(bool u) { used = u; }
    bool isUsed() { return used; }

protected:
    QRectF boundingRect() const;
    void paint(QPainter* painter, const QStyleOptionGraphicsItem* option, QWidget* widget);
    //void mousePressEvent(QGraphicsSceneMouseEvent* event);

private:
    bool selected = false;
    bool used = false;
    qreal width;
    qreal height;
    std::string itype;
    std::map<int, QPointF> inputs;
    std::map<int, QPointF> outputs;
    void Ports();

    const double MUXN_X = 60.;
    const double MUXN_Y = 60.;
    const double ALU_X = 60.;
    const double ALU_Y = 60.;
    const double SRAM_X = 60.;
    const double SRAM_Y = 60.;
    const double RF_X = 60.;
    const double RF_Y = 60.;
    const double DELAY_X = 60.;
    const double DELAY_Y = 60.;
    const double CONST_X = 60.;
    const double CONST_Y = 60.;
    const double PIN_S = 10.;
};

class CGRANet : public QGraphicsItem
{
public:
    enum { Type = UserType + NetT };
    int type() const { return Type; }

    CGRANet() { /*setFlags(QGraphicsItem::ItemIsSelectable);*/ }
    CGRANet(ADGLink*, int, std::pair<QPointF, QPointF>, int, std::pair<QPointF, QPointF>);
    ADGLink* link;
    //ADGNode* src;
    //ADGNode* dst;
    void setNet(ADGLink*, int, std::pair<QPointF, QPointF>, int, std::pair<QPointF, QPointF>);
    void setNet(ADGLink*, int, QPointF, int, QPointF);

    std::string NetType() { return itype; }
    void setUsed(int u) { used = u; }
    void addUsed() { used++; }
    void delUsed() { if(used > 0) used--; }
    int isUsed() { return used; }

protected:
    QRectF boundingRect() const;
    void paint(QPainter* painter, const QStyleOptionGraphicsItem* option, QWidget* widget);
    //void mousePressEvent(QGraphicsSceneMouseEvent* event);

private:
    //qreal width;
    //qreal height;
    std::string itype = "interconnect";
    bool selected = false;
    int used = 0;
    int sid;
    int did;
    QPointF src0;
    QPointF dst0;
    QPointF src1;
    QPointF dst1;

    QPainterPath Path();
    QPen Pen();

    const double PIN_SPACE = 2.0;
    const double CLUSTER_SPACE = 60.0;
    const double TILE_SPACE = 600.0;
};

class CGRAPin : public QGraphicsItem
{
public:
    enum { Type = UserType + PinT };
    int type() const { return Type; }

    CGRAPin()   {}
    CGRAPin(int i, int l, std::string t) { id = i; level = l; itype = t; }

    void setPin(int i, int l, std::string t) { id = i; level = l; itype = t; }
protected:
    QRectF boundingRect() const;
    void paint(QPainter* painter, const QStyleOptionGraphicsItem* option, QWidget* widget);
    
private:
    int id;
    int level;
    std::string itype;

    const double L0_Y = 5.0;
    const double L0_X = 15.0;
};

#endif
