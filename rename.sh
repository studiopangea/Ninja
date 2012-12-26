#!/bin/bash

if [ ! "$1" -o ! "$2" ] ; then
    echo "Usage: $0 <Original Name> <New Name>";
    exit
fi

ORIGNAME=$1
NEWNAME=$2

echo Replacing "<$ORIGNAME>" with "<$NEWNAME>"

## RENAME FILES
find proj.* -depth -name "$ORIGNAME*" | while read file; do 
	newfile=$(echo "$file" | sed s/"$ORIGNAME"/"$NEWNAME"/)
	mv "$file" "$newfile";
done

## Search And Replace in files

find proj.* -not -iwholename '*.git*' -type f -print -exec file {} \; | grep text | cut -d ':' -f1 | while read file; do
	echo $file
	sed -i.bak s/"$ORIGNAME"/"$NEWNAME"/g $file #>/dev/null 2>&1
	rm $file.bak >/dev/null 2>&1
done
