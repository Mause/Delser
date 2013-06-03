"""
http://www.brandonstaggs.com/2007/07/26/implementing-a-partial-serial-number-verification-system-in-delphi/
"""
import os
import json
import time
import warnings


class Delser(object):
    def __init__(self, byte_to_check=1, sequences=None, blacklist=None):

        if not os.path.exists(blacklist or 'blacklist.json'):
            warnings.warn("Could not locate blacklist")
            self.blacklist = set()
        else:
            with open(blacklist or 'blacklist.json', 'r') as fh:
                self.blacklist = [key.lower() for key in json.load(fh)]

        if not sequences:
            sequences = [
                (24, 3, 200),
                (10, 0, 56),
                (15, 2, 91),
                (25, 3, 200),
                (25, 3, 56)]

        self.sequences = sequences
        for cur_seq in self.sequences:
            assert len(cur_seq) == 3, 'Not enough values; "%s"' % cur_seq

        assert byte_to_check in range(len(self.sequences)), 'please choose a valid sequence to check against'
        self.byte_to_check = byte_to_check

        self._cached_checksums = {}
        self._cached_key_bytes = {}

    def _get_key_byte(self, seed: int, a: int, b: int, c: int):
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
        return result

    def get_checksum(self, string: str):

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

    def format_result(self, key: list):
        seed = key[0][2:].rjust(4, '0')
        key_bytes = '-'.join([x[2:].rjust(4, '0') for x in key[1:]])
        return seed + '-' + key_bytes

    def make_key(self, seed: int):
        key = []
        seed = int(seed)

        # the key string begins with a hexadecimal string of the seed
        key.append(hex(seed))

        # Fill keybytes with values derived from seed.
        # The parameters used here must be exactly the same
        # as the ones used in the check_key function.
        # A real key system should use more than four bytes.
        # then is followed by hexadecimal strings of each byte in the key
        for cur_seq in self.sequences:
            cur_byte = hex(self._get_key_byte(seed, *cur_seq) // 2)
            key.append(cur_byte)

        key = self.format_result(key)

        # add checksum to key string
        checksum = self.get_checksum(key)
        key += '-{}'.format(checksum)

        return key

    def check_key_checksum(self, key: str):

        if key.count('-') != (len(self.sequences) + 1):
            raise KeyInvalid('wrong number of sections; {} <> {}'.format(
                key.count('-') + 1, key))

        # remove cosmetic hyphens and normalize case
        key = key.upper().split('-')

        # last four characters are the checksum
        grabbed_checksum = key[-1]
        s = '-'.join(key[:-1])

        # compare the supplied checksum against the real checksum for
        # the key string.
        if not int(grabbed_checksum, 16) != int(self.get_checksum(s), 16):
            raise KeyBadChecksum('Incorrect checksum; {} is not equal to {}'.format(
                grabbed_checksum, self.get_checksum(s)))

        return True

    def check_key(self, key: str):
        # test against blacklist
        if len(self.blacklist) > 0:
            if key.lower() in self.blacklist:
                raise KeyBlacklisted(key)

        self.check_key_checksum(key)

        # remove cosmetic hyphens and normalize case
        key = key.upper().split('-')

        # At this point, the key is either valid or forged,
        # because a forged key can have a valid checksum.
        # We now test the "bytes" of the key to determine if it is
        # actually valid.

        # extract the seed from the supplied key string
        try:
            seed = int('0x' + key[0], 16)
        except ValueError:
            raise KeyPhony('Not hex; {}'.format(repr(key)))

        selected_sequence = self.sequences[self.byte_to_check]

        key_byte = key[self.byte_to_check + 1].upper()
        generated_byte = hex(self._get_key_byte(seed, *selected_sequence) // 2)[2:]

        if key_byte != generated_byte.upper().rjust(4, '0'):
            raise KeyPhony(key)

        # If we get this far, then it means the key is either good, or was made
        # with a keygen derived from "this" release.
        return True


class KeyGenException(Exception):
    pass


class KeyInvalid(KeyGenException):
    pass


class KeyBlacklisted(KeyGenException):
    pass


class KeyPhony(KeyGenException):
    pass


class KeyBadChecksum(KeyGenException):
    pass


def main():
    cur_delser = Delser()

    space = (65535, 524288)
    assert space[0] >= 0, 'the lower bound for the key must not be below zero'

    to_test = map(cur_delser.make_key, range(*space))

    bad = set()
    for test in to_test:
        if not cur_delser.check_key(test):
            print('For the key', test, 'with length', len(test))
            bad.add(test)
    print(len(bad), 'bad keys')


if __name__ == '__main__':
    # cur_delser = Delser()
    # while True:
    #     key = cur_delser.make_key(input('seed> '))
    #     cur_delser.check_key(key)
    #     print('key> {}\n'.format(key))
    main()
