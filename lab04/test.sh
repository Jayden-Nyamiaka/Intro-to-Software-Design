#!/bin/bash
function do_test() {
    ./main "$1" "$2"
    student=$?
    echo "$2" | grep "^$1$" > /dev/null
    ref=$?
    if [ $student -ne $ref ]; then
        echo "Failed Test: pattern=\"$1\", text=\"$2\"";
        exit 1;
    fi
}

# Write Your Tests Here
# The do_test function tests if your program gives the 
# same result as the reference implementation on the pattern
# and text provided.
do_test 'adam' 'hello'

# Feature 1 Tests:
do_test 'algorithm' 'algorithm'
do_test 'cabababab' 'cabababab'
do_test 'abcdef' 'ab'
do_test 'a' 'aaaaaaaaaaaaaaaaaaaaa'
do_test 'try this' 'no spaces'

# Feature 2 Tests:
do_test 'al.or.th.' 'algorithm'
do_test '....' 'caba'
do_test '.....' 'ab'
do_test '.' 'aaaaa'
do_test '.ye.ig..' 'byetiger'

# Feature 3 Tests:
do_test 'a*' 'aaaaaaaaaaaaaaaaaaaaaaaaaa'
do_test 'keep*' 'keeper'
do_test 'a*ba' 'ba'
do_test 'a*' ''
do_test 'a*aab' 'b'
do_test 'a*aab' 'aab'
do_test 'a*aab' 'aabb'
do_test 'a*aab' 'aaaaaa'
do_test 'a*aab' 'aaaaaaab'
do_test "*a*" "ShouldError"
do_test "O*G*OOOOOO" "OOOOOO"

# Feature 4 Tests:
do_test '.*' ''
do_test '.*' 'a'
do_test '.*' 'abc'
do_test '.*' 'aaaaaaaaaaaaaaaaaaaaaaaaaa'
do_test '.*ab' 'ab'
do_test '.*ab' 'abababab'
do_test '..*..' 'a'
do_test '..*..' 'abc'
do_test '..*..' 'abgdytdyfkuyfydtyfjhjguyh'
do_test ".*z*z*ll.l" "yyll*ll.lyy.ll*l*"
do_test "*...Auu*...*.u.u*.Au.*.uu*.*.*u*" "*.u.Au.u*.*.A.u*uAu.u.Ewujx"


echo '-----------------';
echo 'All Tests Passed!';
