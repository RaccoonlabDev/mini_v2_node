import numpy as np
import matplotlib.pyplot as plt

def read_results(filename):
    lines = []
    with open(filename, 'r') as f:
        lines = f.readlines()
    return lines

def get_failed_tests(lines):
    failed_tests = []
    for i, line in enumerate(lines):
        if "listed below" in line:
            failed_tests.append(lines[i+1])

    return failed_tests

def get_test_name(line):
    return line.replace('[  FAILED  ]', '').split(' ')[1]

def get_total_tests(lines):
    total_tests = 0
    for line in lines:
        if 'Repeating all tests ' in line:
            total_tests = int(line.replace(")", '').split(' ')[4])
    return total_tests

if __name__ == "__main__":
    res = read_results("results.txt")
    failed_tests = get_failed_tests(res)
    dict_failed_tests = {}
    for test in failed_tests:
        test_name = get_test_name(test)
        if test_name in dict_failed_tests:
            dict_failed_tests[test_name] += 1
        else:
            dict_failed_tests[test_name] = 1
    for key, value in dict_failed_tests.items():
        print(key, value)
    print(get_total_tests(res))
