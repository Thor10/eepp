#include <eepp/ui/uilinearlayout.hpp>
#include <eepp/ui/uimessagebox.hpp>
#include <eepp/ui/uiscenenode.hpp>
#include <eepp/ui/uistyle.hpp>
#include <eepp/ui/uitheme.hpp>

namespace EE { namespace UI {

UIMessageBox* UIMessageBox::New( const Type& type, const String& message,
								 const Uint32& windowFlags ) {
	return eeNew( UIMessageBox, ( type, message, windowFlags ) );
}

UIMessageBox::UIMessageBox( const Type& type, const String& message, const Uint32& windowFlags ) :
	UIWindow(), mMsgBoxType( type ), mTextInput( NULL ), mCloseWithKey( KEY_UNKNOWN ) {
	mStyleConfig.WinFlags = windowFlags;

	updateWinFlags();

	mLayoutCont = UILinearLayout::New();
	mLayoutCont->setLayoutSizePolicy( SizePolicy::WrapContent, SizePolicy::WrapContent )
		->setParent( mContainer );

	UILinearLayout* vlay = UILinearLayout::NewVertical();
	vlay->setLayoutSizePolicy( SizePolicy::WrapContent, SizePolicy::WrapContent )
		->setLayoutMargin( Rect( 8, 8, 8, 8 ) )
		->setParent( mLayoutCont )
		->clipDisable();

	mTextBox = UITextView::New();
	mTextBox->setText( message )
		->setLayoutSizePolicy( SizePolicy::WrapContent, SizePolicy::WrapContent )
		->setParent( vlay );

	if ( mMsgBoxType == INPUT ) {
		mTextInput = UITextInput::New();
		mTextInput->setLayoutSizePolicy( SizePolicy::MatchParent, SizePolicy::WrapContent )
			->setLayoutMargin( Rect( 0, 4, 0, 4 ) )
			->setParent( vlay )
			->addEventListener( Event::OnPressEnter, [&]( const Event* ) {
				sendCommonEvent( Event::MsgBoxConfirmClick );
			} );
	}

	UILinearLayout* hlay = UILinearLayout::NewHorizontal();
	hlay->setLayoutMargin( Rect( 0, 8, 0, 0 ) )
		->setLayoutSizePolicy( SizePolicy::WrapContent, SizePolicy::WrapContent )
		->setLayoutGravity( UI_HALIGN_RIGHT | UI_VALIGN_CENTER )
		->setParent( vlay )
		->clipDisable();

	mButtonOK = UIPushButton::New();
	mButtonOK->setSize( 90, 0 )->setParent( hlay );

	mButtonCancel = UIPushButton::New();
	mButtonCancel->setLayoutMargin( Rect( 8, 0, 0, 0 ) )->setSize( 90, 0 )->setParent( hlay );

	switch ( mMsgBoxType ) {
		case UIMessageBox::INPUT:
		case UIMessageBox::OK_CANCEL: {
			mButtonOK->setText(
				getUISceneNode()->getTranslatorString( "@string/msg_box_ok", "Ok" ) );
			mButtonCancel->setText(
				getUISceneNode()->getTranslatorString( "@string/msg_box_cancel", "Cancel" ) );
			break;
		}
		case UIMessageBox::YES_NO: {
			mButtonOK->setText(
				getUISceneNode()->getTranslatorString( "@string/msg_box_yes", "Yes" ) );
			mButtonCancel->setText(
				getUISceneNode()->getTranslatorString( "@string/msg_box_no", "No" ) );
			break;
		}
		case UIMessageBox::RETRY_CANCEL: {
			mButtonOK->setText(
				getUISceneNode()->getTranslatorString( "@string/msg_box_retry", "Retry" ) );
			mButtonCancel->setText(
				getUISceneNode()->getTranslatorString( "@string/msg_box_cancel", "Cancel" ) );
			break;
		}
		case UIMessageBox::OK: {
			mButtonOK->setText(
				getUISceneNode()->getTranslatorString( "@string/msg_box_ok", "Ok" ) );
			mButtonCancel->setVisible( false );
			mButtonCancel->setEnabled( false );
			break;
		}
	}

	reloadStyle( true, true );

	applyDefaultTheme();
}

UIMessageBox::~UIMessageBox() {}

void UIMessageBox::setTheme( UITheme* theme ) {
	UIWindow::setTheme( theme );

	mTextBox->setTheme( theme );
	mButtonOK->setTheme( theme );
	mButtonCancel->setTheme( theme );

	if ( "Retry" != mButtonOK->getText() ) {
		Drawable* okIcon = getUISceneNode()->findIcon( "ok" );
		Drawable* cancelIcon = getUISceneNode()->findIcon( "cancel" );

		if ( NULL != okIcon ) {
			mButtonOK->setIcon( okIcon );
		}

		if ( NULL != cancelIcon ) {
			mButtonCancel->setIcon( cancelIcon );
		}
	}

	onThemeLoaded();
}

Uint32 UIMessageBox::onMessage( const NodeMessage* Msg ) {
	switch ( Msg->getMsg() ) {
		case NodeMessage::Click: {
			if ( Msg->getFlags() & EE_BUTTON_LMASK ) {
				if ( Msg->getSender() == mButtonOK ) {
					sendCommonEvent( Event::MsgBoxConfirmClick );
					closeWindow();
				} else if ( Msg->getSender() == mButtonCancel ) {
					sendCommonEvent( Event::MsgBoxCancelClick );
					closeWindow();
				}
			}

			break;
		}
	}

	return UIWindow::onMessage( Msg );
}

UITextView* UIMessageBox::getTextBox() const {
	return mTextBox;
}

UIPushButton* UIMessageBox::getButtonOK() const {
	return mButtonOK;
}

UIPushButton* UIMessageBox::getButtonCancel() const {
	return mButtonCancel;
}

Uint32 UIMessageBox::onKeyUp( const KeyEvent& event ) {
	if ( mCloseWithKey && event.getKeyCode() == mCloseWithKey &&
		 ( event.getMod() & mCloseWithKey.mod ) ) {
		sendCommonEvent( Event::MsgBoxCancelClick );
		closeWindow();
	}

	return 1;
}

bool UIMessageBox::show() {
	bool b = UIWindow::show();
	if ( NULL != mTextInput ) {
		mTextInput->setFocus();
	} else {
		mButtonOK->setFocus();
	}
	return b;
}

const KeyBindings::Shortcut& UIMessageBox::getCloseWithKey() const {
	return mCloseWithKey;
}

void UIMessageBox::setCloseWithKey( const KeyBindings::Shortcut& closeWithKey ) {
	mCloseWithKey = closeWithKey;
}

UITextInput* UIMessageBox::getTextInput() const {
	return mTextInput;
}

void UIMessageBox::onWindowReady() {
	UIWindow::onWindowReady();
	setMinWindowSize( mLayoutCont->getSize() );
	center();
}

}} // namespace EE::UI
