if [ "$3" = "full" ]; then
    make clean && make init && make && valgrind --leak-check=full --num-callers=100 ./build/aji_tests $1 $2
elif [ "$3" = "make" ]; then
    make && valgrind --leak-check=full --num-callers=100 ./build/aji_tests $1 $2
else
    valgrind --leak-check=full --num-callers=100 ./build/aji_tests $1 $2
fi

