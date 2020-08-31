#include "abstractinstrumentpaneltreeitem.h"

#include "log.h"

using namespace mu::instruments;
using namespace mu::notation;

AbstractInstrumentPanelTreeItem::AbstractInstrumentPanelTreeItem(const InstrumentTreeItemType::ItemType& type, notation::INotationParts* notationParts, QObject* parent)
    : QObject(parent), m_notationParts(notationParts)
{
    setType(type);
}

AbstractInstrumentPanelTreeItem::~AbstractInstrumentPanelTreeItem()
{
    for (AbstractInstrumentPanelTreeItem* child : m_children) {
        child->deleteLater();
    }
}

bool AbstractInstrumentPanelTreeItem::canAcceptDrop(const int type) const
{
    return static_cast<InstrumentTreeItemType::ItemType>(type) == m_type;
}

QString AbstractInstrumentPanelTreeItem::id() const
{
    return m_id;
}

QString AbstractInstrumentPanelTreeItem::title() const
{
    return m_title;
}

int AbstractInstrumentPanelTreeItem::type() const
{
    return static_cast<int>(m_type);
}

bool AbstractInstrumentPanelTreeItem::isVisible() const
{
    return m_isVisible;
}

bool AbstractInstrumentPanelTreeItem::isSelectable() const
{
    return m_type == InstrumentTreeItemType::ItemType::PART
           || m_type == InstrumentTreeItemType::ItemType::INSTRUMENT
           || m_type == InstrumentTreeItemType::ItemType::STAFF;
}

AbstractInstrumentPanelTreeItem* AbstractInstrumentPanelTreeItem::parentItem() const
{
    return m_parent;
}

void AbstractInstrumentPanelTreeItem::setParentItem(AbstractInstrumentPanelTreeItem* parent)
{
    m_parent = parent;
}

QList<AbstractInstrumentPanelTreeItem*> AbstractInstrumentPanelTreeItem::childrenItems() const
{
    return m_children;
}

AbstractInstrumentPanelTreeItem* AbstractInstrumentPanelTreeItem::childAtRow(const int row)
{
    if (row < 0 || row >= childCount()) {
        return nullptr;
    }

    return static_cast<AbstractInstrumentPanelTreeItem*>(m_children.at(row));
}

void AbstractInstrumentPanelTreeItem::appendChild(AbstractInstrumentPanelTreeItem* child)
{
    if (!child) {
        return;
    }

    child->setParentItem(this);

    m_children.append(child);
}

void AbstractInstrumentPanelTreeItem::moveChildren(const int sourceRow, const int count, AbstractInstrumentPanelTreeItem* destinationParent, const int destinationRow)
{
    for (int i = sourceRow; i < sourceRow + count; ++i) {
        AbstractInstrumentPanelTreeItem* sourceRowItem = this->childAtRow(i);

        if (!sourceRowItem) {
            continue;
        }

        if (sourceRow + count - 1 < destinationRow) {
            destinationParent->insertChild(sourceRowItem, destinationRow);
            this->removeChildren(sourceRow);
        } else {
            this->removeChildren(sourceRow);
            destinationParent->insertChild(sourceRowItem, destinationRow);
        }
    }
}

void AbstractInstrumentPanelTreeItem::insertChild(AbstractInstrumentPanelTreeItem* child, const int beforeRow)
{
    if (!child) {
        return;
    }

    child->setParentItem(this);

    m_children.insert(beforeRow, child);
}

void AbstractInstrumentPanelTreeItem::removeChildren(const int row, const int count, const bool deleteChild)
{
    if (count == 1) {
        m_children.removeAt(row);
        return;
    }

    for (int i = row + count - 1; i <= row; --i) {
        AbstractInstrumentPanelTreeItem* child = m_children.at(i);

        m_children.removeAt(i);

        if (deleteChild) {
            child->deleteLater();
        }
    }
}

int AbstractInstrumentPanelTreeItem::childCount() const
{
    return m_children.size();
}

int AbstractInstrumentPanelTreeItem::row() const
{
    if (!parentItem()) {
        return 0;
    }

    return parentItem()->childrenItems().indexOf(const_cast<AbstractInstrumentPanelTreeItem*>(this));
}

void AbstractInstrumentPanelTreeItem::setType(InstrumentTreeItemType::ItemType type)
{
    if (m_type == type)
        return;

    m_type = type;
    emit typeChanged(m_type);
    emit isSelectableChanged(isSelectable());
}

void AbstractInstrumentPanelTreeItem::setTitle(QString title)
{
    if (m_title == title) {
        return;
    }

    m_title = title;
    emit titleChanged(m_title);
}

void AbstractInstrumentPanelTreeItem::setIsVisible(bool isVisible)
{
    if (m_isVisible == isVisible) {
        return;
    }

    m_isVisible = isVisible;

    for (int i = 0; i < childCount(); ++i) {
        childAtRow(i)->setIsVisible(isVisible);
    }

    emit isVisibleChanged(m_isVisible);
}

void AbstractInstrumentPanelTreeItem::setId(const QString& id)
{
    m_id = id;
}

INotationParts* AbstractInstrumentPanelTreeItem::notationParts() const
{
    IF_ASSERT_FAILED(m_notationParts) {
        return nullptr;
    }

    return m_notationParts;
}
