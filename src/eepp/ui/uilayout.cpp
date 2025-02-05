#include <eepp/ui/uilayout.hpp>
#include <eepp/ui/uiscenenode.hpp>

namespace EE { namespace UI {

UILayout* UILayout::New() {
	return eeNew( UILayout, () );
}

UILayout::UILayout() : UIWidget( "layout" ) {
	mNodeFlags |= NODE_FLAG_LAYOUT;
	unsetFlags( UI_TAB_FOCUSABLE );
}

UILayout::UILayout( const std::string& tag ) : UIWidget( tag ) {
	mNodeFlags |= NODE_FLAG_LAYOUT;
	unsetFlags( UI_TAB_FOCUSABLE );
}

void UILayout::onChildCountChange( Node* child, const bool& removed ) {
	UIWidget::onChildCountChange( child, removed );

	if ( child->isLayout() ) {
		UILayout* layout = child->asType<UILayout>();
		if ( removed ) {
			mLayouts.erase( layout );
		} else {
			mLayouts.insert( layout );
		}
	}

	tryUpdateLayout();
}

void UILayout::onSizeChange() {
	UIWidget::onSizeChange();
	tryUpdateLayout();
}

void UILayout::onPaddingChange() {
	UIWidget::onPaddingChange();
	tryUpdateLayout();
}

void UILayout::onParentSizeChange( const Vector2f& sizeChange ) {
	UIWidget::onParentSizeChange( sizeChange );
	if ( !getParent()->isLayout() ) {
		mPacking = false;
		tryUpdateLayout();
	}
}

void UILayout::onLayoutUpdate() {
	sendCommonEvent( Event::OnLayoutUpdate );
}

Uint32 UILayout::getType() const {
	return UI_TYPE_LAYOUT;
}

bool UILayout::isType( const Uint32& type ) const {
	return UILayout::getType() == type ? true : UIWidget::isType( type );
}

const Sizef& UILayout::getSize() const {
	if ( mDirtyLayout )
		const_cast<UILayout*>( this )->updateLayout();
	return UIWidget::getSize();
}

void UILayout::updateLayout() {}

void UILayout::setLayoutDirty() {
	if ( !mDirtyLayout ) {
		mUISceneNode->invalidateLayout( this );
		mDirtyLayout = true;
	}
}

bool UILayout::isGravityOwner() const {
	return mGravityOwner;
}

void UILayout::setGravityOwner( bool gravityOwner ) {
	mGravityOwner = gravityOwner;
}

void UILayout::tryUpdateLayout() {
	if ( mUISceneNode->isUpdatingLayouts() ) {
		updateLayout();
	} else if ( !mDirtyLayout ) {
		setLayoutDirty();
	}
}

void UILayout::updateLayoutTree() {
	updateLayout();

	for ( auto layout : mLayouts ) {
		layout->updateLayoutTree();
	}

	onLayoutUpdate();
}

}} // namespace EE::UI
