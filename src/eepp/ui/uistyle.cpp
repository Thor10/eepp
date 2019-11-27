#include <eepp/ui/uistyle.hpp>
#include <eepp/ui/uiwidget.hpp>
#include <eepp/ui/uiscenenode.hpp>
#include <eepp/ui/uithememanager.hpp>
#include <eepp/graphics/fontmanager.hpp>

using namespace EE::UI::CSS;

namespace EE { namespace UI {

UIStyle * UIStyle::New( UIWidget * widget ) {
	return eeNew( UIStyle, ( widget ) );
}

UIStyle::UIStyle( UIWidget * widget ) :
	UIState(),
	mWidget( widget ),
	mChangingState( false )
{
	load();
}

UIStyle::~UIStyle() {
	removeRelatedWidgets();
	unsubscribeNonCacheableStyles();
}

bool UIStyle::stateExists( const EE::Uint32&  ) const {
	return true;
}

void UIStyle::setStyleSheetProperty( const StyleSheetProperty& attribute ) {
	if ( attribute.getName() == "padding" ) {
		Rectf rect(  NodeAttribute( attribute.getName(), attribute.getValue() ).asRectf() );
		mElementStyle.setProperty( StyleSheetProperty( "padding-left", String::toStr( rect.Left ), attribute.getSpecificity(), attribute.isVolatile() ) );
		mElementStyle.setProperty( StyleSheetProperty( "padding-right", String::toStr( rect.Right ), attribute.getSpecificity(), attribute.isVolatile() ) );
		mElementStyle.setProperty( StyleSheetProperty( "padding-top", String::toStr( rect.Top ), attribute.getSpecificity(), attribute.isVolatile() ) );
		mElementStyle.setProperty( StyleSheetProperty( "padding-bottom", String::toStr( rect.Bottom ), attribute.getSpecificity(), attribute.isVolatile() ) );
	} else if ( attribute.getName() == "margin" ) {
		Rect rect(  NodeAttribute( attribute.getName(), attribute.getValue() ).asRect() );
		mElementStyle.setProperty( StyleSheetProperty( "margin-left", String::toStr( rect.Left ), attribute.getSpecificity(), attribute.isVolatile() ) );
		mElementStyle.setProperty( StyleSheetProperty( "margin-right", String::toStr( rect.Right ), attribute.getSpecificity(), attribute.isVolatile() ) );
		mElementStyle.setProperty( StyleSheetProperty( "margin-top", String::toStr( rect.Top ), attribute.getSpecificity(), attribute.isVolatile() ) );
		mElementStyle.setProperty( StyleSheetProperty( "margin-bottom", String::toStr( rect.Bottom ), attribute.getSpecificity(), attribute.isVolatile() ) );
	} else {
		mElementStyle.setProperty( attribute );
	}
}

void UIStyle::load() {
	unsubscribeNonCacheableStyles();

	mCacheableStyles.clear();
	mNoncacheableStyles.clear();
	mElementStyle.clearProperties();
	mProperties.clear();

	UISceneNode * uiSceneNode = mWidget->getSceneNode()->isUISceneNode() ? static_cast<UISceneNode*>( mWidget->getSceneNode() ) : NULL;

	if ( NULL != uiSceneNode ) {
		CSS::StyleSheet& styleSheet = uiSceneNode->getStyleSheet();

		if ( !styleSheet.isEmpty() ) {
			StyleSheetStyleVector styles = styleSheet.getElementStyles( mWidget );

			for ( auto& style : styles ) {
				const StyleSheetSelector& selector = style.getSelector();

				if ( selector.isCacheable() ) {
					mCacheableStyles.push_back( style );
				} else {
					mNoncacheableStyles.push_back( style );
				}
			}

			subscribeNonCacheableStyles();
		}
	}
}

void UIStyle::setStyleSheetProperties( const CSS::StyleSheetProperties& properties ) {
	if ( !properties.empty() ) {
		for ( const auto& it : properties ) {
			setStyleSheetProperty( it.second );
		}
	}
}

bool UIStyle::hasTransition( const std::string& propertyName ) {
	return mTransitions.find( propertyName ) != mTransitions.end() || mTransitions.find( "all" ) != mTransitions.end();
}

TransitionDefinition UIStyle::getTransition( const std::string& propertyName ) {
	auto propertyTransitionIt = mTransitions.find( propertyName );

	if ( propertyTransitionIt != mTransitions.end() ) {
		return propertyTransitionIt->second;
	} else if ( ( propertyTransitionIt = mTransitions.find( "all" ) ) != mTransitions.end() ) {
		return propertyTransitionIt->second;
	}

	return TransitionDefinition();
}

const bool& UIStyle::isChangingState() const {
	return mChangingState;
}

void UIStyle::subscribeRelated( UIWidget * widget ) {
	mRelatedWidgets.insert( widget );
}

void UIStyle::unsubscribeRelated( UIWidget * widget ) {
	mRelatedWidgets.erase( widget );
}

void UIStyle::tryApplyStyle( const StyleSheetStyle& style ) {
	if ( style.getSelector().select( mWidget ) ) {
		for ( const auto& prop : style.getProperties() ) {
			const StyleSheetProperty& property = prop.second;
			const auto& it = mProperties.find( property.getName() );

			if ( it == mProperties.end() || property.getSpecificity() >= it->second.getSpecificity() ) {
				mProperties[ property.getName() ] = property;

				if ( String::startsWith( property.getName(), "transition" ) )
					mTransitionAttributes.push_back( property );
			}
		}
	}
}

void UIStyle::onStateChange() {
	if ( NULL != mWidget ) {
		mChangingState = true;

		mProperties.clear();
		mTransitionAttributes.clear();

		tryApplyStyle( mElementStyle );

		for ( auto& style : mCacheableStyles ) {
			tryApplyStyle( style );
		}

		for ( auto& style : mNoncacheableStyles ) {
			tryApplyStyle( style );
		}

		mTransitions = TransitionDefinition::parseTransitionProperties( mTransitionAttributes );

		mWidget->beginAttributesTransaction();

		for ( const auto& prop : mProperties ) {
			const StyleSheetProperty& property = prop.second;

			mWidget->setAttribute( NodeAttribute( property.getName(), property.getValue(), property.isVolatile() ), mCurrentState );
		}

		mWidget->endAttributesTransaction();

		for ( auto& related : mRelatedWidgets ) {
			if ( NULL != related->getUIStyle() ) {
				related->getUIStyle()->onStateChange();
			}
		}

		mChangingState = false;
	}
}

StyleSheetProperty UIStyle::getStatelessStyleSheetProperty( const std::string& propertyName ) const {
	if ( !propertyName.empty() ) {
		if  ( !mElementStyle.getSelector().hasPseudoClasses() ) {
			StyleSheetProperty property = mElementStyle.getPropertyByName( propertyName );

			if ( !property.isEmpty() )
				return property;
		}

		for ( const StyleSheetStyle& style : mCacheableStyles ) {
			if  ( !style.getSelector().hasPseudoClasses() ) {
				StyleSheetProperty property = style.getPropertyByName( propertyName );

				if ( !property.isEmpty() )
					return property;
			}
		}
	}

	return StyleSheetProperty();
}

StyleSheetProperty UIStyle::getStyleSheetProperty( const std::string& propertyName ) const {
	auto propertyIt = mProperties.find( propertyName );

	if ( propertyIt != mProperties.end() )
		return propertyIt->second;

	return StyleSheetProperty();
}

NodeAttribute UIStyle::getNodeAttribute( const std::string& attributeName ) const {
	StyleSheetProperty property( getStyleSheetProperty( attributeName ) );
	return NodeAttribute( property.getName(), property.getValue() );
}

void UIStyle::updateState() {
	for ( int i = StateFlagCount - 1; i >= 0; i-- ) {
		if ( ( mState & getStateFlag(i) ) == getStateFlag(i) ) {
			if ( stateExists( getStateFlag(i) ) ) {
				if ( mCurrentState != getStateFlag(i) ) {
					mPreviousState = mCurrentState;
					mCurrentState = getStateFlag(i);
					break;
				}
			}
		}
	}

	onStateChange();
}

void UIStyle::subscribeNonCacheableStyles() {
	for ( auto& style : mNoncacheableStyles ) {
		std::vector<CSS::StyleSheetElement*> elements = style.getSelector().getRelatedElements( mWidget, false );

		if ( !elements.empty() ) {
			for ( auto& element : elements ) {
				UIWidget * widget = dynamic_cast<UIWidget*>( element );

				if ( NULL != widget && NULL != widget->getUIStyle() ) {
					widget->getUIStyle()->subscribeRelated( mWidget );

					mSubscribedWidgets.insert( widget );
				}
			}
		}
	}
}

void UIStyle::unsubscribeNonCacheableStyles() {
	for ( auto& widget : mSubscribedWidgets ) {
		if ( NULL != widget->getUIStyle() ) {
			widget->getUIStyle()->unsubscribeRelated( mWidget );
		}
	}

	mSubscribedWidgets.clear();
}

void UIStyle::removeFromSubscribedWidgets( UIWidget * widget ) {
	mSubscribedWidgets.erase( widget );
}

void UIStyle::removeRelatedWidgets() {
	for ( auto& widget : mRelatedWidgets ) {
		if ( NULL != widget->getUIStyle() ) {
			widget->getUIStyle()->removeFromSubscribedWidgets( mWidget );
		}
	}

	mRelatedWidgets.clear();
}

}}
