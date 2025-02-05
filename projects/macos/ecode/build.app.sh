#!/bin/sh
SDL2_CONFIG=$(which sdl2-config)
if [ -z $SDL2_CONFIG ]; then
echo "Building using frameworks"
../make.sh config=release ecode || exit
else
echo "Building using sdl2-config"
../make_no_fw.sh config=release ecode || exit
fi
ARCH=$(uname -m)
rm -rf ./ecode.app
mkdir -p ecode.app/Contents/MacOS/
mkdir -p ecode.app/Contents/Resources/
cp ../../../bin/assets/icon/ecode.icns ecode.app/Contents/Resources/ecode.icns
VERSIONPATH=../../../src/tools/ecode/version.hpp
ECODE_MAJOR_VERSION=$(grep "define ECODE_MAJOR_VERSION" $VERSIONPATH | awk '{print $3}')
ECODE_MINOR_VERSION=$(grep "define ECODE_MINOR_VERSION" $VERSIONPATH | awk '{print $3}')
ECODE_PATCH_LEVEL=$(grep "define ECODE_PATCH_LEVEL" $VERSIONPATH | awk '{print $3}')
ECODE_VERSION_STRING="$ECODE_MAJOR_VERSION"."$ECODE_MINOR_VERSION"."$ECODE_PATCH_LEVEL"
cat Info.plist.tpl | sed "s/ECODE_VERSION_STRING/${ECODE_VERSION_STRING}/g" | sed "s/ECODE_MAJOR_VERSION/${ECODE_MAJOR_VERSION}/g"  | sed "s/ECODE_MINOR_VERSION/${ECODE_MINOR_VERSION}/g" > Info.plist
cp Info.plist ecode.app/Contents/
rm Info.plist
chmod +x run.sh
cp run.sh ecode.app/Contents/MacOS
cp ../../../libs/macosx/libeepp.dylib ecode.app/Contents/MacOS
cp ../../../bin/ecode ecode.app/Contents/MacOS

if [ -z $SDL2_CONFIG ]; then
SDL2_LIB_PATH="/Library/SDL2.framework/Versions/A/"
cp "$SDL2_LIB_PATH/SDL2" ecode.app/Contents/MacOS/SDL2
install_name_tool -change @rpath/SDL2.framework/Versions/A/SDL2 @executable_path/SDL2 ecode.app/Contents/MacOS/libeepp.dylib
codesign --force -s - ecode.app/Contents/MacOS/SDL2
install_name_tool -change @rpath/libeepp.dylib @executable_path/libeepp.dylib ecode.app/Contents/MacOS/ecode
else
SDL2_LIB_PATH=$(sdl2-config --libs | awk '{ print $1 }' | cut -b 3-)
cp $SDL2_LIB_PATH/libSDL2-2.0.0.dylib ecode.app/Contents/MacOS
install_name_tool -change $SDL2_LIB_PATH/libSDL2-2.0.0.dylib @executable_path/libSDL2-2.0.0.dylib ecode.app/Contents/MacOS/libeepp.dylib
install_name_tool -change libeepp.dylib @executable_path/libeepp.dylib ecode.app/Contents/MacOS/ecode
fi

#cp -r ../../../bin/assets ecode.app/Contents/MacOS/assets
mkdir -p ecode.app/Contents/MacOS/assets/colorschemes
cp -r ../../../bin/assets/colorschemes/ ecode.app/Contents/MacOS/assets/colorschemes/
#cp -r ../../../bin/assets/fonts ecode.app/Contents/MacOS/assets/
mkdir -p ecode.app/Contents/MacOS/assets/fonts
cp -r ../../../bin/assets/fonts/DejaVuSansMono.ttf ecode.app/Contents/MacOS/assets/fonts/
cp -r ../../../bin/assets/fonts/DejaVuSansMonoNerdFontComplete.ttf ecode.app/Contents/MacOS/assets/fonts/
cp -r ../../../bin/assets/fonts/nonicons.ttf ecode.app/Contents/MacOS/assets/fonts/
cp -r ../../../bin/assets/fonts/codicon.ttf ecode.app/Contents/MacOS/assets/fonts/
cp -r ../../../bin/assets/fonts/NotoSans-Regular.ttf ecode.app/Contents/MacOS/assets/fonts/
cp -r ../../../bin/assets/fonts/remixicon.ttf ecode.app/Contents/MacOS/assets/fonts/
cp -r ../../../bin/assets/fonts/NotoEmoji-Regular.ttf ecode.app/Contents/MacOS/assets/fonts/
cp -r ../../../bin/assets/fonts/NotoColorEmoji.ttf ecode.app/Contents/MacOS/assets/fonts/
cp -r ../../../bin/assets/fonts/DroidSansFallbackFull.ttf ecode.app/Contents/MacOS/assets/fonts/
cp -r ../../../bin/assets/plugins ecode.app/Contents/MacOS/assets/
# cp -r ../../../bin/assets/icon ecode.app/Contents/MacOS/assets/
mkdir -p ecode.app/Contents/MacOS/assets/icon
cp ../../../bin/assets/icon/ecode.png ecode.app/Contents/MacOS/assets/icon
cp ../../../bin/assets/ca-bundle.pem ecode.app/Contents/MacOS/assets/ca-bundle.pem
mkdir ecode.app/Contents/MacOS/assets/ui
cp ../../../bin/assets/ui/breeze.css ecode.app/Contents/MacOS/assets/ui/
