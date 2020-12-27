#ifndef FILESYSTEMLISTENER_HPP
#define FILESYSTEMLISTENER_HPP

#include <eepp/system/fileinfo.hpp>
#include <eepp/ui/models/filesystemmodel.hpp>
#include <eepp/ui/tools/uicodeeditorsplitter.hpp>
#include <eepp/ui/uicodeeditor.hpp>
#include <efsw/efsw.hpp>

using namespace EE::System;
using namespace EE::UI;
using namespace EE::UI::Models;
using namespace EE::UI::Tools;

class FileSystemListener : public efsw::FileWatchListener {
  public:
	FileSystemListener( UICodeEditorSplitter* codeSplitter,
						std::shared_ptr<FileSystemModel> fileSystemModel );

	virtual ~FileSystemListener() {}

	void handleFileAction( efsw::WatchID, const std::string& dir, const std::string& filename,
						   efsw::Action action, std::string );

	void setFileSystemModel( std::shared_ptr<FileSystemModel> model ) { mFileSystemModel = model; }

  protected:
	UICodeEditorSplitter* mSplitter;
	std::shared_ptr<FileSystemModel> mFileSystemModel;

	bool isFileOpen( const FileInfo& file );

	void notifyChange( const FileInfo& file );
};

#endif // FILESYSTEMLISTENER_HPP
