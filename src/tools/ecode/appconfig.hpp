#ifndef ECODE_APPCONFIG_HPP
#define ECODE_APPCONFIG_HPP

#include <eepp/config.hpp>
#include <eepp/math/size.hpp>
#include <eepp/system/inifile.hpp>
#include <eepp/ui/css/stylesheetlength.hpp>
#include <eepp/ui/tools/uicodeeditorsplitter.hpp>
#include <eepp/window/window.hpp>

#include <nlohmann/json.hpp>
using json = nlohmann::json;

using namespace EE;
using namespace EE::Math;
using namespace EE::UI;
using namespace EE::UI::CSS;
using namespace EE::UI::Tools;
using namespace EE::System;
using namespace EE::Window;

namespace ecode {
class App;
class PluginManager;

enum class PanelPosition { Left, Right };

struct UIConfig {
	StyleSheetLength fontSize{ 11, StyleSheetLength::Dp };
	StyleSheetLength panelFontSize{ 11, StyleSheetLength::Dp };
	bool showSidePanel{ true };
	PanelPosition panelPosition{ PanelPosition::Left };
	std::string serifFont;
	std::string monospaceFont;
	std::string terminalFont;
	std::string fallbackFont;
	ColorSchemePreference colorScheme{ ColorSchemePreference::Dark };
	std::string theme;
};

struct WindowStateConfig {
	Float pixelDensity{ 0 };
	Sizei size{ 1280, 720 };
	std::string winIcon;
	bool maximized{ false };
	std::string panelPartition;
	int displayIndex{ 0 };
	Vector2i position{ -1, -1 };
};

struct CodeEditorConfig {
	std::string colorScheme{ "ecode" };
	StyleSheetLength fontSize{ 11, StyleSheetLength::Dp };
	StyleSheetLength lineSpacing{ 0, StyleSheetLength::Dp };
	bool showLineNumbers{ true };
	bool showWhiteSpaces{ true };
	bool showLineEndings{ false };
	bool showIndentationGuides{ false };
	bool highlightMatchingBracket{ true };
	bool verticalScrollbar{ true };
	bool horizontalScrollbar{ true };
	bool highlightCurrentLine{ true };
	bool highlightSelectionMatch{ true };
	bool colorPickerSelection{ false };
	bool colorPreview{ false };
	bool minimap{ true };
	bool showDocInfo{ true };
	bool hideTabBarOnSingleTab{ true };
	bool singleClickTreeNavigation{ false };
	bool syncProjectTreeWithEditor{ true };
	bool autoCloseXMLTags{ true };
	bool linesRelativePosition{ false };
	std::string autoCloseBrackets{ "" };
	Time cursorBlinkingTime{ Seconds( 0.5f ) };
};

struct DocumentConfig {
	bool trimTrailingWhitespaces{ false };
	bool forceNewLineAtEndOfFile{ false };
	bool autoDetectIndentType{ true };
	bool writeUnicodeBOM{ false };
	bool indentSpaces{ false };
	TextDocument::LineEnding lineEndings{ TextDocument::LineEnding::LF };
	int indentWidth{ 4 };
	int tabWidth{ 4 };
	int lineBreakingColumn{ 100 };
};

struct SearchBarConfig {
	bool caseSensitive{ false };
	bool luaPattern{ false };
	bool wholeWord{ false };
	bool escapeSequence{ false };
};

struct GlobalSearchBarConfig {
	bool caseSensitive{ false };
	bool luaPattern{ false };
	bool wholeWord{ false };
	bool escapeSequence{ false };
};

struct ProjectDocumentConfig {
	bool useGlobalSettings{ true };
	DocumentConfig doc;
	ProjectDocumentConfig() {}
	ProjectDocumentConfig( const DocumentConfig& doc ) { this->doc = doc; }
};

class NewTerminalOrientation {
  public:
	enum Orientation { Same, Vertical, Horizontal };

	static NewTerminalOrientation::Orientation fromString( const std::string& orientation ) {
		if ( "same" == orientation )
			return Orientation::Same;
		if ( "horizontal" == orientation )
			return Orientation::Horizontal;
		return Orientation::Vertical;
	}

	static std::string toString( const Orientation& orientation ) {
		switch ( orientation ) {
			case Orientation::Vertical:
				return "vertical";
			case Orientation::Horizontal:
				return "horizontal";
			case Orientation::Same:
			default:
				return "same";
		}
	}
};

struct TerminalConfig {
	std::string shell;
	std::string colorScheme{ "eterm" };
	StyleSheetLength fontSize{ 11, StyleSheetLength::Dp };
	NewTerminalOrientation::Orientation newTerminalOrientation{
		NewTerminalOrientation::Horizontal };
};

struct WorkspaceConfig {
	bool restoreLastSession{ false };
	bool checkForUpdatesAtStartup{ false };
};

class AppConfig {
  public:
	WindowStateConfig windowState;
	ContextSettings context;
	CodeEditorConfig editor;
	DocumentConfig doc;
	TerminalConfig term;
	std::map<std::string, bool> pluginsConfig;
	UIConfig ui;
	IniFile ini;
	IniFile iniState;
	FileInfo iniInfo;
	SearchBarConfig searchBarConfig;
	GlobalSearchBarConfig globalSearchBarConfig;
	WorkspaceConfig workspace;

	void load( const std::string& confPath, std::string& keybindingsPath,
			   std::string& initColorScheme, std::vector<std::string>& recentFiles,
			   std::vector<std::string>& recentFolders, const std::string& resPath,
			   PluginManager* pluginManager, const Sizei& displaySize, bool sync );

	void save( const std::vector<std::string>& recentFiles,
			   const std::vector<std::string>& recentFolders, const std::string& panelPartition,
			   EE::Window::Window* win, const std::string& colorSchemeName,
			   const SearchBarConfig& searchBarConfig,
			   const GlobalSearchBarConfig& globalSearchBarConfig, PluginManager* pluginManager );

	void saveProject( std::string projectFolder, UICodeEditorSplitter* editorSplitter,
					  const std::string& configPath, const ProjectDocumentConfig& docConfig );

	void loadProject( std::string projectFolder, UICodeEditorSplitter* editorSplitter,
					  const std::string& configPath, ProjectDocumentConfig& docConfig,
					  std::shared_ptr<ThreadPool> pool, ecode::App* app );

  protected:
	Int64 editorsToLoad{ 0 };

	void loadDocuments( UICodeEditorSplitter* editorSplitter, std::shared_ptr<ThreadPool> pool,
						json j, UITabWidget* curTabWidget, ecode::App* app );

	void editorLoadedCounter( ecode::App* app );
};

} // namespace ecode

#endif // ECODE_APPCONFIG_HPP
