#!/bin/sh

# set project init variables
ProjectName="MyProjectName"
PROJECTNAME=$(echo ${ProjectName} | tr '[:lower:]' '[:upper:]')
Description="Description of project"
Author="Author"
Today=$(date '+%Y%m%d')
Version="0.1"
Machine="$(uname -s)"

ReplaceItems="\
s,{{ProjectName}},${ProjectName},g;\
s,{{PROJECTNAME}},${PROJECTNAME},g;\
s,{{Description}},${Description},g;\
s,{{Author}},${Author},g;\
s,{{Today}},${Today},g;\
s,{{Version}},${Version},g;\
" 

echo "Rendering file templates"
case "${Machine}" in
    Linux*)  
        find . -type d -name .git -prune -o -type f -print0 | xargs -0 sed -i "${ReplaceItems}"
        ;;
    Darwin*) 
        find . -type d -name .git -prune -o -type f -print0 | LC_ALL=C xargs -0 sed -i "" "${ReplaceItems}"
        ;;
    *)
esac

# rename files
echo "Rendering file names"
mv "src/ProjectName.h" "src/${ProjectName}.h"
mv "src/ProjectNameVersion.h" "src/${ProjectName}Version.h"
mv "src/ProjectName.c" "src/${ProjectName}.c"
mv "tests/ProjectNameTest.c" "tests/${ProjectName}Test.c"
mv "cmake/ProjectNameConfig.cmake" "cmake/${ProjectName}Config.cmake"

echo "All Done"
