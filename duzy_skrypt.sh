#!/bin/bash
#
# Author           : Wiktor Dąbrowski
# Created On       : 11.05.2021
# Last Modified By : Wiktor Dąbrowski
# Last Modified On : 11.05.2021 
# Version          : 1.0
#
# Description      :
# Program służy do wyszukiwania plików mp3 po id tagach oraz zmianie ich id tagów.
#
# Licensed under GPL (see /usr/share/common-licenses/GPL for more details
# or contact # the Free Software Foundation for a copy)

source parameters.rc

. helpers.rc

. program.rc

if [ ! -z $1 ];
then
    while getopts hvf:q OPT
    do
        case $OPT in
            h) help ;;
            v) version ;;
            *) echo "Unknown option" ;;
        esac
    done
else
    program
fi