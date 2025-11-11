# run test until it fails

set -e
# add iterator to make sure it will run at least once
i=0
while [ $i -lt 10 ]; do
    ./build/Tests/fft --gtest_repeat=5000 --disable-randomization &> results.txt
    # check if putput has "FAILED" in it
    if grep -q "FAILED" results.txt; then
        echo "Test failed"
        break
    fi

    # if [ $? -ne 0 ]; then
        # echo "Test failed"
        # break
    # fi
    i=$((i+1))
done

echo "Tests finished"
echo "Number of tests: $i"
