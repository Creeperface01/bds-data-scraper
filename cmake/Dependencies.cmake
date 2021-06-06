include(FetchContent)

macro(load_dep git_url)
    FetchContent_Declare(
            git_dep
            GIT_REPOSITORY ${git_url}
    )
    FetchContent_GetProperties(git_dep)

    if (NOT git_dep_POPULATED)
        FetchContent_Populate(git_dep)
        add_subdirectory(${git_dep_SOURCE_DIR})
    endif ()
endmacro()

## Load BDS Symbols
load_dep(https://github.com/BDS-API/symbols)