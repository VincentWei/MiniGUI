#! /bin/bash

keyword_list="
license_create
license_destroy
screensaver_create
screensaver_destroy
splash_draw_framework
splash_progress
splash_delay
license_on_input
license_on_timeout
license_get_processor_id
#\s*include.*license\/c_files.*
#\s*include.*license\.h
LICENSE_MODIFY_MESSAGE
LICENSE_SET_MESSAGE_OFFSET
LICENSE_CHECK_CUSTIMER_ID
g_license_processor_id
"

license_files="
src/include/license.h
src/misc/license.c
"

function main ()
{
    tarball_name="$1"

    # get target packet
    while [ "$tarball_name" = "" -o ! -f "$tarball_name" ]; do
        echo "Not found the file or make a wrong name: $tarball_name."
        echo -n "Please input tarball name, or 'q' to exit: " 
        read tarball_name

        if [ "x$tarball_name" = "xq" ];then exit 1; fi
    done

    #get folder name
    tmp_dir=${tarball_name%.*.*}

    #decompress packet for clean
    echo "Decompresing..."
    /bin/tar xf $tarball_name

    #empty license file
    for file_name in $license_files;do
        > $tmp_dir/$file_name
    done
    #remove resource files for license
    rm -rf $tmp_dir/src/sysres/license

    #enter
    #file_list=`find $tmp_dir -name \*.c`

    #find string and clean
    echo "Cleaning files..."
    #for file_name in $file_list; do
    #    for keyword in $keyword_list; do
    #        #sed "s/^\s*$keyword.*\s*//g"  -i $file_name
    #        sed "/$keyword/d" -i $file_name
    #    done
    #done
    for keyword in $keyword_list; do
        file_list=`find $tmp_dir -name \*.c|xargs grep -l $keyword`
        for file_name in $file_list; do
            if [[ $keyword == \#* ]]
            then
                sed "/$keyword/d" -i $file_name
            else
                sed "/$keyword/d" -i $file_name
                #TODO:when the function \n
                #sed "/^\s*$keyword.*\s*/,/.*\;/d" -i $file_name
            fi
        done
    done
    #make new packet
    /bin/rm -rf "$tarball_name"
    echo "Making new tarball..."
    /bin/tar zcf $tarball_name $tmp_dir

    /bin/rm -rf "$tmp_dir"
}
main $*

