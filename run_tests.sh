#!/bin/sh
./mysort test_case1.txt -o out.txt
cmp --silent out.txt test_soln1.txt && echo "pass (5pts)" || echo "Fail (0pts)"
./mysort test_case2.txt -o out.txt
cmp --silent out.txt test_soln2.txt && echo "pass (5pts)" || echo "Fail (0pts)"
./mysort test_case3.txt -o out.txt
cmp --silent out.txt test_soln3.txt && echo "pass (5pts)" || echo "Fail (0pts)"
./mysort test_case4.txt -o out.txt
cmp --silent out.txt test_soln4.txt && echo "pass (5pts)" || echo "Fail (0pts)"
./mysort test_case5.txt -o out.txt
cmp --silent out.txt test_soln5.txt && echo "pass (5pts)" || echo "Fail (0pts)"
./mysort test_case6.txt -o out.txt
cmp --silent out.txt test_soln6.txt && echo "pass (5pts)" || echo "Fail (0pts)"
./mysort test_case7.txt -o out.txt
cmp --silent out.txt test_soln7.txt && echo "pass (5pts)" || echo "Fail (0pts)"
./mysort test_case8.txt -o out.txt
cmp --silent out.txt test_soln8.txt && echo "pass (5pts)" || echo "Fail (0pts)"
./mysort test_case9.txt -o out.txt
cmp --silent out.txt test_soln9.txt && echo "pass (5pts)" || echo "Fail (0pts)"
./mysort test_case10.txt -o out.txt
cmp --silent out.txt test_soln10.txt && echo "pass (5pts)" || echo "Fail (0pts)"
./mysort test_case11.txt -o out.txt
cmp --silent out.txt test_soln11.txt && echo "pass (5pts)" || echo "Fail (0pts)"
./mysort test_case12.txt -o out.txt
cmp --silent out.txt test_soln12.txt && echo "pass (5pts)" || echo "Fail (0pts)"
./mysort test_case13.txt -o out.txt
cmp --silent out.txt test_soln13.txt && echo "pass (5pts)" || echo "Fail (0pts)"
./mysort test_case14.txt -o out.txt
cmp --silent out.txt test_soln14.txt && echo "pass (5pts)" || echo "Fail (0pts)"
./mysort test_case15.txt -o out.txt
cmp --silent out.txt test_soln15.txt && echo "pass (5pts)" || echo "Fail (0pts)"
./mysort test_case16.txt -o out.txt
cmp --silent out.txt test_soln16.txt && echo "pass (5pts)" || echo "Fail (0pts)"