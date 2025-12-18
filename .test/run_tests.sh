# Positive tests:
pos_failed=0
declare -a positive_tests=(
	"test_positive_case"
);
for name in positive_tests 
do
	eclipse -noSplash -data /tmp/workspace -application com.ti.ccstudio.apps.projectImport -ccs.location "/work/.test/$name"
    eclipse -noSplash -data /tmp/workspace -application com.ti.ccstudio.apps.projectBuild -ccs.projects "$name" -ccs.configuration "Debug"
    
	if [[ $? != 0 ]]; then
		>&2 echo "[ERR] Positive test failed"
		((pos_failed++))
		continue
	fi
	>&2 echo "[OK ] Positive test success"
done
if [[ $pos_failed == 0 ]]; then
	>&2 echo "[OK ] All positive tests passed."
else
	>&2 echo "[ERR] $pos_failed positive tests failed."
fi
# Negative tests:
neg_failed=0
declare -a negative_tests=(
	"negative_test_1" "Error #1234"
	"negative_test_2" "Error #5678"
);
len=$(( ${#negative_tests[@]} / 2 ))
echo "Negative len: $len"
for i in 0 .. $len
do
	name=negative_tests[i+0]
	err=negative_tests[i+1]
	rm -f build.log
	
    eclipse -noSplash -data /tmp/workspace -application com.ti.ccstudio.apps.projectImport -ccs.location "/work/.test/$name"
    eclipse -noSplash -data /tmp/workspace -application com.ti.ccstudio.apps.projectBuild -ccs.projects "$name" -ccs.configuration "Debug" | tee build.log
	
	if [[ ${PIPESTATUS[0]} == 0 ]]; then
		>&2 echo "[ERR] Negative test did not fail"
		((neg_failed++))
		continue
	fi
	
	if grep -q $err build.log; then
		>&2 echo "[OK ] Negative test failed with expected error"
	else 
		>&2 echo "[ERR] Negative test failed with unexpected error"
		((neg_failed++))
	fi
done
if [[ $neg_failed == 0 ]]; then
	>&2 echo "[OK ] All negative tests passed."
else
	>&2 echo "[ERR] $neg_failed negative tests failed."
fi
exit $(($pos_failed + $neg_failed))
