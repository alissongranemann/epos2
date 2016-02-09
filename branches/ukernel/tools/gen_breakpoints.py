#! /usr/bin/python3
import sys

def has_a_function_of_interest(line, functions_of_interest):
    for function in functions_of_interest:
        assert(function)
        if function in line:
            return True

    return False

def select_breakpoints(functions_of_interest: list, breakpoints: list):
    selected_breakpoints = []
    interesting_function_found = False

    for i in range(len(breakpoints)):
        line = breakpoints[i]
        if '#' in line:
            if has_a_function_of_interest(line, functions_of_interest):
                interesting_function_found = True
                selected_breakpoints.append(line + '\n')
        elif 'break' in line:
            if interesting_function_found:
                selected_breakpoints.append(line + '\n')
            interesting_function_found = False

    return selected_breakpoints


def main(argv):
    if len(argv) < 3:
        print('Usage ./gen_breakpoints functions_of_interest.txt BREAKPOINTS.txt')
        return

    if 'functions_of_interest.txt' not in argv[1]:
        print('functions_of_interest.txt must be provided before BREAKPOINTS.txt and must have that name')
        return

    functions_of_interest_file = open(argv[1])
    functions_of_interest = functions_of_interest_file.read()
    functions_of_interest_file.close()
    functions_of_interest = functions_of_interest.split('\n')
    functions_of_interest = list(filter(None, functions_of_interest))

    breakpoints_file = open(argv[2], 'r')
    breakpoints = breakpoints_file.read()
    breakpoints_file.close()
    breakpoints = breakpoints.split('\n')

    selected_breakpoints = select_breakpoints(functions_of_interest, breakpoints)

    selected_breakpoints_file = open('selected_breakpoints.txt', 'w')
    selected_breakpoints_file.write(''.join(selected_breakpoints))
    selected_breakpoints_file.close()

if __name__ == '__main__':
    main(sys.argv)

