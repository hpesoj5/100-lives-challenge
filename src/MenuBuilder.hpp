#pragma once

#include <Geode/Geode.hpp>

using namespace geode::prelude;

class MenuBuilder final {
public:
    MenuBuilder() : m_menu { CCMenu::create() }, m_zOrder{} {}

    MenuBuilder& layout(Layout* layout, bool apply = true, bool respectAnchor = true) {
        m_menu->setLayout(layout, apply, respectAnchor);
        return *this;
    }
    MenuBuilder& position(float x, float y) {
        m_menu->setPosition(x, y);
        return *this;
    }
    MenuBuilder& anchor(float x, float y) {
        m_menu->setAnchorPoint({x, y});
        return *this;
    }
    MenuBuilder& contentHeight(float height) {
        m_menu->setContentHeight(height);
        return *this;
    }
    MenuBuilder& contentWidth(float width) {
        m_menu->setContentWidth(width);
        return *this;
    }
    MenuBuilder& contentSize(CCSize const& size) {
        m_menu->setContentSize(size);
        return *this;
    }
    MenuBuilder& ID(std::string const& id) {
        m_menu->setID(id);
        return *this;
    }
    MenuBuilder& zOrder(int order) {
        m_zOrder = order;
        return *this;
    }
    MenuBuilder& parent(CCNode* par) {
        par->addChild(m_menu, m_zOrder);
        par->updateLayout();
        return *this;
    }
    MenuBuilder& child(CCNode* ch, int zOrder = 0) {
        m_menu->addChild(ch, zOrder);
        m_menu->updateLayout();
        return *this;
    }

    MenuBuilder& child_fast(CCNode* ch, int zOrder = 0) {  // must call updateLayout() afterwards
        m_menu->addChild(ch, zOrder);
        return *this;
    }

    MenuBuilder& ignoreAnchorPointForPosition(bool ignore) {
        m_menu->ignoreAnchorPointForPosition(ignore);
        return *this;
    }

    MenuBuilder& keypad(bool enabled) {
        m_menu->setKeypadEnabled(enabled);
        return *this;
    }

    void removeAllChildren() { m_menu->removeAllChildren(); }
    void updateLayout() { m_menu->updateLayout(); }
    CCPoint getPosition() const { return m_menu->getPosition(); }
    float getContentWidth() const { return m_menu->getContentWidth(); }
    float getContentHeight() const { return m_menu->getContentHeight(); }
    CCSize const& getContentSize() const { return m_menu->getContentSize(); }
    CCMenu* build() { return m_menu; }
    int getZOrder() const { return m_zOrder; }

private:
    CCMenu* m_menu;
    int m_zOrder;
};
