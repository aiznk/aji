items=(`./build/aji tests/tests.aji list`)

echo "" > pmm.log

for item in "${items[@]}"; do
    items2=(`./build/aji tests/tests.aji "$item" list`)
    for item2 in "${items2[@]}"; do
        echo "$item $item2" >> pmm.log
        echo "-------------------" >> pmm.log
        valgrind --leak-check=full ./build/aji tests/tests.aji "$item" "$item2" >> pmm.log 2>&1
        echo "" >> pmm.log
    done
done