#!/bin/bash

function run_test {

    # parameters
		EXE="../../cgi-bin/$1"
    TAG=$2
		ENVVAR=$3

    # file paths
    OUTPUT=outputs/$TAG.txt
    DIFF=outputs/$TAG.diff
    EXPECT=expected/$TAG.txt

		if [ -n "$ENVVAR" ] ; then
			ENVV=$(echo $ENVVAR | awk '{ print $1 }')
			ENVP=$(echo $ENVVAR | awk '{ print $2 }')
			export "${ENVV}=${ENVP}"
		fi

    # run test and compare output to the expected version
    $EXE 2>/dev/null >"$OUTPUT"

		if [ -n "$ENVVAR" ] ; then
			ENVV=$(echo $ENVVAR | awk '{ print $1 }')
			unset $ENVV
		fi

    diff -u "$OUTPUT" "$EXPECT" >"$DIFF"
    PTAG=$(printf '%-30s' "$TAG")
    if [ -s "$DIFF" ]; then

        # try alternative solution (if it exists)
        EXPECT=expected/$TAG-2.txt
        if [ -e "$EXPECT" ]; then
            diff -u "$OUTPUT" "$EXPECT" >"$DIFF"
            if [ -s "$DIFF" ]; then
                echo "$PTAG FAIL (see $DIFF for details)"
            else
                echo "$PTAG pass"
            fi
        else
            echo "$PTAG FAIL (see $DIFF for details)"
        fi
    else
        echo "$PTAG pass"
    fi
}

# initialize output folders
mkdir -p outputs
rm -f outputs/*

# run individual tests
source itests.include

