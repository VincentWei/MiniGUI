#
macro (mg_sort_option_group _group)
    foreach (i ${_group_${_group}}) 
        set (_name ${_group}_${i})
        # message (STATUS "_name=${_name} value of _name=${${_name}}")
        list (APPEND _group_${_group}_${${_name}}_ ${i})
    endforeach (i)
endmacro (mg_sort_option_group)

# 
macro (mg_declare_option _option _type _default _description)
    # message ("declare ${_option}")
    if (${_type} STREQUAL "BOOL")
        option (${_option} "${_description} <default=${_default}>" ${_default})
    elseif (${_type} STREQUAL "STRING")
        set (${_option} "${_default}" CACHE STRING "${_description} <default=${_default}>")
    elseif (${_type} STREQUAL "PATH")
        set (${_option} "${_default}" CACHE PATH "${_description} <default=${_default}>")
    else ()
        message (FATAL_ERROR "wrong type of declare_option: it should be BOOL, STRING, or PATH")
    endif (${_type} STREQUAL "BOOL")
    # put define in one of the output groud
    string (REGEX MATCH "(.*)_(.*)" _not_used ${_option})
    if (NOT CMAKE_MATCH_0)
        message(FATAL_ERROR me=misc_havetracemsg
            -- name=misc_havemsgstr
            -- name=misc_havemenu
            "bad option name: '${_option}'")
    else ()
        set (_group "_group_${CMAKE_MATCH_1}")
        list (APPEND ${_group} ${CMAKE_MATCH_2})
    endif (NOT CMAKE_MATCH_0)
endmacro (mg_declare_option)

# 
macro (mg_set_variables_on)
    foreach (_i ${ARGV})
        set (${_i} ON)
    endforeach (_i)
endmacro (mg_set_variables_on)

# 
macro (mg_set_value _value _option)
    #check option
    set (_conditions_is_true TRUE)

    string (REGEX MATCH "([^;]+).(.*)" _not_use "${ARGN}")
    if (CMAKE_MATCH_0)
        # message ("${CMAKE_MATCH_1} ${CMAKE_MATCH_2}")
        if (CMAKE_MATCH_1 STREQUAL "IF")
            if (${CMAKE_MATCH_2})
                # message ("set TRUE " "${CMAKE_MATCH_2}")
            else ()
                set (_conditions_is_true FALSE)
                # message ("set FALSE " "${CMAKE_MATCH_2}")
            endif (${CMAKE_MATCH_2})
        else ()
            message (FATAL_ERROR "common.cmake:58 > wrong argument 3 in mg_set_value")
        endif (CMAKE_MATCH_1 STREQUAL "IF")
    endif (CMAKE_MATCH_0)

    if (_conditions_is_true)
        # set the _option of this _value
        if (${_option} STREQUAL ON)
            set (${_value} ON)
            #message (STATUS "${_value} is ON")
        elseif (${_option} STREQUAL OFF)
            set (${_value} OFF)
            #message (STATUS "${_value} is OFF")
        else ()
            set (${_value} ${_option})
            #message (STATUS "${_value} is ${_option}")
        endif (${_option} STREQUAL ON)
    endif (_conditions_is_true)
endmacro (mg_set_value)

# 
macro (mg_declare_option_and_set_value _value _option _type _default _description)
    mg_declare_option (${_option} ${_type} ${_default} ${_description})
    mg_set_value (${_value} ${_option})
endmacro (mg_declare_option_and_set_value)

#
macro (mg_declare_option_and_set_value_with_compile _code _value _option _type _default _description)
    mg_declare_option (${_option} ${_type} ${_default} ${_description})
    if (${_option}) 
        include (CheckCSourceCompiles)
        CHECK_C_SOURCE_COMPILES ("${_code}" _compile_ret)
        if (_compile_ret)
            mg_set_value (${_value} ON)
            set (${_option} ON)
        else ()
            mg_set_value (${_value} OFF)
            set (${_option} OFF)
        endif(_compile_ret)
    endif (${_option})
endmacro (mg_declare_option_and_set_value_with_compile)

#
macro (mg_set_number_select_value _value _option)
    if (${ARGC} LESS 3)
        message (FATAL_ERROR "no number list found for ${_option}")
    else ()
        set (${_value} ${ARGV2})
        foreach (_i ${ARGN})
            if (${_option} STREQUAL ${_i})
                mg_set_value (${_value} ${_i})
                break ()
            endif (${_option} STREQUAL ${_i})
        endforeach (_i ${ARGN})
    endif (${ARGC} LESS 3)
endmacro (mg_set_number_select_value)

#
macro (mg_set_conditional _condition _option _value)
    if (${_option} STREQUAL ${_value})
        set (${_condition} ON)
        # message (STATUS "conditional ${_condition} is set")
    endif (${_option} STREQUAL ${_value})
endmacro (mg_set_conditional)

#print list
macro (mg_print_list)
    set (_line "   ")
    if (${ARGC} GREATER 0)
        foreach (_i ${ARGV})
            string (LENGTH ${_line} _length)
            if (_length GREATER 80)
                message (${_line})
                set (_line "   ")
            endif (_length GREATER 80)
            set (_line "${_line} ${_i}")
        endforeach (_i)
        message (${_line})
    else ()
        message (${_line}(null))
    endif (${ARGC} GREATER 0)
    message ("")
endmacro (mg_print_list)

# macros to deal with source tree
macro(mg_add_sub_source_dirs)
    set (_parent_source_list_ ${_source_list_})

    foreach (_i ${ARGV})
        set (_source_list_ "")

        add_subdirectory (${_i})

        if (_source_list_)
            foreach (_file ${_source_list_})
                list (APPEND _parent_source_list_ "${_i}/${_file}")
            endforeach (_file)
        endif (_source_list_)
    endforeach (_i ${ARGV})

    set (_source_list_ ${_parent_source_list_})
    set (_source_list_ ${_source_list_} PARENT_SCOPE)
endmacro(mg_add_sub_source_dirs)

# add sub source dir with condition
macro(mg_add_sub_dir_with_condition _dir _condition)
    if (${_condition})
        mg_add_sub_source_dirs (${_dir})
    endif (${_condition})
endmacro(mg_add_sub_dir_with_condition)


# add one file
macro(mg_add_source_files)
    list (APPEND _source_list_ ${ARGV})
endmacro(mg_add_source_files)

# add all files
macro(mg_add_all_source_files)
    set (_source_postfix "*.[ch]" "*.cc" "*.hh" "*.[ch]pp" "*.[ch]xx")
    # There are more than once 'file' command invoked because the
    # globbing expressions are similar to regular expressions, 
    # but much simpler, so it can't supports compound regex sentence.
    set (_file_list "")
    foreach (_postfix ${_source_postfix})
        file (GLOB _file_list_sub_ RELATIVE ${CMAKE_CURRENT_SOURCE_DIR} ${_postfix})
        list (APPEND _file_list ${_file_list_sub_})
    endforeach (_postfix)
    #set (_source_list_ ${_file_list})
    list (APPEND _source_list_ ${_file_list})
    #message(STATUS " " ${CMAKE_CURRENT_SOURCE_DIR} " : "${_source_list_})
endmacro(mg_add_all_source_files)

# 
macro(mg_commit_source_files)
    set (_source_list_ ${_source_list_} PARENT_SCOPE)
endmacro(mg_commit_source_files)
