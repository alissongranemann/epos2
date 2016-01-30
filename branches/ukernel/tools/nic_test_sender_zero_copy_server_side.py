#! /usr/bin/python3

# In order to run this program properly, you must be able to execute tcpdump as a non-root user.
# See: http://askubuntu.com/questions/530920/tcpdump-permissions-problem
# for how to accomplish that in Ubuntu.

# Author: Mateus Krepsky Ludwich.
# Contact: mateus@lisha.ufsc.br
# Year: 2015

import subprocess
import sys
sys.path.append('../res')
import nic_test_sender_zero_copy_res


def execute_tcpdump():
    # tcpdump -i eth1 -e -nn -vvvv -XX ether src 00:90:27:9a:3b:97
    pilsener_e100_mac = '00:90:27:9a:3b:97'

    timeout = 60 # in seconds

    tcpdump = ['tcpdump']
    tcpdump.append('-i')
    tcpdump.append('eth1')
    tcpdump.append('-e')
    tcpdump.append('-nn')
    tcpdump.append('-vvvv')
    tcpdump.append('-XX')
    tcpdump.append('ether src ' + pilsener_e100_mac)
    try:
        proc = subprocess.Popen(tcpdump, stdout=subprocess.PIPE, stderr = subprocess.STDOUT, universal_newlines = True)
        proc.wait(timeout)
    except subprocess.TimeoutExpired:
        print('Timed out after ' + str(timeout) + ' seconds.')
        # proc.kill() # Does not work. Output contains less than expected even with high timeout values.
        proc.terminate() # Works fine.

    obtained = proc.stdout.read()

    print(obtained)

    return obtained


def obtained_output():
    return execute_tcpdump()

# def obtained_output():
#    return nic_test_sender_zero_copy_res._obtained_output()


def ignore_white_space_tabs_and_new_lines(s):
    s = s.replace(' ', '')
    s = s.replace('\n', '')
    s = s.replace('\t', '')

    return s


def ignore_tcpdump_timestamp(s: str):
    # Very dummy implementation
    lines = s.split('\n')
    result = ''
    for line in lines:
        if not 'length' in line: # it is a line with timestamp
            result += line

    return result


def check_output(expected: str, obtained: str):
    clean_expected = ignore_tcpdump_timestamp(expected)
    clean_obtained = ignore_tcpdump_timestamp(obtained)

    clean_expected = ignore_white_space_tabs_and_new_lines(clean_expected)
    clean_obtained = ignore_white_space_tabs_and_new_lines(clean_obtained)

    if (clean_expected != clean_obtained):
        print('Failure: outputs differ.')
        print('Expected:')
        print (expected)
        print('Obtained:')
        print (obtained)

    assert(clean_expected == clean_obtained)


def main():
    print('NIC Zero-Copy Sender Application (Server Side)')
    check_output(nic_test_sender_zero_copy_res.expected_output(), obtained_output())
    print('All tests passed.')
    print('Bye!')

if __name__ == '__main__':
    main()

