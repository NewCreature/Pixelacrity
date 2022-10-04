#!/bin/bash

if (( $EUID != 0 )); then
    echo "This script requires root priveleges."
    exit 0
fi

prefix=${PREFIX=/usr/local}
app_title="T3F_APP_TITLE"
app_name=T3F_APP_NAME
app_dir=T3F_APP_DIR
app_categories=T3F_APP_CATEGORIES

mkdir -p $prefix/share/$app_name
mkdir -p $prefix/share/doc
mkdir -p $prefix/share/doc/$app_name
mkdir -p $prefix/share/icons
mkdir -p $prefix/share/applications
mkdir -p $prefix/share/menu
cp -a bin/data $prefix/share/$app_name
cp docs/changelog $prefix/share/doc/$app_name/changelog
cp docs/README $prefix/share/doc/$app_name/README
cp docs/copyright $prefix/share/doc/$app_name/copyright
cp icons/icon.svg $prefix/share/icons/$app_name.svg
cp bin/$app_name $prefix/$app_dir/$app_name
printf "[Desktop Entry]\nName="$app_title"\nExec=$prefix/$app_dir/$app_name\nIcon=$app_name\nTerminal=false\nType=Application\nCategories=$app_categories;" > $prefix/share/applications/$app_name.desktop
chmod 755 $prefix/$app_dir/$app_name
find $prefix/share/$app_name -type f -exec chmod 644 {} \;
chmod 644 $prefix/share/doc/$app_name/README
chmod 644 $prefix/share/doc/$app_name/changelog
chmod 644 $prefix/share/doc/$app_name/copyright
chmod 644 $prefix/share/icons/$app_name.svg
chmod 644 $prefix/share/applications/$app_name.desktop
update-desktop-database
