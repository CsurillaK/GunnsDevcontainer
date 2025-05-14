print_usage() {
    echo "Usage: $0 <no_trick/test/trick/trick_if> <make_settings>"
    echo "Arguments:"
    echo "  <no_trick/test/trick/trick_if>  Specify the library to compile:"
    echo "    - no_trick: /gunns/lib/no_trick"
    echo "    - test:     /gunns/lib/test"
    echo "    - trick:    /gunns/lib/trick"
    echo "    - trick_if: /gunns/lib/trick_if"
    echo "  <make_settings>                Additional make settings (optional)"
}

if [ -z "$1" ]; then
    echo "Error: No library specified."
    print_usage
    exit 1
fi

library_location=""
case "$1" in
    no_trick)
        library_location="./lib/no_trick"
        ;;
    test)
        library_location="./lib/test"
        ;;
    trick)
        library_location="./lib/trick"
        ;;
    trick_if)
        library_location="./lib/trick_if"
        ;;
    *)
        echo "Error: Invalid library specified."
        print_usage
        exit 1
        ;;
esac

# For detailed description of compilation arguments, please refer to makefiles
makefile_arguments="${@:2}"

clean_libraries() {
    cd $GUNNS_HOME
    cd ./lib/no_trick
    make clean
    cd ../test
    make clean
    cd ../trick
    make clean
    cd ../trick_if
    make clean
}

build_library() {
    clean_libraries
    
    cd $GUNNS_HOME
    cd $library_location
    if [ -z "$makefile_arguments" ]; then
        make
    else
        make $makefile_arguments
    fi
}

# Build the specified library
build_library