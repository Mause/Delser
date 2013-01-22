import re
import os
import json
import time
import warnings
from pprint import pprint
# http://www.brandonstaggs.com/2007/07/26/implementing-a-partial-serial-number-verification-system-in-delphi/


class Delser:
    def __init__(self, sequences=None, blacklist=None):

        if not os.path.exists(blacklist or 'blacklist.json'):
            warnings.warn("Could not locate blacklist")
        else:
            with open(blacklist or 'blacklist.json', 'r') as fh:
                self.blacklist = map(unicode.lower, json.load(fh))

        if not sequences:
            sequences = [
                (24, 3, 200),
                (10, 0, 56),
                (15, 2, 91),
                (25, 3, 200)]

        self.sequences = sequences
        for cur_seq in self.sequences:
            assert type(cur_seq) == tuple, 'Incorrect sequence type; %S' % cur_seq
            assert len(cur_seq) == 3, 'Not enough values; "%s"' % cur_seq

        self.status = {
            'KEY_GOOD': 0,
            'KEY_INVALID': 1,
            'KEY_BLACKLISTED': 2,
            'KEY_PHONY': 3,
            'KEY_BADCHECKSUM': 4}
        self.inverse_status = dict(zip(self.status.values(), self.status.keys()))

        self._cached_checksums = {}
        self._cached_key_bytes = {}

    def _get_key_byte(self, seed, a, b, c):
        if (seed, a, b, c) not in self._cached_key_bytes:
            a = a % 25
            b = b % 3
            if a % 2 == 0:
                result = ((seed << a) & 0x000000FF) ^ ((seed << b) | c)
            else:
                result = ((seed << a) & 0x000000FF) ^ ((seed << b) & c)
            self._cached_key_bytes[(seed, a, b, c)] = result
        else:
            result = self._cached_key_bytes[(seed, a, b, c)]
        # print 'res', result,
        return result

    def get_checksum(self, string):
        if string not in self._cached_checksums:
            i = 0
            left = 0x0056
            right = 0x00AF
            if len(string) > 0:
                for i in range(len(string)):
                    right = right + ord(string[i])
                    if right > 0x00FF:
                        right -= 0x00FF
                    left += right
                    if left > 0x00FF:
                        left -= 0x00FF
            result = hex((left >> 8) + right)[2:].rjust(4, '0')
            self._cached_checksums[string] = result
        else:
            result = self._cached_checksums[string]
        return result

    def make_key(self, seed):
        result = ''
        # Fill keybytes with values derived from seed.
        # The parameters used here must be exactly the same
        # as the ones used in the check_key function.
        # A real key system should use more than four bytes.

        # the key string begins with a hexadecimal string of the seed
        result = hex(seed)[2:].rjust(8, '0')
        # print 'Seed', seed, 'first part;', result

        # then is followed by hexadecimal strings of each byte in the key
        for cur_seq in self.sequences:
            cur_byte = hex(self._get_key_byte(seed, *cur_seq) / 2)[2:].upper().replace('L', '').rjust(2, '0')
            result += cur_byte
            # print 'cu', cur_byte, cur_seq,
        # print

        # add checksum to key string
        checksum = self.get_checksum(result)
        # print 'Checksum calculated against;', result, result + checksum, 'checksum;', checksum
        result += str(checksum)

        end_result = '-'.join(re.findall('....', result))

        print end_result, result, seed
        # make sure nothing got discarded by the regex
        assert len(end_result.replace('-', '')) == len(result), 'Some characters were discarded by the regex'

        return end_result

    def check_key_checksum(self, key):
        # remove cosmetic hyphens and normalize case
        s = key.split('-')

        if len(s) not in [4, 5, 6]:
            print 'wrong number of sections;', len(s)
            return self.status['KEY_INVALID']  # Our keys are always 20 characters long (?)
        # last four characters are the checksum
        grabbed_checksum = s[-1]
        s = ''.join(s[:-1])
        # print 'grabbed;', c, 'recalculated;', PKV_GetChecksum(s), s, key
        # compare the supplied checksum against the real checksum for
        # the key string.

        result = bool(int(grabbed_checksum, 16) == int(self.get_checksum(s), 16))

        if not result:
            # print 'Incorrect checksum; %s is not equal to %s' % (grabbed_checksum, self.get_checksum(s))
            return self.status['KEY_BADCHECKSUM']

        return self.status['KEY_GOOD']

    def check_key(self, key):
        # test against blacklist
        if len(self.blacklist) > 0:
            if key.lower() in self.blacklist:
                return self.status['KEY_BLACKLISTED']

        checksum_status = self.check_key_checksum(key)
        if checksum_status != self.status['KEY_GOOD']:
            # bad checksum or wrong number of characters
            return checksum_status

        # remove cosmetic hyphens and normalize case
        key = key.replace('-', '').upper()

        # At this point, the key is either valid or forged,
        # because a forged key can have a valid checksum.
        # We now test the "bytes" of the key to determine if it is
        # actually valid.

        # extract the seed from the supplied key string
        try:
            seed = int('0x' + key[0:8], 16)
        except ValueError:
            print 'Not hex', repr(key)
            return self.status['KEY_PHONY']

        byte_check = 0
        assert byte_check in range(len(self.sequences))

        selected_sequence = self.sequences[byte_check]

        key = key.replace('L', '')
        kb = key[8:10]
        # b = hex(self._get_key_byte(seed, *selected_sequence))[2:].upper().replace('L', '').rjust(2, '0')
        b = hex(self._get_key_byte(seed, *selected_sequence) / 2)[2:].upper().replace('L', '').rjust(2, '0')
        # print key, kb, b
        if kb != b:
            return self.status['KEY_PHONY']

        # If we get this far, then it means the key is either good, or was made
        # with a keygen derived from "this" release.
        return self.status['KEY_GOOD']


def main():
    cur_delser = Delser()

    # space = (0, 524288 * 2)
    space = (512, 513)
    # space = (4096, 4096 + 3)
    # space = (65536, 65536 + 3)
    # space = (524288, 524288 + 3)
    assert space[0] >= 0, 'the lower bound for the key must not be below zero'
    assert space[1] - space[0]

    start_time = time.time()
    to_test = [
        (cur_delser.make_key(y), y)
        for y in range(*space)]
    print 'Took', time.time() - start_time, 'to generate', len(to_test), 'key(s)'

    # to_test.append(('bfghkjdhkhkl', 'key'))
    # to_test.append(('0000-1000-10C8-1038-00de', 'key'))
    # to_test.append(('0000-1001-10C8-1039-00dd', 'key'))
    # to_test.append(('1002-10CA-103A-00ef', 'key'))
    # to_test = set(to_test)
    if not len(to_test) > 15:
        pprint(to_test)

    if len(to_test) >= (space[1] - space[0]):
        print 'keys appear to have been generated as expected'
    else:
        print 'Not enough keys!'

    bad = []
    for test in to_test:
        stat = cur_delser.check_key(test[0])
        if stat != cur_delser.status['KEY_GOOD']:
            print 'For the key', test[0], 'with length', len(test[0]), 'the status was', cur_delser.inverse_status[stat], test
            if test not in bad:
                bad.append(test)
            print '\n'

    print len(bad), 'bad keys'


if __name__ == '__main__':
    main()
