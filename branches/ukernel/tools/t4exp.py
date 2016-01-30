# Author: Mateus Krepsky Ludwich.
# Contact: mateus@lisha.ufsc.br
# Year: 2015

class Ignore_white_space_tabs_and_new_lines:

    def apply_filter(self, s):
        s = s.replace(' ', '')
        s = s.replace('\n', '')
        s = s.replace('\t', '')

        return s


class Ignore_tcpdump_timestamp:

    def apply_filter(self, s):
        # Very dummy implementation
        lines = s.split('\n')
        result = ''
        for line in lines:
            if not 'length' in line: # it is a line with timestamp
                result += line

        return result


def check_output(expected: str, obtained: str, filters: list):

    clean_expected = expected
    clean_obtained = obtained

    for filt in filters:
        clean_expected = filt.apply_filter(clean_expected)
        clean_obtained = filt.apply_filter(clean_obtained)

    if (clean_expected != clean_obtained):
        print('Failure: outputs differ.')
        print('Expected:')
        print (expected)
        print('Obtained:')
        print (obtained)

    assert(clean_expected == clean_obtained)
